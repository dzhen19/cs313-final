//--------------------------------------------------
// Author:
// Date:
// Description: Loads PLY files in ASCII format
//--------------------------------------------------

#include <cmath>
#include <string>
#include <sstream>
#include <vector>
#include "agl/window.h"
#include "plymesh.h"
#include "osutils.h"
#include <iostream>
#include <fstream>
#include <limits>

using namespace std;
using namespace glm;
using namespace agl;

struct Stroke
{
   int parentStrokeIdx;
   int parentVertexIdx;
   std::vector<vec3> vertices;
};

struct Line
{
   vec3 base;
   vec3 tip;
};

struct Particle
{
   glm::vec3 pos;
   glm::vec3 vel;
   glm::vec4 color;
   float size;
   float rot;
   float startTime;
};

template <typename Out>
void _split(const std::string &s, char delim, Out result)
{
   std::istringstream iss(s);
   std::string item;
   while (std::getline(iss, item, delim))
   {
      *result++ = item;
   }
}

std::vector<std::string> _split(const std::string &s, char delim)
{
   std::vector<std::string> elems;
   _split(s, delim, std::back_inserter(elems));
   return elems;
}

float RandomFloat(float a, float b)
{
   float random = ((float)rand()) / (float)RAND_MAX;
   float diff = b - a;
   float r = random * diff;
   return a + r;
}

class MeshViewer : public Window
{
public:
   int meshIdx = 0;
   float scaleFactor = 1;
   float translateX = 0;
   float translateY = 0;
   float translateZ = 0;

   float radius = 10;
   // in degrees [0, 360]
   float azimuth = 36;
   // in degrees [-90, 90]
   float elevation = 17;

   bool singleMeshBrowsing = true;
   bool canRotate = false;
   vector<string> shaders = {"normals", "phong-vertex", "phong-pixel", "distort", "colorshift"};
   int currentShader = 0;

   vector<Stroke> strokes;
   Stroke currentStroke;

   std::vector<Line> vectorField;
   std::vector<Line> treeVectors;

   MeshViewer() : Window()
   {
      setup();
   }

   vec3 getEyePos()
   {
      return vec3(radius * sin(radians(azimuth)) * std::max(cos(radians(elevation)), float(0.01)),
                  radius * sin(radians(elevation)),
                  radius * cos(radians(azimuth)) * std::max(cos(radians(elevation)), float(0.01)));
   }

   void loadStrokes()
   {
      if (strokes.size() > 0)
      {
         return;
      }

      string line;
      std::vector<std::string> tokenizedLine;
      ifstream myfile("../strokes.txt");
      int numV = 0;
      int numF = 0;
      int endHeaderLine = INT_MAX;

      if (myfile.is_open())
      {
         while (getline(myfile, line))
         {
            tokenizedLine = _split(line, ' ');
            if (tokenizedLine[0] == "BEGIN_STROKE")
            {
               continue;
            }
            else if (tokenizedLine[0] == "PARENT_STROKE_ID")
            {
               currentStroke.parentStrokeIdx = stoi(tokenizedLine[1]);
               continue;
            }
            else if (tokenizedLine[0] == "PARENT_VERTEX_ID")
            {
               currentStroke.parentVertexIdx = stoi(tokenizedLine[1]);
               continue;
            }
            else if (tokenizedLine[0] == "VERTICES")
            {
               for (int i = 0; i < (tokenizedLine.size() - 1) / 2; i++)
               {
                  float x = stof(tokenizedLine[2 * i + 1]);
                  float y = stof(tokenizedLine[2 * i + 2]);
                  currentStroke.vertices.push_back(
                      vec3(x, y, 0));
               }
               continue;
            }
            else if (tokenizedLine[0] == "END_STROKE")
            {
               strokes.push_back(currentStroke);
               currentStroke.vertices.clear();
               continue;
            }
         }
         myfile.close();
      }
   }

   void interpolateDepth()
   {
      float zDir;
      // go through and hydrate vertices w/interpolated depth
      for (int i = 0; i < strokes.size(); i++)
      {
         zDir = i == 0 ? 0 : RandomFloat(-.04, .04);
         for (int j = 0; j < strokes[i].vertices.size(); j++)
         {
            vec3 v = strokes[i].vertices[j];
            strokes[i].vertices[j] = vec3(v.x, v.y, zDir * (j / 2));
         }
      }

      // go through and translate each branch so it attaches to the correct root
      // update bbox and get center of mass and dir of each stroke
      for (int i = 0; i < strokes.size(); i++)
      {
         Stroke s = strokes[i];
         vec3 rootPos = strokes[s.parentStrokeIdx].vertices[s.parentVertexIdx];
         vec3 interpolatedRootPos = s.vertices[0];
         vec3 diff = interpolatedRootPos - rootPos;
         // take one loop to interpolate depth
         for (int j = 0; j < strokes[i].vertices.size(); j++)
         {
            strokes[i].vertices[j] -= diff;
         }
      }
   }

   void setup()
   {
      // reset eye position
      radius = 10;
      azimuth = 180;
      elevation = 90;

      renderer.loadShader("phong-vertex", "../shaders/phong-vertex.vs", "../shaders/phong-vertex.fs");
      renderer.loadShader("phong-pixel", "../shaders/phong-pixel.vs", "../shaders/phong-pixel.fs");
      renderer.loadShader("normals", "../shaders/normals.vs", "../shaders/normals.fs");
      renderer.loadShader("colorshift", "../shaders/normals.vs", "../shaders/colorshift.fs");
      renderer.loadShader("distort", "../shaders/distort.vs", "../shaders/distort.fs");

      loadStrokes();
      interpolateDepth();
   }

   void mouseMotion(int x, int y, int dx, int dy)
   {
      if (!canRotate)
      {
         return;
      }

      float newAzimuth = azimuth + dx;
      azimuth = std::max(std::min(newAzimuth, float(360)), float(0));

      float newElevation = elevation + dy;
      elevation = std::max(std::min(newElevation, float(90)), float(-90));
   }

   void mouseDown(int button, int mods)
   {
      canRotate = true;
   }

   void mouseUp(int button, int mods)
   {
      canRotate = false;
   }

   void scroll(float dx, float dy)
   {
      float newRadius = radius += dy / 10;
      // radius should not be negative
      radius = std::max(newRadius, float(.01));
   }

   void keyUp(int key, int mods)
   {
   }

   void draw()
   {
      renderer.beginShader(shaders[currentShader]);
      renderer.setUniform("Light.La", vec3(.1, .1, .2));
      renderer.setUniform("Light.Ld", vec3(.8, .8, 1.0));
      renderer.setUniform("Light.Ls", vec3(.8, .8, 1.0));
      renderer.setUniform("Light.Position", vec4(0.0, 10.0, 0, 1.0));
      renderer.setUniform("Material.f", 3.0f);
      renderer.setUniform("Material.Ka", vec3(.8));
      renderer.setUniform("Material.Kd", vec3(.8));
      renderer.setUniform("Material.Ks", vec3(1.0));
      renderer.setUniform("uTime", elapsedTime());
      eyePos = getEyePos();
      up = vec3(0, 1, 0);

      float aspect = ((float)width()) / height();
      renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);
      renderer.lookAt(eyePos, lookPos, up);

      // red = x axis
      renderer.line(vec3(0, 0, 0), vec3(100, 0, 0), vec3(1, 0, 0), vec3(1, 0, 0));
      // green = y axis
      renderer.line(vec3(0, 0, 0), vec3(0, 100, 0), vec3(0, 1, 0), vec3(0, 1, 0));
      // blue = z axis
      renderer.line(vec3(0, 0, 0), vec3(0, 0, 100), vec3(0, 0, 1), vec3(0, 0, 1));

      for (int i = 0; i < strokes.size(); i++)
      {
         for (int j = 0; j < strokes[i].vertices.size() - 1; j++)
         {
            vec3 v = strokes[i].vertices[j];
            vec3 v_1 = strokes[i].vertices[j + 1];

            renderer.line(v, v_1, vec3(.5, .5, .5), vec3(.5, .5, .5));
         }
      }

      renderer.endShader();
   }

   ~MeshViewer()
   {
   }

protected:
   PLYMesh mesh;
   vec3 eyePos = getEyePos();
   vec3 lookPos = vec3(0, 0, 0);
   vec3 up = vec3(0, 1, 0);
   std::vector<Particle> mParticles;
};

int main(int argc, char **argv)
{
   MeshViewer viewer;
   viewer.run();
   return 0;
}
