#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <QGuiApplication>
#include <QThread>
#include "AddedObjectsModel.hpp"

class Application : public QGuiApplication
{
  Q_OBJECT
public:
  Application(int &argc, char **argv, int flags = ApplicationFlags);
  ~Application();

  AddedObjectsModel *getAddedObjects();

private:
  AddedObjectsModel addedObjects;
  QThread objectsModelThread;
};

#endif // APPLICATION_HPP
