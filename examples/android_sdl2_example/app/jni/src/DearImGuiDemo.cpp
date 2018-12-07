// dear imgui: standalone example application for Android + SDL2 + OpenGL
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

// **THIS CODE USES MODERN OPENGL (SHADERS, VBO, VAO, etc.)**
// **Prefer using the code in the example_sdl_opengl3/ folder**
// See imgui_impl_sdl.cpp for details.

// The Julia fractal code is adapted from the example in the OpenGl Superbible 7 book
// https://github.com/openglsuperbible/sb7code/blob/master/src/julia/julia.cpp
//
// Copyright � 2012-2015 Graham Sellers
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice (including the next
// paragraph) shall be included in all copies or substantial portions of the
// Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include <string>
#include <sstream>
#include <random>
#include <cstdlib>
#include <cmath>

#include <SDL.h>
// #include <SDL_opengl.h>

#include <GLES3/gl3.h>

#include <imgui.h>

#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

// This data structure represents the options for rendering Julia fractal with OpenGL ES
class JuliaFractal {
private:
    GLuint      program;
    GLuint      vao;
    GLuint      palette_texture;

    GLint   uniform_loc_zoom;
    GLint   uniform_loc_offset;
    GLint   uniform_loc_C;

public:
    bool paused;
    float time_offset; float zoom; float x_offset; float y_offset;

public:
    JuliaFractal(bool paused = false,
        float time_offset = 0.0f,
        float zoom = 0.0f,
        float x_offset = 0.0f,
        float y_offset = 0.0f)
    : paused(paused), time_offset(time_offset), zoom(zoom), x_offset(x_offset), y_offset(y_offset)
    {
        initOpenGl();
    }

    void shutdown()
    {
        glDeleteTextures(1, &palette_texture);
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(program);
    }

    void initOpenGl()
    {
        static const unsigned char palette[] = {
    0xFF, 0x00, 0x00, 0xFF, 0x0E, 0x03, 0xFF, 0x1C,
    0x07, 0xFF, 0x2A, 0x0A, 0xFF, 0x38, 0x0E, 0xFF,
    0x46, 0x12, 0xFF, 0x54, 0x15, 0xFF, 0x62, 0x19,
    0xFF, 0x71, 0x1D, 0xFF, 0x7F, 0x20, 0xFF, 0x88,
    0x22, 0xFF, 0x92, 0x25, 0xFF, 0x9C, 0x27, 0xFF,
    0xA6, 0x2A, 0xFF, 0xB0, 0x2C, 0xFF, 0xBA, 0x2F,
    0xFF, 0xC4, 0x31, 0xFF, 0xCE, 0x34, 0xFF, 0xD8,
    0x36, 0xFF, 0xE2, 0x39, 0xFF, 0xEC, 0x3B, 0xFF,
    0xF6, 0x3E, 0xFF, 0xFF, 0x40, 0xF8, 0xFE, 0x40,
    0xF1, 0xFE, 0x40, 0xEA, 0xFE, 0x41, 0xE3, 0xFD,
    0x41, 0xDC, 0xFD, 0x41, 0xD6, 0xFD, 0x42, 0xCF,
    0xFC, 0x42, 0xC8, 0xFC, 0x42, 0xC1, 0xFC, 0x43,
    0xBA, 0xFB, 0x43, 0xB4, 0xFB, 0x43, 0xAD, 0xFB,
    0x44, 0xA6, 0xFA, 0x44, 0x9F, 0xFA, 0x45, 0x98,
    0xFA, 0x45, 0x92, 0xF9, 0x45, 0x8B, 0xF9, 0x46,
    0x84, 0xF9, 0x46, 0x7D, 0xF8, 0x46, 0x76, 0xF8,
    0x46, 0x6F, 0xF8, 0x47, 0x68, 0xF8, 0x47, 0x61,
    0xF7, 0x47, 0x5A, 0xF7, 0x48, 0x53, 0xF7, 0x48,
    0x4C, 0xF6, 0x48, 0x45, 0xF6, 0x49, 0x3E, 0xF6,
    0x49, 0x37, 0xF6, 0x4A, 0x30, 0xF5, 0x4A, 0x29,
    0xF5, 0x4A, 0x22, 0xF5, 0x4B, 0x1B, 0xF5, 0x4B,
    0x14, 0xF4, 0x4B, 0x0D, 0xF4, 0x4C, 0x06, 0xF4,
    0x4D, 0x04, 0xF1, 0x51, 0x0D, 0xE9, 0x55, 0x16,
    0xE1, 0x59, 0x1F, 0xD9, 0x5D, 0x28, 0xD1, 0x61,
    0x31, 0xC9, 0x65, 0x3A, 0xC1, 0x69, 0x42, 0xB9,
    0x6D, 0x4B, 0xB1, 0x71, 0x54, 0xA9, 0x75, 0x5D,
    0xA1, 0x79, 0x66, 0x99, 0x7D, 0x6F, 0x91, 0x81,
    0x78, 0x89, 0x86, 0x80, 0x81, 0x8A, 0x88, 0x7A,
    0x8E, 0x90, 0x72, 0x92, 0x98, 0x6A, 0x96, 0xA1,
    0x62, 0x9A, 0xA9, 0x5A, 0x9E, 0xB1, 0x52, 0xA2,
    0xBA, 0x4A, 0xA6, 0xC2, 0x42, 0xAA, 0xCA, 0x3A,
    0xAE, 0xD3, 0x32, 0xB2, 0xDB, 0x2A, 0xB6, 0xE3,
    0x22, 0xBA, 0xEB, 0x1A, 0xBE, 0xF4, 0x12, 0xC2,
    0xFC, 0x0A, 0xC6, 0xF5, 0x02, 0xCA, 0xE6, 0x09,
    0xCE, 0xD8, 0x18, 0xD1, 0xCA, 0x27, 0xD5, 0xBB,
    0x36, 0xD8, 0xAD, 0x45, 0xDC, 0x9E, 0x54, 0xE0,
    0x90, 0x62, 0xE3, 0x82, 0x6F, 0xE6, 0x71, 0x7C,
    0xEA, 0x61, 0x89, 0xEE, 0x51, 0x96, 0xF2, 0x40,
    0xA3, 0xF5, 0x30, 0xB0, 0xF9, 0x20, 0xBD, 0xFD,
    0x0F, 0xE3, 0xFF, 0x01, 0xE9, 0xFF, 0x01, 0xEE,
    0xFF, 0x01, 0xF4, 0xFF, 0x00, 0xFA, 0xFF, 0x00,
    0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x0A, 0xFF, 0xFF,
    0x15, 0xFF, 0xFF, 0x20, 0xFF, 0xFF, 0x2B, 0xFF,
    0xFF, 0x36, 0xFF, 0xFF, 0x41, 0xFF, 0xFF, 0x4C,
    0xFF, 0xFF, 0x57, 0xFF, 0xFF, 0x62, 0xFF, 0xFF,
    0x6D, 0xFF, 0xFF, 0x78, 0xFF, 0xFF, 0x81, 0xFF,
    0xFF, 0x8A, 0xFF, 0xFF, 0x92, 0xFF, 0xFF, 0x9A,
    0xFF, 0xFF, 0xA3, 0xFF, 0xFF, 0xAB, 0xFF, 0xFF,
    0xB4, 0xFF, 0xFF, 0xBC, 0xFF, 0xFF, 0xC4, 0xFF,
    0xFF, 0xCD, 0xFF, 0xFF, 0xD5, 0xFF, 0xFF, 0xDD,
    0xFF, 0xFF, 0xE6, 0xFF, 0xFF, 0xEE, 0xFF, 0xFF,
    0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9,
    0xFF, 0xFF, 0xF3, 0xFF, 0xFF, 0xED, 0xFF, 0xFF,
    0xE7, 0xFF, 0xFF, 0xE1, 0xFF, 0xFF, 0xDB, 0xFF,
    0xFF, 0xD5, 0xFF, 0xFF, 0xCF, 0xFF, 0xFF, 0xCA,
    0xFF, 0xFF, 0xC4, 0xFF, 0xFF, 0xBE, 0xFF, 0xFF,
    0xB8, 0xFF, 0xFF, 0xB2, 0xFF, 0xFF, 0xAC, 0xFF,
    0xFF, 0xA6, 0xFF, 0xFF, 0xA0, 0xFF, 0xFF, 0x9B,
    0xFF, 0xFF, 0x96, 0xFF, 0xFF, 0x90, 0xFF, 0xFF,
    0x8B, 0xFF, 0xFF, 0x86, 0xFF, 0xFF, 0x81, 0xFF,
    0xFF, 0x7B, 0xFF, 0xFF, 0x76, 0xFF, 0xFF, 0x71,
    0xFF, 0xFF, 0x6B, 0xFF, 0xFF, 0x66, 0xFF, 0xFF,
    0x61, 0xFF, 0xFF, 0x5C, 0xFF, 0xFF, 0x56, 0xFF,
    0xFF, 0x51, 0xFF, 0xFF, 0x4C, 0xFF, 0xFF, 0x47,
    0xFF, 0xFF, 0x41, 0xF9, 0xFF, 0x40, 0xF0, 0xFF,
    0x40, 0xE8, 0xFF, 0x40, 0xDF, 0xFF, 0x40, 0xD7,
    0xFF, 0x40, 0xCF, 0xFF, 0x40, 0xC6, 0xFF, 0x40,
    0xBE, 0xFF, 0x40, 0xB5, 0xFF, 0x40, 0xAD, 0xFF,
    0x40, 0xA4, 0xFF, 0x40, 0x9C, 0xFF, 0x40, 0x95,
    0xFF, 0x40, 0x8D, 0xFF, 0x40, 0x86, 0xFF, 0x40,
    0x7E, 0xFF, 0x40, 0x77, 0xFF, 0x40, 0x6F, 0xFF,
    0x40, 0x68, 0xFF, 0x40, 0x60, 0xFF, 0x40, 0x59,
    0xFF, 0x40, 0x51, 0xFF, 0x40, 0x4A, 0xFA, 0x43,
    0x42, 0xF3, 0x48, 0x3E, 0xED, 0x4E, 0x3D, 0xE6,
    0x53, 0x3B, 0xDF, 0x58, 0x39, 0xD8, 0x5E, 0x37,
    0xD2, 0x63, 0x35, 0xCB, 0x68, 0x34, 0xC4, 0x6D,
    0x32, 0xBD, 0x73, 0x30, 0xB7, 0x78, 0x2E, 0xB0,
    0x7D, 0x2D, 0xA9, 0x83, 0x2B, 0xA2, 0x88, 0x29,
    0x9C, 0x8D, 0x27, 0x95, 0x92, 0x25, 0x8E, 0x98,
    0x24, 0x87, 0x9D, 0x22, 0x81, 0xA2, 0x20, 0x7A,
    0xA6, 0x1E, 0x74, 0xAB, 0x1D, 0x6E, 0xB0, 0x1B,
    0x68, 0xB5, 0x1A, 0x61, 0xB9, 0x18, 0x5B, 0xBE,
    0x17, 0x55, 0xC3, 0x15, 0x4F, 0xC8, 0x13, 0x48,
    0xCD, 0x12, 0x42, 0xD1, 0x10, 0x3C, 0xD6, 0x0F,
    0x36, 0xDB, 0x0D, 0x2F, 0xE0, 0x0C, 0x29, 0xE4,
    0x0A, 0x23, 0xE9, 0x08, 0x1D, 0xEE, 0x07, 0x16,
    0xF3, 0x05, 0x10, 0xF7, 0x04, 0x0A, 0xFC, 0x02,
    0x04, 0xFB, 0x01, 0x04, 0xEF, 0x00, 0x12, 0xE4,
    0x00, 0x1F, 0xD9, 0x00, 0x2D, 0xCE, 0x00, 0x3B,
    0xC2, 0x00, 0x48, 0xB7, 0x00, 0x56, 0xAC, 0x00,
    0x64, 0xA0, 0x00, 0x72, 0x95, 0x00, 0x7F, 0x8A,
    0x00, 0x88, 0x7F, 0x00, 0x92, 0x75, 0x00, 0x9C,
    0x6B, 0x00, 0xA6, 0x61, 0x00, 0xB0, 0x57, 0x00,
    0xBA, 0x4E, 0x00, 0xC4, 0x44, 0x00, 0xCE, 0x3A,
    0x00, 0xD8, 0x30, 0x00, 0xE2, 0x27, 0x00, 0xEC,
    0x1D, 0x00, 0xF6, 0x13, 0x00, 0xFF, 0x09, 0x00};

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        static const char * vs_source[] =
        {
            "// Julia set renderer - Vertex Shader                                                  \n"
            "// Graham Sellers                                                                      \n"
            "// OpenGL SuperBible                                                                   \n"
            "#version 300 es                                                                        \n"
            "                                                                                       \n"
            "// Zoom factor                                                                         \n"
            "uniform float zoom;                                                                    \n"
            "                                                                                       \n"
            "// Offset vector                                                                       \n"
            "uniform vec2 offset;                                                                   \n"
            "                                                                                       \n"
            "out vec2 initial_z;                                                                    \n"
            "                                                                                       \n"
            "void main(void)                                                                        \n"
            "{                                                                                      \n"
            "    const vec4 vertices[4] = vec4[4](vec4(-1.0, -1.0, 0.5, 1.0),                       \n"
            "                                     vec4( 1.0, -1.0, 0.5, 1.0),                       \n"
            "                                     vec4( 1.0,  1.0, 0.5, 1.0),                       \n"
            "                                     vec4(-1.0,  1.0, 0.5, 1.0));                      \n"
            "    initial_z = (vertices[gl_VertexID].xy * zoom) + offset;                            \n"
            "    gl_Position = vertices[gl_VertexID];                                               \n"
            "}                                                                                      \n"
        };

        static const char * fs_source[] =
        {
            "// Julia set renderer - Fragment Shader                                                \n"
            "// Graham Sellers                                                                      \n"
            "// OpenGL SuperBible                                                                   \n"
            "#version 300 es                                                                      \n"
            "                                                                                       \n"
            "in vec2 initial_z;                                                                     \n"
            "                                                                                       \n"
            "out vec4 color;                                                                        \n"
            "                                                                                       \n"
            "uniform sampler2D tex_gradient;                                                        \n"
            "uniform vec2 C;                                                                        \n"
            "                                                                                       \n"
            "void main(void)                                                                        \n"
            "{                                                                                      \n"
            "    vec2 Z = initial_z;                                                                \n"
            "    int iterations = 0;                                                                \n"
            "    const float threshold_squared = 16.0;                                              \n"
            "    const int max_iterations = 256;                                                    \n"
            "    while (iterations < max_iterations && dot(Z, Z) < threshold_squared) {             \n"
            "        vec2 Z_squared;                                                                \n"
            "        Z_squared.x = Z.x * Z.x - Z.y * Z.y;                                           \n"
            "        Z_squared.y = 2.0 * Z.x * Z.y;                                                 \n"
            "        Z = Z_squared + C;                                                             \n"
            "        iterations++;                                                                  \n"
            "    }                                                                                  \n"
            "    color = vec4(1.0, 1.0, 1.0, 1.0);                                                                                  \n"
            "    //if (iterations == max_iterations)                                                  \n"
            "      //  color = vec4(0.0, 0.0, 0.0, 1.0);                                              \n"
            "    //else                                                                               \n"
            "      //  color = texture(tex_gradient, vec2(float(iterations) / float(max_iterations), 1.0f));      \n"
            "}                                                                                      \n"
        };

        program = glCreateProgram();
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, fs_source, NULL);
        glCompileShader(fs);

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, vs_source, NULL);
        glCompileShader(vs);

        glAttachShader(program, vs);
        glAttachShader(program, fs);

        glLinkProgram(program);

        uniform_loc_zoom   = glGetUniformLocation(program, "zoom");
        uniform_loc_offset = glGetUniformLocation(program, "offset");
        uniform_loc_C      = glGetUniformLocation(program, "C");

        glDeleteShader(vs);
        glDeleteShader(fs);

        glGenTextures(1, &palette_texture);
        glBindTexture(GL_TEXTURE_2D, palette_texture);
        glTexStorage2D(GL_TEXTURE_2D, 8, GL_RGB8, 256, 1);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 1, GL_RGB, GL_UNSIGNED_BYTE, palette);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void render()
    {
        // static const GLfloat color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        // glClearBufferfv(GL_COLOR, 0, color);

        static float t = 0.0f;
        float r = 0.0f;

        if (!paused)
            t = ((float) SDL_GetTicks()) / 1000.0f;

        r = t + time_offset;

        float C[2] = { (sinf(r * 0.1f) + cosf(r * 0.23f)) * 0.5f, (cosf(r * 0.13f) + sinf(r * 0.21f)) * 0.5f };
        float offset[2] = { x_offset, y_offset };

        glUseProgram(program);

        glUniform2fv(uniform_loc_C, 1, C);
        glUniform2fv(uniform_loc_offset, 1, offset);
        glUniform1f(uniform_loc_zoom, zoom);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
}; // JuliaFractal class

// Must use conventional parameters here 
// or else there will be an undefined reference to SDL_main
int main(int argc, char** argv)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

    // Setup window
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    SDL_Window* window = SDL_CreateWindow(nullptr, 0, 0, current.w, current.h, 
        SDL_WINDOW_OPENGL|SDL_WINDOW_FULLSCREEN);
    if (window == 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create the SDL Window: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Query OpenGL device information
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* vendor = glGetString(GL_VENDOR);
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    std::stringstream ss;
    ss << "\n-------------------------------------------------------------\n";
    ss << "GL Vendor    : " << vendor;
    ss << "\nGL GLRenderer : " << renderer;
    ss << "\nGL Version   : " << version;
    ss << "\nGLSL Version : " << glslVersion;
    ss << "\n-------------------------------------------------------------\n";
    SDL_Log("%s", ss.str().c_str());

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init();

    // Setup Style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    JuliaFractal julia_fractal;

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_QUIT)
                done = true;
        }

        // Start the OpenGl rendering frame
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        julia_fractal.render();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        // ImGui_ImplSdlGLES2_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        
        //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    julia_fractal.shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    // ImGui_ImplSdlGLES2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
} // main
