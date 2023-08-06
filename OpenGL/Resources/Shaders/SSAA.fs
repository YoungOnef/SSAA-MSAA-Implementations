// Specify GLSL version
#version 330

// Declare uniforms
uniform sampler2D texture0;
uniform vec2 texOffset;
uniform vec2 resolution;
uniform int AASamples;

// Declare input variable
in vec2 texCoord;

// Declare output variable
layout (location=0) out vec4 fragColour;

void main(void) {
	// Calculate the size of each pixel on the screen
	float tx = 1.0 / resolution.x;
	float ty = 1.0 / resolution.y;
	vec2 texelSize = vec2(tx, ty);

	// Initialize outColour as black
	vec3 outColour = vec3(0.0);
	
	// Calculate the amount of samples taken by halving the AASamples uniform
	int amountofCount = AASamples / 2;

	// Loop over each anti-aliasing sample
	for(int x = -amountofCount; x <= amountofCount; ++x)
	{
		for(int y = -amountofCount; y<= amountofCount; ++y)
		{
			// Sample the texture at the offset from the current texCoord and add the resulting color to outColour
			outColour += texture(texture0, texCoord + vec2(x,y) * texelSize).rgb;
		}
	}
	
	// Calculate the divider as the square of AASamples plus one
	float divider = pow(AASamples+1, 2);

	// Assign the resulting color to fragColour with an alpha value of 1.0
	fragColour = vec4(outColour / divider, 1.0f); 
}
