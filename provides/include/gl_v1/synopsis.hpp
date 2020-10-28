#pragma once

#include "gl_v1/config.hpp"

#include "data_v1/synopsis.hpp"
#include "math3d_v1/synopsis.hpp"

#include <cstdint>

#include <emscripten/html5.h>

#include <GLES3/gl3.h>

namespace gl_v1 {

enum Type : GLenum {
  BYTE = GL_BYTE,
  FIXED = GL_FIXED,
  FLOAT = GL_FLOAT,
  HALF_FLOAT = GL_HALF_FLOAT,
  INT = GL_INT,
  INT_2_10_10_10_REV = GL_INT_2_10_10_10_REV,
  SHORT = GL_SHORT,
  UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
  UNSIGNED_INT = GL_UNSIGNED_INT,
  UNSIGNED_INT_2_10_10_10_REV = GL_UNSIGNED_INT_2_10_10_10_REV,
  UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
};
template <class T> auto TypeOf(const T *value = nullptr);

//

EMSCRIPTEN_WEBGL_CONTEXT_HANDLE Init(const char *target);

//

using Deleter1 = void (*)(GLuint);
using DeleterN = void (*)(GLsizei, const GLuint *);

template <class Deleter, Deleter deleter> struct Resource {
  GLuint name;

  ~Resource();

  Resource();
  explicit Resource(GLuint name);
  Resource(Resource &&that);

  auto &operator=(Resource &&that);

  auto reset(GLuint new_name = 0);
};

//

using Shader = Resource<Deleter1, glDeleteShader>;
enum ShaderType : GLenum {
  VERTEX_SHADER = GL_VERTEX_SHADER,
  FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
};
Shader CreateShader(ShaderType type);
void ShaderSource(const Shader &shader, const GLchar *source);
void CompileShader(const Shader &shader);
Shader CompileShader(ShaderType type, const GLchar *source);

//

using Program = Resource<Deleter1, glDeleteProgram>;
Program CreateProgram();
void AttachShader(const Program &program, const Shader &shader);
void LinkProgram(const Program &program);
Program LinkProgram(const Shader &vertexShader, const Shader &fragmentShader);

enum Location : GLint {};
Location GetAttribLocation(const Program &program, const GLchar *name);
Location GetUniformLocation(const Program &program, const GLchar *name);
void UseProgram(const Program &program);

//

using Buffer = Resource<DeleterN, glDeleteBuffers>;
Buffer GenBuffer();
enum Target : GLenum {
  ARRAY_BUFFER = GL_ARRAY_BUFFER,
  COPY_READ_BUFFER = GL_COPY_READ_BUFFER,
  COPY_WRITE_BUFFER = GL_COPY_WRITE_BUFFER,
  ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER,
  PIXEL_PACK_BUFFER = GL_PIXEL_PACK_BUFFER,
  PIXEL_UNPACK_BUFFER = GL_PIXEL_UNPACK_BUFFER,
  TRANSFORM_FEEDBACK_BUFFER = GL_TRANSFORM_FEEDBACK_BUFFER,
  UNIFORM_BUFFER = GL_UNIFORM_BUFFER,
};
void BindBuffer(Target target, const Buffer &buffer);
enum Usage : GLenum {
  DYNAMIC_COPY = GL_DYNAMIC_COPY,
  DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
  DYNAMIC_READ = GL_DYNAMIC_READ,
  STATIC_COPY = GL_STATIC_COPY,
  STATIC_DRAW = GL_STATIC_DRAW,
  STATIC_READ = GL_STATIC_READ,
  STREAM_COPY = GL_STREAM_COPY,
  STREAM_DRAW = GL_STREAM_DRAW,
  STREAM_READ = GL_STREAM_READ,
};
void BufferData(Target target, GLsizeiptr size, const void *data, Usage usage);
template <class T, size_t N>
auto BufferData(Target target, const T (&data)[N], Usage usage);
template <class T, size_t N>
auto BufferData(Target target, const contiguous_t<T, N> &data, Usage usage);

//

using VertexArray = Resource<DeleterN, glDeleteVertexArrays>;
VertexArray GenVertexArray();
void BindVertexArray(const VertexArray &array);
void EnableVertexAttribArray(GLuint index);
void VertexAttribPointer(Location index,
                         GLint size,
                         Type type,
                         GLboolean normalized,
                         GLsizei stride,
                         const void *pointer);
template <class Vertex, class S, size_t N>
auto VertexAttribPointer(Location index,
                         vec_t<S, N>(Vertex::*member),
                         GLboolean normalized = false);

//

void Viewport(GLint x, GLint y, GLsizei width, GLsizei height);
void Viewport(const vec_t<GLint, 2> &lower_left,
              const vec_t<GLsizei, 2> &extent);
void Viewport(const vec_t<GLsizei, 2> &extent);

//

auto ClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
enum Mask : GLbitfield {
  COLOR_BUFFER_BIT = GL_COLOR_BUFFER_BIT,
  DEPTH_BUFFER_BIT = GL_DEPTH_BUFFER_BIT,
  STENCIL_BUFFER_BIT = GL_STENCIL_BUFFER_BIT,
};
auto operator|(Mask lhs, Mask rhs);
void Clear(Mask mask);

//

enum Mode : GLenum {
  LINES = GL_LINES,
  LINE_LOOP = GL_LINE_LOOP,
  LINE_STRIP = GL_LINE_STRIP,
  POINTS = GL_POINTS,
  TRIANGLES = GL_TRIANGLES,
  TRIANGLE_FAN = GL_TRIANGLE_FAN,
  TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
};
void DrawArrays(Mode mode, GLint first, GLsizei count);
void DrawElements(Mode mode, GLsizei count, Type type, const void *indices);

//

void Uniform(GLint location,
             const mtx_t<float, 4> &matrix,
             bool transpose = true);

void Uniform(GLint location, const vec_t<GLfloat, 3> &vec);
void Uniform(GLint location, const vec_t<GLfloat, 4> &vec);

//

enum Capability : GLenum {
  BLEND = GL_BLEND,
  CULL_FACE = GL_CULL_FACE,
  DEPTH_TEST = GL_DEPTH_TEST,
  DITHER = GL_DITHER,
  POLYGON_OFFSET_FILL = GL_POLYGON_OFFSET_FILL,
  PRIMITIVE_RESTART_FIXED_INDEX = GL_PRIMITIVE_RESTART_FIXED_INDEX,
  RASTERIZER_DISCARD = GL_RASTERIZER_DISCARD,
  SAMPLE_ALPHA_TO_COVERAGE = GL_SAMPLE_ALPHA_TO_COVERAGE,
  SAMPLE_COVERAGE = GL_SAMPLE_COVERAGE,
  SCISSOR_TEST = GL_SCISSOR_TEST,
  STENCIL_TEST = GL_STENCIL_TEST,
};
void Enable(Capability capability);

} // namespace gl_v1
