#pragma once

// Minimal OpenGL declarations to avoid platform headers in core.
// We only need glClearColor, glClear, and glViewport for Phase 1.

#ifdef _WIN32
#ifndef APIENTRY
#define APIENTRY __stdcall
#endif
#else
#ifndef APIENTRY
#define APIENTRY
#endif
#endif

using GLenum = unsigned int;
using GLbitfield = unsigned int;
using GLint = int;
using GLsizei = int;
using GLfloat = float;

#ifndef GL_COLOR_BUFFER_BIT
#define GL_COLOR_BUFFER_BIT 0x00004000
#endif

extern "C" {
void APIENTRY glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void APIENTRY glClear(GLbitfield mask);
void APIENTRY glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
}
