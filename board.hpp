#ifndef board_HPP_
#define board_HPP_

#include <iostream>
#include <cstdint>
#include <cstring>
#include <vector>

#define NO 0//nothing

#define BP 1//black pawn
#define BB 2//black bishop
#define BH 3//black horse
#define BR 4//black rook
#define BQ 5//black queen
#define BK 6//black king

#define WP -1//white pawn
#define WB -2//white bishop
#define WH -3//white horse
#define WR -4//white rook
#define WQ -5//white queen
#define WK -6//white king

#define LONGCASTLINGCODE  64 //special move
#define SHORTCASTLINGCODE 65 //special move


class chessboard{
public:
    int8_t board[8][8];
    bool moveswhite;//this value stores who is moving the pieces

    struct{
        bool longcastling;
        bool shortcastling;
    } blackcastling, whitecastling;

    chessboard(chessboard* cp);

    chessboard();

    void reset(chessboard * cp);
    /**applies move on the board.
    bool pawn indicates if its a pawn.
    if it is and you come to the final move you can coronate your pawn

    */

    struct moves{
        ///first position in 2d array represents starting position, second represents ending position
        int8_t _move[2][2];

        /**when you are coronating a pawn, it will get a different position value, else just put the same.
        If your move is to castling, you will get the castling code in this variable
        */
        int8_t posval;
        moves();

        void print();

        bool operator == (moves & operand);

    };

    void applymove(moves & _move);

    std::vector<moves> get_valid_moves_for_position(moves lastmove);

    ///black king is in checkmate
    bool blackcheckmate(moves &lastmove);

    ///white king is in checkmate
    bool whitecheckmate(moves &lastmove);

    bool tie();

    void printboard();

//private:

    ///includes all the above, and dont matter if you are black or white
    std::vector<std::pair<int8_t, int8_t>> valid_moves_for_piece(uint8_t x, uint8_t y, bool &pawn, moves &lastmv);

    //black pieces
    inline std::vector<std::pair<int8_t, int8_t>> blackrookmv(uint8_t x, uint8_t y);

    inline std::vector<std::pair<int8_t, int8_t>> blackbishopmv(uint8_t x, uint8_t y);

    inline std::vector<std::pair<int8_t, int8_t>> blackhorsemv(uint8_t x, uint8_t y);

    ///no not includes coronation
    inline std::vector<std::pair<int8_t, int8_t>> blackpawnmv(uint8_t x, uint8_t y, moves &lastmv);

    inline std::vector<std::pair<int8_t, int8_t>> blackqueenmv(uint8_t x, uint8_t y);

    ///if castling, returns the castling code in the first element
    inline std::vector<std::pair<int8_t, int8_t>> blackkingmv(uint8_t x, uint8_t y);

    //white pieces
    inline std::vector<std::pair<int8_t, int8_t>> whiterookmv(uint8_t x, uint8_t y);

    inline std::vector<std::pair<int8_t, int8_t>> whitebishopmv(uint8_t x, uint8_t y);

    inline std::vector<std::pair<int8_t, int8_t>> whitehorsemv(uint8_t x, uint8_t y);

    ///do not includes coronation
    inline std::vector<std::pair<int8_t, int8_t>> whitepawnmv(uint8_t x, uint8_t y, moves &lastmv);

    inline std::vector<std::pair<int8_t, int8_t>> whitequeenmv(uint8_t x, uint8_t y);

    ///if castling, returns the castling code in the first element
    inline std::vector<std::pair<int8_t, int8_t>> whitekingmv(uint8_t x, uint8_t y);



    ///the white king is in check
    bool whitecheck();

    ///the black king is in check
    bool blackcheck();

};

#endif
