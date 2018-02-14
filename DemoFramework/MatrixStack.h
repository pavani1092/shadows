#pragma once
#include <stack>
#include "Matrix.h"
#include <map>

using namespace Crawfis::Math;

namespace Crawfis
{
	namespace Graphics
	{
		class MatrixStack {
		public:
			static void Init() {
				Matrix4 m;
				modelViewStack.push(m);
				projectionStack.push(m);
				depthmodelViewStack.push(m);
				orthographicStack.push(m);
				view = true;
				state = true;
				pos = 0;
			}
			static std::stack<Matrix4> modelViewStack;
			static std::stack<Matrix4> projectionStack;
			static std::stack<Matrix4> depthmodelViewStack;
			static std::stack<Matrix4> orthographicStack;
			static std::map<int, Matrix4> dMVPMap;
			static bool view;
			static bool state;
			static int pos;
		};
	}
}

