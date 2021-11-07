// GAME IV
// By: Zachary Bahr
/*
Changes from Game III:
    * Added quiescent search functionality and a quiescent state evaluation function
    * Created and integrated a history table into the algorithm
    * Tested various things and cleaned up code here and there
    * Searches at deeper depth if losing

To Do:
    * Ordering nodes by history table value still needs work
    * Improve state evaluation heuristic
    * Refine quiescent state evaluation function
    * Test with differing depths to find optimal performance
 */

// AI
// This is where you build your AI

#include "ai.hpp"

// <<-- Creer-Merge: includes -->> - Code you add between this comment and the end comment will be preserved between Creer re-runs.
// You can add #includes here for your AI.
// <<-- /Creer-Merge: includes -->>

namespace cpp_client
{

namespace chess
{
    // Global state variable that persists between turns and represents a player's internal representation of the game
    State s;

    ////////////////////// History table type definition ///////////////////////
    struct key_hash
    {
        std::size_t operator()(const MyMove& k) const
        {
            size_t hashValue;

            /*std::cout << std::get<0>(s.getGlobalLocation(std::get<0>(k))) + s.convertFile(std::get<1>(s.getGlobalLocation(std::get<0>(k)))) << std::endl;
            std::cout << std::get<0>(std::get<1>(k)) + s.convertFile(std::get<1>(std::get<1>(k))) << std::endl;*/

            // Hash is: (id of piece + 1) ^ (initial rank + initial file) ^ (final rank + final file)
            hashValue = (std::get<0>(k).id + 1) ^ (std::get<0>(s.getGlobalLocation(std::get<0>(k))) + s.convertFile(std::get<1>(s.getGlobalLocation(std::get<0>(k))))) ^ (std::get<0>(std::get<1>(k)) + s.convertFile(std::get<1>(std::get<1>(k))));

            return hashValue;
        }
    };
    struct key_equal
    {
        bool operator()(const MyMove& lhs, const MyMove& rhs) const
        {
            if (std::get<0>(lhs) == std::get<0>(rhs))
            {
                if (std::get<0>(std::get<1>(lhs)) == std::get<0>(std::get<1>(rhs)) && std::get<1>(std::get<1>(lhs)) == std::get<1>(std::get<1>(rhs)))
                    return true;
            }

            return false;
        }
    };
    ////////////////////////////////////////////////////////////////////////////

    // History Table
    std::unordered_map<MyMove, int, key_hash, key_equal> historyTable;

    // Global time variables
    float TIME_LIMIT = 15.0;
    clock_t t;

/// <summary>
/// This returns your AI's name to the game server.
/// Replace the string name.
/// </summary>
/// <returns>The name of your AI.</returns>
std::string AI::get_name() const
{
    // <<-- Creer-Merge: get-name -->> - Code you add between this comment and the end comment will be preserved between Creer re-runs.
    // REPLACE WITH YOUR TEAM NAME!
    return "Daenerys Targaryen";
    // <<-- /Creer-Merge: get-name -->>
}

/// <summary>
/// This is automatically called when the game first starts, once the game objects are created
/// </summary>
void AI::start()
{
    // <<-- Creer-Merge: start -->> - Code you add between this comment and the end comment will be preserved between Creer re-runs.

    // Initialize each board state by parsing FEN notation
    initState();

    // Initialize seed
    srand(time(NULL));

    // <<-- /Creer-Merge: start -->>
}

/// <summary>
/// This is automatically called the game (or anything in it) updates
/// </summary>
void AI::game_updated()
{
    // <<-- Creer-Merge: game-updated -->> - Code you add between this comment and the end comment will be preserved between Creer re-runs.
    // <<-- /Creer-Merge: game-updated -->>
}

/// <summary>
/// This is automatically called when the game ends.
/// </summary>
/// <param name="won">true if you won, false otherwise</param>
/// <param name="reason">An explanation for why you either won or lost</param>
void AI::ended(bool won, const std::string& reason)
{
    //<<-- Creer-Merge: ended -->> - Code you add between this comment and the end comment will be preserved between Creer re-runs.
    //<<-- /Creer-Merge: ended -->>
}

/// <summary>
/// This is called every time it is this AI.player's turn.
/// </summary>
/// <returns>Represents if you want to end your turn. True means end your turn, False means to keep your turn going and re-call this function.</returns>
bool AI::run_turn()
{
    // <<-- Creer-Merge: runTurn -->> - Code you add between this comment and the end comment will be preserved between Creer re-runs.
    // Variables to hold move and depth info
    std::string depthString = get_setting("depth_limit");
    std::string fromFile, toFile, promotion;
    int depth, fromRank, toRank;
    int qsDepth = 2;
    t = clock();

    if (!depthString.empty())
        depth = stoi(depthString);
    else
        depth = 3;

    // Update the state of the chess board after opponent's turn
    if (!game->moves.empty())
    {
        Move lastMove = game->moves.back();
        s.updateState(lastMove->from_rank, lastMove->from_file, lastMove->to_rank, lastMove->to_file, lastMove->promotion);
    }

    // Container for the best move returned by MiniMax
    std::tuple<PieceInfo, std::tuple<int, std::string, std::string>> bestMove;

    // Display the board state before making move
    std::cout << "Original State: " << std::endl << s << std::endl;

    // History Table Time-Limited Quiesence Search IDDLMM with Alpha-Beta Pruning
    historyTable.clear();
    try
    {
        // Search at deeper depth if losing.
        if (s.stateHeuristic(s.getPlayerColor()) < 0)
        {
            depth = 4;
            TIME_LIMIT = 35.0;
        }
        else
        {
            depth = 3;
            TIME_LIMIT = 15.0;
        }

        for (int i = 1; i <= depth; i++)
        {
            bestMove = AlphaBetaSearch(s, i, qsDepth);

            /*std::cout << "History Table (after " << i << " iteration(s)): " << std::endl;
            for (std::unordered_map<MyMove, int, key_hash, key_equal>::iterator it = historyTable.begin(); it != historyTable.end(); ++it)
                std::cout << std::get<0>(it->first) << " to " << std::get<1>(std::get<1>(it->first)) << std::get<0>(std::get<1>(it->first)) << " with value " << it->second << std::endl;
            std::cout << std::endl;*/
        }
    }
    catch (int i)
    {
        std::cout << "Time limit up! Using search result with depth: " << i << std::endl;
    }

    // Get the specifics of the move specified by bestMove
    fromRank = std::get<0>(s.getLocation(std::get<0>(bestMove)));
    fromFile = std::get<1>(s.getLocation(std::get<0>(bestMove)));
    toFile = std::get<1>(std::get<1>(bestMove));
    toRank = std::get<0>(std::get<1>(bestMove));
    promotion = std::get<2>(std::get<1>(bestMove));

    // Print out the best move
    std::cout << "Best move: " << std::get<0>(bestMove) << " at " << fromFile << fromRank << " to " << toFile << toRank << std::endl << std::endl;

    // Update the global state with the state that would result from the move
    s = State(s, fromRank, fromFile, std::get<1>(bestMove));

    // Print out new state
    std::cout << "New State: " << std::endl << s << std::endl;

    // Print out previous moves
    s.printPrevMoves();

    std::cout << "----------------------------------------------------------" << std::endl;

    ///////////////// Instruct the framework to make the move //////////////////
    Piece p;

    for (unsigned int i = 0; i < player->pieces.size(); i++)
    {
        if (player->pieces.at(i)->rank == fromRank && player->pieces.at(i)->file == fromFile)
        {
            p = player->pieces.at(i);
            break;
        }
    }
    p->move(toFile, toRank, promotion);

    // <<-- /Creer-Merge: runTurn -->>

    return true;
}

//<<-- Creer-Merge: methods -->> - Code you add between this comment and the end comment will be preserved between Creer re-runs.

MyMove AI::AlphaBetaSearch(const State& parent, const int& depth, const int& qsDepth)
{
    // Vector containing all child states paired with the move that results in that state
    StateActionPair childStates = parent.generateChildren();
    std::random_shuffle(childStates.begin(), childStates.end());

    // Establish initial alpha-beta values
    int alpha = INT_MIN;
    int beta = INT_MAX;

    // Tuple containing the max utility value paired with the associated move
    std::tuple<int, MyMove> currentMax;
    std::get<0>(currentMax) = INT_MIN;

    // Generate utility values for all child states and keep track of highest utility value
    for (unsigned int i = 0; i < childStates.size(); i++)
    {
        int value = MinValue(std::get<0>(childStates.at(i)), depth - 1, qsDepth, depth, alpha, beta);

        if (value >= std::get<0>(currentMax))
            currentMax = std::make_tuple(value, std::make_tuple(std::get<1>(childStates.at(i)), std::get<2>(childStates.at(i))));

        alpha = std::max(alpha, value);
    }

    // Add to history table
    if (!historyTable.count(std::get<1>(currentMax)))
        historyTable[std::get<1>(currentMax)] = 1;
    else
        historyTable[std::get<1>(currentMax)] = historyTable[std::get<1>(currentMax)] + 1;

    return std::get<1>(currentMax);
}

int AI::MinValue(State parent, const int& depth, const int& qsDepth, const int& orgDepth, int alpha, int beta)
{
    // Make sure time does not exceed the set time limit
    clock_t test = clock() - t;
    if (((float)test)/CLOCKS_PER_SEC >= TIME_LIMIT && orgDepth != 1)
        throw (orgDepth - 1);

    // Update state so Min-Player is at play
    parent.switchSides();

    // Terminal state test
    if (parent.isDraw())
        return 0;
    else if (parent.isWin(s.getPlayerColor()))
        return INT_MAX;
    else if (parent.isLoss(s.getPlayerColor()))
        return INT_MIN;

    // Depth limit reached test with non-quiescent state possibility
    if (depth == 0 && (parent.isQuiet() || qsDepth == 0))
        return parent.stateHeuristic(s.getPlayerColor());
    else
    {
        StateActionPair childStates = parent.generateChildren();
        std::random_shuffle(childStates.begin(), childStates.end());

        // Variable containing the highest utility value thus far
        int value = INT_MAX;
        int minIndex = 0;
        MyMove move;

        for (unsigned int i = 0; i < childStates.size(); i++)
        {
            int maxValue;

            // Different calls depending on whether depth limit has been reached.
            if (depth == 0)
                maxValue = MaxValue(std::get<0>(childStates.at(i)), depth, qsDepth - 1, orgDepth, alpha, beta);
            else
                maxValue = MaxValue(std::get<0>(childStates.at(i)), depth - 1, qsDepth, orgDepth, alpha, beta);

            // Get the minimum of value and maxValue. Keep track of the index.
            if (value > maxValue)
            {
                value = maxValue;
                minIndex = i;
            }

            // Pruning possibility
            if (value < alpha)
            {
                // Get the move that caused the prune
                move = std::make_tuple(std::get<1>(childStates.at(i)), std::get<2>(childStates.at(i)));

                // Add to history table
                if (!historyTable.count(move))
                    historyTable[move] = 1;
                else
                    historyTable[move] = historyTable[move] + 1;

                return value;
            }

            beta = std::min(value, beta);
        }

        // Get the move that leads to the lowest value
        if (!childStates.empty())
        {
            move = std::make_tuple(std::get<1>(childStates.at(minIndex)), std::get<2>(childStates.at(minIndex)));

            // Add to history table
            if (!historyTable.count(move))
                historyTable[move] = 1;
            else
                historyTable[move] = historyTable[move] + 1;
        }

        return value;
    }
}

int AI::MaxValue(State parent, const int& depth, const int& qsDepth, const int& orgDepth, int alpha, int beta)
{
    // Make sure time does not exceed the set time limit
    clock_t test = clock() - t;
    if (((float)test)/CLOCKS_PER_SEC >= TIME_LIMIT && orgDepth != 1)
        throw (orgDepth - 1);

    // Update state so Max-Player is at play
    parent.switchSides();

    // Terminal state test
    if (parent.isDraw())
        return 0;
    else if (parent.isWin(s.getPlayerColor()))
        return INT_MAX;
    else if (parent.isLoss(s.getPlayerColor()))
        return INT_MIN;

    // Depth limit reached test with non-quiescent state possibility
    if (depth == 0 && (parent.isQuiet() || qsDepth == 0))
        return parent.stateHeuristic(s.getPlayerColor());
    else
    {
        StateActionPair childStates = parent.generateChildren();
        std::random_shuffle(childStates.begin(), childStates.end());

        // Variable containing the highest utility value thus far
        int value = INT_MIN;
        int maxIndex = 0;
        MyMove move;

        for (unsigned int i = 0; i < childStates.size(); i++)
        {
            int minValue;

            // Different calls depending on whether depth limit has been reached.
            if (depth == 0)
                minValue = MinValue(std::get<0>(childStates.at(i)), depth, qsDepth - 1, orgDepth, alpha, beta);
            else
                minValue = MinValue(std::get<0>(childStates.at(i)), depth - 1, qsDepth, orgDepth, alpha, beta);

            // Get the maximum of value and minValue. Keep track of the index.
            if (value < minValue)
            {
                value = minValue;
                maxIndex = i;
            }

            // Pruning possibility
            if (value > beta)
            {
                // Get the move that caused the prune
                move = std::make_tuple(std::get<1>(childStates.at(i)), std::get<2>(childStates.at(i)));

                // Add to history table
                if (!historyTable.count(move))
                    historyTable[move] = 1;
                else
                    historyTable[move] = historyTable[move] + 1;

                return value;
            }

            alpha = std::max(value, alpha);
        }

        // Get the move that leads to the lowest value
        if (!childStates.empty())
        {
            move = std::make_tuple(std::get<1>(childStates.at(maxIndex)), std::get<2>(childStates.at(maxIndex)));

            // Add to history table
            if (!historyTable.count(move))
                historyTable[move] = 1;
            else
                historyTable[move] = historyTable[move] + 1;
        }

        return value;
    }
}

// Parses the FEN string and places pieces in state's 2D array
void AI::initState()
{
    // Set the color for this player
    s.setPlayerColor(player->color);

    // Variables to parse the FEN string
    int index = 0, i = 8, j = 0;

    // Info to construct a PieceInfo struct (my version of the framework's Piece)
    char l = game->fen.at(index);
    char color;
    int id = 1;

    // Determine color of piece
    if (islower(l))
        color = 'b';
    else if (isupper(l))
        color = 'w';
    else
        color = '-';

    // Parse string one character at a time
    while (l != ' ')
    {
        if (l > '0' && l < '9')
        {
            for (int k = 0; k < (l - '0'); ++k)
            {
                s.setBoard(color, '-', 0, i, j);
                j++;
            }
        }
        else if (l == '/')
        {
            j = 0;
            i--;
        }
        else
        {
            s.setBoard(color, l, id, i, j);
            id++;
            j++;
        }
        index++;
        l = game->fen.at(index);

        // Update color of new char
        if (islower(l))
            color = 'b';
        else if (isupper(l))
            color = 'w';
        else
            color = '-';
    }

    // Get to the portion of the string with castling info
    index += 3;
    l = game->fen.at(index);
    while (l != ' ')
    {
        if (l == 'K' && s.getPlayerColor() == 'w')
            s.myKingCanCastle();
        else if (l == 'K')
            s.oppKingCanCastle();
        else if (l == 'Q' && s.getPlayerColor() == 'w')
            s.myQueenCanCastle();
        else if (l == 'Q')
            s.oppQueenCanCastle();
        else if (l == 'k' && s.getPlayerColor() == 'b')
            s.myKingCanCastle();
        else if (l == 'k')
            s.oppKingCanCastle();
        else if (l == 'q' && s.getPlayerColor() == 'b')
            s.myQueenCanCastle();
        else if (l == 'q')
            s.oppQueenCanCastle();

        index++;
        l = game->fen.at(index);
    }

    // Get to the portion of the string with en passant info
    index++;
    l = game->fen.at(index);
    if (l == '-')
        s.setEnPassant(-1, "NULL");
    else
    {
        index++;
        std::string file(1, l);
        int rank = game->fen.at(index) - '0';

        std::cout << "En passant space set to " << file << rank << std::endl << std::endl;

        s.setEnPassant(rank, file);
    }

    // Get to portion of string with halfmove clock info.
    index += 2;
    std::string number;
    while (game->fen.at(index) != ' ')
    {
        number += game->fen.at(index);
        index++;
    }
    s.setMoveTracker(std::stoi(number) / 2);

    // Keep track of both King's position
    for (int i = 1 ; i <= RANK; i++)
    {
        for (int j = 0; j < FILE; j++)
        {
            if (s(i, j).letter == 'K' && s.getPlayerColor() == 'w')
            {
                s.setMyKingRank(i);
                s.setMyKingFile(s.convertToFile(j + 1));
            }
            else if (s(i, j).letter == 'K')
            {
                s.setOppKingRank(i);
                s.setOppKingFile(s.convertToFile(j + 1));
            }

            if (s(i, j).letter == 'k' && s.getPlayerColor() == 'b')
            {
                s.setMyKingRank(i);
                s.setMyKingFile(s.convertToFile(j + 1));
            }
            else if (s(i, j).letter == 'k')
            {
                s.setOppKingRank(i);
                s.setOppKingFile(s.convertToFile(j + 1));
            }
        }
    }

    return;
}

////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

//<<-- /Creer-Merge: methods -->>

} // chess

} // cpp_client
