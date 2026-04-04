// include glad before glfw
#include <engine.h>

#include <iostream>
// settings
const unsigned int SCR_WIDTH = 832;
const unsigned int SCR_HEIGHT = 832;

int main(int argc, char** argv) {
  Engine engine("Chess", 832, 832);
  try {
    /* code */
    std::cout << "started";
    engine.start();
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
}