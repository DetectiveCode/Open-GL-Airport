#pragma once
#include <iostream>

class ModelProperties
{
public:
	std::string m_ModelFile;
	std::string m_BMPFile;

	float m_xAngle{ 0.0f };
	float m_yAngle{ 0.0f };
	float m_zAngle{ 0.0f };

	float m_xTranslate{ 0.0f };
	float m_yTranslate{ 0.0f };
	float m_zTranslate{ 0.0f };

	float m_xScale{ 1.0f };
	float m_yScale{ 1.0f };
	float m_zScale{ 1.0f };
};