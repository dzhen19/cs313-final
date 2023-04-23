#version 400

out vec4 FragColor;
in vec3 LightIntensity;
in vec2 uv;
in vec2 local;
uniform sampler2D image;

void main() {
   // vec4 texColor = texture(image, uv);
   vec4 texColor = texture(image, local);
   FragColor = vec4(LightIntensity, 1.0) * texColor;
}
