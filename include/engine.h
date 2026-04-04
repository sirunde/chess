#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chess.h>
#include <shader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void draw_bg(Shader& shader, unsigned int& useTexLoc, unsigned int& colorLoc, unsigned int& VAO);
void draw_pieces(Board& board, Shader& shader, std::vector<Vertex> quadVertices, unsigned int& pieceTexture, unsigned int& useTexLoc, unsigned int& colorLoc, unsigned int& pieceColor, unsigned int& VAO);
void draw_hovering(int row, int col, Shader& shader,
                   unsigned int& colorLoc, unsigned int& VAO);
class Engine {
 private:
  int width;
  int height;
  const char* title;
  Board board;
  GLFWwindow* window;
  std::vector<std::pair<int,int>> history;

 public:
  Engine(const char* title, int w = 832, int h = 832);
  ~Engine();
  void load();
  void drawBoard();
  void drawPieces();
  int start();
};