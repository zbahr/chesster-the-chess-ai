#include "ai.hpp"

namespace cpp_client
{
namespace chess
{

bool State::isRook(const PieceInfo& p) const
{
    if (p.letter == 'R' || p.letter == 'r')
        return true;
    else
        return false;
}

bool State::isQueen(const PieceInfo& p) const
{
    if (p.letter == 'Q' || p.letter == 'q')
        return true;
    else
        return false;
}

bool State::isBishop(const PieceInfo& p) const
{
    if (p.letter == 'B' || p.letter == 'b')
        return true;
    else
        return false;
}

bool State::isKnight(const PieceInfo& p) const
{
    if (p.letter == 'N' || p.letter == 'n')
        return true;
    else
        return false;
}

bool State::isPawn(const PieceInfo& p) const
{
    if (p.letter == 'P' || p.letter == 'p')
        return true;
    else
        return false;
}

bool State::isKing(const PieceInfo& p) const
{
    if (p.letter == 'K' || p.letter == 'k')
        return true;
    else
        return false;
}

bool State::isPromotion(const std::string& promotion) const
{
    if (promotion == "Queen" || promotion == "Rook" || promotion == "Bishop" || promotion == "Knight")
        return true;
    else
        return false;
}

int State::convertFile(const std::string& file) const
{
    switch(file.at(0))
    {
        case 'a': return 0;
        case 'b': return 1;
        case 'c': return 2;
        case 'd': return 3;
        case 'e': return 4;
        case 'f': return 5;
        case 'g': return 6;
        case 'h': return 7;
        default: exit(1);
    }
}

std::string State::convertToFile(const int& i) const
{
    switch(i)
    {
        case 1: return "a";
        case 2: return "b";
        case 3: return "c";
        case 4: return "d";
        case 5: return "e";
        case 6: return "f";
        case 7: return "g";
        case 8: return "h";
        default: return "NULL";
    }
}

std::string State::leftFile(const std::string& file) const
{
    switch (file.at(0))
    {
        case 'a': return "NULL";
        case 'b': return "a";
        case 'c': return "b";
        case 'd': return "c";
        case 'e': return "d";
        case 'f': return "e";
        case 'g': return "f";
        case 'h': return "g";
        case 'N': return "NULL";
        default: exit(1);
    }

}
std::string State::rightFile(const std::string& file) const
{
    switch (file.at(0))
    {
        case 'a': return "b";
        case 'b': return "c";
        case 'c': return "d";
        case 'd': return "e";
        case 'e': return "f";
        case 'f': return "g";
        case 'g': return "h";
        case 'h': return "NULL";
        case 'N': return "NULL";
        default: exit(1);
    }
}

////////////////////////////////////////////////////////////////////////////////

State::State()
{
    for (int i = 0; i < RANK; ++i)
    {
        for (int j = 0; j < FILE; ++j)
        {
            board[i][j].letter = '-';
            board[i][j].color = '-';
            board[i][j].id = 0;
        }
    }
}

// Copy Constructor (primarily used to generate child states)
State::State(const State& state, const int& fromRank, const std::string& fromFile, const std::tuple<int, std::string, std::string>& move)
{
    //////////////////////////// COPY FROM PARENT //////////////////////////////
    // Get the destination of moving piece
    int toRank = std::get<0>(move);
    std::string toFile = std::get<1>(move);

    // Get moving piece and destination piece
    PieceInfo pieceMoved = state(fromRank, fromFile);
    PieceInfo oppPiece = state(toRank, toFile);

    // Set the board state from parent state
    for (int i = 0; i < RANK; ++i)
    {
        for (int j = 0; j < FILE; ++j)
        {
            board[i][j].letter = state(i + 1, j).letter;
            board[i][j].color = state(i + 1, j).color;
            board[i][j].id = state(i + 1, j).id;
        }
    }

    // Set king ranks from parent state for myself and opponent.
    if (pieceMoved.letter == 'K' || pieceMoved.letter == 'k')
    {
        this->setMyKingRank(toRank);
        this->setMyKingFile(toFile);
    }
    else
    {
        myKingRank = state.getMyKingRank();
        myKingFile = state.getMyKingFile();
    }
    oppKingFile = state.getOppKingFile();
    oppKingRank = state.getOppKingRank();

    // Copy the containers containg pieces in play for both players
    inPlay = state.getInPlay();
    oppInPlay = state.getOppInPlay();

    // Copy color
    playerColor = state.getPlayerColor();

    moveTracker = state.getMoveTracker();
    ////////////////////////////////////////////////////////////////////////////

    /////////////////////// MONITOR CASTLING/EN PASSANT ////////////////////////

    // Set/update en passant space for new state
    if (isPawn(pieceMoved) && abs(toRank - fromRank) == 2)
    {
        int oppDirection = (pieceMoved.color == 'w') ? -1 : 1;
        setEnPassant(toRank + oppDirection, fromFile);
    }
    else
        setEnPassant(-1, "NULL");

    // Set/update castling for new state
    if (std::get<2>(move) == "CASTLE-KING" || std::get<2>(move) == "CASTLE-QUEEN")
    {
        myKingCastle = false;
        myQueenCastle = false;
    }
    else
    {
        myKingCastle = state.kingCastleStatus();
        myQueenCastle = state.queenCastleStatus();
    }

    // If rooks/king move from original position, take away castling status
    if (isRook(pieceMoved) && (fromRank == 1 || fromRank == 8) && (fromFile == "a"))
        myQueenCastle = false;
    else if (isRook(pieceMoved) && (fromRank == 1 || fromRank == 8) && (fromFile == "h"))
        myKingCastle = false;
    else if (isKing(pieceMoved))
    {
        myKingCastle = false;
        myQueenCastle = false;
    }

    oppKingCastle = state.oppKingCastleStatus();
    oppQueenCastle = state.oppQueenCastleStatus();

    ////////////////////////////////////////////////////////////////////////////

    ///////////////////////////// PIECE MANAGEMENT /////////////////////////////

    // If piece captures opponent's, remove from opponent's list of pieces
    std::map<PieceInfo, std::tuple<int, std::string>, Comparator>::iterator it = oppInPlay.find(oppPiece);
    if (it != oppInPlay.end())
    {
        // If my pieces capture opponent's rooks that haven't moved, take away castling status.
        if (isRook(oppPiece) && (std::get<0>(std::get<1>(*it)) == 1 || std::get<0>(std::get<1>(*it))== 8) && std::get<1>(std::get<1>(*it)) == "h")
            oppKingCastle = false;
        else if (isRook(oppPiece) && (std::get<0>(std::get<1>(*it)) == 1 || std::get<0>(std::get<1>(*it))== 8) && std::get<1>(std::get<1>(*it)) == "a")
            oppQueenCastle = false;

        oppInPlay.erase(oppPiece);
    }

    it = inPlay.find(oppPiece);
    if (it != inPlay.end())
    {
        inPlay.erase(oppPiece);
    }

    /////// Update location of moved pieces in lists and on the board //////////
    it = oppInPlay.find(pieceMoved);
    if (it != oppInPlay.end())
        oppInPlay[pieceMoved] = std::make_tuple(toRank, toFile);

    it = inPlay.find(pieceMoved);
    if (it != inPlay.end())
        inPlay[pieceMoved] = std::make_tuple(toRank, toFile);

    if (std::get<2>(move) == "CASTLE-KING")
    {
        PieceInfo rookToMove = this->operator()(fromRank,"h");
        inPlay[rookToMove] = std::make_tuple(fromRank,"f");
        remove(fromRank,"h");
        add(rookToMove, fromRank, "f");
    }
    else if (std::get<2>(move) == "CASTLE-QUEEN")
    {
        PieceInfo rookToMove = this->operator()(fromRank,"a");
        inPlay[rookToMove] = std::make_tuple(fromRank,"d");
        remove(fromRank,"a");
        add(rookToMove, fromRank, "d");
    }
    else if (std::get<2>(move) == "EN PASSANT")
    {
        PieceInfo pawnToRemove = this->operator()(fromRank, toFile);
        oppInPlay.erase(pawnToRemove);
        remove(fromRank, toFile);
    }

    // Update board with piece moved and piece removed
    remove(fromRank, fromFile);
    add(pieceMoved, toRank, toFile);

    // Accounting for possible promotions
    if (std::get<2>(move) == "Rook")
    {
        PieceInfo pawnToPromote = this->operator()(toRank, toFile);
        inPlay.erase(pawnToPromote);
        pawnToPromote.letter = (pawnToPromote.color == 'w') ? 'R' : 'r';
        inPlay[pawnToPromote] = std::make_tuple(toRank, toFile);

        remove(toRank, toFile);
        add(pawnToPromote, toRank, toFile);
    }
    else if (std::get<2>(move) == "Knight")
    {
        PieceInfo pawnToPromote = this->operator()(toRank, toFile);
        inPlay.erase(pawnToPromote);
        pawnToPromote.letter = (pawnToPromote.color == 'w') ? 'N' : 'n';
        inPlay[pawnToPromote] = std::make_tuple(toRank, toFile);

        remove(toRank, toFile);
        add(pawnToPromote, toRank, toFile);
    }
    else if (std::get<2>(move) == "Bishop")
    {
        PieceInfo pawnToPromote = this->operator()(toRank, toFile);
        inPlay.erase(pawnToPromote);
        pawnToPromote.letter = (pawnToPromote.color == 'w') ? 'B' : 'b';
        inPlay[pawnToPromote] = std::make_tuple(toRank, toFile);

        remove(toRank, toFile);
        add(pawnToPromote, toRank, toFile);
    }
    else if (std::get<2>(move) == "Queen")
    {
        PieceInfo pawnToPromote = this->operator()(toRank, toFile);
        inPlay.erase(pawnToPromote);
        pawnToPromote.letter = (pawnToPromote.color == 'w') ? 'Q' : 'q';
        inPlay[pawnToPromote] = std::make_tuple(toRank, toFile);

        remove(toRank, toFile);
        add(pawnToPromote, toRank, toFile);
    }

    ////////////////////////////////////////////////////////////////////////////

    /////////////////////// KEEP TABS ON PREVIOUS MOVES ////////////////////////
    prevMoves = state.getPrevMoves();

    if (prevMoves.size() >= 8)
    {
        prevMoves.erase(prevMoves.begin());
        prevMoves.push_back(std::make_tuple(std::make_tuple(fromRank, fromFile), std::make_tuple(toRank, toFile)));
    }
    else
        prevMoves.push_back(std::make_tuple(std::make_tuple(fromRank, fromFile), std::make_tuple(toRank, toFile)));
}

// Function to update state when opposing player makes a move
void State::updateState(const int fromRank, const std::string fromFile, const int toRank, const std::string toFile, const std::string promotion)
{
    PieceInfo pieceMoved = this->operator()(fromRank, fromFile);
    PieceInfo destPiece = this->operator()(toRank, toFile);

    // If one of my pieces is captured from opponent move, remove from my inventory
    if (destPiece.letter != '-')
        inPlay.erase(destPiece);

    ///////////////////////// MONITOR CASTLING STATUS //////////////////////////
    if (isKing(pieceMoved))
    {
        oppKingCastle = false;
        oppQueenCastle = false;

        setOppKingRank(toRank);
        setOppKingFile(toFile);
    }
    else if (isRook(pieceMoved) && fromFile == "a")
        oppQueenCastle = false;
    else if (isRook(pieceMoved) && fromFile == "h")
        oppKingCastle = false;

    // If opponent piece captures rooks, update castling status
    if (toRank == myKingRank && toFile == "a")
        myQueenCastle = false;
    else if (toRank == myKingRank && toFile == "h")
        myKingCastle = false;
    //////////////////////////// OPP KING-CASTLE //////////////////////////////
    if (isKing(pieceMoved) && toFile == rightFile(rightFile(fromFile)))
    {
        std::cout << "test" << std::endl;
        PieceInfo rookToMove = this->operator()(fromRank, "h");
        oppInPlay.erase(rookToMove);
        oppInPlay[rookToMove] = std::make_tuple(fromRank, "f");

        remove(fromRank, "h");
        add(rookToMove, fromRank, "f");
    }
    /////////////////////////// OPP QUEEN-CASTLE ///////////////////////////////
    if (isKing(pieceMoved) && toFile == leftFile(leftFile(fromFile)))
    {
        PieceInfo rookToMove = this->operator()(fromRank, "a");
        oppInPlay.erase(rookToMove);
        oppInPlay[rookToMove] = std::make_tuple(fromRank, "d");

        remove(fromRank, "a");
        add(rookToMove, fromRank, "d");
    }
    ///////////////////////////// OPP EN PASSANT ///////////////////////////////
    if (isPawn(pieceMoved) && isEmpty(toRank, toFile) && toFile != fromFile)
    {
        int oppDirection = (pieceMoved.color == 'w') ? -1 : 1;
        PieceInfo pieceToRemove = this->operator()(toRank + oppDirection, toFile);

        inPlay.erase(pieceToRemove);
        remove(toRank + oppDirection, toFile);
    }

    ///////////////////////// UPDATE 50 MOVE TRACKER ///////////////////////////
    if (destPiece.letter != '-' || isPawn(pieceMoved))
        moveTracker = 0;
    else
        moveTracker++;

    // Update location of opponent's moved piece in inventory
    oppInPlay.erase(pieceMoved);
    oppInPlay[pieceMoved] = std::make_tuple(toRank, toFile);

    // Update the board representation with the moved piece
    remove(toRank, toFile);
    remove(fromRank, fromFile);
    add(pieceMoved, toRank, toFile);

    // Account for promotions
    if (promotion == "Rook")
    {
        PieceInfo pawnToPromote = this->operator()(toRank, toFile);
        oppInPlay.erase(pawnToPromote);
        pawnToPromote.letter = (pawnToPromote.color == 'w') ? 'R' : 'r';
        oppInPlay[pawnToPromote] = std::make_tuple(toRank, toFile);

        remove(toRank, toFile);
        add(pawnToPromote, toRank, toFile);
    }
    else if (promotion == "Bishop")
    {
        PieceInfo pawnToPromote = this->operator()(toRank, toFile);
        oppInPlay.erase(pawnToPromote);
        pawnToPromote.letter = (pawnToPromote.color == 'w') ? 'B' : 'b';
        oppInPlay[pawnToPromote] = std::make_tuple(toRank, toFile);

        remove(toRank, toFile);
        add(pawnToPromote, toRank, toFile);
    }
    else if (promotion == "Knight")
    {
        PieceInfo pawnToPromote = this->operator()(toRank, toFile);
        oppInPlay.erase(pawnToPromote);
        pawnToPromote.letter = (pawnToPromote.color == 'w') ? 'N' : 'n';
        oppInPlay[pawnToPromote] = std::make_tuple(toRank, toFile);

        remove(toRank, toFile);
        add(pawnToPromote, toRank, toFile);
    }
    else if (promotion == "Queen")
    {
        PieceInfo pawnToPromote = this->operator()(toRank, toFile);
        oppInPlay.erase(pawnToPromote);
        pawnToPromote.letter = (pawnToPromote.color == 'w') ? 'Q' : 'q';
        oppInPlay[pawnToPromote] = std::make_tuple(toRank, toFile);

        remove(toRank, toFile);
        add(pawnToPromote, toRank, toFile);
    }

    // Monitor en passant status
    int primeDirectionOpp = (playerColor == 'w') ? -1 : 1;
    if (this->operator()(toRank, toFile).letter == 'P' || this->operator()(toRank, toFile).letter == 'p')
    {
        if ((fromRank == WHITE_PAWN_INIT_RANK || fromRank == BLACK_PAWN_INIT_RANK) && toRank == (fromRank + (2 * primeDirectionOpp)))
            setEnPassant(toRank + (-1 * primeDirectionOpp), toFile);
        else
            setEnPassant(-1, "NULL");
    }
    else
        setEnPassant(-1, "NULL");

    std::cout << "En passant space: " << std::get<1>(getEnPassant()) << std::get<0>(getEnPassant()) << std::endl;

    // Monitor history

    if (prevMoves.size() >= 8)
    {
        prevMoves.erase(prevMoves.begin());
        prevMoves.push_back(std::make_tuple(std::make_tuple(fromRank, fromFile), std::make_tuple(toRank, toFile)));
    }
    else
        prevMoves.push_back(std::make_tuple(std::make_tuple(fromRank, fromFile), std::make_tuple(toRank, toFile)));
}

// Swaps sides so Min-Player can generate child states
void State::switchSides()
{
    std::map<PieceInfo, std::tuple<int, std::string>, Comparator> temp;
    temp = inPlay;
    inPlay = oppInPlay;
    oppInPlay = temp;

    std::swap(myKingCastle, oppKingCastle);
    std::swap(myQueenCastle, oppQueenCastle);
    std::swap(myKingRank, oppKingRank);
    std::swap(myKingFile, oppKingFile);

    playerColor = (playerColor == 'w') ? 'b' : 'w';

    return;
}

// State evaluation heuristic function
int State::stateHeuristic(const char& playerColor) const
{
    int value = 0;

    // Iterates through all pieces, incrementing and decrementing based on the value of the piece
    // and the player that is calling the state eval heuristic.
    std::map<PieceInfo, std::tuple<int, std::string>, Comparator>::const_iterator it;
    for (it = inPlay.begin(); it != inPlay.end(); it++)
    {
        switch(std::get<0>(*it).letter)
        {
            case 'Q': value += (playerColor == 'w') ? 9 : -9; break;
            case 'R': value += (playerColor == 'w') ? 5 : -5; break;
            case 'B': value += (playerColor == 'w') ? 3 : -3; break;
            case 'N': value += (playerColor == 'w') ? 3 : -3; break;
            case 'P': value += (playerColor == 'w') ? 1 : -1; break;
            case 'q': value += (playerColor == 'w') ? -9 : 9; break;
            case 'r': value += (playerColor == 'w') ? -5 : 5; break;
            case 'b': value += (playerColor == 'w') ? -3 : 3; break;
            case 'n': value += (playerColor == 'w') ? -3 : 3; break;
            case 'p': value += (playerColor == 'w') ? -1 : 1; break;
            default: break;
        }
    }

    for (it = oppInPlay.begin(); it != oppInPlay.end(); it++)
    {
        switch(std::get<0>(*it).letter)
        {
            case 'Q': value += (playerColor == 'w') ? 9 : -9; break;
            case 'R': value += (playerColor == 'w') ? 5 : -5; break;
            case 'B': value += (playerColor == 'w') ? 3 : -3; break;
            case 'N': value += (playerColor == 'w') ? 3 : -3; break;
            case 'P': value += (playerColor == 'w') ? 1 : -1; break;
            case 'q': value += (playerColor == 'w') ? -9 : 9; break;
            case 'r': value += (playerColor == 'w') ? -5 : 5; break;
            case 'b': value += (playerColor == 'w') ? -3 : 3; break;
            case 'n': value += (playerColor == 'w') ? -3 : 3; break;
            case 'p': value += (playerColor == 'w') ? -1 : 1; break;
            default: break;
        }
    }

    // Bonus points for castling
    // To add later

    return value;
}

// Quiescent state evaluation function
bool State::isQuiet()
{
    // For now, just checks if either player is in check. Returns false if one is in check.
    if (!kingInCheck())
    {
        switchSides();

        if (!kingInCheck())
        {
            switchSides();
            return true;
        }

        switchSides();
    }

    return false;
}

bool State::isDraw()
{
    // Three-fold repetition
    if (prevMoves.size() == 8)
    {
        if (prevMoves.at(0) == prevMoves.at(4) && prevMoves.at(1) == prevMoves.at(5) && prevMoves.at(2) == prevMoves.at(6) && prevMoves.at(3) == prevMoves.at(7))
            return true;
    }

    // Stalemate possibility
    switchSides();
    if (!kingInCheck() && kingInCheckMate())
    {
        switchSides();
        return true;
    }
    switchSides();

    // Insufficient material
    if (inPlay.size() <= 2 && oppInPlay.size() <= 2)
    {
        // King vs. King
        if (inPlay.size() == 1 && oppInPlay.size() == 1)
            return true;

        // King vs. Knight-King or King vs. Bishop-King
        if (inPlay.size() == 1)
        {
            int count = 0;
            for (std::map<PieceInfo, std::tuple<int, std::string>, Comparator>::iterator it = oppInPlay.begin(); it != oppInPlay.end(); it++)
                if (toupper(it->first.letter) == 'N' || toupper(it->first.letter) == 'K') {count++;}

            if (count == 2)
                return true;
            ///////////////////////////////////////////////////////////////////
            count = 0;
            for (std::map<PieceInfo, std::tuple<int, std::string>, Comparator>::iterator it = oppInPlay.begin(); it != oppInPlay.end(); it++)
                if (toupper(it->first.letter) == 'B' || toupper(it->first.letter) == 'K') {count++;}

            if (count == 2)
                return true;
        }

        // Knight-King vs. King or Bishop-King vs. King
        switchSides();
        if (inPlay.size() == 1)
        {
            int count = 0;
            for (std::map<PieceInfo, std::tuple<int, std::string>, Comparator>::iterator it = oppInPlay.begin(); it != oppInPlay.end(); it++)
                if (toupper(it->first.letter) == 'N' || toupper(it->first.letter) == 'K') {count++;}

            if (count == 2)
                return true;
            ///////////////////////////////////////////////////////////////////
            count = 0;
            for (std::map<PieceInfo, std::tuple<int, std::string>, Comparator>::iterator it = oppInPlay.begin(); it != oppInPlay.end(); it++)
                if (toupper(it->first.letter) == 'B' || toupper(it->first.letter) == 'K') {count++;}

            if (count == 2)
                return true;
        }
        switchSides();
    }

    // 50 move rule
    if (moveTracker == 50)
    {
        std::cout << "50 move state ahead!" << std::endl;
        return true;
    }

    return false;
}

bool State::isWin(const char& color)
{
    bool switchedSides = false;

    if (playerColor == color)
    {
        switchedSides = true;
        switchSides();
    }

    if (kingInCheck() && kingInCheckMate())
    {
        if (switchedSides)
            switchSides();

        return true;
    }

    if (switchedSides)
        switchSides();

    return false;
}

bool State::isLoss(const char& color)
{
    bool switchedSides = false;

    if (playerColor != color)
    {
        switchedSides = true;
        switchSides();
    }

    if (kingInCheck() && kingInCheckMate())
    {
        if (switchedSides)
            switchSides();

        return true;
    }

    if (switchedSides)
        switchSides();

    return false;

}

bool State::isEmpty(const int a, const std::string& b) const
{
    int c = convertFile(b);

    if (board[a - 1][c].letter == '-')
        return true;
    else
        return false;
}

bool State::isOpponent(const char color, const int a, const std::string& b) const
{
    int c = convertFile(b);
    char opponent = ((color == 'w') ? 'b' : 'w');

    if (board[a - 1][c].color == opponent)
        return true;
    else
        return false;

}

void State::printCaptured() const
{
    /*for (unsigned int i = 0; i < captured.size(); ++i)
    {
        std::cout << captured.at(i)->type << std::endl;
    }*/

    return;
}

void State::setBoard(const char& c, const char& l, const int& num, const int a, const int b)
{
    char oppColor;

    if (c == 'w')
        oppColor = 'b';
    else if (c == 'b')
        oppColor = 'w';
    else
        oppColor = '-';

    // Set PieceInfo in the board
    board[a - 1][b].letter = l;
    board[a - 1][b].color = c;
    board[a - 1][b].id = num;

    // Keep tabs on where pieces are using maps.
    if (playerColor == c)
        inPlay[PieceInfo(c, l, num)] = std::make_tuple(a, convertToFile(b + 1));
    else if (playerColor == oppColor)
        oppInPlay[PieceInfo(c, l, num)] = std::make_tuple(a, convertToFile(b + 1));

    return;
}

void State::setEnPassant(const int& rank, const std::string& file)
{
    enPassantSpace = std::make_tuple(rank, file);

    return;
}

void State::add(const PieceInfo p, const int a, const std::string& b)
{
    int d = convertFile(b);

    board[a - 1][d].letter = p.letter;
    board[a - 1][d].color = p.color;
    board[a - 1][d].id = p.id;

    return;
}

void State::remove(const int a, const std::string& b)
{
    int c = convertFile(b);

    board[a - 1][c].letter = '-';
    board[a - 1][c].color = '-';
    board[a - 1][c].id = 0;

    return;
}

void State::printPieces() const
{
    for (std::map<PieceInfo, std::tuple<int, std::string>>::const_iterator it = inPlay.begin(); it != inPlay.end(); it++)
    {
        auto rankFile = it->second;
        std::cout << it->first << " at " << std::get<1>(rankFile) << std::get<0>(rankFile) << std::endl;
    }

    return;
}

void State::printOppPieces()
{
    for (std::map<PieceInfo, std::tuple<int, std::string>>::iterator it = oppInPlay.begin(); it != oppInPlay.end(); it++)
    {
        auto rankFile = it->second;
        std::cout << it->first << " at " << std::get<1>(rankFile) << std::get<0>(rankFile) << std::endl;
    }

    return;
}

void State::printPrevMoves()
{
    std::cout << "Previous Moves: " << std::endl;
    for (unsigned int i = 0; i < prevMoves.size(); i++)
    {
        std::cout << std::get<1>(std::get<0>(prevMoves.at(i))) << std::get<0>(std::get<0>(prevMoves.at(i)))
                  << " to " << std::get<1>(std::get<1>(prevMoves.at(i))) << std::get<0>(std::get<1>(prevMoves.at(i))) << std::endl;
    }

    return;
}

// Function to generate all of the valid child states of a particular state
std::vector<std::tuple<State, PieceInfo, std::tuple<int, std::string, std::string>>> State::generateChildren() const
{
    // Container for child states with associated move that results in that child state
    std::vector<std::tuple<State, PieceInfo, std::tuple<int, std::string, std::string>>> childStates;

    // My implementation of a priority queue that sorts by history table value needs work. Will be fixed for Phase IV makeup.
    //std::priority_queue<StateActionPair, std::vector<StateActionPair>, myComparison> childStates;

    for (std::map<PieceInfo, std::tuple<int, std::string>>::const_iterator it = inPlay.begin(); it != inPlay.end(); it++)
    {
        std::vector<std::tuple<int, std::string, std::string>> possibleMoves;
        PieceInfo piece = it->first;
        auto rankFile = it->second;

        if (toupper(piece.letter) == 'P')
            pawnMoves(piece, rankFile, possibleMoves);
        else if (toupper(piece.letter) == 'R')
            rookMoves(piece, rankFile, possibleMoves);
        else if (toupper(piece.letter) == 'N')
            knightMoves(piece, rankFile, possibleMoves);
        else if (toupper(piece.letter) == 'B')
            bishopMoves(piece, rankFile, possibleMoves);
        else if (toupper(piece.letter) == 'Q')
            queenMoves(piece, rankFile, possibleMoves);
        else
            kingMoves(piece, rankFile, possibleMoves);

        /*if (possibleMoves.size() != 0)
            std::cout << "Possible states:" << std::endl;*/
        for (unsigned int i = 0; i < possibleMoves.size(); i++)
        {
            State child(*this, std::get<0>(rankFile), std::get<1>(rankFile), possibleMoves.at(i));

            if (!child.kingInCheck())
            {
                /*std::cout << piece << " at " << std::get<1>(rankFile) << std::get<0>(rankFile) << " to " << std::get<1>(possibleMoves.at(i))
                          << std::get<0>(possibleMoves.at(i)) << std::endl;
                std::cout << child << std::endl << std::endl;*/

                childStates.push_back(std::make_tuple(child, piece, possibleMoves.at(i)));
            }
        }
    }

    return childStates;
}

bool State::kingInCheck() const
{
    int r;
    std::string f;

    ///////////// Check if king is in check by opposing rook/queen /////////////
    r = myKingRank + 1;
    f = myKingFile;
    while (r <= RANK)
    {
        // In check by opposing rook/queen
        if (isOpponent(playerColor, r, f) && (isRook(this->operator()(r, f)) || (isQueen(this->operator()(r, f)))))
            return true;
        // Empty space, keep looking
        else if (isEmpty(r, f))
            r++;
        // Own piece/opponent's piece that can't take king
        else
            break;
    }

    r = myKingRank - 1;
    while (r > 0)
    {
        if (isOpponent(playerColor, r, f) && (isRook(this->operator()(r, f)) || (isQueen(this->operator()(r, f)))))
            return true;
        else if (isEmpty(r, f))
            r--;
        else
            break;
    }

    r = myKingRank;
    f = rightFile(myKingFile);
    while (f != "NULL")
    {
        if (isOpponent(playerColor, r, f) && (isRook(this->operator()(r, f)) || (isQueen(this->operator()(r, f)))))
            return true;
        else if (isEmpty(r, f))
            f = rightFile(f);
        else
            break;
    }

    f = leftFile(myKingFile);
    while (f != "NULL")
    {
        if (isOpponent(playerColor, r, f) && (isRook(this->operator()(r, f)) || (isQueen(this->operator()(r, f)))))
            return true;
        else if (isEmpty(r, f))
            f = leftFile(f);
        else
            break;
    }
    ////////////////////////////////////////////////////////////////////////////

    //////////// Check if king is in check by opposing bishop/queen ////////////
    r = myKingRank + 1;
    f = leftFile(myKingFile);
    while (r <= RANK && f != "NULL")
    {
        if (isOpponent(playerColor, r, f) && (isBishop(this->operator()(r, f)) || (isQueen(this->operator()(r,f)))))
            return true;
        else if (isEmpty(r, f))
        {
            r++;
            f = leftFile(f);
        }
        else
            break;
    }

    r = myKingRank + 1;
    f = rightFile(myKingFile);
    while (r <= RANK && f != "NULL")
    {
        if (isOpponent(playerColor, r, f) && (isBishop(this->operator()(r, f)) || (isQueen(this->operator()(r,f)))))
            return true;
        else if (isEmpty(r, f))
        {
            r++;
            f = rightFile(f);
        }
        else
            break;
    }

    r = myKingRank - 1;
    f = leftFile(myKingFile);
    while (r > 0 && f != "NULL")
    {
        if (isOpponent(playerColor, r, f) && (isBishop(this->operator()(r, f)) || (isQueen(this->operator()(r,f)))))
            return true;
        else if (isEmpty(r, f))
        {
            r--;
            f = leftFile(f);
        }
        else
            break;
    }

    r = myKingRank - 1;
    f = rightFile(myKingFile);
    while (r > 0 && f != "NULL")
    {
        if (isOpponent(playerColor, r, f) && (isBishop(this->operator()(r, f)) || (isQueen(this->operator()(r,f)))))
            return true;
        else if (isEmpty(r, f))
        {
            r--;
            f = rightFile(f);
        }
        else
            break;
    }
    ////////////////////////////////////////////////////////////////////////////

    ////////////// Check if king is in check by opposing knight ////////////////

    r = myKingRank + 2;
    f = rightFile(myKingFile);
    if (r <= RANK && f != "NULL" && isOpponent(playerColor, r, f) && isKnight(this->operator()(r, f)))
        return true;

    f = leftFile(myKingFile);
    if (r <= RANK && f != "NULL" && isOpponent(playerColor, r, f) && isKnight(this->operator()(r, f)))
        return true;

    r = myKingRank + 1;
    f = rightFile(rightFile(myKingFile));
    if (r <= RANK && f != "NULL" && isOpponent(playerColor, r, f) && isKnight(this->operator()(r, f)))
        return true;

    f = leftFile(leftFile(myKingFile));
    if (r <= RANK && f != "NULL" && isOpponent(playerColor, r, f) && isKnight(this->operator()(r, f)))
        return true;

    r = myKingRank - 2;
    f = rightFile(myKingFile);
    if (r > 0 && f != "NULL" && isOpponent(playerColor, r, f) && isKnight(this->operator()(r, f)))
        return true;

    f = leftFile(myKingFile);
    if (r > 0 && f != "NULL" && isOpponent(playerColor, r, f) && isKnight(this->operator()(r, f)))
        return true;

    r = myKingRank - 1;
    f = rightFile(rightFile(myKingFile));
    if (r > 0 && f != "NULL" && isOpponent(playerColor, r, f) && isKnight(this->operator()(r, f)))
        return true;

    f = leftFile(leftFile(myKingFile));
    if (r > 0 && f != "NULL" && isOpponent(playerColor, r, f) && isKnight(this->operator()(r, f)))
        return true;
    ////////////////////////////////////////////////////////////////////////////

    /////////////// Check if king is in check by opposing pawns ////////////////
    r = (playerColor == 'w') ? (myKingRank + 1) : (myKingRank - 1);
    f = leftFile(myKingFile);
    if (r <= RANK && r > 0 && f != "NULL" && isOpponent(playerColor, r, f) && isPawn(this->operator()(r, f)))
        return true;

    f = rightFile(myKingFile);
    if (r <= RANK && r > 0 && f != "NULL" && isOpponent(playerColor, r, f) && isPawn(this->operator()(r, f)))
        return true;
    ////////////////////////////////////////////////////////////////////////////

    ////////////// Check if king is in check by opposing king //////////////////

    r = myKingRank + 1;
    f = myKingFile;
    if (r <= RANK && f != "NULL" && isOpponent(playerColor, r, f) && isKing(this->operator()(r, f)))
        return true;

    f = leftFile(myKingFile);
    if (r <= RANK && f != "NULL" && isOpponent(playerColor, r, f) && isKing(this->operator()(r, f)))
        return true;

    f = rightFile(myKingFile);
    if (r <= RANK && f != "NULL" && isOpponent(playerColor, r, f) && isKing(this->operator()(r, f)))
        return true;

    r = myKingRank;
    f = leftFile(myKingFile);
    if (r <= RANK && f != "NULL" && isOpponent(playerColor, r, f) && isKing(this->operator()(r, f)))
        return true;

    f = rightFile(myKingFile);
    if (r <= RANK && f != "NULL" && isOpponent(playerColor, r, f) && isKing(this->operator()(r, f)))
        return true;

    r = myKingRank - 1;
    f = myKingFile;
    if (r > 0 && f != "NULL" && isOpponent(playerColor, r, f) && isKing(this->operator()(r, f)))
        return true;

    f = leftFile(myKingFile);
    if (r > 0 && f != "NULL" && isOpponent(playerColor, r, f) && isKing(this->operator()(r, f)))
        return true;

    f = rightFile(myKingFile);
    if (r > 0 && f != "NULL" && isOpponent(playerColor, r, f) && isKing(this->operator()(r, f)))
        return true;

    return false;
}

bool State::kingInCheckMate() const
{
    StateActionPair childStates = this->generateChildren();

    for (unsigned int i = 0; i < childStates.size(); i++)
    {
        if (!std::get<0>(childStates.at(i)).kingInCheck())
            return false;
    }

    //std::cout << "Checkmate ahead!" << std::endl;
    return true;


    /*unsigned int numInCheck = 0;

    std::vector<std::tuple<int, std::string, std::string>> possibleMoves;
    kingMoves(this->operator()(myKingRank, myKingFile), std::make_tuple(myKingRank, myKingFile), possibleMoves);

    for (unsigned int i = 0; i < possibleMoves.size(); i++)
    {
        State child(*this, myKingRank, myKingFile, possibleMoves.at(i));

        if (child.kingInCheck())
            numInCheck++;
    }

    if (numInCheck == possibleMoves.size())
        return true;
    else
        return false;*/
}

const PieceInfo& State::operator()(int a, const std::string& b) const
{
    int c = convertFile(b);

    if (a > 0 && a <= RANK)
    {
        return board[a - 1][c];
    }
    else
        exit(1);
}

const PieceInfo& State::operator()(int a, int b) const
{
    if (a > 0 && b >= 0 && a <= RANK && b < FILE)
        return board[a - 1][b];
    else
        exit(1);
}

std::ostream& operator<<(std::ostream& out, const State& s)
{
    out << "    ";
    for (int i = 1; i <= 8; i++)
        out << s.convertToFile(i) << "   ";

    out << std::endl;

    for (int i = 8; i >= 1; i--)
    {
        out << i <<  " | ";
        for (int j = 0; j < FILE; ++j)
        {
            out << s(i, j).letter << " | ";
        }

        out << std::endl;
    }

    out << "My King is at position " << s.myKingFile << s.myKingRank << std::endl;
    /*out << "King-side castle status: " << s.kingCastleStatus() << std::endl;
    out << "Queen-side castle status: " << s.queenCastleStatus()  << std::endl;
    out << "Opponent king-side castle status: " << s.oppKingCastleStatus() << std::endl;
    out << "Opponent queen-side castle status: " << s.oppQueenCastleStatus() << std::endl;*/

    return out;
}

std::ostream& operator<<(std::ostream& out, const PieceInfo& p)
{
    switch(p.letter)
    {
        case 'K': out << "White King with ID " << p.id; break;
        case 'Q': out << "White Queen with ID " << p.id; break;
        case 'N': out << "White Knight with ID " << p.id; break;
        case 'B': out << "White Bishop with ID " << p.id; break;
        case 'R': out << "White Rook with ID " << p.id; break;
        case 'P': out << "White Pawn with ID " << p.id; break;
        case 'k': out << "Black King with ID " << p.id; break;
        case 'q': out << "Black Queen with ID " << p.id; break;
        case 'n': out << "Black Knight with ID " << p.id; break;
        case 'b': out << "Black Bishop with ID " << p.id; break;
        case 'r': out << "Black Rook with ID " << p.id; break;
        case 'p': out << "Black Pawn with ID " << p.id; break;
        default: break;
    }

    return out;
}

}
}
