#version 400

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vUV;

// pass normal to fragment shader
out vec3 _vNormal;

uniform mat3 NormalMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform bool HasUV;
uniform float uTime;

void main() {
   _vNormal = vNormal;
   vec3 v = cross(vNormal, vec3(1, 1, 0));
   vec3 pos = vPos + v * sin(uTime) * .5;
   gl_Position = MVP * vec4(pos, 1.0);
}
