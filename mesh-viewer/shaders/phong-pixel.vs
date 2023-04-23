#version 400

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormals;

out vec3 n;
out vec3 pos;

uniform mat3 NormalMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;

void main() {
   n = normalize(NormalMatrix * vNormals);
   pos = vec3(ModelViewMatrix * vec4(vPos, 1.0));
   gl_Position = MVP * vec4(vPos, 1.0);
}
