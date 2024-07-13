#include "ChessBoard.h"

bool operator==(const Coord &a, const Coord &b) {
  return (a.col == b.col) && (a.row == b.row);
}

ChessBoard::ChessBoard() {
  board[0][0].piece = 'R';
  board[0][1].piece = 'K';
  board[0][2].piece = 'B';
  board[0][3].piece = 'Q';
  board[0][4].piece = 'L';
  board[0][5].piece = 'B';
  board[0][6].piece = 'K';
  board[0][7].piece = 'R';

  for (unsigned i = 0; i < 8; ++i)
    for (unsigned j = 0; j < 8; ++j) {
      if (i <= 1) {
        board[i][j].team = BLACK;
        if (i == 1) board[i][j].piece = 'P';
      } else if (i >= 6) {
        board[i][j].team = WHITE;
        board[i][j].piece = board[7 - i][j].piece;
      }
    }
}

bool ChessBoard::Enemies(Team a, Team b) const { return a + b == 3; }

bool ChessBoard::ValidSpot(Team teamOfPiece, const Coord &spot) const {
  if (~7 & spot.row || ~7 & spot.col) return false;

  return board[spot.row][spot.col].team == NONE ||
         Enemies(teamOfPiece, board[spot.row][spot.col].team);
}

std::vector<Coord> ChessBoard::GetHitTilesDirn(Team team, const Coord &currPos,
                                               int rowInc, int colInc) const {
  std::vector<Coord> toReturn;

  if (ValidSpot(team, {currPos.row, currPos.col})) {
    if (board[currPos.row][currPos.col].team == NONE)
      toReturn = GetHitTilesDirn(
          team, {currPos.row + rowInc, currPos.col + colInc}, rowInc, colInc);

    toReturn.push_back(currPos);
  }

  return toReturn;
}

bool ChessBoard::CastlingValid(const Coord &kingP, const Coord &rookP) const {
  BoardPosn rook = board[rookP.row][rookP.col];
  BoardPosn king = board[kingP.row][kingP.col];
  if (king.hasMoved || rook.hasMoved) return false;

  int rookToKing = abs(kingP.col - rookP.col) / (kingP.col - rookP.col);
  for (int i = rookP.col + rookToKing; i != kingP.col; i += rookToKing)
    if (board[kingP.row][i].team != NONE) return false;

  return true;
}

std::vector<Coord> ChessBoard::GetHitTiles(const Coord &coord) const {
  // Gets all the tiles this piece's movements hit. This includes
  // moves that would put own king in check.
  std::vector<Coord> hitTiles;

  int col = coord.col;
  int row = coord.row;
  BoardPosn piece = board[row][col];
  Team team = piece.team;
  char pieceChar = piece.piece;

  switch (pieceChar) {
    case 'K': {  // Knight
      for (int i = 1; i > -2; i -= 2)
        for (int j = 1; j > -2; j -= 2) {
          Coord a = {row + i, col + j * 2};
          Coord b = {row + i * 2, col + j};
          if (ValidSpot(team, a)) hitTiles.push_back(a);
          if (ValidSpot(team, b)) hitTiles.push_back(b);
        }
    } break;
    case 'L': {  // Lord/King
      for (int i = -1; i < 2; ++i)
        for (int j = -1; j < 2; ++j) {
          Coord move = {row + i, col + j};
          if (ValidSpot(team, move)) hitTiles.push_back(move);
        }

      // Castling.
      if (board[row][0].piece == 'R' && CastlingValid(coord, {row, 0}))
        hitTiles.push_back({row, col - 2});

      if (board[row][7].piece == 'R' && CastlingValid(coord, {row, 7}))
        hitTiles.push_back({row, col + 2});
    } break;
    case 'P': {                                  // Pawn
      int forwardDirn = team == WHITE ? -1 : 1;  // Forward dirn for pawns
      int nextRow = row + forwardDirn;           // Capturing and en passant.

      if (col > 0) {
        if (Enemies(team, board[nextRow][col - 1].team) ||
            (Enemies(team, board[row][col - 1].team) &&
             board[row][col - 1].enPassant))
          hitTiles.push_back({nextRow, col - 1});
      }

      if (col < 7) {
        if (Enemies(team, board[nextRow][col + 1].team) ||
            (Enemies(team, board[row][col + 1].team) &&
             board[row][col + 1].enPassant))
          hitTiles.push_back({nextRow, col + 1});
      }

      // Moving forward.
      if (board[nextRow][col].piece == '.') {
        hitTiles.push_back({nextRow, col});
        if (!piece.hasMoved && board[nextRow + forwardDirn][col].piece == '.')
          hitTiles.push_back({nextRow + forwardDirn, col});
      }

    } break;
    case 'Q':    // Queen
    case 'R': {  // Rook
      std::vector<Coord> temp;
      temp = GetHitTilesDirn(team, {row + 1, col}, 1, 0);
      hitTiles.insert(hitTiles.end(), temp.begin(), temp.end());
      temp = GetHitTilesDirn(team, {row, col + 1}, 0, 1);
      hitTiles.insert(hitTiles.end(), temp.begin(), temp.end());
      temp = GetHitTilesDirn(team, {row - 1, col}, -1, 0);
      hitTiles.insert(hitTiles.end(), temp.begin(), temp.end());
      temp = GetHitTilesDirn(team, {row, col - 1}, 0, -1);
      hitTiles.insert(hitTiles.end(), temp.begin(), temp.end());
    }
      if (pieceChar == 'R' || pieceChar == 'r')  // Q also has B moves
        break;
    case 'B': {  // Bishop
      std::vector<Coord> temp;
      temp = GetHitTilesDirn(team, {row + 1, col + 1}, 1, 1);
      hitTiles.insert(hitTiles.end(), temp.begin(), temp.end());
      temp = GetHitTilesDirn(team, {row - 1, col + 1}, -1, 1);
      hitTiles.insert(hitTiles.end(), temp.begin(), temp.end());
      temp = GetHitTilesDirn(team, {row - 1, col - 1}, -1, -1);
      hitTiles.insert(hitTiles.end(), temp.begin(), temp.end());
      temp = GetHitTilesDirn(team, {row + 1, col - 1}, 1, -1);
      hitTiles.insert(hitTiles.end(), temp.begin(), temp.end());
    } break;
  }

  return hitTiles;
}

bool ChessBoard::ChecksEnemyKing(const Coord &loc) const {
  // Find enemy king.
  Coord kingLoc;
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
      if (board[i][j].piece == 'L' &&
          Enemies(board[i][j].team, board[loc.row][loc.col].team)) {
        kingLoc = {i, j};
        break;
      }

  // Check if this piece's movement hits the king's tile.
  std::vector<Coord> hitTiles = GetHitTiles(loc);
  return std::find(hitTiles.begin(), hitTiles.end(), kingLoc) != hitTiles.end();
}

std::vector<Coord> ChessBoard::GetLegalMoves(const Coord &loc) {
  std::vector<Coord> legalMoves;
  BoardPosn piece = board[loc.row][loc.col];
  if (Enemies(playerTurn, piece.team)) return legalMoves;

  std::vector<Coord> tiles = GetHitTiles(loc);
  for (const Coord &move : tiles) {
    // Try move. Check if move results in friendly king check.
    BoardPosn temp = board[move.row][move.col];
    board[move.row][move.col] = piece;
    board[loc.row][loc.col] = BoardPosn();
    for (int i = 0; i < 8; ++i)
      for (int j = 0; j < 8; ++j) {
        if (Enemies(piece.team, board[i][j].team) && ChecksEnemyKing({i, j}))
          goto fixBoard;  // Movement causes king check, move not legal.
      }

    legalMoves.push_back(move);
  fixBoard:
    board[move.row][move.col] = temp;
    board[loc.row][loc.col] = piece;
  }

  return legalMoves;
}
void ChessBoard::Promote(const Coord &coord, char piece) {
  board[coord.row][coord.col].piece = piece;
}

bool ChessBoard::MakeMove(const Coord &from, const Coord &to) {
  board[from.row][from.col].hasMoved = true;

  if (board[from.row][from.col].piece == 'P' && abs(from.row - to.row) == 2)
    board[from.row][from.col].enPassant = true;
  else
    board[from.row][from.col].enPassant = false;

  board[to.row][to.col] = board[from.row][from.col];
  board[from.row][from.col] = BoardPosn();

  // You can only enpassant pawns that moved to the enpassant position
  // on the previous turn. Since we just moved, all enemy enpassant moves
  // are now invalid.
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
      if (Enemies(board[to.row][to.col].team, board[i][j].team))
        board[i][j].enPassant = false;

  // If castling, also need to move the rook.
  if (board[to.row][to.col].piece == 'L') {
    if (from.col - to.col == 2) MakeMove({to.row, 0}, {to.row, 3});

    if (from.col - to.col == -2) MakeMove({to.row, 7}, {to.row, 5});
  }

  if (playerTurn == WHITE)
    playerTurn = BLACK;
  else
    playerTurn = WHITE;

  // returns true if pawn promotion should take place.
  return (to.row == 7 || to.row == 0) && board[to.row][to.col].piece == 'P';
}

bool ChessBoard::Checked(Team team) {
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
      if (Enemies(team, board[i][j].team) && ChecksEnemyKing({i, j}))
        return true;

  return false;
}

bool ChessBoard::Checkmated(Team team) {
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
      if (board[i][j].team == team && GetLegalMoves({i, j}).size() != 0)
        return false;

  return true;
}

void ChessBoard::PrintBoard() {
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j)
      printf("%c", board[i][j].team == BLACK ? board[i][j].piece + 32
                                             : board[i][j].piece);
    printf("\n");
  }
}