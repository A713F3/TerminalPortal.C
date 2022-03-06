#include <stdio.h>
#include <windows.h>

#define WIDTH 20
#define HEIGHT 20


struct portal{
    int x, y;
    int exists;

    int l_velo_x, l_velo_y;
};
typedef struct portal PORTAL;

struct player{
    int x, y; 
};
typedef struct player PLAYER;

struct map{
    char map[HEIGHT][WIDTH];
    int player, wall, p1, p2;
};
typedef struct map MAP;

void clearMap(MAP * map){
    int i, j;
    for (i = 0; i < HEIGHT; i++){
        for (j = 0; j < WIDTH; j++){
            (*map).map[i][j] = ' ';

            //Init walls 
            if      (i == 0 || i == HEIGHT - 1) (*map).map[i][j] = '#';
            else if (j == 0 || j == WIDTH  - 1) (*map).map[i][j] = '#';                          
        }
    }
}

void updateMap(MAP *map, PLAYER p, PORTAL p1, PORTAL p2){
    clearMap(map);

    (*map).map[p.y][p.x] = 'P';

    if (p1.exists) (*map).map[p.y][p.x] = 'o';
    if (p2.exists) (*map).map[p.y][p.x] = 'O';
}

void setColor(HANDLE console, int color, int mode){ //0:RESET 1:COLOR 2:INIT RESET
    static CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    static WORD wOldColorAttrs;

    if (mode == 0){
        SetConsoleTextAttribute(console, wOldColorAttrs);
    }  
    else if (mode == 1){
        SetConsoleTextAttribute(console, color);
    }
    else if (mode == 2){
        GetConsoleScreenBufferInfo(console, &csbiInfo);
        wOldColorAttrs = csbiInfo.wAttributes;
    }
}


void renderMap(HANDLE console, MAP map){
    int i, j;
    char c;
    for (i = 0; i < HEIGHT; i++){
        for (j = 0; j < WIDTH; j++){
            c = map.map[i][j];

            if      (c == '#') setColor(console, map.wall, 1);
            else if (c == 'P') setColor(console, map.player, 1);
            else if (c == 'o') setColor(console, map.p1, 1);
            else if (c == 'O') setColor(console, map.p2, 1);

            printf("%c%c", c, c);

            setColor(console, 0, 0);
        }
        printf("\n");
    }
}

int main(){
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    //Init setColor
    setColor(console, 0, 2);

    PLAYER player;
    player.x = 10;
    player.y = 10;

    PORTAL p1;
    p1.exists = 0;

    PORTAL p2;
    p2.exists = 0;

    MAP map;
    map.wall   = 15 + 15*16; // WHITE WALL
    map.player =  2 +  2*16; // GREEN PLAYER
    map.p1     =  1 +  1*16; // BLUE PORTAL 1
    map.p2     = 16 + 16*16; // YELLOW PORTAL 2

    clearMap(&map);

    updateMap(&map, player, p1, p2);

    renderMap(console, map);

    return 0;
}
