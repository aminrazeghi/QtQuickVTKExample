# QtQuick VTK Example

This project is a minimal, practical example demonstrating how to integrate the Visualization Toolkit (**VTK**) with **Qt 6 / QtQuick** (QML) using `QQuickVTKItem`.

## 🌟 Features

- **QML Integration**: Seamlessly embeds a VTK 3D viewport within a purely QML-based user interface.
- **STL File Handling**: Load `.stl` geometries via a native file dialog. Loading is handled asynchronously using `QtConcurrent` to prevent blocking the UI thread.
- **Geometry Management**: 
  - View a list of all loaded models in a sidebar UI (QML `ListView`).
  - Right-click models in the list to **Rename** or **Remove** them from the scene.
- **Dynamic Render Grid**: Features a 3D floor grid that automatically scales and positions itself according to the bounds of the loaded actors.
- **Custom Viewport Controls**: 
  - Context menu to update the viewport's background color dynamically.
  - Custom VTK mouse interactor style setup.

## 🛠️ Prerequisites

To build and run this example, you will need:
- **CMake** (3.16 or newer)
- **Qt 6** (Requires `Core`, `Gui`, `Qml`, `Quick`, and `Concurrent` modules)
- **VTK 9.3** or newer

## 🚀 Building the Project

Standard CMake build sequence:

```bash
# Clone the repository and navigate into the project directory
cd qt_vtk_example_quick

# Create a build directory
mkdir build && cd build

# Configure using CMake
# Make sure your VTK_DIR and Qt6_DIR are properly set if they are not in standard locations
cmake ..

# Build the project
cmake --build .
```

## 🏃 Running the Application

After a successful build, you can run the executable from your build directory:

```bash
./appqtquickExampleVTK
```

## 📁 Code Structure Overview

- `main.cpp`: Entry point setting up `QGuiApplication`, `QQmlApplicationEngine`, and initializing the `QQuickVTKItem` graphics API.
- `CMakeLists.txt`: CMake configuration mapping Qt 6 modules, VTK 9.3 dependencies, and QML resources.
- `Main.qml`: The core user interface defined in QML. Houses the file dialogues, geometry list, and the `MyVTKItem` component.
- `MyVTKItem.h/.cpp`: The C++ backend subclassing `QQuickVTKItem`. It interacts with the VTK render window and manages the loaded actors (STL pipeline, rendering, grid scaling).
- `MouseInteractorStyle.h/.cpp`: Custom interaction behavior for the VTK render window.
- `MyVTKItemUserData.h`: Contains internal VTK objects like the renderer, grid actors, and a mapping of the loaded actors to easily bridge VTK and Qt.
