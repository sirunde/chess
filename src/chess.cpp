#include <chess.h>

#include <iostream>

Piece::Piece(PieceType type, int color, int col, int row)
    : type(type), color(color) {
  position.col = col;
  position.row = row;
}

Piece::~Piece() {}

void Piece::move(Position position) {
  this->position.col = position.col;
  this->position.row = position.row;
}
int Piece::getColor() const { return this->color; }  // 0 is white, 1 is black
Position Piece::getPosition() const { return this->position; }
PieceType Piece::getType() const { return this->type; }

std::vector<std::vector<Position>> Pawn::possibleMoves() const {
  std::vector<std::vector<Position>> output;
  Position pos = getPosition();
  int dir = (getColor() == 0) ? 1 : -1;

  // forward
  if (pos.row + dir >= 0 && pos.row + dir < 8)
    output.push_back({{pos.col, pos.row + dir}});

  // double move
  if ((getColor() == 0 && pos.row == 1) || (getColor() == 1 && pos.row == 6))
    output.push_back({{pos.col, pos.row + 2 * dir}});

  // diagonals
  if (pos.col > 0) output.push_back({{pos.col - 1, pos.row + dir}});
  if (pos.col < 7) output.push_back({{pos.col + 1, pos.row + dir}});

  return output;
}

std::vector<std::vector<Position>> Rook::possibleMoves() const {
  std::vector<std::vector<Position>> output;
  Position pos = getPosition();
  std::vector<std::pair<int, int>> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

  for (auto [dx, dy] : directions) {
    std::vector<Position> path;
    int x = pos.col + dx;
    int y = pos.row + dy;
    while (x >= 0 && x < 8 && y >= 0 && y < 8) {
      path.push_back({x, y});
      x += dx;
      y += dy;
    }
    output.push_back(path);
  }
  return output;
}

std::vector<std::vector<Position>> Bishop::possibleMoves() const {
  std::vector<std::vector<Position>> output;
  Position pos = getPosition();
  std::vector<std::pair<int, int>> directions = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

  for (auto [dx, dy] : directions) {
    std::vector<Position> path;
    int x = pos.col + dx;
    int y = pos.row + dy;
    while (x >= 0 && x < 8 && y >= 0 && y < 8) {
      path.push_back({x, y});
      x += dx;
      y += dy;
    }
    output.push_back(path);
  }
  return output;
}

std::vector<std::vector<Position>> Queen::possibleMoves() const {
  std::vector<std::vector<Position>> output;
  Position pos = getPosition();
  std::vector<std::pair<int, int>> directions = {
      {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

  for (auto [dx, dy] : directions) {
    std::vector<Position> path;
    int x = pos.col + dx;
    int y = pos.row + dy;
    while (x >= 0 && x < 8 && y >= 0 && y < 8) {
      path.push_back({x, y});
      x += dx;
      y += dy;
    }
    output.push_back(path);
  }
  return output;
}

std::vector<std::vector<Position>> Knight::possibleMoves() const {
  std::vector<std::vector<Position>> output;
  Position pos = getPosition();
  std::vector<std::pair<int, int>> jumps = {
      {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};

  for (auto [dx, dy] : jumps) {
    int x = pos.col + dx;
    int y = pos.row + dy;
    if (x >= 0 && x < 8 && y >= 0 && y < 8)
      output.push_back({{x, y}});
  }
  return output;
}

std::vector<std::vector<Position>> King::possibleMoves() const {
  std::vector<std::vector<Position>> output;
  Position pos = getPosition();
  std::vector<std::pair<int, int>> steps = {
      {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

  for (auto [dx, dy] : steps) {
    int x = pos.col + dx;
    int y = pos.row + dy;
    if (x >= 0 && x < 8 && y >= 0 && y < 8)
      output.push_back({{x, y}});
  }
  return output;
}

Board::Board() {
  Board::setBoard();
};

Board::~Board() {
  for (int x = 0; x < 8; ++x) {
    for (int y = 0; y < 8; ++y) {
      delete board[x][y];  // delete each piece safely
      board[x][y] = nullptr;
    }
  }
}

// logic for the checkmate. turn false is checking white, turn true is checking the black
bool Board::checkmate(bool turn) {
  bool found = true;
  if (!turn) {  // white
    for (int i = 0; i < 16; i++) {
      Piece* p = getWhite()[i];
      if (p == nullptr) {
        continue;
      }

      if (!this->possible(p).empty()) {
        found = false;
        break;
      }
    }
  } else {
    for (int i = 0; i < 16; i++) {
      Piece* p = getBlack()[i];
      if (p == nullptr) {
        continue;
      }
      if (!this->possible(p).empty()) {
        found = false;
        break;
      }
    }
  }
  return found;
}

// initial setup for the board
bool Board::setBoard() {
  // pawns
  for (size_t i = 0; i < 8; i++) {
    board[i][1] = new Pawn(0, i, 1);  // white
    board[i][6] = new Pawn(1, i, 6);  // black

    // put the piece in the white and black to calculate easily later
    white[i] = board[i][1];
    black[i] = board[i][6];
  }
  // white
  board[0][0] = new Rook(0, 0, 0);
  board[1][0] = new Knight(0, 1, 0);
  board[2][0] = new Bishop(0, 2, 0);
  board[3][0] = new Queen(0, 3, 0);
  board[4][0] = new King(0, 4, 0);
  board[5][0] = new Bishop(0, 5, 0);
  board[6][0] = new Knight(0, 6, 0);
  board[7][0] = new Rook(0, 7, 0);
  // black
  board[0][7] = new Rook(1, 0, 7);
  board[1][7] = new Knight(1, 1, 7);
  board[2][7] = new Bishop(1, 2, 7);
  board[3][7] = new Queen(1, 3, 7);
  board[4][7] = new King(1, 4, 7);
  board[5][7] = new Bishop(1, 5, 7);
  board[6][7] = new Knight(1, 6, 7);
  board[7][7] = new Rook(1, 7, 7);

  white[8] = board[0][0];
  white[9] = board[1][0];
  white[10] = board[2][0];
  white[11] = board[3][0];
  white[12] = board[4][0];  // KING
  white[13] = board[5][0];
  white[14] = board[6][0];
  white[15] = board[7][0];

  black[8] = board[0][7];
  black[9] = board[1][7];
  black[10] = board[2][7];
  black[11] = board[3][7];
  black[12] = board[4][7];  // KING
  black[13] = board[5][7];
  black[14] = board[6][7];
  black[15] = board[7][7];
  return true;
}

Piece* (*Board::getBoard())[8] { return board; }
Piece* (&Board::getBlack())[16] { return black; }
Piece* (&Board::getWhite())[16] { return white; }

void Board::setWhite(int i, Piece* piece) {
  if(white[i])
    delete white[i];
  white[i] = piece;
}
void Board::setBlack(int i, Piece* piece) {
  if (black[i])
    delete black[i];
  black[i] = piece;
}

// shows all possible paths of the piece.
std::vector<Position> Board::possible(Piece* piece) {
  std::vector<std::vector<Position>> paths = piece->possibleMoves();
  std::vector<Position> output;

  Position original = piece->getPosition();
  int color = piece->getColor();

  for (const auto& path : paths) {
    for (const auto& [x, y] : path) {
      Piece* target = board[x][y];
      if (piece->getType() == PieceType::PAWN) {
        int dx = x - original.col;
        int dy = y - original.row;

        // Forward move (same column)
        if (dx == 0) {
          // must be empty
          if (target != nullptr) break;

          // double move must not jump over piece
          if (abs(dy) == 2) {
            int midY = original.row + (dy / 2);
            if (board[x][midY] != nullptr) break;
          }
        }

        else {
          // diagonal move → must capture enemy
          if (target == nullptr || target->getColor() == color) {
            if (target == nullptr) {
              std::cout << "side is empty" << std::endl;
            } else if (target->getColor() == color) {
              std::cout << "side is same color" << std::endl;
            }
            continue;
          }
        }
      }
      // simulate
      board[original.col][original.row] = nullptr;
      board[x][y] = piece;
      piece->move({x, y});

      bool stillSafe = !check(color);

      // undo
      board[original.col][original.row] = piece;
      board[x][y] = target;
      piece->move(original);

      if (stillSafe && (target == nullptr || target->getColor() != color)) {
        output.push_back({x, y});
      }

      // stop sliding if blocked
      if (target != nullptr) break;
    }
  }

  return output;
}

// capturing the piece
void Board::capture(Piece* piece, Position new_position) {
  Piece* before = board[new_position.col][new_position.row];
  if (piece->getColor()) {
    for (int i = 0; i < 16; i++) {
      if (this->getWhite()[i] == before) {
        this->setWhite(i, nullptr);
      }
    }
  } else {
    for (int i = 0; i < 16; i++) {
      if (this->getBlack()[i] == before) {
        this->setBlack(i, nullptr);
      }
    }
  }

  Position position = piece->getPosition();
  board[new_position.col][new_position.row] = board[position.col][position.row];
  piece->move(new_position);
  board[position.col][position.row] = nullptr;
}

// move the piece if it is safe.
void Board::move(Piece* piece, Position new_position) {  // need to see if king is safe or not
  std::vector<Position> moves = Board::possible(piece);
  if (board[new_position.col][new_position.row] != nullptr) {
    Board::capture(piece, new_position);
  } else {
    Position position = piece->getPosition();
    board[new_position.col][new_position.row] = board[position.col][position.row];
    piece->move(new_position);
    board[position.col][position.row] = nullptr;
  }

  if (piece->getType() == PieceType::PAWN && (piece->getPosition().row == 0 || piece->getPosition().row == 7)) {
    std::cout << "done" << std::endl;
    Piece* newPiece = new Queen(piece->getColor(), new_position.col, new_position.row);
    board[new_position.col][new_position.row] = newPiece;
    if (piece->getColor()) {
      for (int i = 0; i < 16; i++) {
        if (this->getWhite()[i] == piece) {
          this->setWhite(i, newPiece);
        }
      }
    } else {
      for (int i = 0; i < 16; i++) {
        if (this->getBlack()[i] == piece) {
          this->setBlack(i, newPiece);
        }
      }
    }
  }
}

// check if the king is safe
bool Board::check(int color) {
  int x;
  int y;

  if (color == 0) {  // bring white king
    Position pos = white[12]->getPosition();
    x = pos.col;
    y = pos.row;
  } else {  // bring black king
    Position pos = black[12]->getPosition();
    x = pos.col;
    y = pos.row;
  }

  std::vector<std::pair<int, int>> directions = {
      {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};  // diagonal
  // {1, 0}, {0, 1}, {-1, 0}, {0, -1}}; // rook

  for (auto [dx, dy] : directions) {
    int newX = x + dx;
    int newY = y + dy;
    while (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {
      if (board[newX][newY] != nullptr) {
        if (board[newX][newY]->getColor() != color) {
          if ((board[newX][newY]->getType() == PieceType::BISHOP ||
               board[newX][newY]->getType() == PieceType::QUEEN)) {
            return true;
          } else
            break;
        } else {
          break;
        }
      }
      newX += dx;
      newY += dy;
    }
  }

  // rook
  directions = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
  for (auto [dx, dy] : directions) {
    int newX = x + dx;
    int newY = y + dy;
    while (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {
      if (board[newX][newY] != nullptr) {
        if (board[newX][newY]->getColor() != color) {
          if ((board[newX][newY]->getType() == PieceType::ROOK ||
               board[newX][newY]->getType() == PieceType::QUEEN)) {
            return true;
          } else
            break;
        } else
          break;
      }
      newX += dx;
      newY += dy;
    }
  }

  // knight
  directions = {{-1, -2}, {-2, -1}, {2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {2, -1}, {1, -2}};
  for (auto [dx, dy] : directions) {
    int newX = x + dx;
    int newY = y + dy;
    if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8 &&
        board[newX][newY] != nullptr &&
        board[newX][newY]->getColor() != color &&
        board[newX][newY]->getType() == PieceType::KNIGHT) {
      return true;
    }
  }

  // king
  directions = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

  for (auto [dx, dy] : directions) {
    int newX = x + dx;
    int newY = y + dy;
    if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8 &&
        board[newX][newY] != nullptr &&
        board[newX][newY]->getColor() != color &&
        board[newX][newY]->getType() == PieceType::KING) {
      return true;
    }
  }
  // pawn
  if (color == 0) {  // white king

    if ((x < 7 && y < 7 && board[x + 1][y + 1] != nullptr &&
         board[x + 1][y + 1]->getColor() != color &&
         board[x + 1][y + 1]->getType() == PieceType::PAWN) ||
        (x > 0 && y < 7 && board[x - 1][y + 1] != nullptr &&
         board[x - 1][y + 1]->getColor() != color &&
         board[x - 1][y + 1]->getType() == PieceType::PAWN)) {
      return true;
    }
  } else {  // black king
    if ((x < 7 && y > 0 && board[x + 1][y - 1] != nullptr &&
         board[x + 1][y - 1]->getColor() != color &&
         board[x + 1][y - 1]->getType() == PieceType::PAWN) ||
        (x > 0 && y > 0 && board[x - 1][y - 1] != nullptr &&
         board[x - 1][y - 1]->getColor() != color &&
         board[x - 1][y - 1]->getType() == PieceType::PAWN)) {
      return true;
    }
  }
  return false;
  // this for checking pin
  // prob beam from king and check if therere bishop, rook and queen
}

void Board::cleanup() {
  for (int i = 0; i < 16; i++) {
    setWhite(i, nullptr);
    setBlack(i, nullptr);
  }
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      board[i][j] = nullptr;
    }
  }
}