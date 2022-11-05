#ifndef VERTEX_H
#define VERTEX_H

#include <QVector4D>
#include <QVector3D>
#include <QVector2D>
#include <QVector>

struct vertex {
  float position[4];
  float tex_coord[2];
  float normal[3];
};

template<typename T, size_t N>
auto qVec(const T (&v)[N]) noexcept
{
  if constexpr(N == 1)
  {
    return static_cast<float>(v[0]);
  } else
  if constexpr(N == 2)
  {
    return QVector2D(v[0], v[1]);
  } else
  if constexpr(N == 3)
  {
    return QVector3D(v[0], v[1], v[2]);
  } else
  if constexpr(N == 4)
  {
    return QVector4D(v[0], v[1], v[2], v[3]);
  } else
  {
    QVector<float> result(N);
    for(size_t i = 0; i < N; ++i)
      result[i] = v[i];
    return result;
  }
}

#endif //VERTEX_H
