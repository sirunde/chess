#pragma once
#include <verts.h>

#include <cmath>
#include <vector>
enum class PieceType { PAWN,
                       ROOK,
                       QUEEN,
                       KING,
                       BISHOP,
                       KNIGHT };

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
  virtual std::vector<std::vector<Position>> possibleMoves() const = 0;
};

class Pawn : public Piece {
 public:
  Pawn(int color, int col, int row)
      : Piece(PieceType::PAWN, color, col, row) {}

  std::vector<std::vector<Position>> possibleMoves() const override;
};
class Rook : public Piece {
 public:
  Rook(int color, int col, int row)
      : Piece(PieceType::ROOK, color, col, row) {}

  std::vector<std::vector<Position>> possibleMoves() const override;
};
class Bishop : public Piece {
 public:
  Bishop(int color, int col, int row)
      : Piece(PieceType::BISHOP, color, col, row) {}

  std::vector<std::vector<Position>> possibleMoves() const override;
};
class Queen : public Piece {
 public:
  Queen(int color, int col, int row)
      : Piece(PieceType::QUEEN, color, col, row) {}

  std::vector<std::vector<Position>> possibleMoves() const override;
};
class Knight : public Piece {
 public:
  Knight(int color, int col, int row)
      : Piece(PieceType::KNIGHT, color, col, row) {}

  std::vector<std::vector<Position>> possibleMoves() const override;
};
class King : public Piece {
 public:
  King(int color, int col, int row)
      : Piece(PieceType::KING, color, col, row) {}

  std::vector<std::vector<Position>> possibleMoves() const override;
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
  void setWhite(int i,Piece* piece);
  void setBlack(int i,Piece* piece);
  std::vector<Position> possible(Piece* piece);
  bool setBoard();
  void capture(Piece* piece, Position new_position);
  void move(Piece* piece, Position new_position);
  bool check(int color);
  bool checkmate(bool turn);
  void cleanup();
};