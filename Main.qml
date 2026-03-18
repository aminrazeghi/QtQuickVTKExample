import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Dialogs
import qtquickExampleVTK 1.0

Window {
    width: 1000
    height: 700
    visible: true
    title: qsTr("QtQuick VTK example")

    FileDialog {
        id: stlReaderFileDialog
        title: "Please choose and STL file"
        nameFilters: ["STL files (*.stl)"]
        onAccepted: {
            vtkView.addSTL(stlReaderFileDialog.selectedFile);
        }
    }

    ColorDialog {
        id: backgroundColorDialog
        title: "Change background color"
        onAccepted: {
            vtkView.changeBackground(backgroundColorDialog.selectedColor);
        }
    }

    Dialog {
        id: renameDialog
        title: "Rename Geometry"
        standardButtons: Dialog.Ok | Dialog.Cancel
        property string oldName: ""

        Column {
            spacing: 10
            Label { text: "New name:" }
            TextField {
                id: renameField
                width: 200
            }
        }
        onAccepted: {
            if (renameField.text !== "" && renameField.text !== oldName) {
                console.log("rename", oldName, renameField.text);
                vtkView.renameActor(oldName, renameField.text);
            }
        }
    }

    Row {
        anchors.fill: parent

        Rectangle {
            width: 250
            height: parent.height

            Column {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 15

                GroupBox {
                    title: "Geometries"
                    width: parent.width

                    Column {
                        anchors.fill: parent
                        spacing: 10

                        Button {
                            text: "Add Geometry"
                            width: parent.width
                            highlighted: true
                            onClicked: {
                                stlReaderFileDialog.open();
                            }
                        }

                        ListView {
                            id: actorListView
                            width: parent.width
                            height: 300
                            model: vtkView.actorList
                            clip: true
                            highlight: Rectangle {
                                color: "lightblue"
                                radius: 2
                            }
                            // focus: true
                            delegate: ItemDelegate {
                                width: parent.width
                                text: "📐 " + modelData

                                MouseArea {
                                    anchors.fill: parent
                                    acceptedButtons: Qt.RightButton
                                    onClicked: contextMenu.popup()
                                }

                                Menu {
                                    id: contextMenu
                                    MenuItem {
                                        text: "✏️ Rename"
                                        onTriggered: {
                                            renameDialog.oldName = modelData
                                            renameField.text = modelData
                                            renameDialog.open()
                                        }
                                    }
                                    MenuItem {
                                        text: "🗑️ Remove"
                                        onTriggered: {
                                            vtkView.removeActor(modelData)
                                        }
                                    }
                                }
                            }
                            ScrollIndicator.vertical: ScrollIndicator {}
                        }

                        Button {
                            id: backgroundBtn
                            text: "Change Background"
                            width: parent.width
                            onClicked: {
                                backgroundColorDialog.open();
                                console.log("clicked");
                            }
                        }
                    }
                }
            }
        }

        MyVTKItem {
            id: vtkView
            width: parent.width - 250
            height: parent.height
            focus: true

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
                onPressed: function(mouse) {
                    vtkView.forceActiveFocus()
                    // allow the event to pass through to the VTK interactor
                    mouse.accepted = false
                }
            }

            Rectangle {
                id: logo
                anchors.margins: 15
                anchors.top: parent.top
                anchors.left: parent.left
                width: 250
                height: 40
                color: "#80000000"
                radius: 5

                Text {
                    anchors.centerIn: parent
                    color: "white"
                    text: "Viewport"
                    font.pixelSize: 10
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        console.log(" clicked");
                        logo.color = "blue";
                    }
                }
            }
        }
    }

// for debugging (print all actors)
    Connections {
        target: vtkView
        function onActorListChanged() {
            console.log("Actors:", vtkView.actorList);
        }
    }

// focus on vtkView when the application starts
    Component.onCompleted: {
        vtkView.forceActiveFocus()
    }
}
