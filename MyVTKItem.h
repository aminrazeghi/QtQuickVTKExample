#pragma once

#include "MyVTKItemUserData.h"

#include <QColor>
#include <QQuickVTKItem.h>
#include <QStringList>
#include <QUrl>
#include <QMap>
#include <vtkActor.h>
#include <vtkSmartPointer.h>

class MyVTKItem : public QQuickVTKItem
{
    Q_OBJECT
    QML_ELEMENT

  public:
    MyVTKItem();
    ~MyVTKItem() override;

    Q_PROPERTY(QStringList actorList READ actorList NOTIFY actorListChanged)

    QStringList actorList() const;

    Q_INVOKABLE void addSTL(const QUrl& fileUrl);

    Q_INVOKABLE void renameActor(const QString& oldName, const QString& newName);
    Q_INVOKABLE bool removeActor(vtkSmartPointer<vtkActor> actor);
    Q_INVOKABLE void removeActor(const QString& name);

    Q_INVOKABLE void changeBackground(const QColor& color);

    void updateGrid();

    vtkUserData initializeVTK(vtkRenderWindow* renderWidnow) override;
  signals:
    void actorListChanged();

  private:
    vtkSmartPointer<MyVTKItemUserData> m_vtkData;
    QStringList m_actorList;
};
