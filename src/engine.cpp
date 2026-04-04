// include glad before glfw
#include <chess.h>
#include <engine.h>
#include <shader.h>
#include <stb_image/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const int h = 832;  // window height
const int w = 832;  // window width

int width, height, nrChannels;

const float boardScale = 0.7f;                           // 70% of screen
const int boardSize = 8;                                 // match 70% of screen
const float squareSize = boardScale * 2.0f / boardSize;  // scaled square
const float offset = (1.0f - boardScale);                // black border

glm::vec3 startPos(-1.0f + offset + squareSize / 2.0f,
                   -1.0f + offset + squareSize / 2.0f,
                   0.0f);  // bottom-left corner
void draw_bg(Shader& shader,
             unsigned int& useTexLoc,
             unsigned int& colorLoc,
             unsigned int& VAO) {
  glUniform1i(useTexLoc, 0);

  const int boardSize = 8;
  const float squareSize = boardScale * 2.0f / boardSize;  // scaled square size

  // bottom-left corner of board
  float offset = (1.0f - boardScale);  // extra space on one side in NDC units
  glm::vec3 startPos(-1.0f + offset + squareSize / 2.0f,
                     -1.0f + offset + squareSize / 2.0f,
                     0.0f);

  for (int row = 0; row < boardSize; row++) {
    for (int col = 0; col < boardSize; col++) {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model,
                             startPos + glm::vec3(col * squareSize,
                                                  row * squareSize,
                                                  0.0f));
      model = glm::scale(model, glm::vec3(squareSize, squareSize, 1.0f));
      glUniformMatrix4fv(glGetUniformLocation(shader.ID, "u_model"), 1, GL_FALSE, &model[0][0]);
      glUniformMatrix4fv(
          glGetUniformLocation(shader.ID, "u_model"),
          1, GL_FALSE, &model[0][0]);

      // checker pattern
      if ((row + col) % 2 == 0)
        glUniform4f(colorLoc, 0.2f, 0.2f, 0.2f, 1.0f);
      else
        glUniform4f(colorLoc, 0.9f, 0.9f, 0.9f, 1.0f);

      glBindVertexArray(VAO);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
  }
}

void draw_pieces(Board& board, Shader& shader,
                 std::vector<Vertex> quadVertices,
                 unsigned int& pieceTexture,
                 unsigned int& useTexLoc,
                 unsigned int& colorLoc,
                 unsigned int& pieceColor,
                 unsigned int& VAO) {
  Piece*(*grid)[8] = board.getBoard();
  glUniform1i(useTexLoc, 1);  // using texture

  int uvOffsetLoc = glGetUniformLocation(shader.ID, "u_uvOffset");
  int uvScaleLoc = glGetUniformLocation(shader.ID, "u_uvScale");

  float tileWidth = 1.0f / 6.0f;
  glm::vec2 uvScale(tileWidth, 1.0f);

  glUniform2fv(uvScaleLoc, 1, &uvScale[0]);

  for (int row = 0; row < boardSize; row++) {
    for (int col = 0; col < boardSize; col++) {
      Piece* piece = grid[col][row];
      if (!piece) continue;

      // model matrix
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model,
                             startPos + glm::vec3(col * squareSize,
                                                  row * squareSize,
                                                  0.0f));
      model = glm::scale(model, glm::vec3(squareSize, squareSize, 1.0f));
      glm::mat4 squareModel = model;
      glUniformMatrix4fv(glGetUniformLocation(shader.ID, "u_model"), 1, GL_FALSE, &model[0][0]);

      // piece color
      glUniform1i(pieceColor, piece->getColor() ? 0 : 1);
      glUniform1i(useTexLoc, 1);

      // select sprite column
      int spriteCol = 0;
      switch (piece->getType()) {
        case PieceType::PAWN:
          spriteCol = 0;
          break;
        case PieceType::ROOK:
          spriteCol = 1;
          break;
        case PieceType::QUEEN:
          spriteCol = 2;
          break;
        case PieceType::KING:
          spriteCol = 3;
          break;
        case PieceType::BISHOP:
          spriteCol = 4;
          break;
        case PieceType::KNIGHT:
          spriteCol = 5;
          break;
      }
      glm::vec2 uvOffset(spriteCol * tileWidth, 0.0f);
      glUniform2fv(uvOffsetLoc, 1, &uvOffset[0]);

      // draw quad
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, pieceTexture);
      glBindVertexArray(VAO);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
  }
}

void draw_hovering(int col, int row, Shader& shader,
                   unsigned int& colorLoc, unsigned int& VAO, float r, float g, float b) {
  if (row < 0 || col < 0 || row >= 8 || col >= 8) return;

  const int boardSize = 8;
  const float squareSize = boardScale * 2.0f / boardSize;  // match board scaling
  float offset = (1.0f - boardScale);
  glm::vec3 startPos(-1.0f + offset + squareSize / 2.0f,
                     -1.0f + offset + squareSize / 2.0f,
                     0.0f);

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model,
                         startPos + glm::vec3(col * squareSize,
                                              row * squareSize,
                                              0.0f));
  model = glm::scale(model, glm::vec3(squareSize, squareSize, 1.0f));

  glUniformMatrix4fv(glGetUniformLocation(shader.ID, "u_model"), 1, GL_FALSE, &model[0][0]);
  glUniform4f(colorLoc, r, g, b, 1.0f);  // green

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void clicked(Board& board, Shader& shader, int c, int r,
             std::vector<Vertex> quadVertices,
             unsigned int& pieceTexture,
             unsigned int& useTexLoc,
             unsigned int& colorLoc,
             unsigned int& pieceColor,
             unsigned int& VAO, double mouseX, double mouseY) {
  Piece*(*grid)[8] = board.getBoard();
  glUniform1i(useTexLoc, 1);  // using texture

  int uvOffsetLoc = glGetUniformLocation(shader.ID, "u_uvOffset");
  int uvScaleLoc = glGetUniformLocation(shader.ID, "u_uvScale");

  float tileWidth = 1.0f / 6.0f;
  glm::vec2 uvScale(tileWidth, 1.0f);

  glUniform2fv(uvScaleLoc, 1, &uvScale[0]);

  for (int row = 0; row < boardSize; row++) {
    for (int col = 0; col < boardSize; col++) {
      Piece* piece = grid[col][row];
      if (!piece) continue;

      // model matrix
      glm::mat4 model = glm::mat4(1.0f);
      if (r == row && c == col) {
        float xNDC = (2.0f * mouseX) / w - 1.0f;
        float yNDC = 1.0f - (2.0f * mouseY) / h;
        model = glm::translate(model, glm::vec3(xNDC, yNDC, 0.0f));
      } else {
        model = glm::translate(model,
                               startPos + glm::vec3(col * squareSize,
                                                    row * squareSize,
                                                    0.0f));
      }
      model = glm::scale(model, glm::vec3(squareSize, squareSize, 1.0f));
      glm::mat4 squareModel = model;
      glUniformMatrix4fv(glGetUniformLocation(shader.ID, "u_model"), 1, GL_FALSE, &model[0][0]);

      // piece color
      glUniform1i(pieceColor, piece->getColor() ? 0 : 1);
      glUniform1i(useTexLoc, 1);

      // select sprite column
      int spriteCol = 0;
      switch (piece->getType()) {
        case PieceType::PAWN:
          spriteCol = 0;
          break;
        case PieceType::ROOK:
          spriteCol = 1;
          break;
        case PieceType::QUEEN:
          spriteCol = 2;
          break;
        case PieceType::KING:
          spriteCol = 3;
          break;
        case PieceType::BISHOP:
          spriteCol = 4;
          break;
        case PieceType::KNIGHT:
          spriteCol = 5;
          break;
      }
      glm::vec2 uvOffset(spriteCol * tileWidth, 0.0f);
      glUniform2fv(uvOffsetLoc, 1, &uvOffset[0]);

      // draw quad
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, pieceTexture);
      glBindVertexArray(VAO);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
  }
}

Engine::Engine(const char* title, int w, int h) : width(w), height(h) {
  this->title = title;
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  // apple
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  // glfw window creation
  // --------------------
};

Engine::~Engine() {};

void Engine::load() {
  this->window = glfwCreateWindow(width, height, title, NULL, NULL);
  if (!this->window) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    raise(-1);
  }
  glfwMakeContextCurrent(this->window);
  glfwSetFramebufferSizeCallback(this->window, framebuffer_size_callback);
  // Load OpenGL functions using GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    raise(-1);
  }
}

int Engine::start() {
  load();
  // Create window

  int success;
  char infoLog[512];

  // blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Shader shader("src/piece.vs", "src/piece.fs");

  // pieces vertices
  std::vector<Vertex> quadVertices = makeMesh();
  board.setBoard();

  size_t offset = 0;
  unsigned int VAO, VBO, EBO;
  unsigned int texture;
  unsigned int indices[] = {
      0, 1, 3,  // first triangle
      1, 2, 3   // second triangle
  };
  bool turn = false;  // false is white turn, true is black turn
  // Chessboard VAO/VBO
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
               quadVertices.size() * sizeof(Vertex),
               quadVertices.data(),
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(indices),
               indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // texture
  unsigned int boardTexture, pieceTexture;
  glGenTextures(1, &pieceTexture);
  glBindTexture(GL_TEXTURE_2D, pieceTexture);

  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  // set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // load image
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);  // tell stb_image.h to flip loaded texture's on the y-axis.

  unsigned char* data = stbi_load("src/spritesheet.png", &width, &height, &nrChannels, 0);

  if (!data) {
    std::cout << "Failed to load texture" << std::endl;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

  shader.use();
  shader.setInt("u_texture", 0);

  float boardMin = -boardScale;  // left/bottom edge in NDC
  float boardMax = boardScale;   // right/top edge in NDC
  float squareSize = (boardMax - boardMin) / 8.0f;
  Piece*(*grid)[8] = board.getBoard();

  bool holding = false;  // track if user is holding the piece
  bool capture = false;  // track if user is capturing the piece

  int holdRow, holdCol;
  // --- Render loop ---
  while (!glfwWindowShouldClose(this->window)) {
    processInput(this->window);
    // clear
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Projection matrix (identity for now)
    glm::mat4 proj = glm::mat4(1.0f);

    // Model matrix (apply transform/rotation/translation)
    glm::mat4 model = glm::mat4(1.0f);

    // model = glm::translate(model, glm::vec3(.0f, .0f, .0f));
    model = glm::scale(model, glm::vec3(1.0 / 6.0f, 1.0f / 6.0f, 1.0f));  // scale x to sprite width
    // model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

    unsigned int colorLoc = glGetUniformLocation(shader.ID, "u_color");
    unsigned int useTexLoc = glGetUniformLocation(shader.ID, "u_using_textures");
    unsigned int modelLoc = glGetUniformLocation(shader.ID, "u_model");
    unsigned int projLoc = glGetUniformLocation(shader.ID, "u_proj");
    unsigned int pieceColor = glGetUniformLocation(shader.ID, "u_chess_piece_white");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

    // bg drawing
    draw_bg(shader, useTexLoc, colorLoc, VAO);
    // need to verify the Check
    // 0 is white, 1 is black
    // hovering
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    // Convert to NDC [-1, 1]
    float xNDC = (2.0f * mouseX) / w - 1.0f;
    float yNDC = 1.0f - (2.0f * mouseY) / h;  // flip Y

    // Board scale and offsets
    float boardMin = -boardScale;  // left/bottom edge in NDC
    float boardMax = boardScale;   // right/top edge in NDC
    float squareSize = (boardMax - boardMin) / 8.0f;

    // Map NDC cursor to board squares
    int col = static_cast<int>(floor((xNDC - boardMin) / squareSize));
    int row = static_cast<int>(floor((yNDC - boardMin) / squareSize));

    // clamp to valid range
    if (col < 0 || col >= 8 || row < 0 || row >= 8) {
      col = row = -1;  // outside board
    }

    int hoveredRow = row;
    int hoveredCol = col;
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    bool isClicked = (state == GLFW_PRESS && !capture);
    capture = (state == GLFW_PRESS);

    if (!holding) {
      if (col >= 0 && row >= 0) {
        Piece* piece = grid[col][row];

        if (piece && piece->getColor() == turn) {
          draw_hovering(col, row, shader, colorLoc, VAO, 1, 1, 0);
          draw_pieces(board, shader, quadVertices, pieceTexture, useTexLoc, colorLoc, pieceColor, VAO);
        }
        if (state == GLFW_PRESS && piece && piece->getColor() == turn) {
          holding = true;
          holdRow = row;
          holdCol = col;

          draw_hovering(hoveredCol, holdRow, shader, colorLoc, VAO, 0, 1, 0);
          // clicked(board, shader, holdCol, holdRow, quadVertices, pieceTexture, useTexLoc, colorLoc, pieceColor, VAO, mouseX, mouseY);
        } else {
          draw_pieces(board, shader, quadVertices, pieceTexture, useTexLoc, colorLoc, pieceColor, VAO);
        }
      }
      else {
        draw_pieces(board, shader, quadVertices, pieceTexture, useTexLoc, colorLoc, pieceColor, VAO);
      }
    }

    else {
      // draw_hovering(holdCol, holdRow, shader, colorLoc, VAO, 0, 1, 0);
      Piece* piece = grid[holdCol][holdRow];
      if (piece) {
        std::vector<Position> temp = board.possible(piece);
        if (temp.empty()){
          holding = false;
          isClicked = false;
          continue;
        }
        for (auto& i : temp) {
          draw_hovering(i.col, i.row, shader, colorLoc, VAO, 0, 1, 0);
        }

        if (isClicked) {
          for (const auto& pos : temp) {
            if (pos.col == hoveredCol && pos.row == hoveredRow) {
              if (grid[hoveredCol][hoveredRow] != nullptr) {
                board.capture(piece, Position{hoveredCol, hoveredRow});
              } else {
                board.move(piece, Position{hoveredCol, hoveredRow});
              }

              turn = !turn;
              holding = false;
              if(board.checkmate(turn)){
                if(turn){
                  std::cout << "Checkmate. White Won" << std::endl;
                }
                else{
                  std::cout << "Checkmate. Black Won" << std::endl;
                }
                board.cleanup();
              }
              break;
            }
          }
        }
      }
      clicked(board, shader, holdCol, holdRow, quadVertices, pieceTexture, useTexLoc, colorLoc, pieceColor, VAO, mouseX, mouseY);
    }

    glfwSwapBuffers(this->window);
    glfwPollEvents();
  }

  // Cleanup
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shader.ID);

  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released
// this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this
// callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that
  // width and height will be significantly larger than specified on retina
  // displays.
  glViewport(0, 0, width, height);
}