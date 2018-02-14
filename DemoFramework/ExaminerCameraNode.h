#pragma once

#include "ITransformNode.h"
#include "IKeyboardHandler.h"
#include "IMouseHandler.h"
#include "PerspectiveTransformSolution.h"
#include "TransformMatrixNodeSolution.h"
#include "LookAtTransformSolution.h"
#include "Trackball.h"

using namespace Crawfis::Controller;

namespace Crawfis
{
	namespace Graphics 
	{
		enum MouseState { NONE, TRACKBALL, ZOOM, PAN };
		
		//
		// A composite ITransformNode used for camera set-up and
		// user-interaction, such as an examiner style manipulation.
		// Also supports the IKeyboardHandler and IMouseHandler interfaces.
		//
		class ExaminerCameraNode : public ITransformNode, public IKeyboardHandler, public IMouseHandler
		{
		public:
			
			//
			// Constructor. 
			//
			ExaminerCameraNode(std::string name, ISceneNode* subject,Vector3 lightinvdir) : dollyDelta(0.1f)
			{
				// Final transform = Perspective * M4 * M3 * M2 * M1 * Camera * Scene
				// M1 and M3 indicate the distance along the view ray to the center of rotation.
				// M2 is the rotation node used in the mouse trackball and the 
				// M4 can bring the camera closer or further from the scene.
				this->name = name;
				rotationDepth = 10;
				LookAtTransformSolution* camera = new LookAtTransformSolution("LookAt", subject,lightinvdir);
				TransformMatrixNodeSolution* preTranslate = new TransformMatrixNodeSolution("PreTranslate", camera);
				preTranslate->Translate(0,0,rotationDepth);
				
				//LookAtTransformLight* light = new LookAtTransformLight("LookAtPOVLight", subject,Vector3(10,0,0),Vector3(0,0,0),Vector3 (0,1,0));
				//TransformMatrixNodeSolution* preTranslatel = new TransformMatrixNodeSolution("PreTranslate", light);
				//preTranslatel->Translate(0, 0, rotationDepth);

				examinerTransform = new TransformMatrixNodeSolution(name, preTranslate);
				//lightexaminerTransform = new TransformMatrixNodeSolution("LightExaminer", preTranslatel);

				TransformMatrixNodeSolution* postTranslate = new TransformMatrixNodeSolution("PostTranslate", examinerTransform);
				postTranslate->Translate(0,0,-rotationDepth);
				
				//TransformMatrixNodeSolution* postTranslatel = new TransformMatrixNodeSolution("PostTranslate", lightexaminerTransform);
				//postTranslatel->Translate(0, 0, -rotationDepth);

				
				dollyNode = new TransformMatrixNodeSolution("Dolly", postTranslate);
				//dollyNodel = new TransformMatrixNodeSolution("Dolly", postTranslatel);

				perspectiveNode = new PerspectiveTransformSolution("Perspective", dollyNode);
				//perspectiveNodel = new PerspectiveTransformSolution("Perspective", dollyNodel);

				trackball = new Crawfis::Controller::Trackball(examinerTransform);
				//trackball2 = new Crawfis::Controller::Trackball(lightexaminerTransform);

				this->rootNode = perspectiveNode;
				
			}
			void setWidth(int width)
			{
				windowWidth = width;
				perspectiveNode->setWidth(width);
				//perspectiveNodel->setWidth(width);
			}
			void setHeight(int height)
			{
				windowHeight = height;
				perspectiveNode->setHeight(height);
				//perspectiveNodel->setHeight(height);
			}
			//
			// Overrided from ISceneNode
			//
			virtual void Accept(IVisitor* visitor)
			{
				visitor->Visit(this);
			}
			//
			// Traverse the child node.
			//
			void AcceptSubject(IVisitor* visitor)
			{
				rootNode->Accept(visitor);
				
		
			}
			// 
			// Apply the transform.
			//
			virtual void Apply() { }
			// 
			// Remove or undo the application of the transform.
			// This is a no-op here, as the real transforms are in
			// the composite.
			//
			virtual void Unapply() { }
			//
			// Overrrided from ISceneNode
			//
			virtual std::string ToString()
			{
				return name;
			}
			//
			// Handle the main keyboard key presses. 
			// Called on both key down and key up.
			//
			virtual void KeyPress(unsigned char key, int x, int y)
			{
				MatrixStack::state = false;
				switch(key)
				{
					case 'z':
						// TODO: Dolly (zoom in) by translating a positive dollyDelta.
						dollyNode->Translate(0,0,dollyDelta);
						//dollyNodel->Translate(0, 0, dollyDelta);
						break;
					case 'Z':
						// TODO: Dolly (zoom out) by translating a negative dollyDelta.
						dollyNode->Translate(0,0,-dollyDelta);
						//dollyNodel->Translate(0, 0, -dollyDelta);
						break;
					case'c':
						std::cout << "Camera View" << std::endl;
						MatrixStack::view = false;
						break;
					case'l':
						std::cout << "Light View" << std::endl;
						MatrixStack::view = true;
						break;
				}
			}
			//
			// Handle the arrow keys and numeric keypad.
			//
			virtual void NumPadPress(int key, int x, int y)
			{
				MatrixStack::state = false;
				switch(key)
				{
					case GLUT_KEY_LEFT:
						// TODO: Pan the camera to the left (translate examinerTransform)
						examinerTransform->Translate(-crabAmount, 0.0f, 0.0f);
						//lightexaminerTransform->Translate(-crabAmount, 0.0f, 0.0f);
						break;
					case GLUT_KEY_RIGHT:
						// TODO: Pan the camera to the right (translate examinerTransform)
						examinerTransform->Translate(crabAmount, 0.0f, 0.0f);
						//lightexaminerTransform->Translate(crabAmount, 0.0f, 0.0f);
						break;
					case GLUT_KEY_UP:
						// TODO: Pan the camera up (translate examinerTransform)
						examinerTransform->Translate(0.0f, -crabAmount, 0.0f);
						//lightexaminerTransform->Translate(0.0f, -crabAmount, 0.0f);
						break;
					case GLUT_KEY_DOWN:
						// TODO: Pan the camera down (translate examinerTransform)
						examinerTransform->Translate(0.0f, crabAmount, 0.0f);
						//lightexaminerTransform->Translate(0.0f, crabAmount, 0.0f);
						break;
					
				}
			}
			//
			// Handle any mouse button events, both press and release.
			//
			virtual void MouseEvent(int button, int state, int ix, int iy)
			{
				if( button == GLUT_LEFT_BUTTON)
				{
					MatrixStack::state = false;
					float x, y;
					MapMouse(ix,iy,x,y);
					if( state == GLUT_DOWN)
					{
						currentMouseState = TRACKBALL;
						trackball->BeginManipulation(x, y);
						//trackball2->BeginManipulation(x, y);
					}
					else
					{
						currentMouseState = NONE;
						trackball->EndManipulation(x, y);
						//trackball2->EndManipulation(x, y);
					}
				}
			}
			//
			// Handle any mouseMove events. Note, with GLUT this 
			// is only called when a mouse button is pressed by
			// default.
			//
			virtual void MouseMoved(int ix, int iy)
			{
				if( currentMouseState == TRACKBALL )
				{
					float x, y;
					MapMouse(ix,iy,x,y);
					trackball->ManipulateTo(x,y);
					//trackball2->ManipulateTo(x, y);
				}
			}

		private:
			void MapMouse(int ix, int iy, float& x, float &y)
			{
				x = (float) ix / (float) windowWidth; // map to 0 to 1
				x = 2*x - 1; // map to -1 to 1
				y = (float) iy / (float) windowHeight; // map to 0 to 1
				y = 1 - 2*y; // map to 1 to -1 (flip the window coordinates)
			}

		private:
			std::string name;
			ITransformNode* rootNode;

			float rotationDepth;
			PerspectiveTransformSolution* perspectiveNode;
			TransformMatrixNodeSolution* dollyNode;

			TransformMatrixNodeSolution* examinerTransform;
			const float dollyDelta;
			Crawfis::Controller::Trackball* trackball;

			MouseState currentMouseState;
			int windowWidth, windowHeight;
			float crabAmount = 0.2f;
			int state = 0;
		};
	}
}