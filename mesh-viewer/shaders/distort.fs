#version 400

in vec3 _vNormal;
out vec4 FragColor;

void main() {
   FragColor = vec4((_vNormal + 1) / 2, 1.0f);
}
