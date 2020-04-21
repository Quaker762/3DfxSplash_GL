/**
 * 
 * 
 */
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL.h>
#include <vector>
#include "3dftex.h"
#include "log.hpp"
#include "shader.h"
#include "types.h"

#define VERTEX_ATTRIB 0
#define NORMAL_ATTRIB 1
#define ST_ATTRIB 2
#define COLOR_ATTRIB 3
#define MATERIAL_NUMBER_ATTRIB 4

#define SHIELD_INDEX_WHITE 0
#define LOGO_INDEX 1
#define SHIELD_INDEX_CYAN 2

#define NUM_LIGHTS 3


static constexpr GLsizei scr_width = 640;
static constexpr GLsizei scr_height = 480;

static GLuint logo_vao, logo_vbo, logo_color_buffer, logo_material_buffer, logo_ibo;
static GLuint shield_cyan_vao, shield_cyan_vbo, shield_cyan_color_buffer, shield_cyan_material_buffer, shield_cyan_ibo;
static GLuint shield_white_vao, shield_white_vbo, shield_white_color_buffer, shield_white_material_buffer, shield_white_ibo;

static GLuint light_vao, light_vbo;

static Texture logo_3d_texture;
static Texture specular_texture;
static Texture shadow_texture;

GLsizei logo_index_count;
GLsizei shield_cyan_index_count;
GLsizei shield_white_index_count;

#include "splashdat.cpp"

glm::mat4 projection;
glm::mat4 view;
glm::mat4 model;
glm::mat4 mvp;

// Lighting
std::vector<glm::vec3> materials;
glm::vec3 light_positions[NUM_LIGHTS] = 
{
    {50.0f, 500.0f, -500.0f},
    {-5.0f, 300.0f, -1500.0f },
    {0.0f, 0.0f, -350}
};

glm::vec3 light_colors[NUM_LIGHTS] = 
{
    {1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f}
};

// Shadowing stuff
static constexpr unsigned int SHADOW_WIDTH = 1024;
static constexpr unsigned int SHADOW_HEIGHT = 1024;
unsigned int depth_map_fbo;
unsigned int depth_map;


void setup_materials()
{
    glm::vec3 color;

    // PANTONE 320 C for 3D
    color = {0.0f, 0.6117647, 0.650980};
    materials.push_back(color);

    // Black for fx
    color = {0.1f, 0.1f, 0.1f};
    materials.push_back(color);

    // Cyan
    color = {0.03921568627f, 0.2941176471f, 0.4705882353f};
    materials.push_back(color);

    // White
    color = {1.0f, 1.0f, 1.0f};
    materials.push_back(color);

    // White
    color = {248.0f / 255.0f, 185.0f / 255.0f, 0.0f};
    materials.push_back(color);
}

// https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
void setup_shadowing()
{
    glGenFramebuffers(1, &depth_map_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);

    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenTextures(1, &depth_map);
    glBindTexture(GL_TEXTURE_2D, depth_map);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, 1024, 1024, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_map, 0);

    glDrawBuffer(GL_NONE);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        log(LogLevel::ERROR, "error with framebuffer!!!\n");
}

void setup_geometry()
{
    std::vector<int> logo_indices;
    std::vector<int> shield_cyan_indices;
    std::vector<int> shield_white_indices;

    std::vector<glm::vec3> colors;
    std::vector<GLint> material_indices;

    // Let's set up the 3Dfx logo first
    glGenVertexArrays(1, &logo_vao);
    glGenBuffers(1, &logo_vbo);
    glGenBuffers(1, &logo_color_buffer);
    glGenBuffers(1, &logo_material_buffer);
    glGenBuffers(1, &logo_ibo);
    glBindVertexArray(logo_vao);
    glBindBuffer(GL_ARRAY_BUFFER, logo_vbo);

    glBufferData(GL_ARRAY_BUFFER, num_verts[LOGO_INDEX] * sizeof(Vert), reinterpret_cast<void*>(vert[LOGO_INDEX]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(VERTEX_ATTRIB);
    glEnableVertexAttribArray(NORMAL_ATTRIB);
    glEnableVertexAttribArray(ST_ATTRIB);
    glVertexAttribPointer(VERTEX_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(offsetof(Vert, Vert::x)));
    glVertexAttribPointer(NORMAL_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(offsetof(Vert, Vert::nx)));
    glVertexAttribPointer(ST_ATTRIB, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(offsetof(Vert, Vert::s)));


    colors.resize(num_verts[LOGO_INDEX]);
    material_indices.resize(num_verts[LOGO_INDEX]);
    for(int i = 0 ; i < num_faces[LOGO_INDEX]; i++)
    {
        Face f = face[LOGO_INDEX][i];
        logo_indices.push_back(f.v[0]);
        logo_indices.push_back(f.v[1]);
        logo_indices.push_back(f.v[2]);

        colors[f.v[0]] = materials[f.mat_index];
        colors[f.v[1]] = materials[f.mat_index];
        colors[f.v[2]] = materials[f.mat_index];
                
        material_indices[f.v[0]] = f.mat_index;
        material_indices[f.v[1]] = f.mat_index;
        material_indices[f.v[2]] = f.mat_index;
    }
    glBindBuffer(GL_ARRAY_BUFFER, logo_material_buffer);
    glBufferData(GL_ARRAY_BUFFER, material_indices.size() * sizeof(GLint), &material_indices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(MATERIAL_NUMBER_ATTRIB, 1, GL_INT, GL_FALSE,  sizeof(GLint), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(MATERIAL_NUMBER_ATTRIB);

    glBindBuffer(GL_ARRAY_BUFFER, logo_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);
    glVertexAttribPointer(COLOR_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(COLOR_ATTRIB);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, logo_ibo);
    logo_index_count = logo_indices.size();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, logo_indices.size() * sizeof(int), &logo_indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    colors.clear();
    material_indices.clear();

    // Now we'll set up the cyan shield
    glGenVertexArrays(1, &shield_cyan_vao);
    glGenBuffers(1, &shield_cyan_vbo);
    glGenBuffers(1, &shield_cyan_color_buffer);
    glGenBuffers(1, &shield_cyan_material_buffer);
    glGenBuffers(1, &shield_cyan_ibo);
    glBindVertexArray(shield_cyan_vao);
    glBindBuffer(GL_ARRAY_BUFFER, shield_cyan_vbo);

    glEnableVertexAttribArray(VERTEX_ATTRIB);
    glEnableVertexAttribArray(NORMAL_ATTRIB);
    glEnableVertexAttribArray(ST_ATTRIB);

    glVertexAttribPointer(VERTEX_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(offsetof(Vert, Vert::x)));
    glVertexAttribPointer(NORMAL_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(offsetof(Vert, Vert::nx)));
    glVertexAttribPointer(ST_ATTRIB, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(offsetof(Vert, Vert::s)));

    glBufferData(GL_ARRAY_BUFFER, num_verts[SHIELD_INDEX_CYAN] * sizeof(Vert), reinterpret_cast<void*>(vert[SHIELD_INDEX_CYAN]), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shield_cyan_ibo);
    colors.resize(num_verts[SHIELD_INDEX_CYAN]);
    material_indices.resize(num_verts[SHIELD_INDEX_CYAN]);
    for(int i = 0 ; i < num_faces[SHIELD_INDEX_CYAN]; i++)
    {
        Face f = face[SHIELD_INDEX_CYAN][i];
        shield_cyan_indices.push_back(f.v[0]);
        shield_cyan_indices.push_back(f.v[1]);
        shield_cyan_indices.push_back(f.v[2]);

        colors[f.v[0]] = materials[f.mat_index];
        colors[f.v[1]] = materials[f.mat_index];
        colors[f.v[2]] = materials[f.mat_index];

        material_indices[f.v[0]] = f.mat_index;
        material_indices[f.v[1]] = f.mat_index;
        material_indices[f.v[2]] = f.mat_index;
    }
    glBindBuffer(GL_ARRAY_BUFFER, shield_cyan_material_buffer);
    glBufferData(GL_ARRAY_BUFFER, material_indices.size() * sizeof(GLint), &material_indices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(MATERIAL_NUMBER_ATTRIB, 1, GL_INT, GL_FALSE, sizeof(GLint), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(MATERIAL_NUMBER_ATTRIB);

    glBindBuffer(GL_ARRAY_BUFFER, shield_cyan_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);
    glVertexAttribPointer(COLOR_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(COLOR_ATTRIB);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, shield_cyan_indices.size() * sizeof(int), &shield_cyan_indices[0], GL_STATIC_DRAW);
    shield_cyan_index_count = shield_cyan_indices.size();
    colors.clear();
    material_indices.clear();

    // Finally set up the white and yellow shield
    glGenVertexArrays(1, &shield_white_vao);
    glGenBuffers(1, &shield_white_vbo);
    glGenBuffers(1, &shield_white_color_buffer);
    glGenBuffers(1, &shield_white_material_buffer);
    glGenBuffers(1, &shield_white_ibo);
    glBindVertexArray(shield_white_vao);
    glBindBuffer(GL_ARRAY_BUFFER, shield_white_vbo);

    glEnableVertexAttribArray(VERTEX_ATTRIB);
    glEnableVertexAttribArray(NORMAL_ATTRIB);
    glEnableVertexAttribArray(ST_ATTRIB);

    glVertexAttribPointer(VERTEX_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(offsetof(Vert, Vert::x)));
    glVertexAttribPointer(NORMAL_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(offsetof(Vert, Vert::nx)));
    glVertexAttribPointer(ST_ATTRIB, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(offsetof(Vert, Vert::s)));

    glBufferData(GL_ARRAY_BUFFER, num_verts[SHIELD_INDEX_WHITE] * sizeof(Vert), reinterpret_cast<void*>(vert[SHIELD_INDEX_WHITE]), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shield_white_ibo);
    colors.resize(num_verts[SHIELD_INDEX_WHITE]);
    material_indices.resize(num_verts[SHIELD_INDEX_WHITE]);
    for(int i = 0 ; i < num_faces[SHIELD_INDEX_WHITE]; i++)
    {
        Face f = face[SHIELD_INDEX_WHITE][i];
        shield_white_indices.push_back(f.v[0]);
        shield_white_indices.push_back(f.v[1]);
        shield_white_indices.push_back(f.v[2]);

        // This is a very dirty hack
        if(colors[f.v[0]].r == 0)
            colors[f.v[0]] = materials[f.mat_index];
        if(colors[f.v[1]].r == 0)
            colors[f.v[1]] = materials[f.mat_index];
        if(colors[f.v[2]].r == 0)
            colors[f.v[2]] = materials[f.mat_index];

        material_indices[f.v[0]] = f.mat_index;
        material_indices[f.v[1]] = f.mat_index;
        material_indices[f.v[2]] = f.mat_index;
    }
    glBindBuffer(GL_ARRAY_BUFFER, shield_white_material_buffer);
    glBufferData(GL_ARRAY_BUFFER, material_indices.size() * sizeof(GLint), &material_indices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(MATERIAL_NUMBER_ATTRIB, 1, GL_INT, GL_FALSE, sizeof(GLint), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(MATERIAL_NUMBER_ATTRIB);

    glBindBuffer(GL_ARRAY_BUFFER, shield_white_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);
    glVertexAttribPointer(COLOR_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(COLOR_ATTRIB);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, shield_white_indices.size() * sizeof(int), &shield_white_indices[0], GL_STATIC_DRAW);
    shield_white_index_count = shield_white_indices.size();

    // Now let's set up the geometry for the lights (so we can draw them)
    glm::vec3 data = {1.0f, 1.0f, 1.0f};
    glGenVertexArrays(1, &light_vao);
    glGenBuffers(1, &light_vbo);
    glBindVertexArray(light_vao);
    glBindBuffer(GL_ARRAY_BUFFER, light_vbo);
    glVertexAttribPointer(VERTEX_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), reinterpret_cast<void*>(0));
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), &data[0], GL_STATIC_DRAW);
}

void create_textures()
{
    // The 3Dfx logo "marbled" texture
    glGenTextures(1, &logo_3d_texture.tex);
    logo_3d_texture.texinfo = reinterpret_cast<Gu3dfInfo*>(text_3dfinfo_raw);
    logo_3d_texture.texinfo->data = reinterpret_cast<void*>(text_3dfinfo_image);
    log(LogLevel::INFO, "Created a new texture! format == 0x%x, width = %d height = %d slod 0x%x, llod 0x%x\n",     logo_3d_texture.texinfo->header.format, 
                                                                                                                    logo_3d_texture.texinfo->header.width, 
                                                                                                                    logo_3d_texture.texinfo->header.height,
                                                                                                                    logo_3d_texture.texinfo->header.small_lod,
                                                                                                                    logo_3d_texture.texinfo->header.large_lod);
}

void download_texture(Texture& tex)
{
    std::vector<uint32_t> data;
    // First we need to work out what format the texture is, and then do some
    // fuckery like 'decompressing' the texture. What kind of fucked up
    // format is this shit??
    yiq422_to_rgb888(&tex.texinfo->table.nccTable, reinterpret_cast<const uint8_t*>(tex.texinfo->data), data, tex.texinfo->header.width * tex.texinfo->header.height);
    glBindTexture(GL_TEXTURE_2D, tex.tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.texinfo->header.width, tex.texinfo->header.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * 3Dfx Splash
 * 
 * Draws the iconic 3Dfx splash screen (the pre-STB logo) in OpenGl 3.3
 */
int main(int argc, char** argv)
{
    // TODO: ARGUMENTS RELATED TO WHICH FRAME TO RENDER HERE!!

    // OpenGL setup
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    // Enable MSAA
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16); // 16x MSAA

    SDL_Window* hwnd = SDL_CreateWindow("3Dfx Splash", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, scr_width, scr_height, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(hwnd);
    glewInit();

    // Do OpenGL setup
    glShadeModel(GL_FLAT);
    glPointSize(5.0f);

    // Enable Depth Testing
    glEnable(GL_DEPTH_TEST);

    // Enable backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Enable MSAA
    glEnable(GL_MULTISAMPLE);

    // Set up 3Dfx geometry
    setup_materials();
    create_textures();
    setup_geometry();
    setup_shadowing();

    download_texture(logo_3d_texture);

    bool running = true;
    bool wireframe = false;
    bool play = true;
    int frame = 1;
    SDL_Event event;
    CShader shadow_pass_shader("shaders/shadow");
    CShader pass2("shaders/logo");

    // Let's set up the projection matrix
    projection = glm::perspective(glm::radians(30.0f), 4.0f / 3.0f, 1.0f, 100000.0f);
    glm::mat4 view = glm::lookAt
    (
        glm::vec3(-10, 0, -450), // Camera is at (4,3,3), in World Space
        glm::vec3(0, 0, 0), // and looks at the origin
        glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // This makes everything draw correctly for some reason?
    // If this is removed, everything stops working?
    view = glm::scale(view, glm::vec3(-1, 1, 1));

    // Light matrices
    glm::mat4 light_projection = glm::ortho(-850.0f, 850.0f, -850.0f, 850.0f, 1.0f, 2700.0f);
    glm::mat4 light_view = glm::lookAt
    (
        light_positions[1],
        glm::vec3(500.0f, -600.0f, 1271.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    light_view = glm::scale(light_view, glm::vec3(-1, 1, 1));
    glm::mat4 mat_lightspace = light_projection * light_view;
    glm::mat4 mvp;

    while(running)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
                running = false;

            if(event.type == SDL_KEYDOWN)
            {
                if(event.key.keysym.sym == SDLK_w)
                {
                    if(!wireframe)
                        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    else
                        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

                    wireframe = !wireframe;
                }

                if(event.key.keysym.sym == SDLK_SPACE)
                {
                    if(frame > total_num_frames)
                        frame = 0;
                    else
                        frame++;
                }

                if(event.key.keysym.sym == SDLK_p)
                {
                    play = !play;
                }
            }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // Draw the shields with color values multiplied by normals
        for(int pass = 1; pass < 3; pass++)
        {
            // First pass is rendering to the shadowmap
            if(pass == 1)
            {
                glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

                // Disable writes to the depth buffer because for some reason the shield gets
                // written to it....
                glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
                glClear(GL_DEPTH_BUFFER_BIT);
                glDepthMask(false); 
                shadow_pass_shader.bind();

                shadow_pass_shader.set_uniform<const glm::mat4&>("mat_projection", light_projection);
                shadow_pass_shader.set_uniform<const glm::mat4&>("mat_view", light_view);

                model = mat[frame][SHIELD_INDEX_CYAN];
                shadow_pass_shader.set_uniform<const glm::mat4&>("mat_model", model);
                glBindVertexArray(shield_cyan_vao);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shield_cyan_ibo);
                glDrawElements(GL_TRIANGLES, shield_cyan_index_count, GL_UNSIGNED_INT, reinterpret_cast<void*>(0));

                // Draw the white part of the shield
                model = mat[frame][SHIELD_INDEX_WHITE];
                shadow_pass_shader.set_uniform<const glm::mat4&>("mat_model", model);
                glBindVertexArray(shield_white_vao);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shield_white_ibo);
                glDrawElements(GL_TRIANGLES, shield_white_index_count, GL_UNSIGNED_INT, reinterpret_cast<void*>(0));

                glDepthMask(true); 
                glDepthFunc(GL_ALWAYS);
                // Get the transformation matrix for the text part of the logo and then draw it
                model = mat[frame][LOGO_INDEX];
                shadow_pass_shader.set_uniform<const glm::mat4&>("mat_model", model);
                glBindVertexArray(logo_vao);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, logo_ibo);
                glDrawElements(GL_TRIANGLES, logo_index_count, GL_UNSIGNED_INT, reinterpret_cast<void*>(0));

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                shadow_pass_shader.unbind();
            }
            else if(pass == 2) // Shadow mapping
            {
                glViewport(0, 0, 640, 480);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glDepthFunc(GL_ALWAYS);

                if(frame > 20)
                    glDepthFunc(GL_LEQUAL);


                pass2.bind();
                pass2.set_uniform<const glm::mat4&>("mat_projection", projection);
                pass2.set_uniform<const glm::mat4&>("mat_view", view);
                

                // Lighting setup
                pass2.set_uniform<const glm::vec3&>("light0_position", light_positions[0]);
                pass2.set_uniform<const glm::vec3&>("light1_position", light_positions[1]);
                pass2.set_uniform<const glm::mat4&>("mat_lightmatrix", mat_lightspace);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, depth_map);

                // Draw the cyan part of the shield
                model = mat[frame][SHIELD_INDEX_CYAN];
                pass2.set_uniform<const glm::mat4&>("mat_model", model);
                glBindVertexArray(shield_cyan_vao);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shield_cyan_ibo);
                glDrawElements(GL_TRIANGLES, shield_cyan_index_count, GL_UNSIGNED_INT, reinterpret_cast<void*>(0));

                // Draw the white part of the shield
                model = mat[frame][SHIELD_INDEX_WHITE];
                pass2.set_uniform<const glm::mat4&>("mat_model", model);
                glBindVertexArray(shield_white_vao);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shield_white_ibo);
                glDrawElements(GL_TRIANGLES, shield_white_index_count, GL_UNSIGNED_INT, reinterpret_cast<void*>(0));

                // Get the transformation matrix for the text part of the logo and then draw it
                model = mat[frame][LOGO_INDEX];
                pass2.set_uniform<const glm::mat4&>("mat_model", model);
                glBindVertexArray(logo_vao);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, logo_ibo);
                glDrawElements(GL_TRIANGLES, logo_index_count, GL_UNSIGNED_INT, reinterpret_cast<void*>(0));

                pass2.unbind();
            }
        }

        if(frame > total_num_frames)
        {
            frame = 0;
            continue;
        }

        if(play)
            frame++;

        SDL_GL_SwapWindow(hwnd);
        SDL_Delay(30);
    }
}