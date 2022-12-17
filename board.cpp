#include "board.hpp"
#
using namespace std;

void chessboard::reset(chessboard* cp){
    memcpy(this, cp, sizeof(chessboard));
    this->moveswhite=!cp->moveswhite;
}

chessboard::chessboard(chessboard* cp){
    reset(cp);
}

chessboard::chessboard(){

    memset(this, 0, sizeof(chessboard));
    int8_t defaultval[8][8]={
        {BR, BH, BB, BQ, BK, BB, BH, BR},
        {BP, BP, BP, BP, BP, BP, BP, BP},
        {NO, NO, NO, NO, NO, NO, NO, NO},
        {NO, NO, NO, NO, NO, NO, NO, NO},
        {NO, NO, NO, NO, NO, NO, NO, NO},
        {NO, NO, NO, NO, NO, NO, NO, NO},
        {WP, WP, WP, WP, WP, WP, WP, WP},
        {WR, WH, WB, WQ, WK, WB, WH, WR}
    };
    memcpy(this->board, defaultval, sizeof(chessboard));
    this->moveswhite=1;
    this->whitecastling.longcastling=1;
    this->whitecastling.shortcastling=1;
    this->blackcastling.shortcastling=1;
    this->blackcastling.longcastling=1;
}

void chessboard::printboard(){
    string pieces[]={"WK", "WQ", "WR", "WH", "WB", "WP", "--", "BP", "BB", "BH", "BR", "BQ", "BK"};

    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            cout<<pieces[this->board[i][j]+6]<<" ";
        }
        cout<<"\n";
    }
    cout<<"\n";

}

chessboard::moves::moves(){
    memset(this, 0, sizeof(chessboard::moves));
}

void chessboard::moves::print(){
    char columns[]="ABCDEFGH";
    if(this->_move[1][0]==LONGCASTLINGCODE || this->_move[1][0]==SHORTCASTLINGCODE){
        printf(this->_move[1][0]==LONGCASTLINGCODE? "Enroque largo\n" : "Enroque corto\n");
        return;
    }
    printf("%c%i%c%i%i\n", columns[this->_move[0][1]],
     8-this->_move[0][0], columns[this->_move[1][1]], 8-this->_move[1][0], this->posval);
}

bool chessboard::moves::operator==(chessboard::moves &operand){
    return ((operand.posval==this->posval &&
    operand._move[0][0]==this->_move[0][0] &&
    operand._move[0][1]==this->_move[0][1] &&
    operand._move[1][0]==this->_move[1][0] &&
    operand._move[1][1]==this->_move[1][1])  ||
    (operand._move[1][0]==this->_move[1][0] && operand._move[1][0]==LONGCASTLINGCODE) ||
    (operand._move[1][0]==this->_move[1][0] && operand._move[1][0]==SHORTCASTLINGCODE));
}

///returns a vector of x and y coordinates reachable
std::vector<std::pair<int8_t, int8_t>> chessboard::valid_moves_for_piece(uint8_t x, uint8_t y, bool &pawn, moves &lastmove){
    //first of all select what piece it is

    pawn=false;
    vector<pair<int8_t, int8_t>> retnull;

    bool whitepiece=this->board[x][y]<0;
    bool blackpiece=this->board[x][y]>0;

    if(this->moveswhite && blackpiece){
        return retnull;
    }

    if((!this->moveswhite) && whitepiece){
        return retnull;
    }

    switch(this->board[x][y]){

        //no pieces there
        case NO:
            return retnull;

        //<blacks>
        case BR:
            return this->blackrookmv(x, y);

        case BB:
            return this->blackbishopmv(x, y);

        case BH:
            return this->blackhorsemv(x, y);

        case BP:
            pawn=true;
            return this->blackpawnmv(x, y, lastmove);

        case BQ:
            return this->blackqueenmv(x, y);

        case BK:
            return this->blackkingmv(x, y);
        //</blacks>

        //<whites>
        case WP:
            pawn=true;
            return this->whitepawnmv(x, y, lastmove);

        case WB:
            return this->whitebishopmv(x, y);

        case WH:
            return this->whitehorsemv(x, y);

        case WR:
            return this->whiterookmv(x, y);

        case WQ:
            return this->whitequeenmv(x, y);

        case WK:
            return this->whitekingmv(x, y);
        //</whites>
    }
    return retnull;
}



inline bool iswhitepiece(int8_t & piece){
    return piece<0;
}

inline bool isblackpiece(int8_t & piece){
    return piece>0;
}

//<rooks>
inline vector<pair<int8_t, int8_t>> chessboard::blackrookmv(uint8_t x, uint8_t y){

    vector<pair<int8_t, int8_t>> ret;
    for(int8_t i=x+1; i<8; i++){
        if(this->board[i][y]!=NO){
            if(iswhitepiece(this->board[i][y])){
                ret.push_back(pair<int8_t, int8_t>(i, y));
            }
            break;
        }
        ret.push_back(pair<int8_t, int8_t>(i, y));
    }
    for(int8_t i=x-1; i>=0; i--){
        if(this->board[i][y]!=NO){
            if(iswhitepiece(this->board[i][y])){
                ret.push_back(pair<int8_t, int8_t>(i, y));
            }
            break;
            }
        ret.push_back(pair<int8_t, int8_t>(i, y));
    }

    for(int8_t j=y+1; j<8; j++){
        if(this->board[x][j]!=NO){
            if(iswhitepiece(this->board[x][j])){
                ret.push_back(pair<int8_t, int8_t>(x, j));
            }
            break;
        }
        ret.push_back(pair<int8_t, int8_t>(x, j));
    }

    for(int8_t j=y-1; j>=0; j--){
        if(this->board[x][j]!=NO){
            if(iswhitepiece(this->board[x][j])){
                ret.push_back(pair<int8_t, int8_t>(x, j));
            }
            break;
        }
        ret.push_back(pair<int8_t, int8_t>(x, j));
    }

    return ret;
}

inline vector<pair<int8_t, int8_t>> chessboard::whiterookmv(uint8_t x, uint8_t y){

    vector<pair<int8_t, int8_t>> ret;
    for(int8_t i=x+1; i<8; i++){
        if(this->board[i][y]!=NO){
            if(isblackpiece(this->board[i][y])){
                ret.push_back(pair<int8_t, int8_t>(i, y));
            }
            break;
        }
        ret.push_back(pair<int8_t, int8_t>(i, y));
    }
    for(int8_t i=x-1; i>=0; i--){
        if(this->board[i][y]!=NO){
            if(isblackpiece(this->board[i][y])){
                ret.push_back(pair<int8_t, int8_t>(i, y));
            }
            break;
            }
        ret.push_back(pair<int8_t, int8_t>(i, y));
    }

    for(int8_t j=y+1; j<8; j++){
        if(this->board[x][j]!=NO){
            if(isblackpiece(this->board[x][j])){
                ret.push_back(pair<int8_t, int8_t>(x, j));
            }
            break;
        }
        ret.push_back(pair<int8_t, int8_t>(x, j));
    }

    for(int8_t j=y-1; j>=0; j--){
        if(this->board[x][j]!=NO){
            if(isblackpiece(this->board[x][j])){
                ret.push_back(pair<int8_t, int8_t>(x, j));
            }
            break;
        }
        ret.push_back(pair<int8_t, int8_t>(x, j));
    }

    return ret;
}
//</rooks>

//<bishops>
inline std::vector<std::pair<int8_t, int8_t>> chessboard::blackbishopmv(uint8_t x, uint8_t y){

    vector<pair<int8_t, int8_t>> ret;

    for(int8_t i=1; (x+i)<8 && (y+i)<8; i++){
        if(this->board[x+i][y+i]!=0){
            if(iswhitepiece(board[i+x][y+i])){
                ret.push_back(pair<int8_t, int8_t>(i+x, y+i));
            }
            break;
        }
        ret.push_back(pair<int8_t, int8_t>(i+x, y+i));
    }

    for(int8_t i=-1; (x+i)>=0 && (y+i)>=0; i--){
        if(this->board[x+i][y+i]!=0){
            if(iswhitepiece(board[i+x][y+i])){
                ret.push_back(pair<int8_t, int8_t>(i+x, y+i));
            }
            break;
        }
        ret.push_back(pair<int8_t, int8_t>(i+x, y+i));
    }

    for(int8_t i=1; (x+i)<8 && (y-i)>=0; i++){
        if(this->board[x+i][y-i]!=0){
            if(iswhitepiece(board[i+x][y-i])){
                ret.push_back(pair<int8_t, int8_t>(i+x, y-i));
            }
            break;
        }
        ret.push_back(pair<int8_t, int8_t>(i+x, y-i));
    }

    for(int8_t i=-1; (x+i)>=0 && (y-i)<8; i--){
        if(this->board[x+i][y-i]!=0){
            if(iswhitepiece(board[i+x][y-i])){
                ret.push_back(pair<int8_t, int8_t>(i+x, y-i));
            }
            break;
        }
        ret.push_back(pair<int8_t, int8_t>(i+x, y-i));
    }

    return ret;
}

inline std::vector<std::pair<int8_t, int8_t>> chessboard::whitebishopmv(uint8_t x, uint8_t y){

    vector<pair<int8_t, int8_t>> ret;

    for(int8_t i=1; (x+i)<8 && (y+i)<8; i++){
        if(this->board[x+i][y+i]!=0){
            if(isblackpiece(board[i+x][y+i])){
                ret.push_back(pair<int8_t, int8_t>(i+x, y+i));
            }
            break;
        }
        ret.push_back(pair<int8_t, int8_t>(i+x, y+i));
    }

    for(int8_t i=-1; (x+i)>=0 && (y+i)>=0; i--){
        if(this->board[x+i][y+i]!=0){
            if(isblackpiece(board[i+x][y+i])){
                ret.push_back(pair<int8_t, int8_t>(i+x, y+i));
            }
            break;
        }
        ret.push_back(pair<int8_t, int8_t>(i+x, y+i));
    }

    for(int8_t i=1; (x+i)<8 && (y-i)>=0; i++){
        if(this->board[x+i][y-i]!=0){
            if(isblackpiece(board[i+x][y-i])){
                ret.push_back(pair<int8_t, int8_t>(i+x, y-i));
            }
            break;
        }
        ret.push_back(pair<int8_t, int8_t>(i+x, y-i));
    }

    for(int8_t i=-1; (x+i)>=0 && (y-i)<8; i--){
        if(this->board[x+i][y-i]!=0){
            if(isblackpiece(board[i+x][y-i])){
                ret.push_back(pair<int8_t, int8_t>(i+x, y-i));
            }
            break;
        }
        ret.push_back(pair<int8_t, int8_t>(i+x, y-i));
    }

    return ret;
}
//</bishops>

//<queens>
inline std::vector<std::pair<int8_t, int8_t>> chessboard::blackqueenmv(uint8_t x, uint8_t y){

    //just join rook and bishop in that position
    vector<pair<int8_t, int8_t>> vec=this->blackrookmv(x, y);//rook
    vector<pair<int8_t, int8_t>> cat=this->blackbishopmv(x, y); //bishop
    vec.insert(vec.end(), cat.begin(), cat.end()); //took + bishop
    return vec;
}

inline std::vector<std::pair<int8_t, int8_t>> chessboard::whitequeenmv(uint8_t x, uint8_t y){

    vector<pair<int8_t, int8_t>> vec=this->whiterookmv(x, y);//rook
    vector<pair<int8_t, int8_t>> cat=this->whitebishopmv(x, y); //bishop
    vec.insert(vec.end(), cat.begin(), cat.end()); //took + bishop
    return vec;//quite easy this piece ;))
}
//</queens>

//<knights>
inline std::vector<std::pair<int8_t, int8_t>> chessboard::blackhorsemv(uint8_t x, uint8_t y){

    //first take all the fucking possible moves
    int8_t possible_moves[8][2]={
        {x+2, y+1},
        {x+2, y-1},
        {x-2, y+1},
        {x-2, y-1},
        {x+1, y+2},
        {x+1, y-2},
        {x-1, y+2},
        {x-1, y-2}
    };

    //now discard
    vector<pair<int8_t, int8_t>> ret;
    for(int8_t i=0; i<8; i++){
        if(
        possible_moves[i][0]<0  ||
        possible_moves[i][0]>=8 ||
        possible_moves[i][1]<0  ||
        possible_moves[i][1]>=8){
            continue;
        }
        if(isblackpiece(this->board[possible_moves[i][0]][possible_moves[i][1]])){
            continue;
        }
        ret.push_back(pair<int8_t, int8_t>(possible_moves[i][0], possible_moves[i][1]));
    }

    return ret;
}

inline std::vector<std::pair<int8_t, int8_t>> chessboard::whitehorsemv(uint8_t x, uint8_t y){

    //first take all the fucking possible moves
    int8_t possible_moves[8][2]={
        {x+2, y+1},
        {x+2, y-1},
        {x-2, y+1},
        {x-2, y-1},
        {x+1, y+2},
        {x+1, y-2},
        {x-1, y+2},
        {x-1, y-2}
    };

    //now discard
    vector<pair<int8_t, int8_t>> ret;
    for(int8_t i=0; i<8; i++){
        if(
        possible_moves[i][0]<0  ||
        possible_moves[i][0]>=8 ||
        possible_moves[i][1]<0  ||
        possible_moves[i][1]>=8){
            continue;
        }
        if(iswhitepiece(this->board[possible_moves[i][0]][possible_moves[i][1]])){
            continue;
        }
        ret.push_back(pair<int8_t, int8_t>(possible_moves[i][0], possible_moves[i][1]));
    }

    return ret;
}

//</knigthts>

inline bool rttb(int8_t &p){ //rook thread to black
    return p==WR || p==WQ;
}

inline bool bttb(int8_t &p){ //bishop thread to black
    return p==WB || p==WQ;
}

bool blackpossitionthreaded(int8_t x, int8_t y, chessboard * board){


    //<rook_like_threads>
    for(int8_t i=x+1; i<8; i++){
        if(board->board[i][y]!=NO){
            if(rttb(board->board[i][y])){
                return 1;
            }
            break;
        }
    }
    for(int8_t i=x-1; i>=0; i--){
        if(board->board[i][y]!=NO){
            if(rttb(board->board[i][y])){
                return 1;
            }
            break;
        }
    }

    for(int8_t j=y+1; j<8; j++){
        if(board->board[x][j]!=NO){
            if(rttb(board->board[x][j])){
                return 1;
            }
            break;
        }
    }

    for(int8_t j=y-1; j>=0; j--){
        if(board->board[x][j]!=NO){
            if(rttb(board->board[x][j])){
                return 1;
            }
            break;
        }
    }
    //</rook_like_threads>

    //<bishop_like_threads>
    for(int8_t i=1; (x+i)<8 && (y+i)<8; i++){
        if(board->board[x+i][y+i]!=0){
            if(bttb(board->board[i+x][y+i])){
               return 1;
            }
            break;
        }
    }

    for(int8_t i=-1; (x+i)>=0 && (y+i)>=0; i--){
        if(board->board[x+i][y+i]!=0){
            if(bttb(board->board[i+x][y+i])){
                return 1;
            }
            break;
        }
    }

    for(int8_t i=1; (x+i)<8 && (y-i)>=0; i++){
        if(board->board[x+i][y-i]!=0){
            if(bttb(board->board[i+x][y-i])){
                return 1;
            }
            break;
        }
    }

    for(int8_t i=-1; (x+i)>=0 && (y-i)<8; i--){
        if(board->board[x+i][y-i]!=0){
            if(bttb(board->board[i+x][y-i])){
                return 1;
            }
            break;
        }
    }
    //</bishop_like_threads>

    //<knight_threads>
    vector<pair<int8_t, int8_t>> horsethreads=board->blackhorsemv(x, y);

    for(int i=0; i<horsethreads.size(); i++){
        if(board->board[horsethreads[i].first][horsethreads[i].second]==WH){
            return 1;
        }
    }
    //</knight_threads>


    //<king_threads>

    int8_t kingmvs[8][2]={
        {x+1, y+1},
        {x+1, y},
        {x+1, y-1},
        {x, y-1},
        {x-1, y-1},
        {x-1, y},
        {x-1, y+1},
        {x, y+1}
    };

    for(int i=0; i<8; i++){
        //discard invalid moves
        if(kingmvs[i][0]<0 || kingmvs[i][0]>=8 ||
        kingmvs[i][1]<0 || kingmvs[i][1]>=8){
            continue;
        }

        if(board->board[kingmvs[i][0]][kingmvs[i][1]]==WK){
            return 1;
        }
    }

    //</king_threads>

    //<pawn_threads>
    int8_t pospawnthreads[2][2]={
        {x+1, y+1},
        {x+1, y-1}
    };

    for(int8_t i=0; i<2; i++){
        if(pospawnthreads[i][0]<0 || pospawnthreads[i][0]>=8 ||
        pospawnthreads[i][1]<0 || pospawnthreads[i][1]>=8){
            continue;
        }
        if(board->board[pospawnthreads[i][0]][pospawnthreads[i][1]]==WP){
            return 1;
        }
    }
    //</pawn_threads>

    return 0;
}

//white threads
inline bool rttw(int8_t &p){ //rook thread to black
    return p==BR || p==BQ;
}

inline bool bttw(int8_t &p){ //bishop thread to black
    return p==BB || p==BQ;
}

bool whitepossitionthreaded(int8_t x, int8_t y, chessboard * board){


    //<rook_like_threads>
    for(int8_t i=x+1; i<8; i++){
        if(board->board[i][y]!=NO){
            if(rttw(board->board[i][y])){
                return 1;
            }
            break;
        }
    }
    for(int8_t i=x-1; i>=0; i--){
        if(board->board[i][y]!=NO){
            if(rttw(board->board[i][y])){
                return 1;
            }
            break;
        }
    }

    for(int8_t j=y+1; j<8; j++){
        if(board->board[x][j]!=NO){
            if(rttw(board->board[x][j])){
                return 1;
            }
            break;
        }
    }

    for(int8_t j=y-1; j>=0; j--){
        if(board->board[x][j]!=NO){
            if(rttw(board->board[x][j])){
                return 1;
            }
            break;
        }
    }
    //</rook_like_threads>

    //<bishop_like_threads>
    for(int8_t i=1; (x+i)<8 && (y+i)<8; i++){
        if(board->board[x+i][y+i]!=0){
            if(bttw(board->board[i+x][y+i])){
               return 1;
            }
            break;
        }
    }

    for(int8_t i=-1; (x+i)>=0 && (y+i)>=0; i--){
        if(board->board[x+i][y+i]!=0){
            if(bttw(board->board[i+x][y+i])){
                return 1;
            }
            break;
        }
    }

    for(int8_t i=1; (x+i)<8 && (y-i)>=0; i++){
        if(board->board[x+i][y-i]!=0){
            if(bttw(board->board[i+x][y-i])){
                return 1;
            }
            break;
        }
    }

    for(int8_t i=-1; (x+i)>=0 && (y-i)<8; i--){
        if(board->board[x+i][y-i]!=0){
            if(bttw(board->board[i+x][y-i])){
                return 1;
            }
            break;
        }
    }
    //</bishop_like_threads>

    //<knight_threads>
    vector<pair<int8_t, int8_t>> horsethreads=board->whitehorsemv(x, y);

    for(int i=0; i<horsethreads.size(); i++){
        if(board->board[horsethreads[i].first][horsethreads[i].second]==BH){
            return 1;
        }
    }
    //</knight_threads>


    //<king_threads>

    int8_t kingmvs[8][2]={
        {x+1, y+1},
        {x+1, y},
        {x+1, y-1},
        {x, y-1},
        {x-1, y-1},
        {x-1, y},
        {x-1, y+1},
        {x, y+1}
    };

    for(int i=0; i<8; i++){
        //discard invalid moves
        if(kingmvs[i][0]<0 || kingmvs[i][0]>=8 ||
        kingmvs[i][1]<0 || kingmvs[i][1]>=8){
            continue;
        }

        if(board->board[kingmvs[i][0]][kingmvs[i][1]]==BK){
            return 1;
        }
    }

    //</king_threads>

    //<pawn_threads>
    int8_t pospawnthreads[2][2]={
        {x-1, y+1},
        {x-1, y-1}
    };

    for(int8_t i=0; i<2; i++){
        if(pospawnthreads[i][0]<0 || pospawnthreads[i][0]>=8 ||
        pospawnthreads[i][1]<0 || pospawnthreads[i][1]>=8){
            continue;
        }
        if(board->board[pospawnthreads[i][0]][pospawnthreads[i][1]]==BP){
            return 1;
        }
    }
    //</pawn_threads>

    return 0;
}



//<kings>
inline std::vector<std::pair<int8_t, int8_t>> chessboard::blackkingmv(uint8_t x, uint8_t y){

    //equal process than the knight
    int8_t possible_moves[8][2]={
        {x+1, y+1},
        {x+1, y},
        {x+1, y-1},
        {x, y-1},
        {x-1, y-1},
        {x-1, y},
        {x-1, y+1},
        {x, y+1}};


    //now discard
    vector<pair<int8_t, int8_t>> ret;
    for(int8_t i=0; i<8; i++){
        if(
        possible_moves[i][0]<0  ||
        possible_moves[i][0]>=8 ||
        possible_moves[i][1]<0  ||
        possible_moves[i][1]>=8){
            continue;
        }
        if(isblackpiece(this->board[possible_moves[i][0]][possible_moves[i][1]])){
            continue;
        }

        if(blackpossitionthreaded(possible_moves[i][0], possible_moves[i][1], this)){
            continue;//threaded positions
        }

        ret.push_back(pair<int8_t, int8_t>(possible_moves[i][0], possible_moves[i][1]));
    }

    //now we still have the option to make castling (long/short)
    if(blackpossitionthreaded(0, 4, this)){
        goto SKIP2;
    }
    //<shortcastling>
    if(this->blackcastling.shortcastling){
        //castle
        bool threaded=0;
        for(int i=5; i<7; i++){
            if(this->board[0][i]!=0){
                goto SKIP1;
            }
            if(threaded=blackpossitionthreaded(0, i, this)){
                break;
            }
        }
        if(threaded){
            goto SKIP1;
        }
        ret.push_back(pair<int8_t, int8_t>(SHORTCASTLINGCODE, 0));
    }
    //</shortcastling>

SKIP1:
    //<longcastling>
    if(this->blackcastling.shortcastling){
        //castle
        bool threaded=0;
        for(int i=3; i>=2; i--){
            if(this->board[0][i]!=0){
                goto SKIP2;
            }
            if(threaded=blackpossitionthreaded(0, i, this)){
                break;
            }
        }
        if(threaded){
            goto SKIP2;
        }
        ret.push_back(pair<int8_t, int8_t>(SHORTCASTLINGCODE, 0));
    }
    //</longcastling>

SKIP2:
    //after moving the king i have to check if its not on CHECK || CHECKMATE (no possible moves for the king)
    return ret;
}

inline std::vector<std::pair<int8_t, int8_t>> chessboard::whitekingmv(uint8_t x, uint8_t y){

    //equal process than the knight
    int8_t possible_moves[8][2]={
        {x+1, y+1},
        {x+1, y},
        {x+1, y-1},
        {x, y-1},
        {x-1, y-1},
        {x-1, y},
        {x-1, y+1},
        {x, y+1}};

    //now discard
    vector<pair<int8_t, int8_t>> ret;
    for(int8_t i=0; i<8; i++){
        if(
        possible_moves[i][0]<0  ||
        possible_moves[i][0]>=8 ||
        possible_moves[i][1]<0  ||
        possible_moves[i][1]>=8){
            continue;
        }
        if(iswhitepiece(this->board[possible_moves[i][0]][possible_moves[i][1]])){
            continue;
        }

        if(whitepossitionthreaded(possible_moves[i][0], possible_moves[i][1], this)){
            continue;//threaded positions
        }

        ret.push_back(pair<int8_t, int8_t>(possible_moves[i][0], possible_moves[i][1]));
    }

        //now we still have the option to make castling (long/short)
    if(whitepossitionthreaded(7, 4, this)){
        goto SKIP2;
    }

    if(this->whitecastling.shortcastling){
        //castle
        bool threaded=0;
        for(int i=5; i<7; i++){
            if(this->board[7][i]!=0){

                goto SKIP1;
            }
            if(threaded=whitepossitionthreaded(7, i, this)){
                //cout<<"castling not possible\n";
                break;
            }
        }
        if(threaded){
            goto SKIP1;
        }
        ret.push_back(pair<int8_t, int8_t>(SHORTCASTLINGCODE, 7));
    }

SKIP1:
    if(this->whitecastling.shortcastling){
        //castle
        bool threaded=0;
        for(int i=3; i>=1; i--){
            if(this->board[0][i]!=0){
                goto SKIP2;
            }
            if(i>=2){
                if(threaded=whitepossitionthreaded(0, i, this)){
                    break;
                }
            }
        }
        if(threaded){
            goto SKIP2;
        }
        ret.push_back(pair<int8_t, int8_t>(SHORTCASTLINGCODE, 0));
    }

SKIP2:
    //after moving the king i have to check if its not on CHECK || CHECKMATE (no possible moves for the king)
    return ret;
}

//</kings>

//<pawns>
inline bool blackpawnfirstmv(int8_t x, int8_t y){
    return x==1;
}


inline bool whitepawnfirstmv(int8_t x, int8_t y){
    return x==6;
}

///returns 1 if is a valid move for black
inline bool blackenpassant(int8_t x, int8_t y, chessboard::moves &lastmv, bool leftmv){
    if(x!=4){
        return 0;
    }

    //two possibilites:

    int8_t possiblelastmv[2][2][2]={
        {
            {6, y+1},
            {4, y+1}
        },
        {
            {6, y-1},
            {4, y-1}
        }
    };

    int8_t i=(int8_t)(leftmv);
    if(possiblelastmv[i][0][0]==lastmv._move[0][0] &&
    possiblelastmv[i][0][1]==lastmv._move[0][1] &&
    possiblelastmv[i][1][0]==lastmv._move[1][0] &&
    possiblelastmv[i][1][1]==lastmv._move[1][1] &&
    lastmv.posval==WP){
        return 1;
    }

    return 0;
}

///returns 1 if its a valid move for whites
inline bool whiteenpassant(int8_t x, int8_t y, chessboard::moves &lastmv, bool leftmv){
    if(x!=3){
        return 0;
    }

    //two possibilites:

    int8_t possiblelastmv[2][2][2]={
        {
            {1, y+1},
            {3, y+1}
        },
        {
            {1, y-1},
            {3, y-1}
        }
    };

    int8_t i=(int8_t)(leftmv);
    if(possiblelastmv[i][0][0]==lastmv._move[0][0] &&
    possiblelastmv[i][0][1]==lastmv._move[0][1] &&
    possiblelastmv[i][1][0]==lastmv._move[1][0] &&
    possiblelastmv[i][1][1]==lastmv._move[1][1] &&
    lastmv.posval==BP){
        return 1;
    }

    return 0;
}


inline vector<pair<int8_t, int8_t>> chessboard::blackpawnmv(uint8_t x, uint8_t y, moves &lastmv){
    //first, as before, take all the possible moves (4)
    /* just for guide. turns out to be innessesary
    int8_t possmoves[4][2]={
        {x+2, y},  //only in the first mv
        {x+1, y},  //generally, an all the moves
        {x+1, y+1},//only if its a black piece
        {x+1, y-1} //the same as above
    };
    */
    vector<pair<int8_t, int8_t>> ret;


    //you can make 2 moves
    if(blackpawnfirstmv(x, y)){
        if(this->board[x+2][y]==NO && this->board[x+1][y]==NO){
            ret.push_back(pair<int8_t, int8_t>(x+2, y));
            ret.push_back(pair<int8_t, int8_t>(x+1, y));
            goto SKIP;
        }
    }

    if(this->board[x+1][y]==NO){
        ret.push_back(pair<int8_t, int8_t>(x+1, y));
    }

SKIP:
    if(y>0 &&
    (iswhitepiece(this->board[x+1][y-1]) ||
    blackenpassant(x, y, lastmv, true))){
        ret.push_back(pair<int8_t, int8_t>(x+1, y-1));
    }

    if(y<7 &&
    (iswhitepiece(this->board[x+1][y+1]) ||
    blackenpassant(x, y, lastmv, false))){
        ret.push_back(pair<int8_t, int8_t>(x+1, y+1));
    }

    return ret;
}

inline vector<pair<int8_t, int8_t>> chessboard::whitepawnmv(uint8_t x, uint8_t y, moves &lastmv){
    //first, as before, take all the possible moves (4)
    /* just for guide. turns out to be innessesary
    int8_t possmoves[4][2]={
        {x-2, y},  //only in the first mv
        {x-1, y},  //generally, an all the moves
        {x-1, y+1},//only if its a black piece
        {x-1, y-1} //the same as above
    };
    */
    vector<pair<int8_t, int8_t>> ret;


    //you can make 2 moves
    if(whitepawnfirstmv(x, y)){
        if(this->board[x-2][y]==NO && this->board[x-1][y]==NO){
            ret.push_back(pair<int8_t, int8_t>(x-2, y));
            ret.push_back(pair<int8_t, int8_t>(x-1, y));
            goto SKIP;
        }
    }

    if(this->board[x-1][y]==NO){
        ret.push_back(pair<int8_t, int8_t>(x-1, y));
    }

SKIP:
    if(y>0 &&
    (isblackpiece(this->board[x-1][y-1]) ||
    whiteenpassant(x, y, lastmv, true))){
        ret.push_back(pair<int8_t, int8_t>(x-1, y-1));
    }

    if(y<7 &&
    (isblackpiece(this->board[x-1][y+1]) ||
    whiteenpassant(x, y, lastmv, false))){
        ret.push_back(pair<int8_t, int8_t>(x-1, y+1));
    }

    return ret;
}

//</pawns>


//<valid_moves>

inline vector<chessboard::moves> addtoretrn(int8_t x, int8_t y, chessboard * board, chessboard::moves lastmv){
    typedef chessboard::moves moves;
    vector<moves> ret;
    bool pawn;
    vector<pair<int8_t, int8_t>> vec=board->valid_moves_for_piece(x, y, pawn, lastmv);

    //<coronation>
    if(pawn){
        if(board->moveswhite && x==1){
            //coronate white pawn
            for(int i=0; i<vec.size(); i++){
                moves add;
                add._move[0][0]=x;
                add._move[0][1]=y;
                add._move[1][0]=vec[i].first;
                add._move[1][1]=vec[i].second;
                add.posval=WQ;//white queen
                ret.push_back(add);
                add.posval=WH;//white knight
                ret.push_back(add);
            }
            return ret;
        }
        else if((!board->moveswhite) && x==6){
            //coronate black pawn
            for(int i=0; i<vec.size(); i++){
                moves add;
                add._move[0][0]=x;
                add._move[0][1]=y;
                add._move[1][0]=vec[i].first;
                add._move[1][1]=vec[i].second;
                add.posval=BQ;//black queen
                ret.push_back(add);
                add.posval=BH;//black horse
                ret.push_back(add);
            }
            return ret;
        }
    }
    //</coronation>

    //"normal move"
    moves add;
    add.posval=board->board[x][y];
    add._move[0][0]=x;
    add._move[0][1]=y;
    for(int i=0; i<vec.size(); i++){
        add._move[1][0]=vec[i].first;
        add._move[1][1]=vec[i].second;
        ret.push_back(add);
    }

    return ret;
}

inline bool notpotentialthread(int kingx, int kingy, int x, int y){
    if(x==kingx || y==kingy){
        return 0;
    }

    if((kingx-x)==(kingy-y) || (kingx-x)==(-(kingy-y))){
        return 0;
    }

    return 1;
}

int* getwhitexy(chessboard* board){
    int x=0, y=0;
    for(x=0; x<8; x++){
        for(y=0; y<8; y++){
            if(board->board[x][y]==WK){
                int *ret=new int[2];
                ret[0]=x;
                ret[1]=y;
                return ret;
            }
        }
    }
    return NULL;
}

int* getblackxy(chessboard* board){
    int x=0, y=0;
    for(x=0; x<8; x++){
        for(y=0; y<8; y++){
            if(board->board[x][y]==BK){
                int *ret= new int[2];
                ret[0]=x;
                ret[1]=y;
                return ret;
            }
        }
    }
    return NULL;
}

vector<chessboard::moves> chessboard::get_valid_moves_for_position(moves lastmove){

    vector<moves> rval;

    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            vector<moves> add=addtoretrn(i, j, this, lastmove);
            rval.insert(rval.end(), add.begin(), add.end());
        }
    }

    /*if((this->moveswhite && this->whitecheck()) ||
    ((!this->moveswhite) && this->blackcheck())){
        goto CHECK;
    }

    return rval;

//CHECK:*/

    //printf("BLACK CHECK: \n");
    vector<moves> retvec;
    chessboard newboard(this);

    if(this->moveswhite){
        int* xy=getwhitexy(this);
        bool check=this->whitecheck();
        for(int i=0; i<rval.size(); i++){
            if((!check) &&notpotentialthread(xy[0], xy[1], rval[i]._move[0][0], rval[i]._move[0][1])){
                retvec.push_back(rval[i]);
                continue;
            }
            newboard.moveswhite=this->moveswhite;
            newboard.applymove(rval[i]);
            if(!newboard.whitecheck()){
                retvec.push_back(rval[i]);
            }
            newboard.reset(this);
        }
        delete xy;
    }else{
        int* xy=getblackxy(this);
        bool check=this->blackcheck();
        for(int i=0; i<rval.size(); i++){
            if((!check) && notpotentialthread(xy[0], xy[1], rval[i]._move[0][0], rval[i]._move[0][1])){
                retvec.push_back(rval[i]);
                continue;
            }
            newboard.moveswhite=this->moveswhite;
            newboard.applymove(rval[i]);
            if(!newboard.blackcheck()){
                retvec.push_back(rval[i]);
            }
            newboard.reset(this);
        }
        delete xy;
    }
    return retvec;

}

//</valid_moves>

bool chessboard::blackcheck(){
    int x=0, y=0;
    for(x=0; x<8; x++){
        for(y=0; y<8; y++){
            if(this->board[x][y]==BK){
                goto CMP;
            }
        }
    }

CMP:
    return blackpossitionthreaded(x, y, this);
}


bool chessboard::whitecheck(){

    int x=0, y=0;
    for(x=0; x<8; x++){
        for(y=0; y<8; y++){
            if(this->board[x][y]==WK){
                goto CMP;
            }
        }
    }

CMP:
    return whitepossitionthreaded(x, y, this);
}

bool chessboard::whitecheckmate(moves &lastmove){
    if(!whitecheck()){
        return 0;
    }

    vector<moves> possible_moves=get_valid_moves_for_position(lastmove);

    //check all possible moves in a new chessboard
    chessboard newboard(this);
    for(int i=0; i<possible_moves.size(); i++){
        newboard.applymove(possible_moves[i]);
        if(!newboard.whitecheck()){
            return 0;
        }
        newboard.reset(this);
    }
    return 1; //none of the moves can make the king safe
}

bool chessboard::blackcheckmate(moves &lastmove){
    if(!blackcheck()){
        return 0;
    }

    //printf("black king is on check\n");
    vector<moves> possible_moves=get_valid_moves_for_position(lastmove);

    //check all possible moves in a new chessboard
    chessboard newboard(this);
    for(int i=0; i<possible_moves.size(); i++){
        newboard.applymove(possible_moves[i]);
        if(!newboard.blackcheck()){
            //printf("the winning move is: ");
            //possible_moves[i].print();
            return 0;
        }
        newboard.reset(this);
    }
    return 1; //none of the moves can make the king safe
}

inline int castling(chessboard::moves mv){
    return mv._move[1][0]>=64? mv._move[1][0] : 0;
}

void chessboard::applymove(moves& _move){

    //<castlings>
    int castlingtype;
    if((castlingtype=castling(_move))!=0){
        //make castling

        //<long_castling>
        if(castlingtype==LONGCASTLINGCODE){
            if(this->moveswhite){
                //white long castling
                this->board[7][4]=0;
                this->board[7][0]=0;
                this->board[7][2]=WK;
                this->board[7][3]=WR;
                this->whitecastling.longcastling=false;
                this->whitecastling.shortcastling=false;
                return;
            }else{
                //black long castling
                this->board[0][4]=0;
                this->board[0][0]=0;
                this->board[0][2]=BK;
                this->board[0][3]=BR;
                this->blackcastling.longcastling=false;
                this->blackcastling.shortcastling=false;
                return;
            }
        }//</long_castling>

        //<short_castling>
        if(this->moveswhite){
            //white short castling
            this->board[7][4]=0;
            this->board[7][7]=0;
            this->board[7][6]=WK;
            this->board[7][5]=WR;
            this->whitecastling.longcastling=false;
            this->whitecastling.shortcastling=false;
            return;
        }else{
            //black short castling
            this->board[0][4]=0;
            this->board[0][7]=0;
            this->board[0][6]=BK;
            this->board[0][5]=BR;
            this->blackcastling.longcastling=false;
            this->blackcastling.shortcastling=false;
            return;
        }
        //</short_castling>

    }//</castlings>

    //delete castlings posibility in case of rook move
    if(_move._move[0][0]==0 && _move._move[0][1]==0){
        this->blackcastling.longcastling=false;
    }

    if(_move._move[0][0]==0 && _move._move[0][1]==7){
        this->blackcastling.shortcastling=false;
    }

    if(_move._move[0][0]==0 && _move._move[0][1]==4){
        this->blackcastling.longcastling=false;
        this->blackcastling.shortcastling=false;
    }

    if(_move._move[0][0]==7 && _move._move[0][1]==0){
        this->whitecastling.longcastling=false;
    }

    if(_move._move[0][0]==7 && _move._move[0][1]==7){
        this->whitecastling.shortcastling=false;
    }

    if(_move._move[0][0]==7 && _move._move[0][1]==4){
        this->whitecastling.longcastling=false;
        this->whitecastling.shortcastling=false;
    }

    //en passant:
    if(_move.posval==WP || _move.posval==BP){//its a pawn
        if(_move._move[0][1]!=_move._move[1][1]){//capture
            if(this->board[_move._move[1][0]][_move._move[1][1]]==NO){//it must be en passant
                this->board[_move._move[0][0]][_move._move[1][1]]=0;//en passant
            }

        }
    }

    //normal move


    this->board[_move._move[0][0]][_move._move[0][1]]=0; //free the where was the piece
    this->board[_move._move[1][0]][_move._move[1][1]]=_move.posval; //set the value of the final position to the move value
    return;
}

bool chessboard::tie(){

    bool blackpieces[7]={1, 1, 1, 1, 1, 1, 1};
    bool whitepieces[7]={1, 1, 1, 1, 1, 1, 1};

    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            if(this->board[i][j]<0){
                whitepieces[-this->board[i][j]]=0;
                continue;
            }
            blackpieces[this->board[i][j]]=0;
        }
    }

    bool tieblack=1, tiewhite=1;

    for(int i=0; i<6; i++){
        tieblack&=(!blackpieces[i]);
        tiewhite&=(!whitepieces[i]);
    }

    if(tieblack && tiewhite){
        return 1;
    }

    return 0;
}

#if 0
int main(void){
    chessboard board;


    //memset(board.board, 0, sizeof(board.board));
    //board.board[7][7]=0;

    int8_t lastmv[2][2]={
        {0, 0},
        {0, 0}
    };

    int8_t bug[8][8]={
        {BR, BH, BB, NO, BK, BB, BH, BR},
        {BP, BP, NO, BP, BP, BP, BP, BP},
        {NO, WP, NO, NO, NO, NO, NO, NO},
        {NO, NO, BP, NO, NO, NO, NO, NO},
        {NO, NO, NO, NO, NO, NO, NO, NO},
        {NO, NO, NO, NO, NO, NO, NO, NO},
        {NO, WP, WP, WP, WP, WP, WP, WP},
        {WR, WH, WB, WQ, WK, WB, WH, WR}
    };

    memcpy(board.board, bug, sizeof(bug));

    //board.moveswhite=0;
    printf(board.blackcheckmate(lastmv)? "true" : "false");

    return 0;
}

#endif //just for debbuging purposes
