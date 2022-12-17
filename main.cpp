#define _main
#include "recursive_exploration_optimized.cpp"

int main(void){

    storechess game;

    int64_t state=0;
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
            cout<<"\ningresa tu jugada: ";
            goto TRY_AGAIN;
        }
        game.last=mv;
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

    }


END:
    cout<<"La cantidad de tableros recorridos fue de: "<<explored_boards<<"\n";
    cout<<"Gracias a std::unordered_map, nos ahorramos "<<skipped_boards<<"\n";
    return 0;
}

