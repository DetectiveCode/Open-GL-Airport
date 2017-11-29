#pragma once
#include "Win32OpenGL.h"
#include "ModelLoader.h"
#include "ModelProperties.h"
#include "Log.h"

/** Model Class */
class Model
{
public:
	Model();
	Model(const string & modelfile, const string & bmpFile = "");

	void CreateModel();
	void DrawModel(GLuint & shaderProgram);

	mat4 m_modelMatrix;
	vector<float> m_vertices; ///< Model vertices, cannot be initialised at compile time.
	vector<float> m_normals; ///< Model normals, used in lighting.
	vector<float> m_textureVertices;

	GLuint m_vao;
	GLuint m_vboVertices;
	GLuint m_vboNormals; ///< Model vertex buffer object normals.
	GLuint m_vboTextures;
	GLuint m_textureData;

	ModelProperties m_properties;

private:
	ModelLoader m_Model;

	string m_texturePath = "resources//Textures//";
	string m_modelPath = "resources//Models//";
};