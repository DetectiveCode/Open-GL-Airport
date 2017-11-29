#include "stdafx.h"
#include "Model.h"
#include "TextureLoader.h"

Model::Model()
{
}

Model::Model(const string & modelfile, const string & bmpFile)
{
	m_modelPath = m_modelPath += modelfile;
	m_texturePath = m_texturePath += bmpFile;
	m_modelMatrix = identity_mat4();

	if (bmpFile != "")
	{
		ifstream f(m_texturePath.c_str());

		if (f.good())
		{
			TextureLoader::LoadBMP(m_texturePath, m_textureData);
		}
		else
		{
			Log::AppendToLogFile("Error opening texture: " + m_texturePath);
		}
	}
}

void Model::CreateModel()
{
	ifstream f(m_modelPath.c_str());
	
	if (f.good())
	{
		m_Model.LoadModel(m_modelPath);
		m_vertices = m_Model.GetVertices();
		m_normals = m_Model.GetNormals();
		m_textureVertices = m_Model.GetTextureVertices();

		//Win32OpenGL::CreateVAO(m_vao, m_vboVertices, m_vboNormals, m_vertices, m_normals);
		Win32OpenGL::CreateVAOWithTextures(m_vao, m_vboVertices, m_vboNormals, m_vboTextures, m_vertices, m_normals, m_textureVertices);
	}
	else
	{
		Log::AppendToLogFile("Error opening model: " + m_modelPath);
	}
}

void Model::DrawModel(GLuint &shaderProgram)
{
	m_modelMatrix = identity_mat4();
	m_modelMatrix = rotate_x_deg(m_modelMatrix, m_properties.m_xAngle);
	m_modelMatrix = rotate_z_deg(m_modelMatrix, m_properties.m_zAngle);
	m_modelMatrix = rotate_y_deg(m_modelMatrix, m_properties.m_yAngle);
	m_modelMatrix = translate(m_modelMatrix, vec3(m_properties.m_xTranslate, m_properties.m_yTranslate, m_properties.m_zTranslate));
	m_modelMatrix = scale(m_modelMatrix, vec3(m_properties.m_xScale, m_properties.m_yScale, m_properties.m_zScale));

	Win32OpenGL::SendUniformMatrixToShader(shaderProgram, m_modelMatrix.m, "model_matrix");
	glBindTexture(GL_TEXTURE_2D, m_textureData);
	Win32OpenGL::Draw(m_vao, m_vertices.size() / 3);
	glBindTexture(GL_TEXTURE_2D, NULL);
}