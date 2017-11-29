#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "ModelProperties.h"
#include "Win32OpenGL.h"
#include "Model.h"
#include "Log.h"

/** Model Handler Class */
class ModelHandler
{
public:
	ModelHandler();
	~ModelHandler();

	void Load(); ///< Load file into memory.
	void Draw(GLuint program);
	void Update();

private:
	void LoadModel(size_t &index);

	vector<ModelProperties> m_modelProps;
	map<string, Model> m_loadedModels;

	string m_modelPath = "resources//map.txt";
};

