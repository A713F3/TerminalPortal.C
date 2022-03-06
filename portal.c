#include <stdio.h>
#include <windows.h>

#define WIDTH 20
#define HEIGHT 20
#define MAX_MOVE 10

#define WALL_ICON     '#'
#define PLAYER_ICON   'P'
#define PORTAL_1_ICON 'B'
#define PORTAL_2_ICON 'R'
#define EMPTY_ICON    ' '


struct portal{
    int x, y;
    int exists;
    //0:DOWN 1:LEFT 2:UP 3:RIGHT
    int spawn_dir; 
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
            (*map).map[i][j] = EMPTY_ICON;

            //Init walls 
            if      (i == 0 || i == HEIGHT - 1) (*map).map[i][j] = WALL_ICON;
            else if (j == 0 || j == WIDTH  - 1) (*map).map[i][j] = WALL_ICON;                          
        }
    }
}

void updateMap(MAP *map, PLAYER p, PORTAL p1, PORTAL p2){
    (*map).map[p.y][p.x] = PLAYER_ICON;

    if (p1.exists) (*map).map[p1.y][p1.x] = PORTAL_1_ICON;
    if (p2.exists) (*map).map[p2.y][p2.x] = PORTAL_2_ICON;
}

void setColor(HANDLE console, int color, int mode){ 
    //0:RESET 1:COLOR 2:INIT RESET
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

            if      (c == WALL_ICON)     setColor(console, map.wall, 1);
            else if (c == PLAYER_ICON)   setColor(console, map.player, 1);
            else if (c == PORTAL_1_ICON) setColor(console, map.p1, 1);
            else if (c == PORTAL_2_ICON) setColor(console, map.p2, 1);

            printf("%c%c", c, c);

            setColor(console, 0, 0);
        }
        printf("\n");
    }
}

void shootPortal(MAP map, PLAYER player, char direction, char portal, PORTAL * p1, PORTAL * p2){
    // 0:UP 1:RIGHT 2:DOWN 3:LEFT
    int velo_x = 0, velo_y = 0;
    int ctrl_x = player.x, ctrl_y = player.y;

    printf("%d %d  ", ctrl_x, ctrl_y);

    if      (direction == '0') velo_y = -1;
    else if (direction == '1') velo_x = 1;
    else if (direction == '2') velo_y = 1;
    else                       velo_x = -1;

    while (map.map[ctrl_y][ctrl_x] != WALL_ICON){
        ctrl_x += velo_x;
        ctrl_y += velo_y;
    }

    //0's ascii code is 48 
    if (portal == PORTAL_1_ICON){
        (*p1).exists = 1;
        (*p1).x = ctrl_x;
        (*p1).y = ctrl_y;
        (*p1).spawn_dir = (int)direction - 48;
    }
    else if (portal == PORTAL_2_ICON){
        (*p2).exists = 1;
        (*p2).x = ctrl_x;
        (*p2).y = ctrl_y;
        (*p2).spawn_dir = (int)direction - 48;
    }
}

void pass2Portal(PLAYER * player, PORTAL portal){
    (*player).x = portal.x;
    (*player).y = portal.y;

    if      (portal.spawn_dir == 0) (*player).y++;
    else if (portal.spawn_dir == 1) (*player).x--;
    else if (portal.spawn_dir == 2) (*player).y--;
    else if (portal.spawn_dir == 3) (*player).x++;
}

void playerMove(MAP map, char * move, PLAYER * player, PORTAL p1, PORTAL p2){
    int i, c_x, c_y;

    int * p_x = &((*player).x);
    int * p_y = &((*player).y); 

    char c;
    for (i = 0; move[i] != '\0'; i++){
        c = move[i];
        c_x = 0;
        c_y = 0;

        if      (c == 'w') c_y = -1;
        else if (c == 'a') c_x = -1;
        else if (c == 's') c_y = +1;
        else if (c == 'd') c_x = +1;

        if (map.map[*p_y + c_y][*p_x + c_x] != WALL_ICON){
            *p_x += c_x;
            *p_y += c_y;

            if (map.map[*p_y][*p_x] == PORTAL_1_ICON && p2.exists)
                pass2Portal(player, p2);

            else if (map.map[*p_y][*p_x] == PORTAL_2_ICON && p1.exists)
                pass2Portal(player, p1);
        }
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
    p1.exists = 0;

    PORTAL p2;
    p2.exists = 0;

    MAP map;
    map.wall   = 15 + 15*16; // WHITE WALL
    map.player =  2 +  2*16; // GREEN PLAYER
    map.p1     =  1 +  1*16; // BLUE PORTAL 1
    map.p2     =  4 +  4*16; // YELLOW PORTAL 2

    clearMap(&map);
    updateMap(&map, player, p1, p2);

    char move[MAX_MOVE], direction, portal;

    while(1){
        renderMap(console, map);

        printf("Enter movement: ");
        fgets(move, sizeof(move), stdin);

        if (cmpr(move, "stop")) break;

        if (cmpr(move, "shoot")) {
            printf("Enter direction and portal (0:3 and R or B): ");
            scanf("%c %c", &direction, &portal);

            shootPortal(map, player, direction, portal, &p1, &p2);
        }
        else {
            playerMove(map, move, &player, p1, p2);
        }

        clearMap(&map);
        updateMap(&map, player, p1, p2);
    }

    return 0;
}
