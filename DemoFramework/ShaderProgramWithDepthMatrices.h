#pragma once

#include "OpenGL.h"

#include "ShaderProgram.h"
#include "ShaderConstantDepthModelViewProj.h"
#include "ShaderConstantCollection.h"
#include <string>

namespace Crawfis
{
	namespace Graphics
	{
		//
		// Concrete implementation of IShaderProgram.
		// This is an immutable class.
		//
		class ShaderProgramWithDepthMatrices : public ShaderProgram
		{
		public:
			//
			// Constructor. A constructor for the simple case of a single vertex 
			// and a single fragment shader (no geometry shader).
			//
			ShaderProgramWithDepthMatrices(VertexRoutine* vertexShader, FragmentRoutine* fragmentShader, IShaderConstant* shaderConstant = 0)
				: ShaderProgram(vertexShader, fragmentShader, 0)
			{
				setShaderConstant(shaderConstant);
			}
			//
			// Constructor. A constructor for the simple case of a single vertex, geometry and 
			// fragment shader.
			//
			ShaderProgramWithDepthMatrices(VertexRoutine* vertexShader, GeometryRoutine* geometryShader, FragmentRoutine* fragmentShader)
				: ShaderProgram(vertexShader, geometryShader, fragmentShader)
			{
			}
			~ShaderProgramWithDepthMatrices()
			{
				if (shaderConstants) delete shaderConstants;
			}
			//
			// Set any shader constants. shaderConstant can be a list of shader constants.
			//
			virtual void setShaderConstant(IShaderConstant* shaderConstant)
			{
				if (shaderConstants) delete shaderConstants;
				shaderConstants = new ShaderConstantCollection();
				if (shaderConstant != 0)
					shaderConstants->AddConstant(shaderConstant);
				AddMatrices();
			}

		protected:
			//
			// Enable the shader.
			//
			virtual void Enable()
			{
				if (!shaderConstants)
					setShaderConstant(0);
				ShaderProgram::Enable();
			}

		private:
			void AddMatrices()
			{
				ShaderConstantDepthModelViewProj* mvpConstant = new ShaderConstantDepthModelViewProj(modelViewProjName);
				shaderConstants->AddConstant(mvpConstant);
				ShaderProgram::setShaderConstant(shaderConstants);
			}
		private:
			std::string modelViewProjName = "depthMVP";
			ShaderConstantCollection* shaderConstants = 0;
		};
	}
}