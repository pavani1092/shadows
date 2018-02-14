#version 330 core

uniform sampler2D texture;

in vec4 depthpos;
out vec4 fragColor;



void main(){
    
	vec4 depth=texture2D(texture,depthpos.xy);
	fragColor=depth;
	
}
