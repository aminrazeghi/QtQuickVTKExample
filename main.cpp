#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickVTKItem.h>

int main(int argc, char *argv[]) {

  QQuickVTKItem::setGraphicsApi();

  QGuiApplication app(argc, argv);

  QQmlApplicationEngine engine;
  const QUrl url(QStringLiteral("qrc:/qtquickExampleVTK/Main.qml"));

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
  engine.load(url);

  return app.exec();
}
