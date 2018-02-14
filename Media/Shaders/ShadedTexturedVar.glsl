#version 330


uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 normalMatrix;
uniform mat4 depthMVP;

layout(location = 0) in vec3 inPosition;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoords;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBiTangent;

out vec2 uvCoords;
out vec3 ecPosition3;
out vec3 outNormal;
out vec4 shadowCoord;
uniform sampler2D normalSampler;
uniform sampler2D OcclusionMap;

void main()
{

	mat4 offset = mat4(
      vec4(0.5f, 0.0f, 0.0f, 0.0f),
      vec4(0.0f, 0.5f, 0.0f, 0.0f),
      vec4(0.0f, 0.0f, 0.5f, 0.0f),
      vec4(0.5f, 0.5f, 0.5f, 1.0f)
   );

	// Calculate the eye position
	vec4 position = vec4(inPosition, 1);
	vec4 ecPosition = modelViewMatrix * position;
	ecPosition3 = (vec3(ecPosition)) / ecPosition.w;
	// transform vertex to clip space coordinates
	gl_Position = modelViewProjectionMatrix * position;
	
	shadowCoord=offset*depthMVP*position;
	
	// Calculate normal;
	vec4 normalMap=2*texture2D(normalSampler,inTexCoords)-1.0;
	normalMap.w=0.0;
	outNormal = mat3(normalMatrix) * normalMap.xyz;
	// Set the color
	uvCoords = inTexCoords;
}
