//--------------------------------------------------
// Author:
// Date:
// Description: Loads PLY files in ASCII format
//--------------------------------------------------

#ifndef plymeshmodel_H_
#define plymeshmodel_H_

#include "agl/aglm.h"
#include "agl/mesh/triangle_mesh.h"

struct Vertex
{
   float x;
   float y;
   float z;
};

namespace agl
{
   class PLYMesh : public TriangleMesh
   {
   public:
      PLYMesh(const std::string &filename);
      PLYMesh();

      virtual ~PLYMesh();

      // Initialize this object with the given file
      // Returns true if successfull. false otherwise.
      bool load(const std::string &filename);

      // Return the minimum point of the axis-aligned bounding box
      glm::vec3 minBounds() const;

      // Return the maximum point of the axis-aligned bounding box
      glm::vec3 maxBounds() const;

      // Return number of vertices in this model
      int numVertices() const;

      // Positions in this model
      const std::vector<GLfloat> &positions() const;

      // Positions in this model
      const std::vector<GLfloat> &normals() const;

      // Return number of faces in this model
      int numTriangles() const;

      // face indices in this model
      const std::vector<GLuint> &indices() const;

   protected:
      void init();

   protected:
      std::vector<GLfloat> _normals;
      std::vector<GLfloat> _positions;
      std::vector<GLfloat> _uvs;
      std::vector<GLuint> _faces;
   };

}

#endif
