#include "FigureData.hpp"

FigureData::FigureData(QObject *parent)
  : QObject(parent),
    type(Type::Sphere)
{
}

FigureData::FigureData(Type type, int count, const QVector3D &scale):
  scale(scale),
  count(count),
  type(type)
{
}

float FigureData::getScaleX() const
{
  return getScale().x();
}

void FigureData::setScaleX(float newScaleX)
{
  if (qFuzzyCompare(getScaleX(), newScaleX))
    return;
  scale.setX(newScaleX);
  emit scaleChanged();
}

float FigureData::getScaleY() const
{
  return getScale().y();
}

void FigureData::setScaleY(float newScaleY)
{
  if (qFuzzyCompare(getScaleY(), newScaleY))
    return;
  scale.setY(newScaleY);
  emit scaleChanged();
}

float FigureData::getScaleZ() const
{
  return getScale().z();
}

void FigureData::setScaleZ(float newScaleZ)
{
  if (qFuzzyCompare(getScaleZ(), newScaleZ))
    return;
  scale.setZ(newScaleZ);
  emit scaleChanged();
}

const QVector3D &FigureData::getScale() const
{
  return scale;
}

void FigureData::setScale(const QVector3D &newScale)
{
  if (scale == newScale)
    return;
  scale = newScale;
  emit scaleChanged();
}

FigureData::Type FigureData::getType() const
{
  return type;
}

void FigureData::setType(Type newType)
{
  if (type == newType)
    return;
  type = newType;
  emit typeChanged();
}

int FigureData::getCount() const
{
  return count;
}

void FigureData::setCount(int newCount)
{
  if (count == newCount)
    return;
  count = newCount;
  emit countChanged();
}
