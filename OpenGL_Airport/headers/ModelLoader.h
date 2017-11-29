#pragma once
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

/** Model Loader Class */
class ModelLoader
{
private:
	vector<float> m_indexedVertices; ///< Stores vertices. ('v' lines).
	vector<float> m_indexedNormals; ///< Stores normal vertices. ('vn' lines).
	vector<float> m_indexedTextures; ///< Stores texture vertices. ('vt' lines).

	vector<unsigned int> m_faceVertexIndices; ///< Stores face vertice index. ('f' line). 
	vector<unsigned int> m_faceNormalIndices; ///< Stores face normal vertice index. ('f' line).
	vector<unsigned int> m_faceTextureIndices; ///< Stores face texture vertice index. ('f' line).

	vector<float> m_vertices; ///< Stores processed vertices.
	vector<float> m_normals; ///< Stores processed normal vertices.
	vector<float> m_textureVertices; ///< Stores processed texture vertices.

public:
	ModelLoader();
	~ModelLoader();

	void LoadModel(string modelName);

	vector<float>& GetVertices();
	vector<float>& GetNormals();
	vector<float>& GetTextureVertices();
};