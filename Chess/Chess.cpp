#include <iostream> //7.13.2024 #3:58 PM test commit

#include "ChessBoard.h"
#include "Stockfish.h"

std::string CoordToString(Coord coord) {
  return "" + (char)(97 + coord.col) + (char)(48 + 8 - coord.row);
}

Coord StringToCoord(std::string coord) {
  return {8 - (coord[1] - 48), coord[0] - 97};
}

int main() {
  Stockfish stockfish;
  ChessBoard chessboard;

  bool aiEnabled = true; 
  bool aiTurn = false;
  bool whiteTurn = true;

  while (true) {
    chessboard.PrintBoard();

    std::string move;
    if (aiTurn)
      move = stockfish.GetMove();
    else
      std::cin >> move;

    const Coord from = StringToCoord(move.substr(0, 2));
    const Coord to = StringToCoord(move.substr(2, 2));
    std::vector<Coord> legalMoves = chessboard.GetLegalMoves(from);
    bool moveMade = false;
    for (const Coord &coord : legalMoves) {
      if (coord == to) {
        if (chessboard.MakeMove(from, to)) {
          if (!aiTurn) {
            std::string promote;
            std::cin >> promote;
            move += promote;
          }
          chessboard.Promote(to, move[4]);
        }
        moveMade = true;
        break;
      }
    }

    if (moveMade) {
      if (!aiTurn)
        stockfish.PlayMove(move);

      whiteTurn = !whiteTurn;
      if (aiEnabled) aiTurn = !aiTurn;

      if (chessboard.Checkmated(whiteTurn ? WHITE : BLACK)) {
        printf("Checkmate!\n");
        break;
      }
    }
  }

  return 0;
}
