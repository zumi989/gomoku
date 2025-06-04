//------------------------------------------------------------------------
// 五目ならべAI
//  ここを作る．
//  下記のサンプルは，直前に自分が置いた石の8近傍のどこかにランダムに石を置くだけ
//
//  引数説明
//    board[BOARD_SIZE][BORARD_SIZE]：五目並べの盤
//    com ： コンピュータが白石(STONE_WHITE)か黒石(STONE_BLACK)かを表す．
//    pos_x, pos_y：石を置く場所のx座標，y座標 両方出力用
//------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>
#include "gomoku.h"
#define DEPTH 4  // 木構造の深さ
#define Penalty 100000 // 三三の対策
#define bonus 50
#define space_bonus 100
#define bias 2
#define bias2 4
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

// 調べる座標にマーク
void mark(int x, int y, int board[][BOARD_SIZE], bool isFlag_board[][BOARD_SIZE]) {
    int directions[8][2] = {{0, 1}, {1, 0}, {1, 1}, {-1, 1}, {0, -1}, {-1, 0}, {-1, -1}, {1, -1}};
    for (int i = 0; i < 8; i++) {
        //for (int j = 1; j <= 2; j++) {  // 周囲2マス分
            int dx = x + directions[i][0] ;
            int dy = y + directions[i][1] ;
            //int dx = x + directions[i][0] * 2;
            //int dy = y + directions[i][1] * 2;

            // 範囲内かどうかを確認
            if (dx >= 0 && dx < BOARD_SIZE && dy >= 0 && dy < BOARD_SIZE && board[dy][dx] == STONE_SPACE) {
                isFlag_board[dy][dx] = true; // フラグを立てる
            }
        //}
    }
}

// 仮想の盤上に調べる座標をマッピング
void maping(int board[][BOARD_SIZE], bool isFlag_board[][BOARD_SIZE]) {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {

            //　碁石が置かれていた場合
            if(board[y][x] != STONE_SPACE){
                mark(x,y,board,isFlag_board);
            }
        }
    }
}

//三三対策
bool isPenaltyCheck(int board[][BOARD_SIZE], int player, int cx, int cy){
    int flag =0, penaltyCount = 0;
    // 横方向の評価
    int horizontalCount = 0;
    for (int i = cx; i < BOARD_SIZE; i++) {
        // 自分の碁石がある場合
        if (board[cy][i] == player) {
            ++horizontalCount;
        // スペースがある場合
        } else if (board[cy][i] == STONE_SPACE) {
            if(flag == 0){
                flag = 1;
            } 
        // 相手の碁石がある場合
        } else {
            break; 
        }
    }
    flag = 0;
    for (int i = cx - 1; i >= 0; i--) {
        // 自分の碁石がある場合
        if (board[cy][i] == player) {
            ++horizontalCount;
        // スペースがある場合
        } else if (board[cy][i] == STONE_SPACE) {
            if(flag == 0){
                flag = 1;
            }
        // 相手の碁石がある場合
        } else {
            break; 
        }
    }
    if(horizontalCount == 3){
        ++penaltyCount;
    }

    // 縦方向の評価
    int verticalCount = 0;
    flag = 0;
    for (int i = cy; i < BOARD_SIZE; i++) {
        // 自分の碁石がある場合
        if (board[i][cx] == player) {
            ++verticalCount;
        // スペースがある場合
        } else if (board[i][cx] == STONE_SPACE) {
            if(flag == 0){
                flag = 1;
            }
        // 相手の碁石がある場合
        } else {
            break; 
        }
    }
    flag = 0;
    for (int i = cy - 1; i >= 0; i--) {
        // 自分の碁石がある場合
        if (board[i][cx] == player) {
            ++verticalCount;
        // スペースがある場合
        } else if (board[i][cx] == STONE_SPACE) {
            if(flag == 0){
                flag = 1;
            }
        // 相手の碁石がある場合
        } else {
            break; 
        }
    }
    if(verticalCount == 3){
        ++penaltyCount;
    }

    // 斜め方向の評価（左下から右上）
    int diagonalCount1 = 0;
    flag = 0;
    for (int i = 0; cx + i < BOARD_SIZE && cy + i < BOARD_SIZE; i++) {
        // 自分の碁石がある場合
        if (board[cy + i][cx + i] == player) {
            ++diagonalCount1;
        // スペースがある場合
        } else if (board[cy + i][cx + i] == STONE_SPACE) {
            if(flag == 0){
                flag = 1;
            }
        // 相手の碁石がある場合
        } else {
            break; 
        }
    }
    flag = 0;
    for (int i = 1; cx - i >= 0 && cy - i >= 0; i++) {
        // 自分の碁石がある場合
        if (board[cy - i][cx - i] == player) {
            ++diagonalCount1;
        // スペースがある場合
        } else if (board[cy - i][cx - i] == STONE_SPACE) {
            if(flag == 0){
                flag = 1;
            }
        // 相手の碁石がある場合
        } else {
            break; 
        }
    }
    if(diagonalCount1 == 3){
        ++penaltyCount;
    }

    // 斜め方向の評価（左上から右下）
    int diagonalCount2 = 0;
    flag = 0;
    for (int i = 0; cx + i < BOARD_SIZE && cy - i >= 0; i++) {
        // 自分の碁石がある場合
        if (board[cy - i][cx + i] == player) {
            ++diagonalCount2;
        // スペースがある場合
        } else if (board[cy - i][cx + i] == STONE_SPACE) {
            if(flag == 0){
                flag = 1;
            }
        // 相手の碁石がある場合
        } else {
            break; 
        }
    }
    flag = 0;
    for (int i = 1; cx - i >= 0 && cy + i < BOARD_SIZE; i++) {
        // 自分の碁石がある場合
        if (board[cy + i][cx - i] == player) {
            ++diagonalCount2;
        // スペースがある場合
        } else if (board[cy + i][cx - i] == STONE_SPACE) {
            if(flag == 0){
                flag = 1;
            }
        // 相手の碁石がある場合
        } else {
            break; 
        }
    }
    if(diagonalCount2 == 3){
        ++penaltyCount;
    }

    if(penaltyCount == 2){
        return true;
    }else{
        return false;
    }
    
}

// 連の長さに応じたポイントを返す
int calculatePoint(int stoneCount) {
    if (stoneCount == 5) {
        return 10000;
    } else if (stoneCount == 4) {
        return 1000;
    } else if (stoneCount == 3) {
        return 100;
    } else {
        return 0;
    }
}

// 碁石それぞれを評価する
int evaluateStone(int board[][BOARD_SIZE], int player, int x, int y) {
    int score = 0, totalCount = 0, flag = 0;

    // 横方向の評価
    int horizontalCount = 0;
    int spaceCount = 0;
    for (int i = x; i < BOARD_SIZE; i++) {
        // 自分の碁石がある場合
        if (board[y][i] == player) {
            ++horizontalCount;
        // スペースがある場合
        } else if (board[y][i] == STONE_SPACE) {
            if(flag == 0){
                flag = 1;
                ++spaceCount;
            } 
        // 相手の碁石がある場合
        } else {
            break; 
        }
    }
    flag = 0;
    for (int i = x - 1; i >= 0; i--) {
        // 自分の碁石がある場合
        if (board[y][i] == player) {
            ++horizontalCount;
        // スペースがある場合
        } else if (board[y][i] == STONE_SPACE) {
            if(flag == 0){
                flag = 1;
                ++spaceCount;
            }
        // 相手の碁石がある場合
        } else {
            break; 
        }
    }
    if(horizontalCount >= 3 && spaceCount == 2){
        score += (2 * horizontalCount - bias2) * space_bonus;
    }else if(horizontalCount >= 3 && spaceCount == 1){
        score += (horizontalCount - bias) * space_bonus / 2;
    }
    score += calculatePoint(horizontalCount);
    totalCount += horizontalCount;

    // 縦方向の評価
    int verticalCount = 0;
    spaceCount = 0;
    flag = 0;
    for (int i = y; i < BOARD_SIZE; i++) {
        // 自分の碁石がある場合
        if (board[i][x] == player) {
            ++verticalCount;
        // スペースがある場合
        } else if (board[i][x] == STONE_SPACE) {
            if(flag == 0){
                flag = 1;
                ++spaceCount;
            }
        // 相手の碁石がある場合
        } else {
            break; 
        }
    }
    flag = 0;
    for (int i = y - 1; i >= 0; i--) {
        // 自分の碁石がある場合
        if (board[i][x] == player) {
            ++verticalCount;
        // スペースがある場合
        } else if (board[i][x] == STONE_SPACE) {
            if(flag == 0){
                flag = 1;
                ++spaceCount;
            }
        // 相手の碁石がある場合
        } else {
            break; 
        }
    }
    if(verticalCount >= 3 && spaceCount == 2){
        score += (2 * verticalCount - bias2) * space_bonus;
    }else if(verticalCount >= 3 && spaceCount == 1){
        score += (verticalCount - bias) * space_bonus / 2;
    }
    score += calculatePoint(verticalCount);
    totalCount += verticalCount;

    // 斜め方向の評価（左下から右上）
    int diagonalCount1 = 0;
    spaceCount = 0;
    flag = 0;
    for (int i = 0; x + i < BOARD_SIZE && y + i < BOARD_SIZE; i++) {
        // 自分の碁石がある場合
        if (board[y + i][x + i] == player) {
            ++diagonalCount1;
        // スペースがある場合
        } else if (board[y + i][x + i] == STONE_SPACE) {
            if(flag == 0){
                flag = 1;
                ++spaceCount;
            }
        // 相手の碁石がある場合
        } else {
            break; 
        }
    }
    flag = 0;
    for (int i = 1; x - i >= 0 && y - i >= 0; i++) {
        // 自分の碁石がある場合
        if (board[y - i][x - i] == player) {
            ++diagonalCount1;
        // スペースがある場合
        } else if (board[y - i][x - i] == STONE_SPACE) {
            if(flag == 0){
                flag = 1;
                ++spaceCount;
            }
        // 相手の碁石がある場合
        } else {
            break; 
        }
    }
    if(diagonalCount1 >= 3 && spaceCount == 2){
        score += (2 * diagonalCount1 - bias2) * space_bonus;
    }else if(diagonalCount1 >= 3 && spaceCount == 1){
        score += (diagonalCount1 - bias) * space_bonus / 2;
    }
    score += calculatePoint(diagonalCount1);
    totalCount += diagonalCount1;

    // 斜め方向の評価（左上から右下）
    int diagonalCount2 = 0;
    spaceCount = 0;
    flag = 0;
    for (int i = 0; x + i < BOARD_SIZE && y - i >= 0; i++) {
        // 自分の碁石がある場合
        if (board[y - i][x + i] == player) {
            ++diagonalCount2;
        // スペースがある場合
        } else if (board[y - i][x + i] == STONE_SPACE) {
            if(flag == 0){
                flag = 1;
                ++spaceCount;
            }
        // 相手の碁石がある場合
        } else {
            break; 
        }
    }
    flag = 0;
    for (int i = 1; x - i >= 0 && y + i < BOARD_SIZE; i++) {
        // 自分の碁石がある場合
        if (board[y + i][x - i] == player) {
            ++diagonalCount2;
        // スペースがある場合
        } else if (board[y + i][x - i] == STONE_SPACE) {
            if(flag == 0){
                flag = 1;
                ++spaceCount;
            }
        // 相手の碁石がある場合
        } else {
            break; 
        }
    }
    if(diagonalCount2 >= 3 && spaceCount == 2){
        score += (2 * diagonalCount2 - bias2) * space_bonus;
    }else if(diagonalCount2 >= 3 && spaceCount == 1){
        score += (diagonalCount2 - bias) * space_bonus / 2;
    }
    score += calculatePoint(diagonalCount2);
    totalCount += diagonalCount2;

    // ボーナス評価
    if(totalCount >= 5){
        score += (totalCount - bias2) * bonus;
    }

    return score;
} 

// 各局面を評価する
int evaluateBoard(int board[][BOARD_SIZE], int com, int enemy) {
    int score = 0;
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            // 自身の碁石を評価
            if(board[y][x] == com) {
                score += evaluateStone(board, com, x, y);
            // 相手の碁石を評価
            } else if(board[y][x] == enemy) {
                score -= evaluateStone(board, enemy, x, y);
            }
        }
    }
    return score;
}

bool isGameOver(int board[][BOARD_SIZE], int depth, int com, int enemy, int x, int y) {
    // プレイヤーの碁石が5つ連続しているかどうかをチェックする関数
    int player = 0;

    if(depth % 2 == 1){
        player = com;
    }else{
        player = enemy;
    }

    // 横方向の評価
    int horizontalCount = 0;
    for (int i = x; i < BOARD_SIZE; i++) {
        if (board[y][i] == player) {
            ++horizontalCount;
        } else {
            break; 
        }
    }
    for (int i = x - 1; i >= 0; i--) {
        if (board[y][i] == player) {
            ++horizontalCount;
        } else {
            break; 
        }
    }
    if(horizontalCount == 5){
        return true;
    }

    // 縦方向の評価
    int verticalCount = 0;
    for (int i = y; i < BOARD_SIZE; i++) {
        if (board[i][x] == player) {
            ++verticalCount;
        } else {
            break; 
        }
    }
    for (int i = y - 1; i >= 0; i--) {
        if (board[i][x] == player) {
            ++verticalCount;
        } else {
            break; 
        }
    }
    if(verticalCount == 5){
        return true;
    }

    // 斜め方向の評価（左上から右下）
    int diagonalCount1 = 0;
    for (int i = 0; x + i < BOARD_SIZE && y + i < BOARD_SIZE; i++) {
        // 自分の碁石がある場合
        if (board[y + i][x + i] == player) {
            ++diagonalCount1;
        } else {
            break; 
        }
    }
    for (int i = 1; x - i >= 0 && y - i >= 0; i++) {
        // 自分の碁石がある場合
        if (board[y - i][x - i] == player) {
            ++diagonalCount1;
        } else {
            break; 
        }
    }
    if(diagonalCount1 == 5){
        return true;
    }

    // 斜め方向の評価（左下から右上）
    int diagonalCount2 = 0;
    for (int i = 0; x + i < BOARD_SIZE && y - i >= 0; i++) {
        // 自分の碁石がある場合
        if (board[y - i][x + i] == player) {
            ++diagonalCount2;
        } else {
            break; 
        }
    }
    for (int i = 1; x - i >= 0 && y + i < BOARD_SIZE; i++) {
        // 自分の碁石がある場合
        if (board[y + i][x - i] == player) {
            ++diagonalCount2;
        } else {
            break; 
        }
    }
    if(diagonalCount2 == 5){
        return true;
    }

    return false;
}

// ミニマックス関数
int minimax(int board[][BOARD_SIZE], int depth, bool isMaximizing, int alpha, int beta, int com, int enemy, int cx, int cy) {
    if (depth == DEPTH || isGameOver(board, depth, com, enemy, cx, cy)) {
        return evaluateBoard(board, com, enemy);
    }
    if(depth == 1  && isPenaltyCheck(board, com, cx, cy)){
        return -Penalty;
    }

    bool isFlag_board[BOARD_SIZE][BOARD_SIZE] = {false};
    maping(board,isFlag_board);
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (!isFlag_board[y][x]) continue;
                board[y][x] = (isMaximizing) ? com : enemy; // put on the stone
                int evaluation = minimax(board, depth + 1, !isMaximizing, alpha, beta, com, enemy, x, y);
                board[y][x] = STONE_SPACE;  // Undo move

            if (isMaximizing) {
                alpha = max(alpha, evaluation);
                if (alpha > beta) break; // α刈り
            } else {
                beta = min(beta, evaluation);
                if (alpha > beta) break; // β刈り
            }
            
        }

        if (isMaximizing) {
            if (alpha > beta) break; // α刈り
        } else {
            if (alpha > beta) break; // β刈り
        }

    }
    return (isMaximizing) ? alpha : beta;
}

// 五目並べのAI
int calcPutPos(int board[][BOARD_SIZE], int com, int *pos_x, int *pos_y) {
    int enemy = (com == STONE_BLACK) ? STONE_WHITE : STONE_BLACK;
    int bestX = -1, bestY = -1;
    int depth=0;
    int alpha = INT_MIN;
    int beta = INT_MAX;
    bool isFlag_board[BOARD_SIZE][BOARD_SIZE] = {false};
    static int flag = 0;

    // 一手目の場合の処理
    if(flag == 0){
        flag=1;
        if(com == STONE_BLACK){
            *pos_y = BOARD_SIZE / 2;
            *pos_x = BOARD_SIZE / 2;
            return 0;
        }
    }

    maping(board,isFlag_board);
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            // 仮想の盤上をもとに調べる
            if (!isFlag_board[y][x]) continue; 
            board[y][x] = com; // put on the stone
            int score = minimax(board, depth + 1, false, alpha, beta, com, enemy, x, y);
            board[y][x] = STONE_SPACE;  // Undo move

            // α刈り
            if (score > alpha) {
                alpha = score;
                bestX = x;
                bestY = y;
            }
        }
    }
    *pos_x = bestX;
    *pos_y = bestY;

    printf("★AIくんは %d, %dに置きました\n", bestX, bestY);
    printf("\n");
    return 0;
}