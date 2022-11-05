#include "Application.hpp"
#include "FigureViewer.hpp"
#include <QQmlApplicationEngine>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
  Application app(argc, argv);

  QSurfaceFormat sfcFmt;
  sfcFmt.setMajorVersion(3);
  sfcFmt.setMinorVersion(3);
  sfcFmt.setProfile(QSurfaceFormat::CoreProfile);
  QSurfaceFormat::setDefaultFormat(sfcFmt);

  qmlRegisterType<FigureViewer>("FigureViewer", 1,0, "FigureViewer");
  qmlRegisterUncreatableMetaObject(
        FigureData::staticMetaObject,
        "Figure", 1,0,
        "FigureType", "Error: only enums");

  QQmlApplicationEngine engine;
  const QUrl url(QStringLiteral("qrc:/main.qml"));
  QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                   &app, [url, &app](QObject *obj, const QUrl &objUrl) {
      if (!obj && url == objUrl)
        QCoreApplication::exit(-1);

      obj->setProperty("addedObjects", QVariant::fromValue(app.getAddedObjects()));
      QObject::connect(obj, SIGNAL(removeObject(int)), app.getAddedObjects(), SLOT(removeObject(int)));
      QObject::connect(obj, SIGNAL(addObject(int,int,qreal,qreal,qreal)), app.getAddedObjects(), SLOT(addObject(int,int,qreal,qreal,qreal)));
    }, Qt::QueuedConnection);
  engine.load(url);

  return app.exec();
}
