#include <stdio.h>
#include <windows.h>

#define WIDTH 20
#define HEIGHT 20
#define MAX_MOVE 10


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
    (*map).map[p.y][p.x] = 'P';

    if (p1.exists) (*map).map[p1.y][p1.x] = 'o';
    if (p2.exists) (*map).map[p2.y][p2.x] = 'O';
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

    printf("\n");
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

//TODO: Implement passing through a portal mechanics
//TODO: Implement portal shoting mechanics
void playerMove(char * move, PLAYER * player){
    int i, c_x, c_y;
    char c;
    for (i = 0; move[i] != '\0'; i++){
        c = move[i];
        c_x = 0;
        c_y = 0;

        if      (c == 'w' && (*player).y != 1)          c_y = -1;
        else if (c == 'a' && (*player).x != 1)          c_x = -1;
        else if (c == 's' && (*player).y != HEIGHT - 2) c_y = +1;
        else if (c == 'd' && (*player).x != WIDTH  - 2) c_x = +1;

        (*player).x += c_x;
        (*player).y += c_y;
    }
}

// Compare two strings
int cmpr(const char * c1, const char * c2){
    int i;
    for (i = 0; c1[i] != '\0' && c2[i] != '\0'; i++){
        if (c1[i] != c2[i]) return 0;
    }
    return 1;
}

int main(){
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    //Init setColor
    setColor(console, 0, 2);

    PLAYER player;
    player.x = 10;
    player.y = 10;

    PORTAL p1;
    p1.exists = 1;
    p1.x = 0;
    p1.y = 10;

    PORTAL p2;
    p2.exists = 1;
    p2.x = WIDTH - 1;
    p2.y = 10;

    MAP map;
    map.wall   = 15 + 15*16; // WHITE WALL
    map.player =  2 +  2*16; // GREEN PLAYER
    map.p1     =  1 +  1*16; // BLUE PORTAL 1
    map.p2     =  4 +  4*16; // YELLOW PORTAL 2

    clearMap(&map);
    updateMap(&map, player, p1, p2);

    int running = 1;
    char move[MAX_MOVE];

    while(running){
        renderMap(console, map);

        printf("Enter movement: ");
        fgets(move, sizeof(move), stdin);

        if (cmpr(move, "stop")) running = 0;

        playerMove(move, &player);

        clearMap(&map);
        updateMap(&map, player, p1, p2);
    }

    return 0;
}
