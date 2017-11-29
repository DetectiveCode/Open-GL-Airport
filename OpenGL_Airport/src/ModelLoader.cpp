#include "stdafx.h"
#include "ModelLoader.h"

ModelLoader::ModelLoader()
{
}

ModelLoader::~ModelLoader()
{
}

void ModelLoader::LoadModel(string modelName)
{
	// code 
	std::fstream modelfile(modelName, std::ios_base::in);
	string line = "";

	if (modelfile.is_open())
	{
		while (getline(modelfile, line))
		{
			string s;
			istringstream iss(line);

			iss >> s; // read to first whitespace

			if (s == "f")
			{
				// process face line
				unsigned int f;

				while (iss >> f)
				{
					// change to zero based indices
					f--;
					// store this in face indices
					m_faceVertexIndices.push_back(f);

					static const int forwardSlash = 0x2F;
					// peek character
					int lookAhead = iss.peek();

					// "/"
					if (lookAhead == forwardSlash)
					{
						iss.get();

						// "/"
						lookAhead = iss.peek();
						if (lookAhead == forwardSlash)
						{
							// here we have v // vn
							iss.get();
							
							// get the normal - store it
							unsigned int normalIndex;

							iss >> normalIndex;
							normalIndex--;
							m_faceNormalIndices.push_back(normalIndex);
						}
						else
						{
							// here we have v / vt / vn
							// so read vt - store it
							unsigned int textureIndex;
							iss >> textureIndex;
							textureIndex--;
							m_faceTextureIndices.push_back(textureIndex);
							
							// get the slash
							iss.get();

							// read the vn - store it
							unsigned int normalIndex;
							iss >> normalIndex;
							normalIndex--;
							m_faceNormalIndices.push_back(normalIndex);
						}
					}
				}
			}
			else if (s == "v")
			{
				// process indexed vertices using floats
				// process vertex line
				float v;

				while (iss >> v)
				{
					// store this in vertex indices
					m_indexedVertices.push_back(v);
				}
			}
			else if (s == "vn")
			{
				// process indexed normals using floats
				// process normal line
				float vn;

				while (iss >> vn)
				{
					// store this in normal indices
					m_indexedNormals.push_back(vn);
				}
			}
			else if (s == "vt")
			{
				// process indexed textures using floats
				// process texture line
				float vt;

				while (iss >> vt)
				{
					// store this in texture indices
					m_indexedTextures.push_back(vt);
				}
			}
		}
		// close the file
		modelfile.close();

		// create the m_vertices vector
		for (size_t i = 0; i < m_faceVertexIndices.size(); i++)
		{
			// this is where the first of the 3 floats is
			unsigned int vertexStartOffset = m_faceVertexIndices[i] * 3;

			for (unsigned int j = 0; j < 3; j++)
			{
				m_vertices.push_back(m_indexedVertices[vertexStartOffset + j]);
			}
		}

		// create the m_normals vector
		for (size_t i = 0; i < m_faceNormalIndices.size(); i++)
		{
			// this is where the first of the 3 floats is
			unsigned int normalVertexStartOffset = m_faceNormalIndices[i] * 3;

			for (unsigned int j = 0; j < 3; j++)
			{
				m_normals.push_back(m_indexedNormals[normalVertexStartOffset + j]);
			}
		}

		// create the m_textures vector
		for (size_t i = 0; i < m_faceTextureIndices.size(); i++)
		{
			// this is where the first of the 3 floats is
			unsigned int textureVertexStartOffset = m_faceTextureIndices[i] * 2;

			for (unsigned int j = 0; j < 2; j++)
			{
				m_textureVertices.push_back(m_indexedTextures[textureVertexStartOffset + j]);
			}
		}
	}
}

vector<float>& ModelLoader::GetVertices()
{
	return m_vertices;
}

vector<float>& ModelLoader::GetNormals()
{
	return m_normals;
}
vector<float>& ModelLoader::GetTextureVertices()
{
	return m_textureVertices;
}