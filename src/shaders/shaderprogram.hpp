#pragma once

#include <string>

#include <GL/glew.h>

#include <util/base.hpp>

class ShaderProgram {
public:
    explicit ShaderProgram(std::string name);
    ~ShaderProgram();

public:
    void init(GLuint vertex_shader_id, GLuint fragment_shader_id);
    void use();

public:
    NODISCARD GLuint get_id() const;
    NODISCARD GLuint get_vertex_shader_id() const;
    NODISCARD GLuint get_fragment_shader_id() const;
    NODISCARD std::string const& get_name() const;

private:
    GLuint _id;
    GLuint _vertex_shader_id;
    GLuint _fragment_shader_id;
    std::string _name;
};
