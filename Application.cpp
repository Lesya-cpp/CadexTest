#include "Application.hpp"

Application::Application(int &argc, char **argv, int flags):
  QGuiApplication(argc, argv, flags),
  addedObjects()
{
  addedObjects.moveToThread(&objectsModelThread);
  objectsModelThread.start();
}

Application::~Application()
{
  objectsModelThread.exit();
  objectsModelThread.wait();
}

AddedObjectsModel *Application::getAddedObjects()
{
  return &addedObjects;
}
