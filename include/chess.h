#pragma once
#include <verts.h>
#include <cmath>
#include <vector>
enum class PieceType { PAWN,
                       ROOK,
                       QUEEN,
                       KING,
                       BISHOP,
                       KNIGHT};

struct Position {
  int col;
  int row;
};

class Piece {
 private:
  PieceType type;
  int color;  // 0: white, 1: black
  Position position;

 public:
  Piece(PieceType type = PieceType::PAWN, int color = 0, int x = 0, int y = 0);
  ~Piece();

  void move(Position position);
  int getColor() const;
  Position getPosition() const;
  PieceType getType() const;

  std::vector<std::vector<Position>> possibleMoves() const;
};

class Board {
 private:
  Piece* board[8][8] = {nullptr};
  Piece* white[16];
  Piece* black[16];

 public:
  Board();   // constructor sets up default board
  ~Board();  // destructor cleans up dynamic memory

  Piece* (*getBoard())[8];
  Piece* (&getBlack())[16];
  Piece* (&getWhite())[16];
  void setWhite(int i);
  void setBlack(int i);
  std::vector<Position> possible(Piece* piece);
  bool setBoard();
  void capture(Piece* piece, Position new_position);
  void move(Piece* piece, Position new_position);
  bool check(int color);
  bool checkmate(bool turn);
  void cleanup();
};