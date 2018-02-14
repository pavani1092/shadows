#version 330 core
// Fragment Shader – file "backdrop.frag"
//   - Should be called either solidColor or Position

in vec4 vertColor;
out vec4 out_Color;
uniform vec2 WindowSize;
float sigma = log(256.0)/0.25;
void main(void)
{
	const float xScale = 1.0 / 40.0;
       	    const float yScale = 1.0 / 40.0;
       	    const vec4 scarlet = vec4(0.733, 0.0, 0.0, 1.0);
            const vec4 grey = vec4(0.4, 0.4, 0.4, 1.0);
       	    float x =  floor(xScale*gl_FragCoord.x);
       	    float y =  floor(yScale*gl_FragCoord.y);
            if(mod(x+y,2.0) == 1.0)
           	  gl_FragColor = scarlet;
            else
              gl_FragColor = grey;
	
}


