#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unordered_map> // i want to create a hash value for every board to not pass 2 times the same board
#include "board.hpp"
#include "gamestate.cpp"
using namespace std;

uint64_t explored_boards=0;
uint64_t skipped_boards=0;

struct explorechess{
    chessboard board;
    bool explored;
    int16_t pmove;//just to conquer 72 bytes in first elements
    int64_t score;
    int8_t deep; //sizeof() must be 81*/

    explorechess(){
        memset(this, 0, sizeof(explorechess));
    }

    explorechess(chessboard &brd, chessmoves &lstmv){

        memcpy(&(this->board), &brd, sizeof(chessboard));
        pmove=0;

        if(lstmv.posval==WP || lstmv.posval==BP){
            if((lstmv._move[0][0]==(lstmv._move[1][0]-2)) || (lstmv._move[0][0]==(lstmv._move[1][0]+2))){
                pmove=lstmv._move[0][1];
            }
        }

        memset(&this->explored, 0, sizeof(bool));
    }

    bool operator == (const explorechess &cmp) const{
        return (memcmp(&(this->board), &cmp.board, sizeof(chessboard))==0 && pmove==cmp.pmove);
    }

};

struct explorechesshash{

    size_t operator () (const explorechess &operand)const {

        uint64_t addv=0;
        uint64_t xorit=0;

        uint64_t *ptr=(uint64_t*)&operand;

        #define UINT64CASTSIZE 9 //0.375 restantes
        for(int i=0; i<UINT64CASTSIZE; i++){
            addv+=ptr[i];
            xorit^=ptr[i];
            addv+=xorit;
            xorit+=ptr[i]*addv;
        }
        return addv^xorit;
    }

};


int64_t recursive_exploration(chessboard &board, int8_t deep, chessmoves &lastmove,
unordered_map <explorechess, explorechess, explorechesshash> &already_explored){


    explorechess cp=explorechess(board, lastmove);
    explorechess &thisboard=already_explored[cp];
    if(thisboard.explored && thisboard.deep>=deep){
        //cout<<"returning prematurely\n";
        skipped_boards++;
        return thisboard.score;
    }
    thisboard=cp;

    //printf("fcall\n");

    if(deep<=0){

        explored_boards++;
        int64_t state=gamestate(board, lastmove);
        thisboard.score=state;
        thisboard.deep=deep;
        thisboard.explored=true;
        return state;
    }

    bool blackmate=board.blackcheckmate(lastmove);
    if(blackmate || board.whitecheckmate(lastmove)){
        explored_boards++;

        int64_t state=blackmate? (-2147483647) : 2147483647;;
        thisboard.score=state;
        thisboard.deep=deep;
        thisboard.explored=true;
        return state;
    }


    vector<chessmoves> possible_moves=board.get_valid_moves_for_position(lastmove);


    int * states=new int[possible_moves.size()];

    chessboard newboard(&board);


    for(int i=0; i<possible_moves.size(); i++){
        //here is the recursion
        newboard.moveswhite=!newboard.moveswhite;
        newboard.applymove(possible_moves[i]);
        newboard.moveswhite=!newboard.moveswhite;

        states[i]=recursive_exploration(newboard, deep-1, possible_moves[i], already_explored);
        newboard.reset(&board);

    }

    int retvalue=(board.moveswhite? 2147483647 : (-2147483647));

    for(int i=0; i<possible_moves.size(); i++){
        //the minimax tree
        if(board.moveswhite){
            retvalue=(states[i]<retvalue? states[i] : retvalue);//min
        }else{
            retvalue=(states[i]>retvalue? states[i] : retvalue);//max
        }
    }
    delete states;

    thisboard.explored=true;
    thisboard.deep=deep;
    thisboard.score=retvalue;

    return retvalue;
}

struct chessvariant{
    chessboard board;
    chessmoves _move;
    chessmoves lastmove;
    int64_t score;
    chessvariant(){
        return;
    }

    chessvariant(chessboard &cboard): board(&cboard){
        return;
    }

};

struct storechess{
    chessboard board;
    chessmoves last;
};

void sortchessvariant(chessvariant * variants, int _size){
    bool notsorted;
SORT:
    notsorted=0;
    for(int i=1; i<_size; i++){
        if(variants[i].score<variants[i-1].score){
            chessvariant carry=variants[i];
            variants[i]=variants[i-1];
            variants[i-1]=carry;
            notsorted=1;
        }
    }
    if(notsorted){
        goto SORT;
    }

}

#define MAXINT32_T 2147483647

chessmoves return_best_move(storechess &chessgame, int depth, int64_t &state){

    vector<chessmoves> valid_moves=chessgame.board.get_valid_moves_for_position(chessgame.last);

    chessvariant * variants=new chessvariant[valid_moves.size()];

    unordered_map<explorechess, explorechess, explorechesshash> already_explored_boards;

    printf("creando subvariantes\n");
    for(int i=0; i<valid_moves.size(); i++){
        variants[i]=chessvariant(chessgame.board);//first call the constructor
        variants[i]._move=valid_moves[i];
        variants[i].board.moveswhite=true;//chessgame.board.moveswhite;
        variants[i].board.applymove(variants[i]._move);
    }

    printf("explorando subvariantes: \n");
    for(int i=0; i<valid_moves.size(); i++){
        printf("\texplorando subvariante (%i/%i)\n", i+1, (int)valid_moves.size());
        /*cout<<"{\n";
        valid_moves[i].print();
        cout<<"}\n";*/
        //variants[i].board.printboard();
        variants[i].score=recursive_exploration(variants[i].board, depth-1, variants[i]._move, already_explored_boards);
    }

    printf("\n");

    sortchessvariant(variants, valid_moves.size());

    if(chessgame.board.moveswhite){
        goto MINIMIZE;
    }
    goto MAXIMIZE;

    int stop;

MAXIMIZE:{
    printf("filtrando los 3 mejores movimientos:\n");
    int64_t best=(-MAXINT32_T);
    chessmoves ret=variants[valid_moves.size()-1]._move;

    stop=(valid_moves.size()>=3? valid_moves.size()-3 : 0);

    for(int i=valid_moves.size()-1; i>=stop; i--){
        printf("\tchequeo de movimiento potencial %i: ", (int)(valid_moves.size()-i));
        variants[i]._move.print();
        int64_t store=recursive_exploration(variants[i].board, depth, variants[i].lastmove, already_explored_boards);
        if(store>best){
            best=store;
            ret=variants[i]._move;
        }

    }
    state=best;
    printf("\n");
    return ret;
}


MINIMIZE:{

    int64_t worst=(-MAXINT32_T);
    chessmoves ret2=variants[valid_moves.size()-1]._move;
    stop=(valid_moves.size()>=3? 3 : valid_moves.size());
    for(int i=0; i<stop; i++){
        printf("\tchequeo de movimiento potencial %i: ", i);
        variants[i]._move.print();
        int64_t store=recursive_exploration(variants[i].board, depth, variants[i].lastmove, already_explored_boards);

        if(store<worst){
            worst=store;
            ret2=variants[i]._move;
        }
    }
    state=worst;
    return ret2;
    }
}

int8_t translateformalmv(char mv){
    switch(mv){
        case 'A':
        case '1':
            return 1;
        case 'B':
        case '2':
            return 2;
        case 'C':
        case '3':
            return 3;
        case 'D':
        case '4':
            return 4;
        case 'E':
        case '5':
            return 5;
        case 'F':
        case '6':
            return 6;
        case 'G':
        case '7':
            return 7;
        case 'H':
        case '8':
            return 8;
        case 'r':
            return SHORTCASTLINGCODE;
        case 'R':
            return LONGCASTLINGCODE;
    }
    return 0;
}

bool isvalidmove(chessmoves & mv, chessboard &board, chessmoves &lastmove, bool prt){

    vector<chessmoves> valid_moves=board.get_valid_moves_for_position(lastmove);

    for(int i=0; i<valid_moves.size(); i++){

        if(mv._move[1][0]==SHORTCASTLINGCODE){
            if(valid_moves[i]._move[1][0]==SHORTCASTLINGCODE){
                return 1;
            }
        }

        if(mv._move[1][0]==LONGCASTLINGCODE){
            if(valid_moves[i]._move[1][0]==LONGCASTLINGCODE){
                return 1;
            }
        }

        if(prt){
            valid_moves[i].print();
        }
        if(valid_moves[i]==mv){
            return 1;
        }
    }
    return 0;

}

chessmoves readmv(string &usermove){
    chessmoves ret;
    int get=translateformalmv(usermove.c_str()[0]);
    if(get==LONGCASTLINGCODE || get==SHORTCASTLINGCODE){
        ret._move[1][0]=get;
        return ret;
    }

    ret._move[0][1]=get-1;
    ret._move[0][0]=8-translateformalmv(usermove.c_str()[1]);
    ret._move[1][1]=translateformalmv(usermove.c_str()[2])-1;
    ret._move[1][0]=8-translateformalmv(usermove.c_str()[3]);

    ret.posval=translateformalmv(usermove.c_str()[4]);

    return ret;

}

#ifndef _main
#define _main

int main(void){


    storechess game;
    //goto BUG;

    char cls;

    int64_t state;
    for(;;){
        static int deep=4;
        game.board.printboard();
        if(game.board.whitecheckmate(game.last)){
        BLACKWINS:
            cout<<"TE GANE!\n";
            goto END;
        }
        if(game.board.tie()){
        WHITEWINS:
            cout<<"TABLAS\n";
            goto END;
        }
        cout<<"ingresa la jugada(r minuscula enroque corto, R enroque largo): ";
        game.board.moveswhite=true;

        string yourmove;

    TRY_AGAIN:
        cin>>yourmove;

        if(yourmove=="EXIT"){
            goto END;
        }

        chessmoves mv=readmv(yourmove);
        mv.posval=(-mv.posval);
        if(!isvalidmove(mv, game.board, game.last, 0)){
            cout<<"ingresa un movimiento valido!\nlista de movimientos:\n";
            isvalidmove(mv, game.board, game.last, 1);
            cout<<"\n";
            goto TRY_AGAIN;
        }
        game.last=mv;
        cout<<"your move: ";
        game.last.print();

        //game.last.posval=(-game.last.posval);

        game.board.applymove(game.last);
        game.board.moveswhite=false;
        game.board.printboard();

        if(game.board.blackcheckmate(game.last)){
            cout<<"ME GANASTE!\n";
            goto END;
        }
        if(game.board.tie()){
            cout<<"TABLAS\n";
            goto END;
        }

        cout<<"ahora juego yo\n";

        time_t cmp1=time(0);
        game.last=return_best_move(game, deep, state);

        switch(state){
            case MAXINT32_T:
                goto BLACKWINS;
            case -MAXINT32_T:
                goto WHITEWINS;
            default:
                break;
        }

        time_t cmp2=time(0);
        if((cmp2-cmp1)>300){
            deep--;
            cout<<"DISMINUYO DIFICULTAD A "<<deep<<"\n";
        }
        else if((cmp2-cmp1)<30){
            deep++;
            cout<<"AUMENTO DIFUCULTAD A "<<deep<<"\n";
        }

        cout<<"esta es mi jugada: ";
        game.last.print();

        game.board.applymove(game.last);

        //game.board.printboard();

    }

/*
BUG:{
    int8_t bugresponse[8][8]={
        {BR, BH, BB, BQ, BK, BB, BH, BR},
        {BP, BP, BP, BP, NO, BP, BP, BP},
        {NO, NO, NO, NO, BP, NO, NO, NO},
        {NO, NO, NO, NO, NO, NO, NO, NO},
        {NO, NO, NO, NO, WP, NO, NO, NO},
        {NO, NO, NO, NO, NO, WH, NO, NO},
        {WP, WP, WP, WP, NO, WP, WP, WP},
        {WR, WH, WB, WQ, WK, WB, NO, WR}
    };
    bool moveswhite=0;

    memcpy(game.board.board, bugresponse, sizeof(bugresponse));
    game.board.moveswhite=moveswhite;

    string usermove="G1F33";
    chessmoves mv=readmv(usermove);
    mv.posval=-mv.posval;
    game.last=mv;

    return_best_move(game, 4);
    }
*/
END:
    cout<<"La cantidad de tableros recorridos fue de: "<<explored_boards<<"\n";
    cout<<"Gracias a std::unordered_map, nos ahorramos "<<skipped_boards<<"\n";
    return 0;
}
#endif
