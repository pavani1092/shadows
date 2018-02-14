#version 330


uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 normalMatrix;
uniform mat4 depthMVP;

layout(location = 0) in vec3 inPosition;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoords;

out vec4 v_position;
void main(){
	gl_Position =  depthMVP * vec4(inPosition,1.0);
	v_position = gl_Position;
}

