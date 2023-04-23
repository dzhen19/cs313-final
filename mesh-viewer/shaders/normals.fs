#version 400

in vec3 _vNormal;
in vec3 local;
out vec4 FragColor;

void main() {
   FragColor = vec4((_vNormal + 1) / 2, 1.0f);
}
