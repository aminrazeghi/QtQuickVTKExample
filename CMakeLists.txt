cmake_minimum_required(VERSION 3.16)
project(qtquickExampleVTK LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}/.local") # for development


find_package(Qt6  REQUIRED Core Gui Qml Quick Concurrent)

find_package(VTK 9.3 REQUIRED)

if (NOT VTK_FOUND)
	message(FATAL_ERROR "qtquickExampleVTK: Unable to find the VTK build folder.")
endif()

set(QT_QML_GENERATE_QMLLS_INI ON)

qt_add_executable(appqtquickExampleVTK 
	main.cpp
	MyVTKItem.cpp
	MyVTKItem.h
	MyVTKItemUserData.h
	MouseInteractorStyle.h
	MouseInteractorStyle.cpp
)

qt_add_qml_module(appqtquickExampleVTK 
	URI qtquickExampleVTK
	VERSION 1.0
	QML_FILES Main.qml
)


target_link_libraries(appqtquickExampleVTK 
	PRIVATE
	Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::Concurrent
	${VTK_LIBRARIES}
)

vtk_module_autoinit(
	TARGETS appqtquickExampleVTK
	MODULES ${VTK_LIBRARIES}
)
