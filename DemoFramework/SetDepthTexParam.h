#pragma once

#include "ICommand.h"
#include "OpenGL.h"

namespace Crawfis
{
	namespace Graphics
	{
		class SetDepthTexParam : public Collections::IGenericCommand
		{
		public:
			SetDepthTexParam()
			{
				
			}
			virtual void Execute()
			{
				
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}
		private:
			
		};
	}
}