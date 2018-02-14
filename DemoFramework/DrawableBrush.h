#pragma once

#include "OpenGL.h"
#include "IDrawable.h"
#include "Vector.h"
#include <math.h>
#include <vector>
#include "Color.h"
#include "IMouseHandler.h"


namespace Crawfis
{
	namespace Graphics
	{
		class DrawableBrush : public IDrawable, public Crawfis::Controller::IMouseHandler
		{
		public:
			DrawableBrush()
			{
			}
			virtual void Draw()
			{
				if (!created)
					CreatePath();
				
				
					
				std::cout << "brush size " << brushPath.size() << std::endl;
				if (brushPath.size() <= 0 || isPainting)
					return;

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				// Draw the brush strokes
				// Bind the Vertex Array, which sets up the vertex attribute pointers.

				glBindVertexArray(vaoBrushPath);
				// Update the brush data.
				// Note that we do not need to bind the VertexAttribPointer, since we are 
				// not changing the association.
				glBindBuffer(GL_ARRAY_BUFFER, vboBrushPath);
				glBufferSubData(GL_ARRAY_BUFFER, 0, brushPath.size() * sizeof(GLfloat), &brushPath[0]);
				// Draw the Brush stroke.
				glDrawArrays(GL_POINTS, 0, brushPath.size() / 2);
				
				
			
				
				brushPath.clear();

				// Be nice and reset things back to the default.
				glBindVertexArray(0);
				std::cout << "Painting Done" << std::endl;

				//glFinish();
			}
		private:
			void CreatePath()
			{
				
				
				std::cout << "create" << std::endl;
				glGenVertexArrays(1, &vaoBrushPath);
				glBindVertexArray(vaoBrushPath);

				// Generate 1 buffer, put the resulting identifier in vertexbuffer
				glGenBuffers(1, &vboBrushPath);
				// The following commands will talk about our 'vertexbuffer' buffer
				glBindBuffer(GL_ARRAY_BUFFER, vboBrushPath);
				// Give our vertices to OpenGL.
				glBufferData(GL_ARRAY_BUFFER, Max_Points * sizeof(GLfloat), 0, GL_DYNAMIC_DRAW);


				glVertexAttribPointer((GLuint)0, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);
				glBindVertexArray(0);



				created = true;
			}

			virtual void MouseEvent(int button, int state, int ix, int iy)
			{
				if (button == GLUT_LEFT_BUTTON)
				{
					float x, y;
					MapMouse(ix, iy, x, y);
					if (state == GLUT_DOWN)
					{
						isPainting = true;
						std::cout << "painting" << std::endl;
					}
					else
					{
						isPainting = false;
						std::cout << "not painting " << brushPath.size() << std::endl;
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
				if (isPainting)
				{
					if (brushPath.size() > Max_Points) {
						std::cout << "clearing " << std::endl;
						brushPath.clear();
					}
					float x, y;
					//std::cout << "pushing "<<std::endl;
					MapMouse(ix, iy, x, y);
					//brushPath.push_back(x);
					//brushPath.push_back(y);
				}
			}
		private:
			bool created = false;
			unsigned int vaoBrushPath;
			unsigned int vboBrushPath;
			bool isPainting = false;
			std::vector<float> brushPath;
			const int Max_Points = 512;
			GLfloat g_vertex_buffer_data[8] = {
				-1.0f,1.0f,
				-1.0f,-1.0f,
				1.0f,1.0f,
				1.0f,-1.0f
			};
			void MapMouse(int ix, int iy, float& x, float &y)
			{

				x = (float)ix / (float)glutGet(GLUT_WINDOW_WIDTH); // map to 0 to 1
				x = 2 * x - 1; // map to -1 to 1
				y = (float)iy / (float)glutGet(GLUT_WINDOW_HEIGHT); // map to 0 to 1
				y = 1 - 2 * y; // map to 1 to -1 (flip the window coordinates)
				brushPath.push_back(x);
				brushPath.push_back(y);
			
			
			}



		};
	}
}