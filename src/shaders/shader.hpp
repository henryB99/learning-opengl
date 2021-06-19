#pragma once

#include <string>
#include <unordered_map>

#include <GL/glew.h>

#include <util/base.hpp>

enum class ShaderType {
    VertexShader,
    FragmentShader,
};

class Shader {
public:
    Shader() = default;
    Shader(ShaderType shader_type, std::string data, std::string name);
    ~Shader();

public:
    void init();

public:
    NODISCARD GLuint get_id() const;
    NODISCARD ShaderType get_shader_type() const;
    NODISCARD std::string const& get_data() const;
    NODISCARD std::string const& get_name() const;

private:
    GLuint _id;
    ShaderType _shader_type;
    std::string _data;
    std::string _name;
};
