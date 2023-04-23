# mesh-viewer

Implements a simple PLY viewer

<img width="480" alt="shoeoncow" src="https://user-images.githubusercontent.com/55254786/226782400-a83cf8a9-bb17-4352-886a-d689c2103500.png">

## Basic features

- loading single meshes
- browse through meshes in a folder by pressing "p" and "n"
- rotate meshes using mouse drag
- scaling meshes using scroll
- phong per vertex and per pixel shading
- "s" to iterate through shaders

## Unique features 

### composite meshes and support for easy mesh manipulation
```
int cow_id = loadMesh("../models/cow.ply");
scaleMesh(cow_id, .3);
int copy_id;
for (int i = 0; i < 10; i++)
{
   for (int j = 0; j < 10; j++)
   {
      copy_id = copyMesh(cow_id);
      scaleMesh(copy_id, .3);
      translateMesh(copy_id, vec3(i * 5, 0, j * 3));
   }

}
 ```
 
**Twilight of the Idols**

<img width="585" alt="101_cows" src="https://user-images.githubusercontent.com/55254786/226783031-69a17243-380d-4977-96ea-7157387804f2.png">

### texture support. 

Only the phong-vertex shader has support for textures. phong-vertex is the shader that is after the normals shader, which you can get to by pressing "s" to iterate through shaders. Then, once the phong-vertex shader is loaded, press "t" to switch between textures.

**Textured Bovine**

<img width="476" alt="textured_bovine" src="https://user-images.githubusercontent.com/55254786/226783186-7bd5fd60-e266-4101-a7e1-0b12fc71fec3.png">

### distortion vertex shader
```
void main() {
   _vNormal = vNormal;
   vec3 v = cross(vNormal, vec3(1, 1, 0));
   vec3 pos = vPos + v * sin(uTime) * .5;
   gl_Position = MVP * vec4(pos, 1.0);
}
```

![ezgif com-resize](https://user-images.githubusercontent.com/55254786/226784692-40e55dea-68c9-43a1-aff3-228a7e8c66b6.gif)
![ezgif com-resize (1)](https://user-images.githubusercontent.com/55254786/226784918-438801d3-c535-4ad1-b44a-577133c6c899.gif)






### Color shift fragment shader
```
float _sin = 1 + sin(uTime);
float _cos = 1 + cos(uTime);
vec3 normal_col = (_vNormal + 1) / 2;
FragColor = vec4(normal_col.x * _sin, normal_col.y * _cos, normal_col.z * _cos, 1.0f);
```

https://user-images.githubusercontent.com/55254786/227328578-c23890c8-dfd2-4a31-a94c-9bdebf367c86.mp4




