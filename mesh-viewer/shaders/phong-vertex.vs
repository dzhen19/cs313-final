#version 400

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormals;
layout(location = 2) in vec2 vTextureCoords;

out vec3 LightIntensity;
out vec2 uv;
out vec2 local;

struct LightInfo {
   vec4 Position;
   vec3 La;
   vec3 Ld;
   vec3 Ls;
};

struct MaterialInfo {
   vec3 Ka;
   vec3 Kd;
   vec3 Ks;
   float f;
};

uniform LightInfo Light;
uniform MaterialInfo Material;

uniform mat3 NormalMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform bool HasUV;

void main() {
   uv = vTextureCoords;
   local = vPos.xy;

   // normal of the vertex 
   vec3 n = normalize(NormalMatrix * vNormals);
   vec4 eyeCoords = ModelViewMatrix * vec4(vPos, 1.0);
   vec3 v = normalize(-eyeCoords.xyz);
   // vector representing the light source
   vec3 s = normalize(vec3(Light.Position - eyeCoords));
   float sDotN = max(dot(s, n), 0.0);
   vec3 r = reflect(-s, n); 

   // if angle between light and surface normal is obtuse, we are lighting the back. Don't light
   vec3 specular = vec3(0.0);
   if(sDotN > 0.0) {
      // ask aline why this isn't working
      // specular = Light.Ls * Material.Ks * pow(dot(r, v), Material.f);
      specular = Light.Ls * Material.Ks * pow(dot(r, v), 3.0);
   }

   // intensity is proportional to dot product of normal and light source vector
   vec3 diffuse = Light.Ld * Material.Kd * sDotN;
   // uniformly reflected light
   vec3 ambient = Light.La * Material.Ka;

   LightIntensity = diffuse + ambient + specular;
   gl_Position = MVP * vec4(vPos, 1.0);
}
