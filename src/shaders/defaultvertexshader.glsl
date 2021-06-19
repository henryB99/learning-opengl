#version 330 core

layout (location = 0) in vec2 my_pos;
layout (location = 1) in vec2 my_uv;
layout (location = 2) in vec4 my_col;

uniform mat4 our_proj;

out vec4 frag_col;
out vec2 frag_uv;

void main() {
    frag_col = my_col;
    frag_uv = my_uv;

    gl_Position = our_proj * vec4(my_pos.xy, 0, 1);
}
