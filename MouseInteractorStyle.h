#pragma once

#include <iostream>

#include <map>
#include <string>
#include <vtkActor.h>
#include <vtkCellPicker.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSetGet.h>
#include <vtkNamedColors.h>
#include <vtkSmartPointer.h>
#include <vtkProperty.h>
#include <vtkPropPicker.h>

#include "MyVTKItemUserData.h"

class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera
{
  public:
    static MouseInteractorStyle* New();
    vtkTypeMacro(MouseInteractorStyle, vtkInteractorStyleTrackballCamera);

    void SetUserData(MyVTKItemUserData* data) { this->UserData = data; }

    bool PickingEnabled = true;

    virtual void OnChar() override
    {
        vtkRenderWindowInteractor* rwi = this->GetInteractor();
        char key = rwi->GetKeyCode();
        if (key == 'r' || key == 'R')
        {
            if (UserData)
            {
                vtkNew<vtkNamedColors> colors;
                for (auto const& actor : UserData->actorMap)
                {
                    actor->GetProperty()->SetColor(colors->GetColor3d("white").GetData());
                    actor->GetProperty()->SetEdgeVisibility(0);
                }
            }
            rwi->Render();
        }
        else if (key == 't' || key == 'T')
        {
            this->PickingEnabled = !this->PickingEnabled;
        }
        vtkInteractorStyleTrackballCamera::OnChar();
    }

    virtual void OnLeftButtonDown() override
    {
        if (!PickingEnabled)
        {
            vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
            return;
        }
        auto interactor = this->GetInteractor();
        if (!interactor)
            return;

        int* pos = this->GetInteractor()->GetEventPosition();

        vtkSmartPointer<vtkRenderer> renderer = this->GetDefaultRenderer();
        if (!renderer)
        {
            std::cerr << "Error: Renderer is null\n";
            vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
            return;
        }

        vtkNew<vtkPropPicker> picker;
        picker->Pick(pos[0], pos[1], 0, renderer);

        vtkSmartPointer<vtkProp> pickedProp = picker->GetViewProp();
        vtkSmartPointer<vtkActor> pickedActor = vtkActor::SafeDownCast(pickedProp);
        if (pickedActor && UserData)
        {
            bool found = false;
            for (auto const& actor : UserData->actorMap)
            {
                if (actor == pickedActor)
                {
                    found = true;
                    break;
                }
            }

            if (found)
            {
                vtkNew<vtkNamedColors> colors;
                pickedActor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());
            }
            else
            {
                std::cout << "[INFO] Picked an actor is not in the map\n";
            }
        }
        else
        {
            std::cout << "EMpty spacee\n";
        }

        interactor->GetRenderWindow()->Render();

        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }

  private:
    MyVTKItemUserData* UserData = nullptr;
};
