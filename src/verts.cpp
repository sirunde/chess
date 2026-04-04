#include <verts.h>

std::vector<Vertex> makeMesh() {
  // for now, use hard coded one
  std::vector<Vertex> quadVertices = {
      {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f},
      {0.5f, -0.5f, 0.0f, 1.0f, 0.0f},
      {0.5f, 0.5f, 0.0f, 1.0f, 1.0f},
      {-0.5f, 0.5f, 0.0f, 0.0f, 1.0f},
  };

  return quadVertices;
}