#pragma once
#include "IShaderConstant.h"
#include "ShaderConstantMatrix4.h"
#include "MatrixStack.h"
#include "MathUtil.h"
#include <string>

using namespace Crawfis::Math;
namespace Crawfis
{
	namespace Graphics
	{
		class ShaderConstantdepthModelViewProj : public IShaderConstant
		{
		public:
			ShaderConstantdepthModelViewProj(std::string constantName) : realConstant(constantName)
			{
			}

			~ShaderConstantdepthModelViewProj()
			{
			}
			//
			//
			// Note that there is no shader behavior that is publically callable. This 
			// object is not called directly in the rendering. As an IShaderConstant, 
			// it will be called by the ShaderProgram it is associated with
			//
		protected:
			//
			// Set-up any logic after a shader program has linked.
			// These should not be called by the user.
			//
			virtual void OnLink(unsigned int shaderID)
			{
				realConstant.OnLink(shaderID);
			}
			//
			// Called once a Shader Program has been enabled (glUseProgram)
			// These should not be called by the user.
			//
			virtual void OnShaderEnabled(unsigned int shaderID)
			{
				int pos = MatrixStack::pos;
				
				Matrix4 depthmodelView = MatrixStack::depthmodelViewStack.top();
				Matrix4 projection = MatrixStack::orthographicStack.top();
				Matrix4 dmvp = projection * depthmodelView;
				if (MatrixStack::state == true ) {
					MatrixStack::dMVPMap[pos % 11] = dmvp;
				}
				if (MatrixStack::view == false) {// camera
					dmvp = MatrixStack::dMVPMap[pos%11];
				}
				realConstant.setValue(dmvp);
				realConstant.OnShaderEnabled(shaderID);
				MatrixStack::pos++;
			}
		private:
			ShaderConstantMatrix4 realConstant;
		};
	}
}
