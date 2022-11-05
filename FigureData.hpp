#ifndef FIGUREDATA_HPP
#define FIGUREDATA_HPP

#include <QObject>
#include <QVector3D>

class FigureData : public QObject
{
  Q_OBJECT

public:
  enum class Type: quint8 {
    Sphere,
    Box,
    Torus
  };
  Q_ENUM(Type);

  Q_PROPERTY(Type type READ getType WRITE setType NOTIFY typeChanged);
  Q_PROPERTY(int count READ getCount WRITE setCount NOTIFY countChanged);

  Q_PROPERTY(float scale_x READ getScaleX WRITE setScaleX NOTIFY scaleChanged);
  Q_PROPERTY(float scale_y READ getScaleY WRITE setScaleY NOTIFY scaleChanged);
  Q_PROPERTY(float scale_z READ getScaleZ WRITE setScaleZ NOTIFY scaleChanged);

public:
  explicit FigureData(QObject *parent = nullptr);
  FigureData(Type type, int count, const QVector3D &scale);

  const QVector3D &getScale() const;
  void setScale(const QVector3D &newScale);

  float getScaleX() const;
  void setScaleX(float newScaleX);

  float getScaleY() const;
  void setScaleY(float newScaleY);

  float getScaleZ() const;
  void setScaleZ(float newScaleZ);

  Type getType() const;
  void setType(Type newType);

  int getCount() const;
  void setCount(int newCount);

signals:
  void positionChanged();
  void scaleChanged();
  void typeChanged();

  void countChanged();

private:
  QVector3D scale;

  int count;
  Type type;
};

#endif // FIGUREDATA_HPP
