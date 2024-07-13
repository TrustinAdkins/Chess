#pragma once

#include <vector>

enum Team { NONE = 0, WHITE = 1, BLACK = 2 };
enum TurnState { NORMAL, PROMOTE, CHECK, CHECKMATE };

struct Coord {
  int row;
  int col;
};

struct BoardPosn {
  char piece = '.';
  Team team = NONE;
  bool enPassant = false;
  bool hasMoved = false;
};

bool operator==(const Coord &a, const Coord &b);

class ChessBoard {
 public:
  ChessBoard();

  bool Checked(Team team);
  bool Checkmated(Team team);
  void Promote(const Coord &coord, char piece);
  bool MakeMove(const Coord &from, const Coord &to);
  std::vector<Coord> GetLegalMoves(const Coord &loc);
  void PrintBoard();

 private:
  bool Enemies(Team a, Team b) const;
  bool ValidSpot(Team teamOfPiece, const Coord &spot) const;
  std::vector<Coord> GetHitTilesDirn(Team team, const Coord &currPos,
                                     int rowInc, int colInc) const;
  std::vector<Coord> GetHitTiles(const Coord &coord) const;
  bool CastlingValid(const Coord &king, const Coord &rook) const;
  bool ChecksEnemyKing(const Coord &loc) const;
  BoardPosn board[8][8];
  Team playerTurn = WHITE;
};