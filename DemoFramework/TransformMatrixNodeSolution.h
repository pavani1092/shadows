#pragma once

#include "TransformNodeBase.h"
#include "Matrix.h"
#include "MatrixStack.h"
#include "MathUtil.h"

using namespace Crawfis::Math;

namespace Crawfis
{
	namespace Graphics 
	{
		//
		// A concrete implementation of the ITransformNode.
		// This implementation is a heavy implementation, providing
		// support for a wide variety of affine transformations.
		//
		class TransformMatrixNodeSolution : public TransformNodeBase
		{
		public:
			//
			// Constructor. 
			//
			TransformMatrixNodeSolution(std::string name, ISceneNode* subject) : TransformNodeBase(name, subject)
			{
				matrix.identity();
				nameT = name;
			}
			// 
			// Apply the transform. This implementation will use the OpenGL
			// state machine to track transformations.
			//
			virtual void Apply()

			{
				Matrix4 transform = MatrixStack::modelViewStack.top() * matrix;
				MatrixStack::modelViewStack.push(transform);
				
				Matrix4 transformlight = MatrixStack::depthmodelViewStack.top()*matrix;
				MatrixStack::depthmodelViewStack.push(transformlight);
					
				
			
			
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
			// Translate the node
			//
			void Translate(float dx, float dy, float dz)
			{
				//std::cout << " trans " << nameT << std::endl;
				matrix[12] += dx;
				matrix[13] += dy;
				matrix[14] += dz;
			}
			//
			// Scale the node
			//
			void Scale(float sx, float sy, float sz)
			{
				//std::cout << " scale " << nameT << std::endl;
				Matrix4 scaleMatrix;
				scaleMatrix.identity();
				scaleMatrix[0] = sx;
				scaleMatrix[5] = sy;
				scaleMatrix[10] = sz;
				// Post-transform for now. Need a switch on whether
				// to pre or post multiply. This will work for the trackball
				matrix = scaleMatrix * matrix;
				//matrix = matrix * scaleMatrix;
			}
			//
			// Rotate the node
			//
			void Rotate(float angle, Vector3 axis)
			{
				//std::cout << " rot " << nameT << std::endl;
				axis.normalize();
				float cosValue = cosf(angle);
				float sinValue = sinf(angle);

				Matrix4 rotationMatrix;
				rotationMatrix[0] = cosValue + (1 - cosValue) * axis.x*axis.x;
				rotationMatrix[4] = (1 - cosValue)*axis.y*axis.x - sinValue*axis.z;
				rotationMatrix[8] = (1 - cosValue)*axis.z*axis.x + sinValue*axis.y;

				rotationMatrix[1] = (1 - cosValue)*axis.y*axis.x + sinValue*axis.z;
				rotationMatrix[5] = cosValue + (1 - cosValue)*axis.y*axis.y;
				rotationMatrix[9] = (1 - cosValue)*axis.z*axis.y - sinValue*axis.x;

				rotationMatrix[2] = (1 - cosValue)*axis.z*axis.x - sinValue*axis.y;
				rotationMatrix[6] = (1 - cosValue)*axis.z*axis.y + sinValue*axis.x;
				rotationMatrix[10] = cosValue + (1 - cosValue)*axis.z*axis.z;
				// Post-transform for now. Need a switch on whether
				// to pre or post multiply. This will work for the trackball
				matrix = rotationMatrix * matrix;
				
				//matrix = matrix * rotationMatrix;
				
			}

		private:
			Matrix4 matrix;
			std::string nameT;
		};
	}
}