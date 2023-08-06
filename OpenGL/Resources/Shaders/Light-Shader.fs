#version 330 core
// Define input variables
in vec2 texCoord;
in vec3 normal;
// Define output color value
out vec4 fragColor;
// Define texture sampler uniform
uniform sampler2D textureSampler;
// Define DirectionalLight struct
struct DirectionalLight {
    vec3 direction;
    vec3 color;
};
// Define PointLight struct
struct PointLight {
    vec3 position;
    vec3 color;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float spotExponent;
    float spotCutoff;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};

void calculateDirectionalLight(DirectionalLight light, vec3 diffuse, vec3 n, inout vec3 totalDiffuse, inout vec3 totalSpecular) {
    float intensity = max(dot(n, -light.direction), 0.0);
    totalDiffuse += light.color * diffuse * intensity;

    vec3 reflectDir = reflect(light.direction, n);
    vec3 viewDir = normalize(-vec3(0, 0, 1));
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), 32);
    totalSpecular += light.color * spec;
}

void calculatePointLight(PointLight light, vec3 diffuse, vec3 n, inout vec3 totalDiffuse, inout vec3 totalSpecular) {
    vec3 lightDir = light.position - vec3(texCoord, 0);
    float distance = length(lightDir);

    float attenuation = 1.0 / (light.constantAttenuation + light.linearAttenuation * distance + light.quadraticAttenuation * distance * distance);

    lightDir = normalize(lightDir);
    float intensity = max(dot(n, -lightDir), 0.0);

    vec3 reflectDir = reflect(lightDir, n);
    vec3 viewDir = normalize(-vec3(0, 0, 1));
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), 32);

    totalDiffuse += light.color * diffuse * intensity * attenuation;
    totalSpecular += light.color * spec * attenuation;
}

void calculateSpotLight(SpotLight light, vec3 diffuse, vec3 n, inout vec3 totalDiffuse, inout vec3 totalSpecular) {
    vec3 lightDir = light.position - vec3(texCoord, 0);
    float distance = length(lightDir);

    float attenuation = 1.0 / (light.constantAttenuation + light.linearAttenuation * distance + light.quadraticAttenuation * distance * distance);

    lightDir = normalize(lightDir);
    float intensity = max(dot(n, -lightDir), 0.0);

    float spotAngle = dot(-lightDir, normalize(light.direction));
    if (spotAngle > light.spotCutoff) {
        float spotAttenuation = pow(spotAngle, light.spotExponent);
        intensity *= spotAttenuation;

        vec3 reflectDir = reflect(lightDir, n);
        vec3 viewDir = normalize(-vec3(0, 0, 1));
        float spec = pow(max(dot(reflectDir, viewDir), 0.0), 32);

        totalDiffuse += light.color * diffuse * intensity * attenuation;
        totalSpecular += light.color * spec * attenuation;
    }
}

void main() {
    vec3 diffuse = texture(textureSampler, texCoord).rgb;
    vec3 ambient = vec3(0.1, 0.1,0.1);
    vec3 n = normalize(normal);
    vec3 totalDiffuse = vec3(0.0);
    vec3 totalSpecular = vec3(0.0);

    // First light source
    DirectionalLight light1;
    light1.direction = normalize(vec3(0.5, 0.5, 1.0));
    light1.color = vec3(1.0, 1.0, 1.0);

    calculateDirectionalLight(light1, diffuse, n, totalDiffuse, totalSpecular);

    // Second light source
    DirectionalLight light2;
    light2.direction = normalize(vec3(-0.5, -0.5, 1.0));
    light2.color = vec3(0.5, 0.5, 1.0);

    calculateDirectionalLight(light2, diffuse, n, totalDiffuse, totalSpecular);

    // Third light source
    DirectionalLight light3;
    light3.direction = normalize(vec3(0.0, -0.5, -1.0));
    light3.color = vec3(1.0, 0.5, 0.5);

    calculateDirectionalLight(light3, diffuse, n, totalDiffuse, totalSpecular);

    // Fourth light source
    PointLight light4;
    light4.position = vec3(0.0, 0.0, 2.0);
    light4.color = vec3(1.0, 1.0, 1.0);
    light4.constantAttenuation = 1.0;
    light4.linearAttenuation = 0.09;
    light4.quadraticAttenuation = 0.032;

    calculatePointLight(light4, diffuse, n, totalDiffuse, totalSpecular);

    // Fifth light source
    SpotLight light5;
    light5.position = vec3(0.0, 1.0, 0.0);
    light5.direction = normalize(vec3(-0.5, 0.0, -1.0));
    light5.color = vec3(1.0, 0.5, 0.0);
    light5.spotExponent = 32.0;
    light5.spotCutoff = cos(radians(15.0));
    light5.constantAttenuation = 1.0;
    light5.linearAttenuation = 0.09;
    light5.quadraticAttenuation = 0.032;

    calculateSpotLight(light5, diffuse, n, totalDiffuse, totalSpecular);

    fragColor = vec4(totalDiffuse + ambient + totalSpecular, 1.0);

}
