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

#define SHIELD_INDEX_WHITE 0
#define LOGO_INDEX 1
#define SHIELD_INDEX_CYAN 2


static constexpr GLsizei scr_width = 640;
static constexpr GLsizei scr_height = 480;

static GLuint logo_vao, logo_vbo, logo_ibo;
static GLuint shield_cyan_vao, shield_cyan_vbo, shield_cyan_ibo;
static GLuint shield_white_vao, shield_white_vbo,shield_white_ibo;

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

void setup_geometry()
{
    std::vector<int> logo_indices;
    std::vector<int> shield_cyan_indices;
    std::vector<int> shield_white_indices;

    // Let's set up the 3Dfx logo first
    glGenVertexArrays(1, &logo_vao);
    glGenBuffers(1, &logo_vbo);
    glGenBuffers(1, &logo_ibo);
    glBindVertexArray(logo_vao);
    glBindBuffer(GL_ARRAY_BUFFER, logo_vbo);

    glEnableVertexAttribArray(VERTEX_ATTRIB);
    glEnableVertexAttribArray(NORMAL_ATTRIB);
    glEnableVertexAttribArray(ST_ATTRIB);

    glVertexAttribPointer(VERTEX_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(offsetof(Vert, Vert::x)));
    glVertexAttribPointer(NORMAL_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(offsetof(Vert, Vert::nx)));
    glVertexAttribPointer(NORMAL_ATTRIB, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(offsetof(Vert, Vert::s)));

    glBufferData(GL_ARRAY_BUFFER, num_verts[LOGO_INDEX] * sizeof(Vert), reinterpret_cast<void*>(vert[LOGO_INDEX]), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, logo_ibo);
    for(int i = 0 ; i < num_faces[LOGO_INDEX]; i++)
    {
        Face f = face[LOGO_INDEX][i];
        logo_indices.push_back(f.v[0]);
        logo_indices.push_back(f.v[1]);
        logo_indices.push_back(f.v[2]);
    }
    logo_index_count = logo_indices.size();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, logo_indices.size() * sizeof(int), &logo_indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Now we'll set up the cyan shield
    glGenVertexArrays(1, &shield_cyan_vao);
    glGenBuffers(1, &shield_cyan_vbo);
    glGenBuffers(1, &shield_cyan_ibo);
    glBindVertexArray(shield_cyan_vao);
    glBindBuffer(GL_ARRAY_BUFFER, shield_cyan_vbo);

    glEnableVertexAttribArray(VERTEX_ATTRIB);
    glEnableVertexAttribArray(NORMAL_ATTRIB);
    glEnableVertexAttribArray(ST_ATTRIB);

    glVertexAttribPointer(VERTEX_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(offsetof(Vert, Vert::x)));
    glVertexAttribPointer(NORMAL_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(offsetof(Vert, Vert::nx)));
    glVertexAttribPointer(NORMAL_ATTRIB, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(offsetof(Vert, Vert::s)));

    glBufferData(GL_ARRAY_BUFFER, num_verts[SHIELD_INDEX_CYAN] * sizeof(Vert), reinterpret_cast<void*>(vert[SHIELD_INDEX_CYAN]), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shield_cyan_ibo);
    for(int i = 0 ; i < num_faces[SHIELD_INDEX_CYAN]; i++)
    {
        Face f = face[SHIELD_INDEX_CYAN][i];
        shield_cyan_indices.push_back(f.v[0]);
        shield_cyan_indices.push_back(f.v[1]);
        shield_cyan_indices.push_back(f.v[2]);
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, shield_cyan_indices.size() * sizeof(int), &shield_cyan_indices[0], GL_STATIC_DRAW);
    shield_cyan_index_count = shield_cyan_indices.size();

    // Finally set up the white and yellow shield
    glGenVertexArrays(1, &shield_white_vao);
    glGenBuffers(1, &shield_white_vbo);
    glGenBuffers(1, &shield_white_ibo);
    glBindVertexArray(shield_white_vao);
    glBindBuffer(GL_ARRAY_BUFFER, shield_white_vbo);

    glEnableVertexAttribArray(VERTEX_ATTRIB);
    glEnableVertexAttribArray(NORMAL_ATTRIB);
    glEnableVertexAttribArray(ST_ATTRIB);

    glVertexAttribPointer(VERTEX_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(offsetof(Vert, Vert::x)));
    glVertexAttribPointer(NORMAL_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(offsetof(Vert, Vert::nx)));
    glVertexAttribPointer(NORMAL_ATTRIB, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(offsetof(Vert, Vert::s)));

    glBufferData(GL_ARRAY_BUFFER, num_verts[SHIELD_INDEX_WHITE] * sizeof(Vert), reinterpret_cast<void*>(vert[SHIELD_INDEX_WHITE]), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shield_white_ibo);
    for(int i = 0 ; i < num_faces[SHIELD_INDEX_WHITE]; i++)
    {
        Face f = face[SHIELD_INDEX_WHITE][i];
        shield_white_indices.push_back(f.v[0]);
        shield_white_indices.push_back(f.v[1]);
        shield_white_indices.push_back(f.v[2]);
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, shield_white_indices.size() * sizeof(int), &shield_white_indices[0], GL_STATIC_DRAW);
    shield_white_index_count = shield_white_indices.size();
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
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    SDL_Window* hwnd = SDL_CreateWindow("3Dfx Splash", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, scr_width, scr_height, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(hwnd);
    glewInit();

    // Do OpenGL setup
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS); // Always pass the Depth Test to prevent Z-fighting of the shields

    // Set up 3Dfx geometry
    setup_geometry();
    create_textures();

    download_texture(logo_3d_texture);

    bool running = true;
    int frame = 1;
    SDL_Event event;
    CShader text_shader("shaders/3dfx_text");
    CShader shield_cyan_shader("shaders/shield_cyan");
    CShader shield_white_shader("shaders/shield_white");

    // Let's set up the projection matrix
    projection = glm::perspective(glm::radians(30.0f), 4.0f / 3.0f, 0.01f, 100000.0f);
    glm::mat4 view = glm::lookAt(
    glm::vec3(-10,0,-450), // Camera is at (4,3,3), in World Space
    glm::vec3(0,0,0), // and looks at the origin
    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    view = glm::scale(view, glm::vec3(-1, 1, 1));
    while(running)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
                running = false;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // Draw the cyan part of the shield
        model = mat[frame][SHIELD_INDEX_CYAN];
        shield_cyan_shader.bind();
        mvp = projection * view * model;
        shield_cyan_shader.set_uniform<const glm::mat4&>("mat_mvp", mvp);
        //glBindTexture(GL_TEXTURE_2D, logo_3d_texture.tex);
        glBindVertexArray(shield_cyan_vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shield_cyan_ibo);
        glDrawElements(GL_TRIANGLES, shield_cyan_index_count, GL_UNSIGNED_INT, reinterpret_cast<void*>(0));

        // Draw the white part of the shield
        model = mat[frame][SHIELD_INDEX_WHITE];
        shield_white_shader.bind();
        mvp = projection * view * model;
        shield_white_shader.set_uniform<const glm::mat4&>("mat_mvp", mvp);
        //glBindTexture(GL_TEXTURE_2D, logo_3d_texture.tex);
        glBindVertexArray(shield_white_vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shield_white_ibo);
        glDrawElements(GL_TRIANGLES, shield_white_index_count, GL_UNSIGNED_INT, reinterpret_cast<void*>(0));


        // Get the transformation matrix for the text part of the logo and then draw it
        model = mat[frame][LOGO_INDEX];
        text_shader.bind();
        mvp = projection * view * model;
        text_shader.set_uniform<const glm::mat4&>("mat_mvp", mvp);
        //glBindTexture(GL_TEXTURE_2D, logo_3d_texture.tex);
        glBindVertexArray(logo_vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, logo_ibo);
        glDrawElements(GL_TRIANGLES, logo_index_count, GL_UNSIGNED_INT, reinterpret_cast<void*>(0));


        if(frame > total_num_frames)
        {
            frame = 0;
            continue;
        }

        frame++;
        SDL_GL_SwapWindow(hwnd);
        SDL_Delay(40);
    }
}