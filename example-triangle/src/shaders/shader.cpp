#include "shader.hpp"

#include <array>
#include <iostream>

static std::unordered_map<ShaderType, GLenum> shader_enums{
    {ShaderType::VertexShader, GL_VERTEX_SHADER},
    {ShaderType::FragmentShader, GL_FRAGMENT_SHADER},
};

Shader::Shader(ShaderType shader_type, std::string data, std::string name): _id{}, _shader_type{shader_type}, _data{std::move(data)}, _name{std::move(name)} { }

Shader::~Shader() {
    glDeleteShader(_id);
}

void Shader::init() {
    _id = glCreateShader(shader_enums[_shader_type]);

    std::array<GLint, 1> lengths{static_cast<GLint>(_data.size())};
    char const* content = _data.data();
    glShaderSource(_id, 1, &content, lengths.data());
    glCompileShader(_id);

    GLint status;
    glGetShaderiv(_id, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE) {
        std::cerr << "Unable to compile shader " << _name << std::endl;

        GLint log_length;

        std::string log;

        glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &log_length);
        log.resize(log_length + 1);

        glGetShaderInfoLog(_id, log_length, nullptr, log.data());

        std::cerr << "Error: " << log << std::endl;
    }
}

GLuint Shader::get_id() const {
    return _id;
}

ShaderType Shader::get_shader_type() const {
    return _shader_type;
}

std::string const& Shader::get_data() const {
    return _data;
}

std::string const& Shader::get_name() const {
    return _name;
}
