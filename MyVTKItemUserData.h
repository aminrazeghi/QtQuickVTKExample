#pragma once

#include <QMap>
#include <QStringList>

#include <vtkActor.h>
#include <vtkNew.h>
#include <vtkObject.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSetGet.h>
#include <vtkSmartPointer.h>
#include <vtkPlaneSource.h>
#include <vtkActor.h>

struct MyVTKItemUserData : public vtkObject
{
    static MyVTKItemUserData* New();
    vtkTypeMacro(MyVTKItemUserData, vtkObject);

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkPlaneSource> gridSource;
    vtkNew<vtkActor> gridActor;

    QMap<QString, vtkSmartPointer<vtkActor>> actorMap;
};
