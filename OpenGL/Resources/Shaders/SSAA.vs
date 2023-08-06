// Declare a uniform variable to hold the model-view-projection matrix
uniform mat4 mvpMatrix;

// Declare input variables for vertex position and texture coordinates
layout (location=0) in vec4 vertexPos;
layout (location=2) in vec2 vertexTexCoord;

// Declare output variable for interpolated texture coordinates
out vec2 texCoord;

void main(void)
{
    // Assign texture coordinates to output variable
    texCoord = vertexTexCoord;
    
    // Transform vertex position to clip space using the mvpMatrix
    gl_Position = mvpMatrix * vertexPos;
}