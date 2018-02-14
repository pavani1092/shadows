#version 330


uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 normalMatrix;
uniform mat4 depthMVP;

layout(location = 0) in vec3 inPosition;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoords;

out vec4 depthpos;

void main(){
	
	mat4 offsetMat = mat4(
      vec4(0.5f, 0.0f, 0.0f, 0.0f),
      vec4(0.0f, 0.5f, 0.0f, 0.0f),
      vec4(0.0f, 0.0f, 0.5f, 0.0f),
      vec4(0.5f, 0.5f, 0.5f, 1.0f)
   );


 gl_Position =  depthMVP * vec4(inPosition,1.0);
 depthpos=offsetMat*depthMVP * vec4(inPosition,1.0);
	
}
