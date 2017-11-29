#pragma once
#include "Win32OpenGL.h"

/** Texture Loader Class */
class TextureLoader
{
private:
	TextureLoader();
	~TextureLoader();

public:
	static int LoadBMP(const string &location, GLuint &texture); 
};