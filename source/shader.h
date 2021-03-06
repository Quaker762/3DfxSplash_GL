/** @file
 *
 *  Class and data structures pertaining to and OpenGL. Note that in this context,
 *  a "shader" refers to a complete OpenGL shader program containing both the vertex
 *  and fragment stage of the pipeline.
 *
 *  @copyright 2016-2019 Palm Studios
 *
 *  @date 23-1-2019
 *
 *  @author Jesse Buhagiar
 */

#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>

/**
 * Shader attribute structure.
 *
 * This struct defines a shader/attribute pair that can be specified at link time for the shader.
 * This way, it is possible to specify at what location we will bind specific buffers that the shader
 * can then read from. If the user does not choose this method of binding attributes, then
 * the attribute locations will be auto-generated by OpenGL, which can then be found via
 * @ref CShader::GetAttribLocation.
 */
struct ShaderAttribute final
{
    std::string name; /**< Name of this attribute */
    GLuint loc;       /**< Location we want to bind this attribute to */
};

/**
 * Shader class represting a complete OpenGL shader program
 */
class CShader final
{
public:
    /**
     * Shader Load Status Enumeration
     */
    enum class LoadStatus : GLint
    {
        SUCCESS = 0,
        IO_ERROR,
        COMPILE_ERROR,
        OPENGL_ERROR,
        LINK_ERROR
    };

    static constexpr GLuint SHADER_RESET = 0; /**< Reset Shader */

public:
    /**
     * Default constructor.
     */
    CShader()
    : programID(0x00), locked(false), name("UNDEFINED"), status(LoadStatus::COMPILE_ERROR), attribs() {}

    /**
     * Constructor
     *
     * Loads a shader source from the disk and compiles and links it.
     *
     * @param _name Name of the shader. Both .frag and .vert shaders <i>MUST</i> have the same name!
     */
    CShader(const std::string& _name);

    /**
     * Constructor
     *
     * Loads a shader from source, and binds an attribute/location pair before this program is linked.
     */
    CShader(const std::string& _name, const std::vector<ShaderAttribute>& _attribs);

    /**
     * Destructor
     *
     * Deletes the program object
     */
    ~CShader();

    /**
     * Binds this program as the current program in use by the current OpenGL context.
     *
     * @note This function does not throw!
     */
    void bind(void) noexcept;

    /**
     * Unbinds this program so that it is no longer in use by the current OpenGL context.
     *
     * Also 'unlocks' this program by setting @ref locked to false.
     *
     * @note This functino does not throw!
     */
    void unbind() noexcept;

    /**
     * Get the location of a Uniform Variable in our shader program.
     *
     * @param name The name of the uniform whose location we want to find
     *
     * @return Index of the program where this uniform is located. This value can be passed
     * to other OpenGL functions in this shader class to modify the contents of the uniform.
     *
     * @note A "uniform" is a global variables that may change per primitive [...],
     * that are passed from the OpenGL application to the shaders. This qualifier can be
     * used in both vertex and fragment shaders. For the shaders this is a read-only variable.
     * See <a href=http://www.lighthouse3d.com/tutorials/glsl-12-tutorial/uniform-variables/>here</a> for more information
     */
    inline GLint get_uniform_loc(const std::string& name) const;

    /**
     * Set a uniform in the shader given a data type, T
     *
     * @param name  Name of the uniform that we want to set in this shader.
     * @param T     Type of uniform. View the specific function for this.
     *
     * @return On success, will return the location of the given uniform.
     */
    template<typename... T>
    GLint set_uniform(const std::string& name, T... args) const;

    /**
     * Get a uniform value from the shader of typename T.
     * If getting the location of the uniform fails (i.e, the uniform does not exist), then a default value is returned.
     *
     * @param name  Name of the uniform whos value we
     * @param T     Type of uniform. View the specific function for this.
     *
     * @return On Success, will return the value of uniform @ref name (if it exists)
     *
     * @warning This is currently unimplemented.
     */
    template<typename T>
    T get_uniform_value(const std::string& name) const;

    /**
     * Bind an attribute name to an index.
     *
     * @param index Index we want to bind the attribute to
     * @param name Name of the attribute whose location we want to define.
     */
    void bind_attrib_location(GLuint index, const std::string& name) const;

    /**
     * Get the location of an attribute from this GL Shader Program
     *
     * @param name The name of attribute whose location we want to get.
     *
     * @return Attribute location of the attribute specified in @ref name
     */
    GLint get_attrib_location(const std::string& name) const { return glGetAttribLocation(programID, name.c_str()); };

    /**
     * Return whether or not this program is currently bound and in use.
     *
     * @return @ref locked
     */
    bool is_locked() const { return locked; }

    /**
     * Load the shader source from disk and compile it.
     *
     * The shader objects are then linked together to form our final Shader Program (stored in @ref program)
     *
     * @param name Name of the shader we want to load from /data/shaders/
     */
    void load(const std::string& name);

    /**
     * Get the load status of this shader.
     */
    LoadStatus load_status() const { return status; }

private:
    /**
     * Load the shader source from disk and compile it.
     *
     * The shader objects are then linked together to form our final Shader Program (stored in @ref program)
     */
    void load(void);

private:
    GLuint programID; /**< Program ID Generated for us by OpenGL */
    bool locked;      /**< Specifies whether or not this shader is 'locked' and currently in use */
    std::string name; /**< Name of this shader */
    LoadStatus status;
    std::vector<ShaderAttribute> attribs; /**< Local attributes list (probably not necessary)*/
};