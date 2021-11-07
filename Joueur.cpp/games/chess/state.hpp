#ifndef STATE_HPP
#define STATE_HPP

// Offsets for King and Knight
const int K_Offset[8][2] = {{1,-1},{1,0},{1,1},{0,-1},{0,1},{-1,-1},{-1,0},{-1,1}};
const int N_Offset[8][2] = {{2,-1},{2,1},{1,2},{1,-2},{-1,-2},{-1,2},{-2,-1},{-2,1}};

struct PieceInfo
{
    char letter, color;
    int id;

    PieceInfo() {letter = '-'; color = '-'; id = 0;}
    PieceInfo(const char c, const char l, const int num) {letter = l; color = c; id = num;}
    friend std::ostream& operator<<(std::ostream& out, const PieceInfo& s);
    bool operator==(const PieceInfo& p) const {bool temp = (letter == p.letter && color == p.color && id == p.id) ? true : false; return temp;}
};

// Comparator for the maps.
struct Comparator
{
    bool operator() (const PieceInfo& lhs, const PieceInfo& rhs) const
    {
        return (lhs.id < rhs.id);
    }
};

class State
{
    private:
        PieceInfo board[RANK][FILE];
        std::map<PieceInfo, std::tuple<int, std::string>, Comparator> inPlay;
        std::map<PieceInfo, std::tuple<int, std::string>, Comparator> oppInPlay;
        std::vector<std::tuple<std::tuple<int, std::string>, std::tuple<int, std::string>>> prevMoves;

        bool myKingCastle = false;
        bool myQueenCastle = false;
        bool oppKingCastle = false;
        bool oppQueenCastle = false;

        int myKingRank, oppKingRank;
        std::string myKingFile, oppKingFile;

        std::tuple<int, std::string> enPassantSpace;
        char playerColor;

        // For 50 move rule
        int moveTracker;

    public:
        State();
        State(const State& state, const int& fromRank, const std::string& fromFile, const std::tuple<int, std::string, std::string>& move);
        const PieceInfo& operator()(int a, const std::string& b) const;
        const PieceInfo& operator()(int a, int b) const;
        friend std::ostream& operator<<(std::ostream& out, const State& s);

        // Accessors
        bool isEmpty (const int a, const std::string& b) const;
        bool isOpponent(const char color, const int a, const std::string& b) const;
        int getMyKingRank() const {return myKingRank;}
        std::string getMyKingFile() const {return myKingFile;}
        int getOppKingRank() const {return oppKingRank;}
        std::string getOppKingFile() const {return oppKingFile;}
        std::tuple<int, std::string> getEnPassant() const {return enPassantSpace;}
        std::map<PieceInfo, std::tuple<int, std::string>, Comparator> getInPlay() const {return inPlay;}
        std::map<PieceInfo, std::tuple<int, std::string>, Comparator> getOppInPlay() const {return oppInPlay;}
        std::tuple<int, std::string> getLocation(const PieceInfo& p) {return inPlay[p];}
        std::tuple<int, std::string> getGlobalLocation(const PieceInfo& p) {if (inPlay.find(p) != inPlay.end()) {return inPlay[p];} else {return oppInPlay[p];}}
        int getInPlaySize() const {return inPlay.size();}
        char getPlayerColor() const {return playerColor;}
        std::vector<std::tuple<std::tuple<int, std::string>, std::tuple<int, std::string>>> getPrevMoves() const {return prevMoves;}
        int getMoveTracker() const {return moveTracker;}
        bool kingCastleStatus() const {return myKingCastle;}
        bool queenCastleStatus() const {return myQueenCastle;}
        bool oppKingCastleStatus() const {return oppKingCastle;}
        bool oppQueenCastleStatus() const {return oppQueenCastle;}
        void printCaptured() const;

        // Mutators
        void setBoard(const char& c, const char& l, const int& num, const int a, const int b);
        void setMyKingRank(const int& rank) {myKingRank = rank; return;}
        void setMyKingFile(const std::string& file) {myKingFile = file; return;}
        void setOppKingRank(const int& rank) {oppKingRank = rank; return;}
        void setOppKingFile(const std::string& file) {oppKingFile = file; return;}
        void setEnPassant(const int& rank, const std::string& file);
        void setPlayerColor(const std::string& color) {if (color == "White") {playerColor = 'w';} else {playerColor = 'b';}  return;}
        void setMoveTracker(const int& halfMoveClock) {moveTracker = halfMoveClock; return;}
        void myKingCanCastle() {myKingCastle = true; return;}
        void myQueenCanCastle() {myQueenCastle = true; return;}
        void oppKingCanCastle() {oppKingCastle = true; return;}
        void oppQueenCanCastle() {oppQueenCastle = true; return;}
        void myKingCannotCastleKing() {myKingCastle = false; return;}
        void myKingCannotCastleQueen() {myQueenCastle = false; return;}
        void add(const PieceInfo p, const int a, const std::string& b);
        void remove(const int a, const std::string& b);

        // Piece moving functions
        void pawnMoves(const PieceInfo& p, const std::tuple<int, std::string>& rankFile, std::vector<std::tuple<int, std::string, std::string>>& possibleMoves) const;
        void rookMoves(const PieceInfo& r, const std::tuple<int, std::string>& rankFile, std::vector<std::tuple<int, std::string, std::string>>& possibleMoves) const;
        void knightMoves(const PieceInfo& k, const std::tuple<int, std::string>& rankFile, std::vector<std::tuple<int, std::string, std::string>>& possibleMoves) const;
        void bishopMoves(const PieceInfo& b, const std::tuple<int, std::string>& rankFile, std::vector<std::tuple<int, std::string, std::string>>& possibleMoves) const;
        void queenMoves(const PieceInfo& q, const std::tuple<int, std::string>& rankFile, std::vector<std::tuple<int, std::string, std::string>>& possibleMoves) const;
        void kingMoves(const PieceInfo& k, const std::tuple<int, std::string>& rankFile, std::vector<std::tuple<int, std::string, std::string>>& possibleMoves) const;
        void validSpace(const int& newRank, const std::string& newFile, std::vector<std::tuple<int, std::string, std::string>>& possibleMoves) const;
        void promotionMoves(const int& rank, const std::string& file, std::vector<std::tuple<int, std::string, std::string>>& possibleMoves) const;

        ////////////////////////////////////////////////////////////////////////

        // General-purpose functions to determine if something is a specific piece
        bool isRook(const PieceInfo& p) const;
        bool isQueen(const PieceInfo& p) const;
        bool isBishop(const PieceInfo& p) const;
        bool isKnight(const PieceInfo& p) const;
        bool isPawn(const PieceInfo& p) const;
        bool isKing(const PieceInfo& p) const;
        bool isPromotion(const std::string& promotion) const;

        // File conversion and file neighbor functions
        int convertFile(const std::string& file) const;
        std::string convertToFile(const int& i) const;
        std::string leftFile(const std::string& file) const;
        std::string rightFile(const std::string& file) const;

        int getRank(std::tuple<int, std::string> p) const {return std::get<0>(p);}
        std::string getFile(std::tuple<int, std::string> p) const {return std::get<1>(p);}

        ////////////////////////////////////////////////////////////////////////

        // Function to ascertain if king is in check
        bool kingInCheck() const;

        bool kingInCheckMate() const;

        // Testing functions
        void printPieces() const;
        void printOppPieces();
        void printPrevMoves();

        // Children state generation
        std::vector<std::tuple<State, PieceInfo, std::tuple<int, std::string, std::string>>> generateChildren() const;

        // Update state when opponent makes a move
        void updateState(const int fromRank, const std::string fromFile, const int toRank, const std::string toFile, const std::string promotion);

        // Switch sides
        void switchSides();

        // State evaluation heuristic function
        int stateHeuristic(const char& playerColor) const;

        // Quiescent state evaluation
        bool isQuiet();

        // Terminal state evaluation functions
        bool isDraw();
        bool isWin(const char& color);
        bool isLoss(const char& color);
};

using StateActionPair = std::vector<std::tuple<State, PieceInfo, std::tuple<int, std::string, std::string>>>;
using MyMove = std::tuple<PieceInfo, std::tuple<int, std::string, std::string>>;

#endif
