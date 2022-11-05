#include "GlTextureWrapper.hpp"
#include <QOpenGLContext>
#include <QOpenGLFunctions>

GlTextureWrapper::GlTextureWrapper()
{
}

void GlTextureWrapper::setSize(int width, int height)
{
  if (!fbo || fbo->width() != width || fbo->height() != height)
    fbo = std::make_unique<QOpenGLFramebufferObject>(width, height, QOpenGLFramebufferObject::Depth);
}

void GlTextureWrapper::reset()
{
  fbo.reset();
}

bool GlTextureWrapper::initialized() const
{
  return fbo.get();
}

void GlTextureWrapper::bindFbo()
{
  fbo->bind();
}

void GlTextureWrapper::releaseFbo()
{
  fbo->release();
}

int GlTextureWrapper::textureId() const
{
  return fbo->texture();
}

QSize GlTextureWrapper::textureSize() const
{
  return fbo->size();
}

bool GlTextureWrapper::hasAlphaChannel() const
{
  return true;
}

bool GlTextureWrapper::hasMipmaps() const
{
  return false;
}

void GlTextureWrapper::bind()
{
  QOpenGLContext::currentContext()->functions()->glBindTexture(GL_TEXTURE_2D, fbo->texture());
}
