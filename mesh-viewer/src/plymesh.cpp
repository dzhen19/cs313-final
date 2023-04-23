//--------------------------------------------------
// Author:
// Date:
// Description: Loads PLY files in ASCII format
//--------------------------------------------------

#include "plymesh.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <limits>

using namespace std;
using namespace glm;

template <typename Out>
void split(const std::string &s, char delim, Out result)
{
   std::istringstream iss(s);
   std::string item;
   while (std::getline(iss, item, delim))
   {
      *result++ = item;
   }
}

std::vector<std::string> split(const std::string &s, char delim)
{
   std::vector<std::string> elems;
   split(s, delim, std::back_inserter(elems));
   return elems;
}

namespace agl
{

   PLYMesh::PLYMesh(const std::string &filename)
   {
      load(filename);
   }

   PLYMesh::PLYMesh()
   {
   }

   void PLYMesh::init()
   {
      assert(_positions.size() != 0);
      initBuffers(&_faces, &_positions, &_normals);
   }

   PLYMesh::~PLYMesh()
   {
      _positions.clear();
      _faces.clear();
      _normals.clear();
   }

   bool PLYMesh::load(const std::string &filename)
   {
      int lineNumber = 0;
      if (_positions.size() != 0)
      {
         std::cout << "WARNING: Cannot load different files with the same PLY mesh\n";
         return false;
      }

      string line;
      std::vector<std::string> tokenizedLine;
      ifstream myfile(filename);
      int numV = 0;
      int numF = 0;
      int endHeaderLine = INT_MAX;

      if (myfile.is_open())
      {
         while (getline(myfile, line))
         {
            // check 'ply' is present in line 0
            if (lineNumber == 0 and line != "ply")
            {
               std::cout << "ply not present on first line, exiting..." << std::endl;
               return false;
            }

            // tokenize line by spaces
            tokenizedLine = split(line, ' ');

            // check for 'element vertex [#vertices]'
            if (tokenizedLine[0] == "element")
            {
               if (tokenizedLine[1] == "vertex")
               {
                  // std::cout << tokenizedLine[2] << " vertices" << std::endl;
                  numV = stoi(tokenizedLine[2]);
               }
               if (tokenizedLine[1] == "face")
               {
                  // std::cout << tokenizedLine[2] << " faces" << std::endl;
                  numF = stoi(tokenizedLine[2]);
               }
            }

            // don't do anything until end_header
            if (line == "end_header")
            {
               endHeaderLine = lineNumber;
            }

            int numTokens = tokenizedLine.size();

            // read #vertices lines of vertices
            if (lineNumber > endHeaderLine && lineNumber <= endHeaderLine + numV)
            {
               _positions.push_back(stof(tokenizedLine[0]));
               _positions.push_back(stof(tokenizedLine[1]));
               _positions.push_back(stof(tokenizedLine[2]));

               _normals.push_back(stof(tokenizedLine[3]));
               _normals.push_back(stof(tokenizedLine[4]));
               _normals.push_back(stof(tokenizedLine[5]));

               // read uvs
               if (numTokens == 8)
               {
                  _uvs.push_back(stof(tokenizedLine[6]));
                  _uvs.push_back(stof(tokenizedLine[7]));
               }
            }

            // read #faces lines of faces
            if (lineNumber > endHeaderLine + numV && lineNumber <= endHeaderLine + numV + numF)
            {
               // idx 0 tells us how many indices, assume 3 (faces are triangle)
               _faces.push_back(stof(tokenizedLine[1]));
               _faces.push_back(stof(tokenizedLine[2]));
               _faces.push_back(stof(tokenizedLine[3]));
            }

            lineNumber++;
         }
         myfile.close();
      }

      return false;
   }

   glm::vec3 PLYMesh::minBounds() const
   {
      float min_x = INT_MAX; // probably should set this to inf
      float min_y = min_x;
      float min_z = min_x;

      for (int i = 0; i < numVertices() * 3; i += 3)
      {
         float x = positions()[i + 0];
         if (x < min_x)
         {
            min_x = x;
         }
         float y = positions()[i + 1];
         if (y < min_y)
         {
            min_y = y;
         }
         float z = positions()[i + 2];
         if (z < min_z)
         {
            min_z = z;
         }
      }

      glm::vec3 v(min_x, min_y, min_z);
      return v;
   }

   glm::vec3 PLYMesh::maxBounds() const
   {
      float max_x = -INT_MAX; // probably should set this to -inf
      float max_y = max_x;
      float max_z = max_x;

      for (int i = 0; i < numVertices() * 3; i += 3)
      {
         float x = positions()[i + 0];
         if (x > max_x)
         {
            max_x = x;
         }
         float y = positions()[i + 1];
         if (y > max_y)
         {
            max_y = y;
         }
         float z = positions()[i + 2];
         if (z > max_z)
         {
            max_z = z;
         }
      }

      glm::vec3 v(max_x, max_y, max_z);
      return v;
   }

   int PLYMesh::numVertices() const
   {
      return _positions.size() / 3;
   }

   int PLYMesh::numTriangles() const
   {
      return _faces.size() / 3;
   }

   const std::vector<GLfloat> &PLYMesh::positions() const
   {
      return _positions;
   }

   const std::vector<GLfloat> &PLYMesh::normals() const
   {
      return _normals;
   }

   const std::vector<GLuint> &PLYMesh::indices() const
   {
      return _faces;
   }
}
