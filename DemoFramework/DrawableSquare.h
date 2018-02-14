#pragma once
#pragma once

#include "OpenGL.h"
#include "IDrawable.h"
#include "Color.h"

namespace Crawfis
{
	namespace Graphics
	{
		class DrawableSquare : public IDrawable
		{
		public:
			DrawableSquare()
			{
			}
			virtual void Draw()
			{
				if (!created)
					CreateCube();
				int error = glGetError();
				//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glBindVertexArray(vaoCube);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				error = glGetError();
			}
		private:
			void CreateCube()
			{
				// Create a Vertex Array Object to organize all of the bindings.
				glGenVertexArrays(1, &vaoCube);
				glBindVertexArray(vaoCube);

				// Steps:
				//  1) Create buffer for vertex positions
				//  2) Set vertex positions to be sent to the shaders at slot 0.
				//  3) Create buffer for colors
				//  4) Set colors to be sent to the shaders at slot 1.
				//  5) Create buffer for indices of the cube.
				//
				// Vertex positions
				// Allocate Vertex Buffer Object (get a handle or ID)
				glGenBuffers(1, &vboVertices);
				// VBO for vertex data
				glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
				// Set the model data into the VBO.
				glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), &positions[0], GL_STATIC_DRAW);
				// Define the layout of the vertex data.
				// This also set's the vertex array's location for slot 0.
				glVertexAttribPointer((GLuint)0, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);

				glGenBuffers(1, &vboTex);
				// VBO for vertex data
				glBindBuffer(GL_ARRAY_BUFFER, vboTex);
				// Set the model data into the VBO.
				glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), &texcoords[0], GL_STATIC_DRAW);
				// Define the layout of the vertex data.
				// This also set's the vertex array's location for slot 0.
				glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(1);




				glBindVertexArray(0);
				created = true;
			}
		private:
			bool created = false;
			unsigned int vaoCube, vboVertices, vboTex, vboIndices;

			const float positions[6 * 2] = {
				// Front face
				-1.0, -1.0,
				1.0, -1.0,
				1.0, 1.0,

				-1.0, -1.0,
				-1.0, 1.0,
				1.0, 1.0,
			};
			const float texcoords[12] = {
				0.0, 0.0,
				1.0, 0.0,
				1.0, 1.0,
				0.0, 0.0,
				0.0, 1.0,
				1.0, 1.0
			};


		};
		
	}
}
	
