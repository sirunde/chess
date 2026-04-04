#pragma once
#include <cmath>
#include <numbers>
#include <vector>
// Use a helper constant if not using C++20
#ifndef PI
#define PI 3.14159265358979323846f
#endif

struct Vertex {
  float x, y,z;
  float u, v;
};

// Forward declarations
std::vector<Vertex> makeMesh();