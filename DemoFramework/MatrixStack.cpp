#include "MatrixStack.h"
#include <map>

		std::stack<Matrix4> Crawfis::Graphics::MatrixStack::modelViewStack;
		std::stack<Matrix4> Crawfis::Graphics::MatrixStack::projectionStack;
		std::stack<Matrix4> Crawfis::Graphics::MatrixStack::depthmodelViewStack;
		std::stack<Matrix4> Crawfis::Graphics::MatrixStack::orthographicStack;
		bool Crawfis::Graphics::MatrixStack::view;
		bool  Crawfis::Graphics::MatrixStack::state;
		int  Crawfis::Graphics::MatrixStack::pos;
		std::map<int, Matrix4> Crawfis::Graphics::MatrixStack::dMVPMap;