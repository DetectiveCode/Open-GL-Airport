#include "stdafx.h"
#include "ModelHandler.h"

ModelHandler::ModelHandler()
{
}

ModelHandler::~ModelHandler()
{
}

void ModelHandler::Load()
{
	if (m_modelProps.size() > 0)
	{
		m_modelProps.clear();
	}

	fstream mapfile(m_modelPath, ios_base::in);
	string line = "";

	if (mapfile.is_open())
	{
		while (getline(mapfile, line))
		{
			istringstream iss(line);

			if (line.length() > 0 && line[0] != '#')
			{
				ModelProperties mProperty;

				iss >> mProperty.m_ModelFile;
				iss >> mProperty.m_BMPFile;
				iss >> mProperty.m_xAngle;
				iss >> mProperty.m_yAngle;
				iss >> mProperty.m_zAngle;
				iss >> mProperty.m_xTranslate;
				iss >> mProperty.m_yTranslate;
				iss >> mProperty.m_zTranslate;
				iss >> mProperty.m_xScale;
				iss >> mProperty.m_yScale;
				iss >> mProperty.m_zScale;

				m_modelProps.push_back(mProperty);
			}
		}
	}

	for (size_t i = 0; i < m_modelProps.size(); i++)
	{
		try
		{
			m_loadedModels.at(m_modelProps.at(i).m_ModelFile);
		}
		catch (const std::out_of_range&)
		{
			LoadModel(i);
		}
	}
}

void ModelHandler::Draw(GLuint program)
{
	if (m_modelProps.size() > 0)
	{
		for (size_t i = 0; i < m_modelProps.size(); i++)
		{
			try
			{
				m_loadedModels[m_modelProps[i].m_ModelFile].m_properties = m_modelProps[i];
				m_loadedModels[m_modelProps[i].m_ModelFile].DrawModel(program);
			}
			catch (const std::exception&)
			{
				Log::AppendToLogFile("An unknown error occured drawing model.");
			}
			catch (const std::out_of_range&)
			{
				Log::AppendToLogFile("Model could not be found, failed to draw.");
			}
		}
	}
}

void ModelHandler::Update()
{
	//we will only update the skybox
	//if anything else needs to be updated, this system needs to be expanded upon

	if (m_modelProps.size() > 0)
	{
		if (m_modelProps[0].m_ModelFile == "skybox.obj")
		{
			if (m_modelProps[0].m_yAngle == 360)
			{
				m_modelProps[0].m_yAngle = 0;
			}
			else
			{
				m_modelProps[0].m_yAngle += 0.01;
			}
		}
	}
}

void ModelHandler::LoadModel(size_t &index)
{
	string &modelfile = m_modelProps.at(index).m_ModelFile;
	string &bmpfile = m_modelProps.at(index).m_BMPFile;

	Model model(modelfile, bmpfile);
	model.CreateModel();

	m_loadedModels[modelfile] = model;
}