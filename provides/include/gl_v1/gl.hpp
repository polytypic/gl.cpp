#pragma once

#include "gl_v1/synopsis.hpp"

#include "math3d_v1/mtx.hpp"

#include "data_v1/strided.hpp"
#include "data_v1/struct.hpp"

#include <cstdio>
#include <cstdlib>
#include <type_traits>

template <> inline auto gl_v1::TypeOf<GLbyte>(const GLbyte *) { return BYTE; }
template <> inline auto gl_v1::TypeOf<GLfloat>(const GLfloat *) {
  return FLOAT;
}
template <> inline auto gl_v1::TypeOf<GLint>(const GLint *) { return INT; }
template <> inline auto gl_v1::TypeOf<GLshort>(const GLshort *) {
  return SHORT;
}
template <> inline auto gl_v1::TypeOf<GLubyte>(const GLubyte *) {
  return UNSIGNED_BYTE;
}
template <> inline auto gl_v1::TypeOf<GLuint>(const GLuint *) {
  return UNSIGNED_INT;
}
template <> inline auto gl_v1::TypeOf<GLushort>(const GLushort *) {
  return UNSIGNED_SHORT;
}

//

template <class Deleter, Deleter deleter>
gl_v1::Resource<Deleter, deleter>::~Resource<Deleter, deleter>() {
  reset();
}

template <class Deleter, Deleter deleter>
gl_v1::Resource<Deleter, deleter>::Resource() : name(0) {}

template <class Deleter, Deleter deleter>
gl_v1::Resource<Deleter, deleter>::Resource(GLuint name) : name(name) {}

template <class Deleter, Deleter deleter>
gl_v1::Resource<Deleter, deleter>::Resource(Resource &&that) {
  name = that.name;
  that.name = 0;
}

template <class Deleter, Deleter deleter>
auto &gl_v1::Resource<Deleter, deleter>::operator=(Resource &&that) {
  if (this != &that) {
    reset(that.name);
    that.name = 0;
  }
  return *this;
}

template <class Deleter, Deleter deleter>
auto gl_v1::Resource<Deleter, deleter>::reset(GLuint new_name) {
  if constexpr (std::is_same_v<Deleter, Deleter1>) {
    deleter(name);
  } else {
    deleter(1, &name);
  }
  name = new_name;
}

//

template <class T, size_t N>
auto gl_v1::BufferData(Target target, const T (&data)[N], Usage usage) {
  BufferData(target, sizeof(T) * N, data, usage);
}

template <class T, size_t N>
auto gl_v1::BufferData(Target target,
                       const contiguous<T, N> &data,
                       Usage usage) {
  BufferData(target, sizeof(T) * data.size(), &data[0], usage);
}

//

template <class Vertex, class S, size_t N>
auto gl_v1::VertexAttribPointer(Location index,
                                vec<S, N>(Vertex::*member),
                                GLboolean normalized) {
  auto pointer = pointer_of(member);
  auto size = N;
  auto type = TypeOf(pointer->values);
  auto stride = sizeof(Vertex);
  VertexAttribPointer(index, size, type, normalized, stride, pointer);
}

//

inline auto gl_v1::ClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
  glClearColor(r, g, b, a);
}
inline auto gl_v1::operator|(Mask lhs, Mask rhs) { return Mask(+lhs | +rhs); }
