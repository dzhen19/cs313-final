#version 400

out vec4 FragColor;
in vec3 n;
in vec3 pos;

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

void main() {
   vec3 v = normalize(-pos);
   vec3 s = normalize(vec3(Light.Position) - pos);
   vec3 r = reflect(-s, n);

   float sDotN = max(dot(s, n), 0);

   vec3 diffuse = Light.Ld * Material.Kd * sDotN;
   vec3 ambient = Light.La * Material.Ka;
   vec3 specular = vec3(0.0);
   if(sDotN > 0.0) {
      specular = Light.Ls * Material.Ks * pow(dot(r, v), 3.0);
   }

   vec3 LightIntensity = diffuse + ambient + specular;
   FragColor = vec4(LightIntensity, 1.0);
}
