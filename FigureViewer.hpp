#ifndef FIGUREVIEWER_HPP
#define FIGUREVIEWER_HPP

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include "GlTextureWrapper.hpp"
#include "FigureData.hpp"

#include <QQuickItem>

class FigureViewer : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(float scaleX READ getScaleX WRITE setScaleX NOTIFY scaleChanged);
  Q_PROPERTY(float scaleY READ getScaleY WRITE setScaleY NOTIFY scaleChanged);
  Q_PROPERTY(float scaleZ READ getScaleZ WRITE setScaleZ NOTIFY scaleChanged);
  Q_PROPERTY(int figuresCount READ getFiguresCount WRITE setFiguresCount NOTIFY figuresCountChanged);
  Q_PROPERTY(FigureData::Type figureType READ getFigureType WRITE setFigureType NOTIFY figureTypeChanged);
  Q_PROPERTY(float camAngleX READ getCamAngleX WRITE setCamAngleX NOTIFY camAngleXChanged);
  Q_PROPERTY(float camAngleY READ getCamAngleY WRITE setCamAngleY NOTIFY camAngleYChanged);
  Q_PROPERTY(float camDistance READ getCamDistance WRITE setCamDistance NOTIFY camDistanceChanged);
  Q_PROPERTY(int trianglesCount READ getTrianglesCount NOTIFY trianglesCountChanged);
  Q_PROPERTY(float randomSeed READ getRandomSeed WRITE setRandomSeed NOTIFY randomSeedChanged)

public:
  FigureViewer();
  ~FigureViewer();

  const QVector3D& getScale() const;

  float getScaleX() const;
  void setScaleX(float newScaleX);

  float getScaleY() const;
  void setScaleY(float newScaleY);

  float getScaleZ() const;
  void setScaleZ(float newScaleZ);

  int getFiguresCount() const;
  void setFiguresCount(int newFiguresCount);

  FigureData::Type getFigureType() const;
  void setFigureType(FigureData::Type newFigureType);

  float getCamAngleX() const;
  void setCamAngleX(float newCamAngleX);

  float getCamAngleY() const;
  void setCamAngleY(float newCamAngleY);

  float getCamDistance() const;
  void setCamDistance(float newCamDistance);

  int getTrianglesCount() const;

  float getRandomSeed() const;
  void setRandomSeed(float newRandomSeed);

signals:
  void scaleChanged();
  void figuresCountChanged();
  void figureTypeChanged();

  void camAngleXChanged();
  void camAngleYChanged();
  void camDistanceChanged();

  void trianglesCountChanged();

  void randomSeedChanged();

  // QQuickItem interface
protected:
  QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *) override;
  void releaseResources() override final;

private:
  GlTextureWrapper tex;
  QOpenGLBuffer glVbo;
  QOpenGLBuffer glIbo;
  QOpenGLVertexArrayObject glVao;
  QOpenGLShaderProgram glShader;
  QOpenGLBuffer glUbo;

  QSurface *sfc;
  QOpenGLContext *ctx;

  QVector3D scale;
  float camAngleX;
  float camAngleY;
  float camDistance;
  float randomSeed;
  int figuresCount;
  int trianglesCount;
  FigureData::Type figureType;

  bool scaleDirty;
  bool camDirty;
  bool geometryDirty;

  void checkShader();
  void checkUbo();
  void checkGeometry();
  void loadCubeVbo();
  void loadSphereVbo();
  void loadTorusVbo();

  void renderToTexture();
};

#endif // FIGUREVIEWER_HPP
