#ifndef GLTEXTUREWRAPPER_HPP
#define GLTEXTUREWRAPPER_HPP

#include <QSGTexture>
#include <QOpenGLFramebufferObject>
#include <memory>

class GlTextureWrapper : public QSGTexture
{
public:
  GlTextureWrapper();

  void setSize(int width, int height);
  void reset();
  bool initialized() const;

  void bindFbo();
  void releaseFbo();

  // QSGTexture interface
public:
  int textureId() const override final;
  QSize textureSize() const override final;
  bool hasAlphaChannel() const override final;
  bool hasMipmaps() const override final;
  void bind() override final;

private:
  std::unique_ptr<QOpenGLFramebufferObject> fbo;
};

#endif // GLTEXTUREWRAPPER_HPP
