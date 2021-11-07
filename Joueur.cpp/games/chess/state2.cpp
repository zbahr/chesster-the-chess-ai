#include "ai.hpp"

namespace cpp_client
{
namespace chess
{
/* Generates all possible moves (tuple containing rank and file of desired
   location) for the pawn p */
void State::pawnMoves(const PieceInfo& p, const std::tuple<int, std::string>& rankFile, std::vector<std::tuple<int, std::string, std::string>>& possibleMoves) const
{
    // Initial rank and file of pawn p
    int initRank = getRank(rankFile);
    std::string initFile = getFile(rankFile);
    auto leftNeighbor = std::make_tuple(initRank, leftFile(initFile));
    auto rightNeighbor = std::make_tuple(initRank, rightFile(initFile));

    // Possible ending rank and file of pawn p
    int newRank = (p.color == 'w') ? (initRank + 1) : (initRank - 1);
    std::string newLeftFile = leftFile(initFile);
    std::string newRightFile = rightFile(initFile);

    // White player's pawns can move 2 ranks up if in starting position
    if (p.color == 'w')
    {
        if (initRank == WHITE_PAWN_INIT_RANK && isEmpty(newRank, initFile) && isEmpty(newRank + 1, initFile))
            possibleMoves.push_back(std::make_tuple(newRank + 1, initFile, "NULL"));
    }
    // Black player's pawns can move 2 ranks down if in starting position
    else
    {
        if (initRank == BLACK_PAWN_INIT_RANK && isEmpty(newRank, initFile) && isEmpty(newRank - 1, initFile))
            possibleMoves.push_back(std::make_tuple(newRank - 1, initFile, "NULL"));
    }

    // Pawns can move one rank up/down (if that spot is empty)
    if (newRank <= RANK && newRank > 0 && isEmpty(newRank, initFile))
    {
        // If the pawn needs promoted, call function to push all possible promotions into possibleMoves
        if (newRank == RANK || newRank == 1)
            promotionMoves(newRank, initFile, possibleMoves);
        else
            possibleMoves.push_back(std::make_tuple(newRank, initFile, "NULL"));

    }

    // Pawns can move left-diagonally/right-diagonally if an opponent's piece is there
    if (newLeftFile != "NULL" && newRank <= RANK && newRank > 0 && isOpponent(p.color, newRank, newLeftFile))
    {
        // Promotion possibility
        if (newRank == RANK || newRank == 1)
            promotionMoves(newRank, newLeftFile, possibleMoves);
        else
            possibleMoves.push_back(std::make_tuple(newRank, newLeftFile, "NULL"));
    }
    if (newRightFile != "NULL" && newRank <= RANK && newRank > 0 && isOpponent(p.color, newRank, newRightFile))
    {
        // Promotion possibility
        if (newRank == RANK || newRank == 1)
            promotionMoves(newRank, newRightFile, possibleMoves);
        else
            possibleMoves.push_back(std::make_tuple(newRank, newRightFile, "NULL"));
    }

    // En Passant possibility
    if (getEnPassant() == std::make_tuple(newRank, newLeftFile))
        possibleMoves.push_back(std::make_tuple(newRank, leftFile(initFile), "EN PASSANT"));
    else if (getEnPassant() == std::make_tuple(newRank, newRightFile))
        possibleMoves.push_back(std::make_tuple(newRank, rightFile(initFile), "EN PASSANT"));

    return;
}

/* Generates all possible moves (tuple containing rank and file of desired
   location) for the rook r */
void State::rookMoves(const PieceInfo& r, const std::tuple<int, std::string>& rankFile, std::vector<std::tuple<int, std::string, std::string>>& possibleMoves) const
{
    // Initial rank and file of rook r
    int initRank = getRank(rankFile);
    std::string initFile = getFile(rankFile);

    // Vector containing the possible ranks if rook were to move in vertical direciton.
    std::vector<int> possibleRanks;

    // Vector containing the possible files if rook were to move in horizontal direction.
    std::vector<std::string> possibleFiles;

    /////////////////////////// GET VALID LOCATIONS ////////////////////////////

    // Get ranks moving in positive vertical direction
    int rankToTest = initRank + 1;
    while (rankToTest <= RANK)
    {
        if (isEmpty(rankToTest, initFile))
            possibleRanks.push_back(rankToTest);
        else if (isOpponent(r.color, rankToTest, initFile))
        {
            possibleRanks.push_back(rankToTest);
            break;
        }
        else
            break;

        rankToTest++;
    }

    // Get ranks moving in negative vertical direction
    rankToTest = initRank - 1;
    while (rankToTest > 0)
    {
        if (isEmpty(rankToTest, initFile))
            possibleRanks.push_back(rankToTest);
        else if (isOpponent(r.color, rankToTest, initFile))
        {
            possibleRanks.push_back(rankToTest);
            break;
        }
        else
            break;

        rankToTest--;
    }

    // Get files moving in positive horizontal direction
    std::string fileToTest = rightFile(initFile);
    while (fileToTest != "NULL")
    {
        if (isEmpty(initRank, fileToTest))
            possibleFiles.push_back(fileToTest);
        else if (isOpponent(r.color, initRank, fileToTest))
        {
            possibleFiles.push_back(fileToTest);
            break;
        }
        else
            break;

        fileToTest = rightFile(fileToTest);
    }

    // Get files moving in negative horizontal direction
    fileToTest = leftFile(initFile);
    while (fileToTest != "NULL")
    {
        if (isEmpty(initRank, fileToTest))
            possibleFiles.push_back(fileToTest);
        else if (isOpponent(r.color, initRank, fileToTest))
        {
            possibleFiles.push_back(fileToTest);
            break;
        }
        else
            break;

        fileToTest = leftFile(fileToTest);
    }

    ////////////////////////////////////////////////////////////////////////////

    // Couple the proper files with the proper ranks and add to possibleMoves
    for (unsigned int i = 0; i < possibleRanks.size(); i++)
        possibleMoves.push_back(std::make_tuple(possibleRanks.at(i), initFile, "NULL"));

    for (unsigned int i = 0; i < possibleFiles.size(); i++)
        possibleMoves.push_back(std::make_tuple(initRank, possibleFiles.at(i), "NULL"));

    return;
}

void State::knightMoves(const PieceInfo& k, const std::tuple<int, std::string>& rankFile, std::vector<std::tuple<int, std::string, std::string>>& possibleMoves) const
{
    // Initial rank and file of knight k
    int initRank = getRank(rankFile);
    std::string initFile = getFile(rankFile);

    // For all possible offsets for a knight, determine if the resulting space is valid.
    for (int i = 0; i < 8; i++)
        validSpace(initRank + N_Offset[i][0], convertToFile(convertFile(initFile) + N_Offset[i][1] + 1), possibleMoves);

    return;
}

void State::bishopMoves(const PieceInfo& b, const std::tuple<int, std::string>& rankFile, std::vector<std::tuple<int, std::string, std::string>>& possibleMoves) const
{
    // Initial rank and file of bishop b
    int initRank = getRank(rankFile);
    std::string initFile = getFile(rankFile);
    int rankToTest;
    std::string fileToTest;

    // Get valid rank/files in top-left diagonal
    rankToTest = initRank + 1;
    fileToTest = leftFile(initFile);
    while (rankToTest <= RANK && fileToTest != "NULL")
    {
        if (isEmpty(rankToTest, fileToTest))
            possibleMoves.push_back(std::make_tuple(rankToTest, fileToTest, "NULL"));
        else if (isOpponent(b.color, rankToTest, fileToTest))
        {
            possibleMoves.push_back(std::make_tuple(rankToTest, fileToTest, "NULL"));
            break;
        }
        else
            break;

        rankToTest = rankToTest + 1;
        fileToTest = leftFile(fileToTest);
    }

    // Get valid rank/files in top-right diagonal
    rankToTest = initRank + 1;
    fileToTest = rightFile(initFile);
    while (rankToTest <= RANK && fileToTest != "NULL")
    {
        if (isEmpty(rankToTest, fileToTest))
            possibleMoves.push_back(std::make_tuple(rankToTest, fileToTest, "NULL"));
        else if (isOpponent(b.color, rankToTest, fileToTest))
        {
            possibleMoves.push_back(std::make_tuple(rankToTest, fileToTest, "NULL"));
            break;
        }
        else
            break;

        rankToTest = rankToTest + 1;
        fileToTest = rightFile(fileToTest);
    }

    // Get valid rank/files in bottom-left diagonal
    rankToTest = initRank - 1;
    fileToTest = leftFile(initFile);
    while (rankToTest > 0 && fileToTest != "NULL")
    {
        if (isEmpty(rankToTest, fileToTest))
            possibleMoves.push_back(std::make_tuple(rankToTest, fileToTest, "NULL"));
        else if (isOpponent(b.color, rankToTest, fileToTest))
        {
            possibleMoves.push_back(std::make_tuple(rankToTest, fileToTest, "NULL"));
            break;
        }
        else
            break;

        rankToTest = rankToTest - 1;
        fileToTest = leftFile(fileToTest);
    }

    // Get valid rank/files in bottom-right diagonal
    rankToTest = initRank - 1;
    fileToTest = rightFile(initFile);
    while (rankToTest > 0 && fileToTest != "NULL")
    {
        if (isEmpty(rankToTest, fileToTest))
            possibleMoves.push_back(std::make_tuple(rankToTest, fileToTest, "NULL"));
        else if (isOpponent(b.color, rankToTest, fileToTest))
        {
            possibleMoves.push_back(std::make_tuple(rankToTest, fileToTest, "NULL"));
            break;
        }
        else
            break;

        rankToTest = rankToTest - 1;
        fileToTest = rightFile(fileToTest);
    }

    return;
}

void State::queenMoves(const PieceInfo& q, const std::tuple<int, std::string>& rankFile, std::vector<std::tuple<int, std::string, std::string>>& possibleMoves) const
{
    rookMoves(q, rankFile, possibleMoves);
    bishopMoves(q, rankFile, possibleMoves);

    return;
}

void State::kingMoves(const PieceInfo& k, const std::tuple<int, std::string>& rankFile, std::vector<std::tuple<int, std::string, std::string>>& possibleMoves) const
{
    // Variables to contain position of king
    int initRank = getRank(rankFile);
    std::string initFile = getFile(rankFile);
    std::string newFile;

    // For all possible offsets for the king, determine if resulting space is valid.
    for (int i = 0; i < 8; i++)
        validSpace(initRank + K_Offset[i][0], convertToFile(convertFile(initFile) + K_Offset[i][1] + 1), possibleMoves);

    // Castling logic
    if ((kingCastleStatus() || queenCastleStatus()) && !kingInCheck())
    {
        if (kingCastleStatus())
        {
            newFile = rightFile(initFile);

            while (isEmpty(initRank, newFile))
            {
                State tempState(*this, initRank, initFile, std::make_tuple(initRank, newFile, "-"));

                if (!tempState.kingInCheck())
                    newFile = rightFile(newFile);
                else
                    break;
            }

            if (k.color == 'w' && this->operator()(initRank, newFile).letter == 'R')
                possibleMoves.push_back(std::make_tuple(initRank, leftFile(newFile), "CASTLE-KING"));
            else if (k.color == 'b' && this->operator()(initRank, newFile).letter == 'r')
                possibleMoves.push_back(std::make_tuple(initRank, leftFile(newFile), "CASTLE-KING"));
        }
        if (queenCastleStatus())
        {
            newFile = leftFile(initFile);

            while (isEmpty(initRank, newFile))
            {
                State tempState(*this, initRank, initFile, std::make_tuple(initRank, newFile, "-"));

                if (!tempState.kingInCheck())
                    newFile = leftFile(newFile);
                else
                    break;
            }

            if (k.color == 'w' && this->operator()(initRank, newFile).letter == 'R')
                possibleMoves.push_back(std::make_tuple(initRank, leftFile(leftFile(initFile)), "CASTLE-QUEEN"));
            else if (k.color == 'b' && this->operator()(initRank, newFile).letter == 'r')
                possibleMoves.push_back(std::make_tuple(initRank, leftFile(leftFile(initFile)), "CASTLE-QUEEN"));
        }
    }

    return;
}

void State::validSpace(const int& newRank, const std::string& newFile, std::vector<std::tuple<int, std::string, std::string>>& possibleMoves) const
{
    if (newRank > 0 && newRank <= RANK && newFile != "NULL")
    {
        if (isEmpty(newRank, newFile) || isOpponent(getPlayerColor(), newRank, newFile))
            possibleMoves.push_back(std::make_tuple(newRank, newFile, "NULL"));
    }

    return;
}

void State::promotionMoves(const int& rank, const std::string& file, std::vector<std::tuple<int, std::string, std::string>>& possibleMoves) const
{
    possibleMoves.push_back(std::make_tuple(rank, file, "Rook"));
    possibleMoves.push_back(std::make_tuple(rank, file, "Bishop"));
    possibleMoves.push_back(std::make_tuple(rank, file, "Knight"));
    possibleMoves.push_back(std::make_tuple(rank, file, "Queen"));

    return;
}

}
}
