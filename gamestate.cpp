#include "board.hpp"

using namespace std;

typedef chessboard::moves chessmoves;

int pieceadvantage(chessboard & board){
    int result=0;
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            result+=board.board[i][j];
        }
    }
    return result;
}

int pawnPosAdv(chessboard & board){
    int result=0;
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            switch(board.board[i][j]){
                case WP:
                    result-=(i<4? 4-i : 0);
                    break;
                case BP:
                    result+=(i>3? i-4 : 0);
                default:
                    break;
            }
        }
    }
    return result;
}

vector<chessmoves> filtermoves(vector<chessmoves> & movelist/*, chessboard * board*/){

    vector<chessmoves> addto;
    for(int i=0; i<movelist.size(); i++){

        if(
        movelist[i]._move[0][0]>=8 || movelist[i]._move[0][0]<0 ||
        movelist[i]._move[0][1]>=8 || movelist[i]._move[0][1]<0 ||
        movelist[i]._move[1][0]>=8 || movelist[i]._move[1][0]<0 ||
        movelist[i]._move[1][1]>=8 || movelist[i]._move[1][1]<0){
            continue;
        }

        #if 0
        if(
        (board->board[movelist[i]._move[0][0]][movelist[i]._move[0][1]]==WP ||
        board->board[movelist[i]._move[0][0]][movelist[i]._move[0][1]]==BP) /*&&
        movelist[i]._move[0][0]==movelist[i]._move[1][0]*/){
            continue;
        }
        #endif
        addto.push_back(movelist[i]);
    }
    return addto;
}


///also returns possible_moves
int positionaladvantage(chessboard board, chessmoves &lastmv){

    board.moveswhite=1;
    vector<chessmoves>moves=board.get_valid_moves_for_position(lastmv);

    moves=filtermoves(moves/*, &board*/);


    bool whites[8][8];

    memset(whites, 0, sizeof(whites));

    /*cout<<"white moves: \n";
    cout<<"("<<moves.size()<<")";*/
    for(int i=0; i<moves.size(); i++){
        //moves[i].print();
        whites[moves[i]._move[1][0]][moves[i]._move[1][1]]=1;
    }

    board.moveswhite=0;
    moves=board.get_valid_moves_for_position(lastmv);

    moves=filtermoves(moves/*, &board*/);

    bool blacks[8][8];
    memset(blacks, 0, sizeof(blacks));

    //cout<<"black moves: \n";
    for(int i=0; i<moves.size(); i++){
        //moves[i].print();
        blacks[moves[i]._move[1][0]][moves[i]._move[1][1]]=1;
    }

    int result=0;

    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            result+=blacks[i][j];
            result-=whites[i][j];
        }
    }

    return result;
}

int gamestate(chessboard & board, chessmoves &lastmove){

    //return 1;//just for debbuging puropses

    if(board.whitecheckmate(lastmove)){
        return 2147483647;//2**31-1
    }
    if(board.blackcheckmate(lastmove)){
        return -2147483647;//-(2**31-1)
    }

    int ret=pieceadvantage(board)*4;
    ret+=pawnPosAdv(board)*2;
    ret+=positionaladvantage(board, lastmove);

    return ret;
}
