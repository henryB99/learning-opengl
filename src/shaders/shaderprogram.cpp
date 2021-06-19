#include "shaderprogram.hpp"

#include <iostream>

ShaderProgram::ShaderProgram(std::string name) :
    _id{},
    _vertex_shader_id{},
    _fragment_shader_id{},
    _name{std::move(name)} {

}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(_id);
}

void ShaderProgram::init(GLuint vertex_shader_id, GLuint fragment_shader_id) {
    _vertex_shader_id = vertex_shader_id;
    _fragment_shader_id = fragment_shader_id;

    _id = glCreateProgram();
    glAttachShader(_id, _vertex_shader_id);
    glAttachShader(_id, _fragment_shader_id);
    glLinkProgram(_id);

    GLint status;
    glGetProgramiv(_id, GL_LINK_STATUS, &status);

    if (status == GL_FALSE) {
        std::cerr << "Unable to link program " << _name << std::endl;

        GLint log_length;

        std::string log;

        glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &log_length);
        log.resize(log_length + 1);

        glGetProgramInfoLog(_id, log_length, nullptr, log.data());

        std::cerr << "Error: " << log << std::endl;
    }
}

void ShaderProgram::use() {
    glUseProgram(_id);
}

GLuint ShaderProgram::get_id() const {
    return _id;
}

GLuint ShaderProgram::get_vertex_shader_id() const {
    return _vertex_shader_id;
}

GLuint ShaderProgram::get_fragment_shader_id() const {
    return _fragment_shader_id;
}

std::string const& ShaderProgram::get_name() const {
    return _name;
}
