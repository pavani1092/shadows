#version 330

struct BasicMaterial
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

const int maxLights = 2;
uniform vec4 lightPositions[maxLights];
uniform vec4 lightColors[maxLights];
uniform vec3 lightAmbient = vec3(0.1);
uniform BasicMaterial frontMaterial;

uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 normalMatrix;

uniform sampler2D shadowSampler;
uniform sampler2D OcclusionMap;


in vec2 uvCoords;
in vec3 ecPosition3;
in vec3 outNormal;
in vec4 shadowCoord;
out vec4 fragColor;

void PointLight(in int i, in vec3 eye, in vec3 ecPosition3, in vec3 normal,
	inout vec4 diffuse, inout vec4 specular)
{
	// Compute vector from surface to light position
	vec4 lightPosInCamera = modelViewMatrix * lightPositions[i];
	vec3 lightDir = normalize(vec3(lightPosInCamera) - ecPosition3);
	// Compute distance between surface and light position
	float lightDistance = length(lightDir);
	lightDir = lightDir / lightDistance;
	// Compute attenuation
	float attenuation = 1.0;
	vec3 viewDir = normalize(-ecPosition3);
	float nDotL = dot(normal, lightDir);
	vec3 reflectionDir = normalize(2*nDotL*normal - lightDir);
	nDotL = max(0.0, nDotL);
	float vDotR = max(0.0, dot(viewDir, reflectionDir));
	float specularAmount = 0;
	if (nDotL > 0.0)
		specularAmount = pow(vDotR, frontMaterial.shininess);
	diffuse += lightColors[i] * nDotL * attenuation;
	specular += lightColors[i] * specularAmount * attenuation;
}


void DirectionalLight(in int i, in vec3 normal, in vec3 ecPosition3,
	inout vec4 diffuse, inout vec4 specular)
{
	vec3 lightDir = normalize(mat3(normalMatrix) * normalize(lightPositions[i].xyz));
	vec3 viewDir = normalize(-ecPosition3);
	float nDotL = dot(normal, lightDir);
	//vec3 reflectionDir = normalize(2*nDotL*normal - lightDir);
	vec3 reflectionDir = normalize(viewDir + lightDir);
	nDotL = max(0.0, nDotL);
	//float vDotR = max(0.0, dot(viewDir, reflectionDir));
	float nDotH = max(0.0, dot(normal, reflectionDir));
	float specularAmount = 0;
	if (nDotL > 0.0)
		specularAmount = pow(nDotH, frontMaterial.shininess);
	diffuse += lightColors[i] * nDotL;
	specular += lightColors[i] * specularAmount;
	specular += lightColors[i] * specularAmount;
}




void main()
{
	vec4 ambient = vec4(lightAmbient, 1.0);
	vec4 occ=texture2D(OcclusionMap,uvCoords);
	if(occ.x>0)
		ambient=ambient*occ;
	
	vec4 diffuse = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 specular = vec4(0.0, 0.0, 0.0, 0.0);

	vec3 eye = vec3(0, 0, 1);
	vec3 normal = normalize(outNormal);
	for (int i = 0; i <maxLights; i++)
	{
		if (lightPositions[i].w <= 0.1)
			DirectionalLight(i, normal, ecPosition3, diffuse, specular);
		else
			PointLight(i, eye, ecPosition3, normal, diffuse, specular);
	}
	// Set the color
	
	
	
	
	
	vec4 lightPosInCamera = modelViewMatrix * lightPositions[1];
	vec3 lightDir = normalize(vec3(lightPosInCamera) - ecPosition3);
	float cosTheta = dot(normal,lightDir);
	float visibility=1.0;
	float bias = 0.005*tan(acos(cosTheta)); // to get smoother edges
	bias = clamp(bias, 0,0.01);
	if(cosTheta>0 && (texture2D( shadowSampler, shadowCoord.xy ).z<shadowCoord.z-bias)){
		visibility=0.5;
	}
	

/*	vec4 shadowCoordinateWdivide = shadowCoord / shadowCoord.w ;
	shadowCoordinateWdivide.z += 0.0005;	
	float distanceFromLight = texture2D(shadowSampler,shadowCoordinateWdivide.st).z;
	 if (shadowCoord.w > 0.0)
	 	visibility = distanceFromLight < shadowCoordinateWdivide.z+0.0005 ? 0.5 : 1.0 ;*/



	
	fragColor = ambient* frontMaterial.ambient
		+ visibility*diffuse * frontMaterial.diffuse
		+ visibility*specular*frontMaterial.specular;
	//fragColor = decalColor;
	//fragColor.rgb = ecPosition3.xyz;
	
	fragColor.a = 1;
}
