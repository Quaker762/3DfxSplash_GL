/** @file
 *
 *  Implementation of shader.hpp
 *
 *  @copyright 2016-2019 Palm Studios
 *
 *  @date 24-1-2019
 *
 *  @author Jesse Buhagiar
 */
#include "shader.h"

#include "log.hpp"

#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

CShader::CShader(const std::string& _name)
: programID(SHADER_RESET), locked(false), name(_name), attribs()
{
    load();
}

CShader::CShader(const std::string& _name, const std::vector<ShaderAttribute>& _attribs)
: programID(SHADER_RESET), locked(false), name(_name), attribs(_attribs)
{
    load();
}

CShader::~CShader()
{
    glDeleteProgram(programID);
}

void CShader::load(const std::string& _name)
{
    name = _name;
    load();
}

void CShader::load()
{
    std::ifstream sourceStream;  // Source file stream
    std::string fragPath;        // Asset path to fragment shader
    std::string vertPath;        // Asset path to vertex shader
    std::string fragSource = ""; // Fragment shader source code
    std::string vertSource = ""; // Vertex Shader source code
    std::string line;            // Temporary line variable
    GLuint fragShader;           // Fragment shader object
    GLuint vertShader;           // Vertex shader object
    GLint compStatus;            // Shader compilation status

    // Firstly, we need to read the source from the disk (as well as verify
    // that the files _actually_ exist on disk)
    vertPath = name + ".vert";
    fragPath = name + ".frag";

    // First, attempt to load and compile the vertex shader
    vertShader = glCreateShader(GL_VERTEX_SHADER);
    if(vertShader == SHADER_RESET)
    {
        log(LogLevel::ERROR, "glCreateShader(): Failed to generate vertex shader!");
        status = CShader::LoadStatus::OPENGL_ERROR;
        return;
    }

    sourceStream.open(vertPath);
    if(!sourceStream.is_open())
    {
        log(LogLevel::ERROR, "Failed to find vertex shader source. The file does not exist.");
        status = CShader::LoadStatus::IO_ERROR;
        return;
    }

    while(std::getline(sourceStream, line))
        vertSource += line + "\n";

    sourceStream.close();

    const GLchar* vs_src = vertSource.c_str();
    glShaderSource(vertShader, 1, &vs_src, nullptr);
    glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compStatus);
    if(!compStatus)
    {
        std::string compile_log;
        GLint logSize;

        glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logSize);
        compile_log.resize(logSize);
        compile_log.reserve(logSize);
        glGetShaderInfoLog(vertShader, logSize, &logSize, reinterpret_cast<GLchar*>(const_cast<char*>(compile_log.data())));

        glDeleteShader(vertShader);
        log(LogLevel::ERROR, "glCompileShader(): Failed to compile vertex shader!\n---------------------------------------------------\n%s", compile_log.c_str());

        status = CShader::LoadStatus::OPENGL_ERROR;
        return;
    }

    // Now, do the same for the fragment shader!
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    if(fragShader == SHADER_RESET)
    {
        log(LogLevel::ERROR, "glCreateShader(): Failed to generate fragment shader!");
        status = CShader::LoadStatus::OPENGL_ERROR;
        return;
    }

    sourceStream.open(fragPath);
    if(!sourceStream.is_open())
    {
        log(LogLevel::ERROR, "Failed to find fragment shader source. The file does not exist.");
        status = CShader::LoadStatus::IO_ERROR;
        return;
    }

    while(std::getline(sourceStream, line))
        fragSource += line + "\n";

    const GLchar* fs_src = fragSource.c_str();
    glShaderSource(fragShader, 1, &fs_src, nullptr);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compStatus);
    if(!compStatus)
    {
        std::string compile_log;
        GLint logSize;

        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logSize);
        compile_log.resize(logSize);
        compile_log.reserve(logSize);
        glGetShaderInfoLog(fragShader, logSize, &logSize, reinterpret_cast<GLchar*>(const_cast<char*>(compile_log.data())));

        glDeleteShader(fragShader);
        log(LogLevel::ERROR, "glCompileShader(): Failed to compile fragment shader!\n-------------------------------------------------------------\n%s", compile_log.c_str());

        status = CShader::LoadStatus::OPENGL_ERROR;
        return;
    }

    // We've made it this far now, let's actually create our OpenGL program
    programID = glCreateProgram();
    if(programID == 0)
    {
        log(LogLevel::ERROR, "glCreateProgram(): Unable to create program!");
        status = CShader::LoadStatus::OPENGL_ERROR;
        return;
    }

    glAttachShader(programID, vertShader);
    glAttachShader(programID, fragShader);

    // Now, let's bind our attribute locations (if we have any, that is!)
    if(attribs.size() != 0)
    {
        for(ShaderAttribute& attrib : attribs)
        {
            glBindAttribLocation(programID, attrib.loc, attrib.name.c_str());
        }
    }

    glLinkProgram(programID);

    // Let's check to make sure the progrma linked correctly!
    GLint linkStatus;
    glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);
    if(linkStatus == GL_FALSE)
    {
        std::string compile_log;
        GLint logSize;

        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logSize);
        compile_log.resize(logSize);
        compile_log.reserve(logSize);
        glGetProgramInfoLog(programID, logSize, &logSize, reinterpret_cast<GLchar*>(const_cast<char*>(compile_log.data())));

        glDeleteProgram(programID);
        log(LogLevel::ERROR, "glLinkProgram(): Failed to link shader! Reason: %s\n", compile_log.c_str());

        status = CShader::LoadStatus::OPENGL_ERROR;
        return;
    }

    glDetachShader(programID, vertShader);
    glDetachShader(programID, fragShader);
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    status = CShader::LoadStatus::SUCCESS;
}

void CShader::bind() noexcept
{
    if(programID != SHADER_RESET)
    {
        locked = true;
        glUseProgram(programID);
    }
}

void CShader::unbind() noexcept
{
    locked = false;
    glUseProgram(SHADER_RESET);
}

GLint CShader::get_uniform_loc(const std::string& name) const
{
    GLint ret;
    const GLchar* str = name.c_str();

    if(!locked)
        log(LogLevel::WARN, "Shader %s not locked! It is impossible to set a uniform!\n", this->name.c_str());

    ret = glGetUniformLocation(programID, str);
    if(ret == -1)
    {
        log(LogLevel::ERROR, "Shader %s: Unable to find uniform %s!\n", this->name.c_str(), name.c_str());
    }

    return ret;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<>
GLint CShader::set_uniform(const std::string& name, GLfloat f) const
{
    GLint loc = get_uniform_loc(name);

    glUniform1f(loc, f);
    return loc;
}

template<>
GLint CShader::set_uniform(const std::string& name, GLfloat f1, GLfloat f2) const
{
    GLint loc = get_uniform_loc(name);

    glUniform2f(loc, f1, f2);
    return loc;
}

template<>
GLint CShader::set_uniform(const std::string& name, GLfloat f1, GLfloat f2, GLfloat f3) const
{
    GLint loc = get_uniform_loc(name);

    glUniform3f(loc, f1, f2, f3);
    return loc;
}

template<>
GLint CShader::set_uniform(const std::string& name, GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4) const
{
    GLint loc = get_uniform_loc(name);

    glUniform4f(loc, f1, f2, f3, f4);
    return loc;
}

template<>
GLint CShader::set_uniform(const std::string& name, GLint i) const
{
    GLint loc = get_uniform_loc(name);

    glUniform1i(loc, i);
    return loc;
}

template<>
GLint CShader::set_uniform(const std::string& name, GLint i1, GLint i2) const
{
    GLint loc = get_uniform_loc(name);

    glUniform2i(loc, i1, i2);
    return loc;
}

template<>
GLint CShader::set_uniform(const std::string& name, GLint i1, GLint i2, GLint i3) const
{
    GLint loc = get_uniform_loc(name);

    glUniform3i(loc, i1, i2, i3);
    return loc;
}

template<>
GLint CShader::set_uniform(const std::string& name, GLint i1, GLint i2, GLint i3, GLint i4) const
{
    GLint loc = get_uniform_loc(name);

    glUniform4i(loc, i1, i2, i3, i4);
    return loc;
}

template<>
GLint CShader::set_uniform(const std::string& name, GLuint i) const
{
    GLint loc = get_uniform_loc(name);

    glUniform1ui(loc, i);
    return loc;
}

template<>
GLint CShader::set_uniform(const std::string& name, GLuint i1, GLuint i2) const
{
    GLint loc = get_uniform_loc(name);

    glUniform2ui(loc, i1, i2);
    return loc;
}

template<>
GLint CShader::set_uniform(const std::string& name, GLuint i1, GLuint i2, GLuint i3) const
{
    GLint loc = get_uniform_loc(name);

    glUniform3ui(loc, i1, i2, i3);
    return loc;
}

template<>
GLint CShader::set_uniform(const std::string& name, GLuint i1, GLuint i2, GLuint i3, GLuint i4) const
{
    GLint loc = get_uniform_loc(name);

    glUniform4ui(loc, i1, i2, i3, i4);
    return loc;
}

template<>
GLint CShader::set_uniform(const std::string& name, const glm::vec2& vec) const
{
    GLint loc = get_uniform_loc(name);

    glUniform2fv(loc, 1, &vec[0]);
    return loc;
}

template<>
GLint CShader::set_uniform(const std::string& name, const glm::vec3& vec) const
{
    GLint loc = get_uniform_loc(name);

    glUniform3fv(loc, 1, &vec[0]);
    return loc;
}

template<>
GLint CShader::set_uniform(const std::string& name, const glm::mat2& mat) const
{
    GLint loc = get_uniform_loc(name);

    glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
    return loc;
}

template<>
GLint CShader::set_uniform(const std::string& name, const glm::mat3& mat) const
{
    GLint loc = get_uniform_loc(name);

    glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
    return loc;
}

template<>
GLint CShader::set_uniform(const std::string& name, const glm::mat4& mat) const
{
    GLint loc = get_uniform_loc(name);

    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
    return loc;
}

///////////////////////////////////////////////////////////////
template<>
GLfloat CShader::get_uniform_value(const std::string& name) const
{
    GLint loc = get_uniform_loc(name);
    GLfloat ret;

    glGetUniformfv(programID, loc, &ret);
    return ret;
}

template<>
GLint CShader::get_uniform_value(const std::string& name) const
{
    GLint loc = get_uniform_loc(name);
    GLint ret;

    glGetUniformiv(programID, loc, &ret);
    return ret;
}

template<>
GLuint CShader::get_uniform_value(const std::string& name) const
{
    GLint loc = get_uniform_loc(name);
    GLuint ret;

    glGetUniformuiv(programID, loc, &ret);
    return ret;
}

template<>
GLdouble CShader::get_uniform_value(const std::string& name) const
{
    GLint loc = get_uniform_loc(name);
    GLdouble ret;

    glGetUniformdv(programID, loc, &ret);
    return ret;
}

//template<>
//glm::vec2 CShader::get_uniform_value(const std::string& name)
//{
//  GLint       loc = get_uniform_loc(name);
//  GLfloat     tmp[2];
//  glm::vec2   ret;
//
//  glGetUniformfv(programID, loc, tmp);
//  ret = glm::make_vec2(tmp);
//  return ret;
//}
//
//template<>
//glm::vec3 CShader::get_uniform_value(const std::string& name)
//{
//  GLint       loc = get_uniform_loc(name);
//  GLfloat     tmp[3];
//  glm::vec3   ret;
//
//  glGetUniformfv(programID, loc, &tmp);
//  ret = glm::make_vec3(tmp);
//  return ret;
//}
//
//template<>
//glm::mat2 CShader::get_uniform_value(const std::string& name)
//{
//  GLint       loc = get_uniform_loc(name);
//  GLfloat     tmp[4];
//  glm::mat2   ret;
//
//  glGetUniformfv(programID, loc, &tmp);
//  ret = glm::make_mat2(tmp);
//  return ret;
//}
//
//template<>
//glm::mat3 CShader::get_uniform_value(const std::string& name)
//{
//  GLint       loc = get_uniform_loc(name);
//  GLfloat     tmp[9];
//  glm::mat3   ret;
//
//  glGetUniformfv(programID, loc, &tmp);
//  ret = glm::make_mat3(tmp);
//  return ret;
//}
//
//template<>
//glm::mat4 CShader::get_uniform_value(const std::string& name)
//{
//  GLint       loc = get_uniform_loc(name);
//  GLfloat     tmp[16];
//  glm::mat4   ret;
//
//  glGetUniformfv(programID, loc, &tmp);
//  ret = glm::make_mat4(tmp);
//  return ret;
//}
