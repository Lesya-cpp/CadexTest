#include "FigureViewer.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLExtraFunctions>
#include <QSGSimpleTextureNode>
#include "models/Vertex.h"
#include <QOpenGLShader>
#include <QQuaternion>
#include <QThread>

namespace {
  struct UboData {
    glm::mat4 mvpMat;
    glm::vec4 camPos;
    glm::vec3 scale;
  };

  template<size_t N>
  constexpr QLatin1String latin1Str(const char (&str)[N])
  {
    return str[N-1] == '\0'? QLatin1String(str, N-1): QLatin1String(str, N);
  }

  void receiveGLDebug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *)
  {
    const uint16_t dbg = 2;

    static const std::unordered_map<GLenum, QLatin1String> sourceNames = {
      {GL_DEBUG_SOURCE_API,             latin1Str("calls to the OpenGL API")},
      {GL_DEBUG_SOURCE_WINDOW_SYSTEM,   latin1Str("calls to a window-system API")},
      {GL_DEBUG_SOURCE_SHADER_COMPILER, latin1Str("compiler for a shading language")},
      {GL_DEBUG_SOURCE_THIRD_PARTY,     latin1Str("application associated with OpenGL")},
      {GL_DEBUG_SOURCE_APPLICATION,     latin1Str("user of this application")},
      {GL_DEBUG_SOURCE_OTHER,           latin1Str("Unknown")}
    };

    static const std::unordered_map<GLenum, QLatin1String> typeNames = {
      {GL_DEBUG_TYPE_ERROR,               latin1Str("Error")},
      {GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, latin1Str("Warning (deprecated)")},
      {GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,  latin1Str("Warning (undefined behavior)")},
      {GL_DEBUG_TYPE_PORTABILITY,         latin1Str("Warning (functionality is not portable)")},
      {GL_DEBUG_TYPE_PERFORMANCE,         latin1Str("Warning (performance issues)")},
      {GL_DEBUG_TYPE_MARKER,              latin1Str("Command stream annotation")},
      {GL_DEBUG_TYPE_PUSH_GROUP,          latin1Str("Group pushed")},
      {GL_DEBUG_TYPE_POP_GROUP,           latin1Str("Group poped")},
      {GL_DEBUG_TYPE_OTHER,               latin1Str("Unknown")}
    };

    if(length < 0 || strlen(reinterpret_cast<const char*>(message)) < static_cast<size_t>(length))
      return;

    switch(severity)
      {
      case GL_DEBUG_SEVERITY_HIGH:
        qCritical().setAutoInsertSpaces(true);
        qCritical() << "OpenGL debug message #" << id << '{';
        qCritical() << "\tFrom " << sourceNames.at(source);
        qCritical() << "\tType: " << typeNames.at(type);
        qCritical() << "\tText: " << reinterpret_cast<const char*>(message);
        qCritical().nospace() << '}';
        break;

      case GL_DEBUG_SEVERITY_MEDIUM:
        if(dbg < 2) break;
        qWarning() << "OpenGL debug message #" << id << '{';
        qWarning() << "\tFrom " << sourceNames.at(source);
        qWarning() << "\tType: " << typeNames.at(type);
        qWarning() << "\tText: " << reinterpret_cast<const char*>(message);
        qWarning().nospace() << '}';
        break;

      default:
        if(dbg < 3) break;
        qInfo() << "OpenGL debug message #" << id << '{';
        qInfo() << "\tFrom " << sourceNames.at(source);
        qInfo() << "\tType: " << typeNames.at(type);
        qInfo() << "\tText: " << reinterpret_cast<const char*>(message);
        qInfo().nospace() << '}';
        break;
      }
  }
}

FigureViewer::FigureViewer():
  glVbo(QOpenGLBuffer::VertexBuffer),
  glIbo(QOpenGLBuffer::IndexBuffer),
  glShader(this),
  glUbo(QOpenGLBuffer::VertexBuffer),
  sfc(nullptr),
  ctx(nullptr),
  camAngleX(0),
  camAngleY(0),
  camDistance(1.5),
  randomSeed(0),
  figuresCount(1),
  trianglesCount(0),
  scaleDirty(true),
  camDirty(true),
  geometryDirty(true)
{
  setFlag(QQuickItem::ItemHasContents);
  connect(this, &FigureViewer::scaleChanged, this, [this] { scaleDirty = true; update(); });
  connect(this, &FigureViewer::camAngleXChanged, this, [this] { camDirty = true; update(); });
  connect(this, &FigureViewer::camAngleYChanged, this, [this] { camDirty = true; update(); });
  connect(this, &FigureViewer::camDistanceChanged, this, [this] { camDirty = true; update(); });
  connect(this, &FigureViewer::widthChanged, this, [this] { camDirty = true; update(); });
  connect(this, &FigureViewer::heightChanged, this, [this] { camDirty = true; update(); });
  connect(this, &FigureViewer::randomSeedChanged, this, [this] { camDirty = true; update(); });
  connect(this, &FigureViewer::figureTypeChanged, this, [this] { geometryDirty = true; update(); });
  connect(this, &FigureViewer::figuresCountChanged, this, &FigureViewer::trianglesCountChanged);
}

FigureViewer::~FigureViewer()
{
  releaseResources();
}

const QVector3D &FigureViewer::getScale() const
{
  return scale;
}

float FigureViewer::getScaleX() const
{
  return getScale().x();
}

void FigureViewer::setScaleX(float newScaleX)
{
  if (qFuzzyCompare(getScaleX(), newScaleX))
    return;
  scale.setX(newScaleX);
  emit scaleChanged();
}

float FigureViewer::getScaleY() const
{
  return getScale().y();
}

void FigureViewer::setScaleY(float newScaleY)
{
  if (qFuzzyCompare(getScaleY(), newScaleY))
    return;
  scale.setY(newScaleY);
  emit scaleChanged();
}

float FigureViewer::getScaleZ() const
{
  return getScale().z();
}

void FigureViewer::setScaleZ(float newScaleZ)
{
  if (qFuzzyCompare(getScaleZ(), newScaleZ))
    return;
  scale.setZ(newScaleZ);
  emit scaleChanged();
}

FigureData::Type FigureViewer::getFigureType() const
{
  return figureType;
}

void FigureViewer::setFigureType(FigureData::Type newFigureType)
{
  if (figureType == newFigureType)
    return;
  figureType = newFigureType;
  emit figureTypeChanged();
}

int FigureViewer::getFiguresCount() const
{
  return figuresCount;
}

void FigureViewer::setFiguresCount(int newFiguresCount)
{
  if (figuresCount == newFiguresCount)
    return;
  figuresCount = newFiguresCount;
  emit figuresCountChanged();
}

QSGNode *FigureViewer::updatePaintNode(QSGNode *node, UpdatePaintNodeData *)
{
  if (ctx != QOpenGLContext::currentContext())
    {
      if (ctx)
        disconnect(ctx, &QOpenGLContext::aboutToBeDestroyed, this, &FigureViewer::releaseResources);
      releaseResources();
      ctx = QOpenGLContext::currentContext();
      if (ctx)
        {
          connect(ctx, &QOpenGLContext::aboutToBeDestroyed, this, &FigureViewer::releaseResources, Qt::DirectConnection);
          sfc = ctx->surface();

          ctx->functions()->glEnable(GL_DEBUG_OUTPUT);
          ctx->functions()->glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
          ctx->extraFunctions()->glDebugMessageCallback(receiveGLDebug, nullptr);
        } else
        sfc = nullptr;
    }

  if (width() <= 0 || height() <= 0)
    {
      tex.reset();
      delete node;
      return nullptr;
    }

  if (!node)
    {
      node = new QSGSimpleTextureNode;
      static_cast<QSGSimpleTextureNode*>(node)->setFiltering(QSGTexture::Nearest);
    } else
    node->markDirty(QSGNode::DirtyForceUpdate);

  tex.setSize(width(), height());
  static_cast<QSGSimpleTextureNode*>(node)->setRect(boundingRect());
  static_cast<QSGSimpleTextureNode*>(node)->setSourceRect(0,0, tex.textureSize().width(), tex.textureSize().height());

  static_cast<QSGSimpleTextureNode*>(node)->setTexture(&tex);
  renderToTexture();

  return node;
}

void FigureViewer::releaseResources()
{
  if(ctx)
    {
      if(QOpenGLContext::currentContext() != ctx)
        ctx->makeCurrent(sfc);
      glVao.destroy();
      glVbo.destroy();
      glIbo.destroy();
      glUbo.destroy();
      for(auto *shader: glShader.findChildren<QOpenGLShader*>(QString(), Qt::FindDirectChildrenOnly))
        delete shader;
      glShader.removeAllShaders();
      tex.reset();
      ctx = nullptr;
      sfc = nullptr;
    }
}

int FigureViewer::getTrianglesCount() const
{
  return trianglesCount * figuresCount;
}

float FigureViewer::getRandomSeed() const
{
  return randomSeed;
}

void FigureViewer::setRandomSeed(float newRandomSeed)
{
  if (qFuzzyCompare(randomSeed, newRandomSeed))
    return;
  randomSeed = newRandomSeed;
  emit randomSeedChanged();
}

float FigureViewer::getCamDistance() const
{
  return camDistance;
}

void FigureViewer::setCamDistance(float newCamDistance)
{
  if (qFuzzyCompare(camDistance, newCamDistance))
    return;
  camDistance = newCamDistance;
  emit camDistanceChanged();
}

float FigureViewer::getCamAngleX() const
{
  return camAngleX;
}

void FigureViewer::setCamAngleX(float newCamAngleX)
{
  if (qFuzzyCompare(camAngleX, newCamAngleX))
    return;
  camAngleX = newCamAngleX;
  emit camAngleXChanged();
}

float FigureViewer::getCamAngleY() const
{
  return camAngleY;
}

void FigureViewer::setCamAngleY(float newCamAngleY)
{
  if (qFuzzyCompare(camAngleY, newCamAngleY))
    return;
  camAngleY = newCamAngleY;
  emit camAngleYChanged();
}

void FigureViewer::checkShader()
{
  if(!glShader.isLinked())
    {
      auto sh = new QOpenGLShader(QOpenGLShader::Vertex);
      sh->compileSourceFile(QStringLiteral(":/main.vs.glsl"));
      sh->moveToThread(glShader.thread());
      sh->setParent(&glShader);
      glShader.addShader(sh);

      sh = new QOpenGLShader(QOpenGLShader::Fragment);
      sh->compileSourceFile(QStringLiteral(":/main.fs.glsl"));
      sh->moveToThread(glShader.thread());
      sh->setParent(&glShader);
      glShader.addShader(sh);
      glShader.link();
    }
  Q_ASSERT(glShader.bind());
}

void FigureViewer::checkUbo()
{
  if(!glUbo.isCreated())
    {
      QVector3D camPos(0,0,-camDistance);
      QQuaternion q = QQuaternion::fromEulerAngles(camAngleX, camAngleY, 0);
      camPos = q.rotatedVector(camPos);
      QVector3D up = q.rotatedVector(QVector3D(0,1,0));

      UboData data;
      data.mvpMat = glm::perspectiveFov<float>(glm::radians(90.0f), tex.textureSize().width(), tex.textureSize().height(), 0.1, 100.0)
          * glm::lookAt(glm::vec3(camPos.x(),camPos.y(),camPos.z()), glm::vec3(0,0,0), glm::vec3(up.x(),up.y(),up.z()));
      data.camPos = glm::vec4(camPos.x(),camPos.y(),camPos.z(), randomSeed);
      data.scale.x = getScaleX();
      data.scale.y = getScaleY();
      data.scale.z = getScaleZ();

      glUbo.create();
      glUbo.bind();
      glUbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
      glUbo.allocate(&data, sizeof(data));
      scaleDirty = camDirty = false;
    }
  if (scaleDirty)
    {
      glUbo.bind();
      glUbo.write(offsetof(UboData, scale), &scale, sizeof(QVector3D));
      scaleDirty = false;
    }
  if (camDirty)
    {
      QVector3D camPos(0,0,-camDistance);
      QQuaternion q = QQuaternion::fromEulerAngles(camAngleX, camAngleY, 0);
      camPos = q.rotatedVector(camPos);
      QVector3D up = q.rotatedVector(QVector3D(0,1,0));

      UboData data;
      data.mvpMat = glm::perspectiveFov<float>(glm::radians(90.0f), tex.textureSize().width(), tex.textureSize().height(), 0.1, 100.0)
          * glm::lookAt(glm::vec3(camPos.x(),camPos.y(),camPos.z()), glm::vec3(0,0,0), glm::vec3(up.x(),up.y(),up.z()));
      data.camPos = glm::vec4(camPos.x(),camPos.y(),camPos.z(), randomSeed);
      glUbo.bind();
      glUbo.write(0, &data, sizeof(glm::mat4) + sizeof(glm::vec4));
      camDirty = false;
    }
  ctx->versionFunctions<QOpenGLFunctions_3_3_Core>()->glBindBufferBase(GL_UNIFORM_BUFFER, 0, glUbo.bufferId());
}

void FigureViewer::checkGeometry()
{
  if (!glVao.isCreated())
    glVao.create();

  if (!glVbo.isCreated())
    {
      glVbo.create();
      geometryDirty = true;
    }

  if (!glIbo.isCreated())
    {
      glIbo.create();
      geometryDirty = true;
    }

  if (geometryDirty)
    switch (figureType)
      {
      case FigureData::Type::Box:
        loadCubeVbo();
        break;

      case FigureData::Type::Sphere:
        loadSphereVbo();
        break;

      case FigureData::Type::Torus:
        loadTorusVbo();
        break;
      }
  glVao.bind();
  glVbo.bind();
  glIbo.bind();
  ctx->functions()->glEnableVertexAttribArray(0);
  ctx->functions()->glVertexAttribPointer(0, 4, GL_FLOAT, false, sizeof(vertex), nullptr);

  ctx->functions()->glEnableVertexAttribArray(1);
  ctx->functions()->glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), (const std::byte*) sizeof(QVector4D)+sizeof(QVector3D));
}

void FigureViewer::loadCubeVbo()
{
  float size = 1;
#include "models/cube.h"
  glVbo.bind();
  glVbo.allocate(vertices, vcount*sizeof(vertex));
  glIbo.bind();
  glIbo.allocate(indices, icount*sizeof(quint32));
  trianglesCount = icount/3;
  emit trianglesCountChanged();
}

void FigureViewer::loadSphereVbo()
{
  float size = 1;
#include "models/sphere.h"
  glVbo.bind();
  glVbo.allocate(vertices, vcount*sizeof(vertex));
  glIbo.bind();
  glIbo.allocate(indices, icount*sizeof(quint32));
  trianglesCount = icount/3;
  emit trianglesCountChanged();
}

void FigureViewer::loadTorusVbo()
{
  float size = 1;
#include "models/torus.h"
  glVbo.bind();
  glVbo.allocate(vertices, vcount*sizeof(vertex));
  glIbo.bind();
  glIbo.allocate(indices, icount*sizeof(quint32));
  trianglesCount = icount/3;
  emit trianglesCountChanged();
}

void FigureViewer::renderToTexture()
{
  tex.bindFbo();
  checkShader();
  checkUbo();
  checkGeometry();

  ctx->functions()->glClearColor(0,0,0,0);
  ctx->functions()->glViewport(0,0, tex.textureSize().width(), tex.textureSize().height());
  ctx->functions()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  ctx->versionFunctions<QOpenGLFunctions_3_3_Core>()->glDrawElementsInstanced(GL_TRIANGLES, 3*trianglesCount, GL_UNSIGNED_INT, nullptr, figuresCount);

  glVao.release();
  glShader.release();
  tex.releaseFbo();
}
