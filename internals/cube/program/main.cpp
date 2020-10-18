#include "gl_v1/gl.hpp"

#include "math3d_v1/mtx.hpp"
#include "math3d_v1/transform.hpp"
#include "math3d_v1/vec.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdio>

#include <utility>

using namespace math3d_v1;
using namespace data_v1;
namespace gl = gl_v1;

struct vertex {
  vec<GLfloat, 3> position;
  vec<GLfloat, 3> normal;
};

static const vertex vertices[] = {
    {{-1, -1, -1}, {0, 0, -1}}, {{+1, -1, -1}, {0, 0, -1}},
    {{-1, +1, -1}, {0, 0, -1}}, {{+1, +1, -1}, {0, 0, -1}},

    {{-1, -1, +1}, {0, 0, +1}}, {{+1, -1, +1}, {0, 0, +1}},
    {{-1, +1, +1}, {0, 0, +1}}, {{+1, +1, +1}, {0, 0, +1}},

    {{+1, -1, -1}, {+1, 0, 0}}, {{+1, -1, +1}, {+1, 0, 0}},
    {{+1, +1, -1}, {+1, 0, 0}}, {{+1, +1, +1}, {+1, 0, 0}},

    {{-1, -1, +1}, {-1, 0, 0}}, {{-1, -1, -1}, {-1, 0, 0}},
    {{-1, +1, +1}, {-1, 0, 0}}, {{-1, +1, -1}, {-1, 0, 0}},

    {{-1, +1, -1}, {0, +1, 0}}, {{+1, +1, -1}, {0, +1, 0}},
    {{-1, +1, +1}, {0, +1, 0}}, {{+1, +1, +1}, {0, +1, 0}},

    {{-1, -1, +1}, {0, -1, 0}}, {{+1, -1, +1}, {0, -1, 0}},
    {{-1, -1, -1}, {0, -1, 0}}, {{+1, -1, -1}, {0, -1, 0}}};

static const GLuint indices[][2][3] = {{{2, 1, 0}, {1, 2, 3}},
                                       {{4, 5, 6}, {6, 5, 7}},
                                       {{10, 9, 8}, {9, 10, 11}},
                                       {{14, 13, 12}, {13, 14, 15}},
                                       {{18, 17, 16}, {17, 18, 19}},
                                       {{22, 21, 20}, {21, 22, 23}}};

struct context {
  gl::VertexArray vertex_array;
  gl::Buffer vertex_buffer;
  gl::Buffer index_buffer;
  gl::Program program;
};

EM_BOOL animation_frame(double time, void *context_void) {
  context &c = *static_cast<context *>(context_void);

  gl::ClearColor(0, 0, 0, 1);
  gl::Clear(gl::COLOR_BUFFER_BIT);

  mtx<float, 4> view = make_translation(vec<float, 3>{0, 0, -5}) *
                       make_rotation(fmodf(time * 0.0009, pi * 2), 1) *
                       make_rotation(fmodf(time * -0.001, pi * 2), 0);

  gl::Uniform(gl::GetUniformLocation(c.program, "view"), view);
  gl::Uniform(gl::GetUniformLocation(c.program, "view_inv_trn"), view);

  mtx<float, 4> view_projection = make_projection<float>(45, 1, 9) * view;
  gl::Uniform(gl::GetUniformLocation(c.program, "view_projection"),
              view_projection);

  gl::DrawElements(gl::TRIANGLES,
                   sizeof(indices) / sizeof(indices[0][0][0]),
                   gl::TypeOf(indices[0][0]),
                   nullptr);

  return true;
}

int main() {
  printf("main()\n");

  gl::Init("canvas");

  gl::Viewport(0, 0, 400, 400);

  auto vertex_shader = gl::CompileShader(gl::VERTEX_SHADER, R"(#version 300 es
    in vec3 position;
    in vec3 normal;

    uniform mat4 view;
    uniform mat4 view_inv_trn;
    uniform mat4 view_projection;

    out vec3 v_normal;

    void main() {
      gl_Position = view_projection * vec4(position, 1);

      v_normal = (view_inv_trn * vec4(normal, 0)).xyz;
    })");
  auto fragment_shader =
      gl::CompileShader(gl::FRAGMENT_SHADER, R"(#version 300 es
    precision highp float;

    in vec3 v_normal;

    uniform vec3 color;

    out vec4 out_color;

    void main() {
      float intensity = max(dot(vec3(0, 0, 1), v_normal), 0.0);

      out_color = vec4(color * intensity, 1);
    })");

  auto program = gl::LinkProgram(vertex_shader, fragment_shader);

  auto vertex_array = gl::GenVertexArray();
  gl::BindVertexArray(vertex_array);

  auto vertex_buffer = gl::GenBuffer();
  gl::BindBuffer(gl::ARRAY_BUFFER, vertex_buffer);
  gl::BufferData(gl::ARRAY_BUFFER, vertices, gl::STATIC_DRAW);

  auto index_buffer = gl::GenBuffer();
  gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, index_buffer);
  gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, indices, gl::STATIC_DRAW);

  gl::UseProgram(program);

  auto position_loc = gl::GetAttribLocation(program, "position");
  gl::EnableVertexAttribArray(position_loc);
  gl::VertexAttribPointer(position_loc, &vertex::position);

  auto normal_loc = gl::GetAttribLocation(program, "normal");
  gl::EnableVertexAttribArray(normal_loc);
  gl::VertexAttribPointer(normal_loc, &vertex::normal);

  gl::Uniform(gl::GetUniformLocation(program, "color"), vec<float, 3>{1, 1, 1});

  gl::Enable(gl::CULL_FACE);

  context *c = new context{std::move(vertex_array),
                           std::move(vertex_buffer),
                           std::move(index_buffer),
                           std::move(program)};

  emscripten_request_animation_frame_loop(animation_frame, c);

  printf("main() -> 0\n");

  return 0;
}
