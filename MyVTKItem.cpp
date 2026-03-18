#include "MyVTKItem.h"
#include "MouseInteractorStyle.h"
#include "MyVTKItemUserData.h"

#include <QFuture>
#include <QtConcurrent>

#include <QQuickVTKItem.h>
#include <qobjectdefs.h>
#include <qtmetamacros.h>
#include <vtkActor.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkPlaneSource.h>
#include <vtkProperty.h>
#include <vtkMath.h>

vtkStandardNewMacro(MyVTKItemUserData);

MyVTKItem::MyVTKItem()
{
    m_vtkData = vtkSmartPointer<MyVTKItemUserData>::New();
}
MyVTKItem::~MyVTKItem() = default;

QQuickVTKItem::vtkUserData
MyVTKItem::initializeVTK(vtkRenderWindow* renderWindow)
{
    vtkNew<MyVTKItemUserData> scene;
    m_vtkData = scene;

    vtkNew<MouseInteractorStyle> style;
    style->SetDefaultRenderer(scene->renderer);
    style->SetUserData(scene);

    if (renderWindow->GetInteractor())
    {
        renderWindow->GetInteractor()->SetInteractorStyle(style);
    }

    scene->renderer->SetBackground(0.1, 0.2, 0.4);
    scene->renderer->SetBackgroundAlpha(1.0);
    scene->renderer->SetErase(1);

    // Setup grid in UserData
    scene->gridSource->SetOrigin(-50.0, -50.0, 0.0);
    scene->gridSource->SetPoint1(50.0, -50.0, 0.0);
    scene->gridSource->SetPoint2(-50.0, 50.0, 0.0);
    scene->gridSource->SetXResolution(10);
    scene->gridSource->SetYResolution(10);

    vtkNew<vtkPolyDataMapper> gridMapper;
    gridMapper->SetInputConnection(scene->gridSource->GetOutputPort());

    scene->gridActor->SetMapper(gridMapper);
    scene->gridActor->GetProperty()->SetRepresentationToWireframe();
    scene->gridActor->GetProperty()->SetColor(0.5, 0.5, 0.5);
    scene->renderer->AddActor(scene->gridActor);

    renderWindow->SetMultiSamples(0);
    renderWindow->AddRenderer(scene->renderer);

    return scene;
}

QStringList MyVTKItem::actorList() const
{
    return m_actorList;
}

void MyVTKItem::addSTL(const QUrl& fileUrl)
{
    QString filePath = fileUrl.toLocalFile();
    if (filePath.isEmpty())
        return;

    std::string path = filePath.toStdString();
    QString fileName = fileUrl.fileName();

    if (!m_actorList.contains(fileName))
    {
        m_actorList.append(fileName);
        emit actorListChanged();
    }

    QFuture<void> future = QtConcurrent::run([this, path, fileName]
                                             {
    vtkNew<vtkSTLReader> reader;
    reader->SetFileName(path.c_str());
    reader->Update();

    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->ShallowCopy(reader->GetOutput());

    dispatch_async([this, polyData, fileName](vtkRenderWindow *renderWindow,
                                              vtkUserData userData) {
      auto vtk = MyVTKItemUserData::SafeDownCast(userData);
      if (!vtk)
        return;

      vtkNew<vtkPolyDataMapper> mapper;
      mapper->SetInputData(polyData);

      vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
      actor->SetMapper(mapper);
      vtk->renderer->AddActor(actor);
      vtk->renderer->ResetCamera();
      updateGrid();

      QMetaObject::invokeMethod(this, [this, fileName, actor] () {
        m_vtkData->actorMap.insert(fileName, actor);
      });
    }); });

    update();
}

bool MyVTKItem::removeActor(vtkSmartPointer<vtkActor> actor)
{
    QString nameToRemove = m_vtkData->actorMap.key(actor, "");
    if (nameToRemove.isEmpty())
        return false;
    removeActor(nameToRemove);
    return true;
}

void MyVTKItem::renameActor(const QString& oldName, const QString& newName)
{
    if (oldName == newName || m_actorList.contains(newName))
        return;

    int index = m_actorList.indexOf(oldName);
    if (index != -1)
    {
        m_actorList.replace(index, newName);
    }

    auto actor = m_vtkData->actorMap.take(oldName);
    m_vtkData->actorMap.insert(newName, actor);

    emit actorListChanged();
}

void MyVTKItem::removeActor(const QString& name)
{
    if (!m_vtkData->actorMap.contains(name))
        return;

    vtkSmartPointer<vtkActor> actorToRemove = m_vtkData->actorMap.value(name);

    dispatch_async(
        [this, actorToRemove](vtkRenderWindow* renderWindow, vtkUserData userData)
        {
            auto vtk = MyVTKItemUserData::SafeDownCast(userData);
            if (!vtk)
                return;
            vtk->renderer->RemoveActor(actorToRemove);
            vtk->renderer->ResetCamera();
        });

    m_actorList.removeAll(name);
    m_vtkData->actorMap.remove(name);
    emit actorListChanged();
    updateGrid();
    update(); // Force QML to redraw the VTK viewport
}

void MyVTKItem::changeBackground(const QColor& color)
{

    dispatch_async([this, color](vtkRenderWindow* renderWindow,
                                 vtkUserData userData)
                   {
    auto vtk = MyVTKItemUserData::SafeDownCast(userData);
    if (!vtk)
      return;
    vtk->renderer->SetBackground(color.redF(), color.greenF(), color.blueF()); });
}

void MyVTKItem::updateGrid()
{
    dispatch_async([this](vtkRenderWindow* renderWindow, vtkUserData userData)
                   {
        auto vtk = MyVTKItemUserData::SafeDownCast(userData);
        if (!vtk)
            return;

        // Temporarily hide grid actor so it's not included in bounds computation
        vtk->gridActor->SetVisibility(false);
        
        double bounds[6];
        vtk->renderer->ComputeVisiblePropBounds(bounds);
        
        vtk->gridActor->SetVisibility(true);

        if (!vtkMath::AreBoundsInitialized(bounds)) {
            // No actors, reset grid to default
            vtk->gridSource->SetOrigin(-50.0, -50.0, 0.0);
            vtk->gridSource->SetPoint1(50.0, -50.0, 0.0);
            vtk->gridSource->SetPoint2(-50.0, 50.0, 0.0);
            return;
        }

        double xMin = bounds[0], xMax = bounds[1];
        double yMin = bounds[2], yMax = bounds[3];
        double zMin = bounds[4];

        double length = std::max(xMax - xMin, yMax - yMin);
        if (length == 0.0) length = 10.0;
        
        double padding = length * 0.2; // 20% padding
        xMin -= padding;
        xMax += padding;
        yMin -= padding;
        yMax += padding;

        double z = zMin - padding * 0.1; // slightly below the lowest point

        vtk->gridSource->SetOrigin(xMin, yMin, z);
        vtk->gridSource->SetPoint1(xMax, yMin, z);
        vtk->gridSource->SetPoint2(xMin, yMax, z);

        // Tell QQuickVTKItem to re-render using our view
        renderWindow->Render(); });
}
