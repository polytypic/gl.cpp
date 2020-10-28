#include "gl_v1/gl.hpp"

#include "math3d_v1/mtx.hpp"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstring>
#include <tuple>

EMSCRIPTEN_WEBGL_CONTEXT_HANDLE gl_v1::Init(const char *target) {
  EmscriptenWebGLContextAttributes attrs;
  emscripten_webgl_init_context_attributes(&attrs);
  attrs.majorVersion = 3;
  attrs.minorVersion = 0;

  auto context = emscripten_webgl_create_context(target, &attrs);
  if (context < 0) {
    printf("emscripten_webgl_create_context: %d\n", context);
    exit(1);
  }

  EMSCRIPTEN_RESULT result = emscripten_webgl_make_context_current(context);
  if (result != EMSCRIPTEN_RESULT_SUCCESS) {
    printf("emscripten_webgl_make_context_current: %d\n", result);
    exit(1);
  }

  return context;
}

//

static const std::array<std::tuple<GLenum, const char *>, 6> s_errors = {
    {{GL_NO_ERROR, "GL_NO_ERROR"},
     {GL_INVALID_ENUM, "GL_INVALID_ENUM"},
     {GL_INVALID_VALUE, "GL_INVALID_VALUE"},
     {GL_INVALID_OPERATION, "GL_INVALID_OPERATION"},
     {GL_OUT_OF_MEMORY, "GL_OUT_OF_MEMORY"},
     {GL_INVALID_FRAMEBUFFER_OPERATION, "GL_INVALID_FRAMEBUFFER_OPERATION"}}};

static const char *ErrorString(GLenum error_code) {
  auto i = std::find_if(s_errors.begin(), s_errors.end(), [=](const auto &t) {
    return std::get<0>(t) == error_code;
  });
  return i != s_errors.end() ? std::get<1>(*i) : "Unknown error?";
}

[[noreturn]] static void GetErrorAndExit(const char *fn) {
  auto error_code = glGetError();
  auto error_name = ErrorString(error_code);
  printf("%s: %s\n", fn, error_name);
  exit(1);
}

#ifdef NDEBUG
static inline void CheckError(const char *) {}
#else
static void CheckError(const char *fn) {
  auto error_code = glGetError();
  if (error_code != GL_NO_ERROR) {
    GetErrorAndExit(fn);
  }
}
#endif

//

gl_v1::Shader gl_v1::CreateShader(ShaderType type) {
  GLuint shader = glCreateShader(type);
  if (!shader)
    GetErrorAndExit("glCreateShader");
  return Shader(shader);
}

void gl_v1::ShaderSource(const Shader &shader, const GLchar *source) {
  GLint length = strlen(source);
  glShaderSource(shader.name, 1, &source, &length);
  CheckError("glShaderSource");
}

void gl_v1::CompileShader(const Shader &shader) {
  glCompileShader(shader.name);
  CheckError("glCompileShader");

  GLint result = GL_FALSE;
  glGetShaderiv(shader.name, GL_COMPILE_STATUS, &result);
  CheckError("glGetShaderiv");

  if (result != GL_TRUE) {
    char info[1024];
    glGetShaderInfoLog(shader.name, sizeof(info), nullptr, info);
    printf("glCompileShader: %s\n", info);
    exit(1);
  }
}

gl_v1::Shader gl_v1::CompileShader(ShaderType type, const char *source) {
  auto shader = CreateShader(type);
  ShaderSource(shader, source);
  CompileShader(shader);
  return shader;
}

//

gl_v1::Program gl_v1::CreateProgram() {
  GLuint program = glCreateProgram();
  if (!program)
    GetErrorAndExit("glCreateProgram");
  return Program(program);
}

void gl_v1::AttachShader(const Program &program, const Shader &shader) {
  glAttachShader(program.name, shader.name);
  CheckError("glAttachShader");
}

void gl_v1::LinkProgram(const Program &program) {
  glLinkProgram(program.name);
  CheckError("glLinkProgram");

  GLint result = GL_FALSE;
  glGetProgramiv(program.name, GL_LINK_STATUS, &result);
  CheckError("glGetProgramiv");

  if (result != GL_TRUE) {
    char info[1024];
    glGetProgramInfoLog(program.name, sizeof(info), nullptr, info);
    printf("glLinkProgram: %s\n", info);
    exit(1);
  }
}

gl_v1::Program gl_v1::LinkProgram(const Shader &vertexShader,
                                  const Shader &fragmentShader) {
  auto program = CreateProgram();
  AttachShader(program, vertexShader);
  AttachShader(program, fragmentShader);
  LinkProgram(program);
  return program;
}

gl_v1::Location gl_v1::GetAttribLocation(const Program &program,
                                         const GLchar *name) {
  auto result = glGetAttribLocation(program.name, name);
  CheckError("glGetAttribLocation");
  return Location(result);
}

gl_v1::Location gl_v1::GetUniformLocation(const Program &program,
                                          const GLchar *name) {
  auto result = glGetUniformLocation(program.name, name);
  CheckError("glGetUniformLocation");
  return Location(result);
}

void gl_v1::UseProgram(const Program &program) {
  glUseProgram(program.name);
  CheckError("glUseProgram");
}

//

gl_v1::Buffer gl_v1::GenBuffer() {
  GLuint result;
  glGenBuffers(1, &result);
  CheckError("glGenBuffers");
  return Buffer(result);
}

void gl_v1::BindBuffer(Target target, const Buffer &buffer) {
  glBindBuffer(target, buffer.name);
  CheckError("glBindBuffer");
}

void gl_v1::BufferData(Target target,
                       GLsizeiptr size,
                       const void *data,
                       Usage usage) {
  glBufferData(target, size, data, usage);
  CheckError("glBufferData");
}

//

gl_v1::VertexArray gl_v1::GenVertexArray() {
  GLuint result;
  glGenVertexArrays(1, &result);
  CheckError("glGenVertexArrays");
  return VertexArray(result);
}

void gl_v1::BindVertexArray(const VertexArray &array) {
  glBindVertexArray(array.name);
  CheckError("glBindVertexArray");
}

void gl_v1::EnableVertexAttribArray(GLuint index) {
  glEnableVertexAttribArray(index);
  CheckError("glBindVertexArray");
}

void gl_v1::VertexAttribPointer(Location index,
                                GLint size,
                                Type type,
                                GLboolean normalized,
                                GLsizei stride,
                                const void *pointer) {
  glVertexAttribPointer(index, size, type, normalized, stride, pointer);
  CheckError("glVertexAttribPointer");
}

//

void gl_v1::Viewport(GLint x, GLint y, GLsizei width, GLsizei height) {
  glViewport(x, y, width, height);
  CheckError("glViewPort");
}

void gl_v1::Viewport(const vec_t<GLint, 2> &lower_left,
                     const vec_t<GLsizei, 2> &extent) {
  Viewport(lower_left[0], lower_left[1], extent[0], extent[1]);
}

void gl_v1::Viewport(const vec_t<GLsizei, 2> &extent) {
  Viewport(zero_vec<GLint, 2>(), extent);
}

void gl_v1::Clear(Mask mask) {
  glClear(mask);
  CheckError("glClear");
}

//

void gl_v1::DrawArrays(Mode mode, GLint first, GLsizei count) {
  glDrawArrays(mode, first, count);
  CheckError("glDrawArrays");
}

void gl_v1::DrawElements(Mode mode,
                         GLsizei count,
                         Type type,
                         const void *indices) {
  glDrawElements(mode, count, type, indices);
  CheckError("glDrawElements");
}

//

void gl_v1::Uniform(GLint location,
                    const mtx_t<float, 4> &matrix,
                    bool transpose) {
  glUniformMatrix4fv(location, 1, transpose, matrix.values[0]);
  CheckError("glUniformMatrix4fv");
}

void gl_v1::Uniform(GLint location, const vec_t<GLfloat, 3> &vec) {
  glUniform3fv(location, 1, vec.values);
  CheckError("glUniform3fv");
}

void gl_v1::Uniform(GLint location, const vec_t<GLfloat, 4> &vec) {
  glUniform4fv(location, 1, vec.values);
  CheckError("glUniform4fv");
}

//

void gl_v1::Enable(Capability capability) {
  glEnable(capability);
  CheckError("glEnable");
}
