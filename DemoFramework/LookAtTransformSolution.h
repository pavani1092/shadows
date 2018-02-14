#pragma once

#include "OpenGL.h"

#include "TransformNodeBase.h"
#include "Vector.h"
#include "MatrixStack.h"
#include "MathUtil.h"

using namespace Crawfis::Math;

namespace Crawfis
{
	namespace Graphics 
	{
		//
		// A concrete implementation of the ITransformNode.
		// This implementation simply mulitplies the frustum 
		// defined by the current projection transform.
		//
		// This class is immutable.
		//
		class LookAtTransformSolution : public TransformNodeBase
		{
		public:
			//
			// Constructor. 
			//
			LookAtTransformSolution(std::string name, ISceneNode* subject) 
				: TransformNodeBase(name, subject)
			{
				this->eyePosition = Crawfis::Math::Vector3(10,0,0);
				this->centerOfInterest = Crawfis::Math::Vector3(0,0,0);
				this->viewUp = Crawfis::Math::Vector3(0,1,0);
			}
			LookAtTransformSolution(std::string name, ISceneNode* subject,
				Vector3& eyePosition, Vector3& centerOfInterest, Vector3& viewUp) 
				: TransformNodeBase(name, subject)
			{
				this->eyePosition = eyePosition;
				this->centerOfInterest = centerOfInterest;
				this->viewUp = viewUp;
			}
			LookAtTransformSolution(std::string name, ISceneNode* subject,
				Vector3& lightinvdir)
				: TransformNodeBase(name, subject)
			{
				this->eyePosition = Crawfis::Math::Vector3(10, 0, 0);
				this->centerOfInterest = Crawfis::Math::Vector3(0, 0, 0);
				this->viewUp = Crawfis::Math::Vector3(0, 1, 0);
				this->lightPosition = lightinvdir;
			}
			// 
			// Apply the transform.
			//
			virtual void Apply()
			{
				Vector3 forward = eyePosition - centerOfInterest;
				forward.normalize();
				Vector3 side = viewUp.cross(forward);
				side.normalize();
				Vector3 up = forward.cross(side);
				Matrix4 modelView;
				modelView.identity();
				modelView[0] = side.x;		modelView[1] = up.x;		modelView[2] = forward.x;
				modelView[4] = side.y;		modelView[5] = up.y;		modelView[6] = forward.y;
				modelView[8] = side.z;		modelView[9] = up.z;		modelView[10] = forward.z;
				modelView[12] = -side.x * eyePosition.x - side.y * eyePosition.y - side.z * eyePosition.z;
				modelView[13] = -up.x * eyePosition.x - up.y * eyePosition.y - up.z * eyePosition.z;
				modelView[14] = -forward.x * eyePosition.x - forward.y * eyePosition.y - forward.z * eyePosition.z;

				forward = lightPosition - centerOfInterest;
				forward.normalize();
				Vector3 lside = viewUp.cross(forward);
				lside.normalize();
				up = forward.cross(lside);
				Matrix4 depthmodelView;
				depthmodelView.identity();
				depthmodelView[0] = lside.x;		depthmodelView[1] = up.x;		depthmodelView[2] = forward.x;
				depthmodelView[4] = lside.y;		depthmodelView[5] = up.y;		depthmodelView[6] = forward.y;
				depthmodelView[8] = lside.z;		depthmodelView[9] = up.z;		depthmodelView[10] = forward.z;
				depthmodelView[12] = -lside.x * lightPosition.x - lside.y * lightPosition.y - lside.z * lightPosition.z;
				depthmodelView[13] = -up.x * lightPosition.x - up.y * lightPosition.y - up.z * lightPosition.z;
				depthmodelView[14] = -forward.x * lightPosition.x - forward.y * lightPosition.y - forward.z * lightPosition.z;


				Matrix4 mv = MatrixStack::modelViewStack.top() * modelView;
				
				Matrix4 dmv = MatrixStack::depthmodelViewStack.top()*depthmodelView;
				MatrixStack::depthmodelViewStack.push(dmv);
				MatrixStack::modelViewStack.push(mv);
				
			
			}
			// 
			// Remove or undo the application of the transform. 
			//
			virtual void Unapply()
			{
				MatrixStack::modelViewStack.pop();
				MatrixStack::depthmodelViewStack.pop();
			}
			//
			// Overrrided from ITransformNode
			//
			virtual std::string ToString()
			{
				std::string name = this->TransformNodeBase::ToString();
				// TODO: add more information about this transform node
				//   to the string.
				return name;
			}

		private:
			Crawfis::Math::Vector3 eyePosition;
			Crawfis::Math::Vector3 centerOfInterest;
			Crawfis::Math::Vector3 viewUp;
			Crawfis::Math::Vector3 lightPosition;
			
		};
	}
}