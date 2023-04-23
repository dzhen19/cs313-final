#version 400

in vec3 _vNormal;
in vec3 local;
out vec4 FragColor;
uniform float uTime;

void main() {
   // FragColor = vec4((_vNormal + 1) / 2 * sin(uTime), 1.0f);
    float _sin = 1 + sin(uTime);
    float _cos = 1 + cos(uTime);
    vec3 normal_col = (_vNormal + 1) / 2;
    FragColor = vec4(normal_col.x * _sin, normal_col.y * _cos, normal_col.z * _cos, 1.0f);
}
