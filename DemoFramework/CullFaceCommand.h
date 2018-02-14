
#pragma once

#include "ICommand.h"
#include "OpenGL.h"
#include "Color.h"
#include <string>

namespace Crawfis
{
	namespace Graphics
	{
		class CullFaceCommand : public Collections::IGenericCommand
		{
		public:
			CullFaceCommand(int cmd)
			{
				this->command = cmd;
			}
			virtual void Execute()
			{
				glCullFace(command);
			}
		private:
			int command;
		};
	}
}