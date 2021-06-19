#version 330 core

in vec2 frag_uv;
in vec4 frag_col;

uniform sampler2D our_tex;

out vec4 out_col;

void main() {
   out_col = frag_col;
}
