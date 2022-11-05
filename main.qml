import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import Figure 1.0
import FigureViewer 1.0

Window {
    id: window
    width: 640
    height: 480
    visible: true
    title: "CADEX Test"
    property var addedObjects: []
    property var supportedFigureTypes: [ "Sphere", "Cube", "Torus" ]

    signal addObject(int type, int count, real scaleX, real scaleY, real scaleZ)
    signal removeObject(int index)

    onAddObject: addObjectDlg.visible=false

    Column {
        spacing: 5
        x: 5
        y: 5
        Text {
            text: "Added objects list"
            height: 40
        }
        ListView {
            id: addedObjectsView
            model: addedObjects
            width: 255
            height: window.height - 235
            spacing: 2
            delegate: Row {
                required property string figure_type
                required property int type
                required property int figures_count
                required property real scale_x
                required property real scale_y
                required property real scale_z
                required property int index
                property string extendedDescription:
                    ListView.isCurrentItem ?
                        "\n\tType: " + figure_type
                        + "\n\tCount: " + figures_count
                        + "\n\tScale: ("
                        + Number(scale_x).toFixed(3) + "; " + Number(scale_y).toFixed(3) + "; " + Number(scale_z).toFixed(3) + ")" :
                        ""
                width: ListView.view.width
                Text {
                    text: "Object " + index + extendedDescription
                    width: parent.ListView.view.width - 26
                    verticalAlignment: Text.AlignVCenter
                }
                Button {
                    width: 24
                    height: 24
                    y: 2
                    text: "X"
                    onClicked: removeObject(index)
                }
            }
            highlight: Rectangle {
                border.width: 1
            }
            MouseArea {
                anchors.fill: parent
                propagateComposedEvents: true
                onClicked: {
                    if (mouseX < parent.width - 18)
                        parent.currentIndex = parent.indexAt(mouseX, mouseY)
                    else
                        mouse.accepted = false
                }
                onPressed: mouse.accepted = (mouseX < parent.width - 18)
                onReleased: mouse.accepted = (mouseX < parent.width - 18)
            }
        }
        Rectangle {
            border.width: 1
            height: 130
            width: addedObjectsView.width
            Text {
                text: addedObjectsView.currentItem?
                          "Selected object information:\n"
                          + addedObjectsView.currentItem.extendedDescription
                          + "\n\tTriangles:" + figureViewer.trianglesCount :
                          "No selected object"
            }
        }
        Button {
            text: "Add an object"
            x: 0.5 * (parent.width - width)
            width: 180
            onClicked: addObjectDlg.visible = true
        }
    }

    Rectangle {
        id: addObjectDlg
        visible: false
//        radius: 7
        border.width: 1
        x: 200
        y: 200
        z: 2
        width: 480
        height: 205
        clip: true
        Column {
            anchors.fill: parent
            spacing: 5
            topPadding: 1
            leftPadding: 1
            rightPadding: 1
            Rectangle {
                width: parent.width-2
                height: 20
                color: "gray"
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: "white"
                    text: "Adding a new object"
                    font.bold: true
                }
                MouseArea {
                    anchors.fill: parent
                    property real lastMouseX: 0
                    property real lastMouseY: 0
                    function setLastMousePos()
                    {
                        lastMouseX = mouseX
                        lastMouseY = mouseY
                    }

                    onPositionChanged: {
                        addObjectDlg.x += mouseX - lastMouseX
                        addObjectDlg.y += mouseY - lastMouseY
                    }
                    onPressed: setLastMousePos()
                    onReleased: setLastMousePos()
                }
            }
            Row {
                spacing: 2
                Text { text: "Type"; anchors.verticalCenter: parent.verticalCenter }
                ComboBox {
                    id: figureTypeSelector
                    model: supportedFigureTypes
                }
            }
            Row {
                spacing: 2
                Text { text: "Count"; anchors.verticalCenter: parent.verticalCenter }
                SpinBox {
                    id: figuresCountSpin
                    from: 1
                    value: 1
                    to: 9999
                }
            }
            Row {
                spacing: 2
                Text { text: "Scale"; anchors.verticalCenter: parent.verticalCenter }
                RealSpinBox {
                    id: scaleXSpin
                }
                RealSpinBox {
                    id: scaleYSpin
                }
                RealSpinBox {
                    id: scaleZSpin
                }
            }
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 2
                Button {
                    text: "Confirm"
                    onClicked: addObject(
                                   figureTypeSelector.currentIndex,
                                   figuresCountSpin.value,
                                   scaleXSpin.realValue,
                                   scaleYSpin.realValue,
                                   scaleZSpin.realValue
                                   )
                }
                Button {
                    text: "Cancel"
                    onClicked: addObjectDlg.visible = false
                }
            }
        }
    }

    Rectangle {
        x: addedObjectsView.x + addedObjectsView.width + 15
        y: 5
        width: parent.width - x - 5
        height: parent.height - 2*y
        border.width: 1
        FigureViewer {
            id: figureViewer
            anchors.fill: parent
            scaleX: addedObjectsView.currentItem? addedObjectsView.currentItem.scale_x: 0
            scaleY: addedObjectsView.currentItem? addedObjectsView.currentItem.scale_y: 0
            scaleZ: addedObjectsView.currentItem? addedObjectsView.currentItem.scale_z: 0
            figureType: addedObjectsView.currentItem? addedObjectsView.currentItem.type: FigureType.Box
            figuresCount: addedObjectsView.currentItem? addedObjectsView.currentItem.figures_count: 0
            randomSeed: addedObjectsView.currentIndex
        }
        MouseArea {
            anchors.fill: parent
            property real lastMouseX: 0
            property real lastMouseY: 0
            function setLastMousePos()
            {
                lastMouseX = mouseX
                lastMouseY = mouseY
            }

            onPositionChanged: {
                figureViewer.camAngleY -= (mouseX - lastMouseX) / 8.0
                figureViewer.camAngleX += (mouseY - lastMouseY) / 8.0
                setLastMousePos()
            }
            onPressed: setLastMousePos()
            onReleased: setLastMousePos()

            onWheel: {
                if(wheel.pixelDelta.y)
                    figureViewer.camDistance -= 0.0025 * wheel.pixelDelta.y
                else if(wheel.angleDelta.y)
                    figureViewer.camDistance -= 0.0025 * wheel.angleDelta.y
                figureViewer.camDistance = Math.max(0, Math.min(figureViewer.camDistance, 100))
            }
        }
    }
}
