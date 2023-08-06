#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;

out vec2 texCoord;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection ;


void main() {
    gl_Position = projection  * model * vec4(vertexPosition, 1.0);
    texCoord = vertexTexCoord;
    normal = vec3(view * vec4(vertexNormal, 0.0));
}
