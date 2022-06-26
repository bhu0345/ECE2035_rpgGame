// Project includes
#include "globals.h"
#include "hardware.h"
#include "map.h"
#include "graphics.h"
#include "speech.h"

// Functions in this file

#define START 0
#define GAME 1
#define RUN  5
int gameState = START;
int get_action (GameInputs inputs);
int update_game (int action);
void draw_game (int init);
void init_main_map ();
int main ();
void map_erase();
void init_map2();
void inventory(int f, int w, int k, int s, int h);
void game_lose(int next_state);
void move_npc(int tm);

#define NO_ACTION_LIMIT 200 // Accelerometer sensitiveity
/**
 * The main game state. Must include Player locations and previous locations for
 * drawing to work properly. Other items can be added as needed.
 */
struct {
    int x,y;    // Current locations
    int px, py; // Previous locations
    int has_key;
    int omni;
    int has_crown;
    int water;
    int fire;
    int health;
    int sh;
    int ha;
    // You can add other properties for the player here
} Player;

/**
 * Given the game inputs, determine what kind of update needs to happen.
 * Possbile return values are defined below.
 */
#define NO_ACTION 0
#define ACTION_BUTTON 1
#define MENU_BUTTON 2
#define GO_LEFT 3
#define GO_RIGHT 4
#define GO_UP 5
#define GO_DOWN 6
#define OMNIPOTENT 7
#define MODE_BUTTON 8
#define RUN_LEFT 9
#define RUN_RIGHT 10
#define RUN_UP 11
#define RUN_DOWN 12
int portal_x=7;
int portal_y=9;
int NPC_x=5;
int NPC_y=5;
int m1x=10;
int m1y=9;
int m2x=12;
int m2y=6;
    int cc=0;
    int nn=0;
    int xx=0;
    int yy=0;

int get_action(GameInputs inputs)
{
    switch(gameState) {
        case START:
            if (inputs.b1 == 0 || inputs.b2 == 0 || inputs.b3 == 0 || inputs.b4 == 0) {
                gameState=GAME;
                return gameState;
            }
            else {return NO_ACTION; }
        case GAME:

            if (inputs.b1 == 0 && inputs.b2 == 1 && inputs.b3 == 1 && inputs.b4 == 1)
            return OMNIPOTENT;
            if (inputs.b1 == 1 && inputs.b2 == 0 && inputs.b3 == 1 && inputs.b4 == 1)
            return MENU_BUTTON;
            if (inputs.b1 == 1 && inputs.b2 == 1 && inputs.b3 == 0 && inputs.b4 == 1)
            return ACTION_BUTTON;
            if (inputs.b1 == 1 && inputs.b2 == 1 && inputs.b3 == 1 && inputs.b4 == 0){
            uLCD.cls();
            uLCD.color(RED);
            uLCD.text_width(2);
            uLCD.text_height(2);
            uLCD.printf("\n\n\n   RUN  \n");
            wait(.5);
            uLCD.cls();
            draw_game(1);

            gameState=RUN;
            return NO_ACTION; 
            }

            if (inputs.ay >= 0.3)
            
            return GO_UP;
            if (inputs.ay <= -0.3)
            return GO_DOWN;
            if (inputs.ax >= 0.3)
            return GO_RIGHT;
            if (inputs.ax <= -0.3)
            return GO_LEFT;

            return NO_ACTION;
            
        
            

        case RUN:         
            if (inputs.b1 == 1 && inputs.b2 == 1 && inputs.b3 == 1 && inputs.b4 == 0){
            uLCD.cls();
            uLCD.color(GREEN);
            uLCD.text_width(2);
            uLCD.text_height(2);
            uLCD.printf("\n\n\n NORMAL  \n");
            wait(.5);
            uLCD.cls();
            draw_game(1);
            gameState=GAME;
            return NO_ACTION; 
            }
            if (inputs.ay >= 0.3)
            return RUN_UP;
            if (inputs.ay <= -0.3)
            return RUN_DOWN;
            if (inputs.ax >= 0.3)
            return RUN_RIGHT;
            if (inputs.ax <= -0.3)
            return RUN_LEFT;

        return NO_ACTION;
    }
}

/**
 * Update the game state based on the user action. For example, if the user
 * requests GO_UP, then this function should determine if that is possible by
 * consulting the map, and update the Player position accordingly.
 *
 * Return values are defined below. FULL_DRAW indicates that for this frame,
 * draw_game should not optimize drawing and should draw every tile, even if
 * the player has not moved.
 */
#define NO_RESULT 0
#define GAME_OVER 1
#define FULL_DRAW 2
MapItem* next;
MapItem* next2;

int update_game(int action)
{
    // Save player previous location before updating
    Player.px = Player.x;
    Player.py = Player.y;


    // Do different things based on the each action.
    // You can define functions like "go_up()" that get called for each case.
    switch(action)
    {
        case RUN_LEFT:
            next= get_west(Player.x, Player.y);
            next2=get_west(Player.x-1, Player.y);
            if(Player.omni || (next->walkable && next2-> walkable)){
                Player.x -= 2;
            return FULL_DRAW;}
            break;
        case RUN_UP:
            next = get_north(Player.x, Player.y);
            next2=get_north(Player.x, Player.y-1);
            if(Player.omni  ||(next->walkable && next2 ->walkable)) {
                Player.y -= 2;
                return FULL_DRAW;}
                break;
        case RUN_DOWN:
            next = get_south(Player.x, Player.y);
            next2=get_south(Player.x, Player.y+1);
            if(Player.omni || (next->walkable && next2 -> walkable)){
                Player.y += 2;
            return FULL_DRAW;}
            break;
        case RUN_RIGHT:
            next = get_east(Player.x, Player.y);
            next2=get_east(Player.x+1, Player.y);
            if(Player.omni || (next->walkable && next2-> walkable)){
                Player.x += 2;
            return FULL_DRAW;}
            break;
        case GO_LEFT:
            next= get_west(Player.x, Player.y);
            if(Player.omni || next->walkable){
                Player.x -= 1;
            return FULL_DRAW;}
            break;
        case GO_UP:
            next = get_north(Player.x, Player.y);
            if(Player.omni  || next->walkable) {
                Player.y -= 1;
                return FULL_DRAW;}
                break;
        case GO_DOWN:
            next = get_south(Player.x, Player.y);
            if(Player.omni || next->walkable){
                Player.y += 1;
            return FULL_DRAW;}
            break;
        case GO_RIGHT:
            next = get_east(Player.x, Player.y);
            if(Player.omni || next->walkable){
                Player.x += 1;
            return FULL_DRAW;}
            break;
       case ACTION_BUTTON:
                MapItem* n = get_north(Player.x, Player.y);
                MapItem* s = get_south(Player.x, Player.y);
                MapItem* e = get_east(Player.x, Player.y);
                MapItem* w = get_west(Player.x, Player.y);
            int count=20;
            int num=1;
            int ref=0;
                if (n && n->type > num) {
                    count = n->type;
                    ref=1;
                }
                if (s && s->type > num) {
                    count = s->type;
                    ref=2;
                }
                if (e && e->type > num) {
                    count = e->type;
                    ref=3;
                }
                if (w && w->type > num) {
                    count = w->type;
                    ref=4;
                }
                if (count==20 ){
                        inventory(Player.fire, Player.water, Player.has_key,Player.sh, Player.ha);
                        draw_game(1);
                    }            

                if (count){
                    if (count==2) { //NPC
                    if(ref==1){

                        add_qu(Player.x+1, Player.y);
                        draw_game(1);
                        const char* line1 = "Defeat the monster!";
                        const char* line2 = "Find the key to escape!";
                        speech(line1, line2);
                        map_erase(Player.x+1, Player.y);
                        draw_game(1);                        
                    }
                    else{
                        add_qu(Player.x, Player.y-1);
                        draw_game(1);
                        const char* line1 = "Defeat the monster!";
                        const char* line2 = "Find the key to escape!";
                        speech(line1, line2);
                        map_erase(Player.x, Player.y-1);
                        draw_game(1);
                    }
                    }
                    if (count==3){// portal
                        if (Player.has_key){
                            for (int i=0; i<500; i=i+100) {
                            speaker.period(1.0/float(i));
                            speaker=0.25;
                            wait(.1);
                            }
                            speaker=0;
                            init_map2();

                            set_active_map(1);
                         
                            draw_game(1);
                        }
                    }
                    if (count==4){ //door
                        if (Player.has_key){
                            map_erase(21,12);
         //                   s->draw = draw_opendoor;
         //                   s->walkable = true;
                            draw_game(1);
                        }
                    }
                    if (count==5){//keyy
                        if (!Player.has_key){
                        if(ref==1){
                            add_qu(Player.x+1, Player.y);
                            draw_game(1);
                            const char* line1 = "I dont have the key";
                            const char* line2 = "but I have the fire spell";
                            speech(line1, line2);
                            Player.fire=1;
                            map_erase(Player.x+1, Player.y);
                            draw_game(1);
                        }
                        else{
                            add_qu(Player.x, Player.y-1);
                            draw_game(1);
                            const char* line1 = "I dont have the key";
                            const char* line2 = "but I have the fire spell";
                            speech(line1, line2);
                            Player.fire=1;
                            map_erase(Player.x, Player.y-1);
                            draw_game(1);
                        }

                        }
                    }
                    if (count==6) { //crown
                        draw_game(1);
                        const char* line1 = "GAME OVER";
                        const char* line2 = "You win";
                        speech(line1, line2);
                        draw_game(1);
                        for (int i=0; i<500; i=i+100) {
                        speaker.period(1.0/float(i));
                        speaker=0.25;
                        wait(.1);
                        }
                        speaker=0;
                        return GAME_OVER;
                    }
                    if (count==15){ //shield
                        Player.sh=true;
                        uLCD.cls();
                    uLCD.color(GREEN);
                    uLCD.text_width(2);
                    uLCD.text_height(2);
                    uLCD.printf("\n\n\n shield  \n");
                    wait(.5);
                    uLCD.cls();
                    draw_game(1);
                    }
                    if (count==16){ //hammer
                        Player.ha=true;
                        uLCD.cls();
                    uLCD.color(GREEN);
                    uLCD.text_width(2);
                    uLCD.text_height(2);
                    uLCD.printf("\n\n\n  Hammer  \n");
                    wait(.5);
                    uLCD.cls();
                    draw_game(1);
                    }



                    if (count==9){//NPC2
                    if (!Player.has_key){
                    if(ref==1){
                        add_qu(Player.x+1, Player.y);
                        draw_game(1);
                        const char* line1 = "I dont have the key";
                        const char* line2 = "but I have the water spell";
                        speech(line1, line2);
                        Player.water=1;
                        map_erase(Player.x+1, Player.y);
                        draw_game(1);
                        }
                        else{
                        add_qu(Player.x, Player.y-1);
                        draw_game(1);
                        const char* line1 = "I dont have the key";
                        const char* line2 = "but I have the water spell";
                        speech(line1, line2);
                        Player.water=1;
                        map_erase(Player.x, Player.y-1);
                        draw_game(1);
                        }
                        }
                    }
                    if (count==8){ // monster <- fire spell
                                if(ref==1){
                                    add_ex(Player.x+1, Player.y);
                                    draw_game(1);
                                }
                                else{
                                    add_ex(Player.x, Player.y-1);
                                    draw_game(1);
                                }
                        if(!Player.has_key){
                            if(!Player.fire){
                        const char* line1 = "Wrong spell";
                        const char* line2 = "So there is no key";
                        speech(line1, line2);
                        if (Player.sh){
                        Player.health-=30;
                        }
                        else{Player.health-=50;}
                            }
                            else if (Player.fire){
                            const char* line1 = "I am defeated";
                            const char* line2 = "You have the key now";
                            speech(line1, line2);
                            if(ref==1){
                            map_erase(m1x,m1y);
                            draw_game(1);
                            m1y--;
                            add_monster(m1x, m1y);
                            draw_game(1);}
                            if(ref==2){
                            map_erase(m1x,m1y);
                            draw_game(1);
                            m1y++;
                            add_monster(m1x, m1y);
                            draw_game(1);}
                            if(ref==3){
                            map_erase(m1x,m1y);
                            draw_game(1);
                            m1x++;
                            add_monster(m1x, m1y);
                            draw_game(1);}
                            if(ref==4){
                            map_erase(m1x,m1y);
                            draw_game(1);
                            m1x--;
                            add_monster(m1x, m1y);
                            draw_game(1);}
                            Player.has_key=true;
                                if(ref==1){
                                    map_erase(Player.x+1, Player.y);
                                    draw_game(1);
                                }
                                else{
                                    map_erase(Player.x, Player.y-1);
                                    draw_game(1);
                                }
                            break;
                            }
                        }
                        else{
                            if(!Player.fire){
                        const char* line1 = "You just need one key";
                        const char* line2 = "leave";
                        speech(line1, line2);
                        if (Player.sh){
                        Player.health-=30;
                        }
                        else{Player.health-=50;}
                            }
                            else if (Player.fire){
                            const char* line1 = "I am defeated";
                            const char* line2 = "now leave";
                            speech(line1, line2);
                            Player.health+=15;
                            }

                        }
                            if(ref==1){
                                 map_erase(Player.x+1, Player.y);
                                draw_game(1);
                            }
                            else{
                                map_erase(Player.x, Player.y-1);
                                draw_game(1);
                            }
                        draw_game(1);
                    }
                    if (count==10){ // monster2 <- water spell
                         /*       if(ref==1){
                                    add_ex(Player.x+1, Player.y);
                                    draw_game(1);
                                }
                                else{
                                    add_ex(Player.x, Player.y-1);
                                    draw_game(1);
                                } */
                        if(!Player.has_key){
                            if(!Player.water){
                        const char* line1 = "Wrong spell";
                        const char* line2 = "So there is no key";
                        speech(line1, line2);
                        if (Player.sh){
                        Player.health-=30;
                        }
                        else{Player.health-=50;}
                        return FULL_DRAW;
                        break;
                            }
                            else if (Player.water){
                            const char* line1 = "I am defeated";
                            const char* line2 = "You have the key now";
                            speech(line1, line2);
                            if(ref==1){
                            map_erase(m2x,m2y);
                            draw_game(1);
                            m2y--;
                            add_monster(m2x, m2y);
                            draw_game(1);}
                            if(ref==2){
                            map_erase(m2x,m2y);
                            draw_game(1);
                            m2y++;
                            add_monster(m2x, m2y);
                            draw_game(1);}
                            if(ref==3){
                            map_erase(m2x,m2y);
                            draw_game(1);
                            m2x++;
                            add_monster(m2x, m2y);
                            draw_game(1);}
                            if(ref==4){
                            map_erase(m2x,m2y);
                            draw_game(1);
                            m1x--;
                            add_monster(m2x, m2y);
                            draw_game(1);}
                            Player.has_key=true;
                            if(ref==1){
                                    map_erase(Player.x+1, Player.y);
                                    draw_game(1);
                                }
                                else{
                                    map_erase(Player.x, Player.y-1);
                                    draw_game(1);
                                }
                            break;
                            }
                        }
                        else{
                            if(!Player.water){
                        const char* line1 = "I like fire";
                        const char* line2 = "back off";
                        speech(line1, line2);
                        if (Player.sh){
                        Player.health-=30;
                        }
                        else{Player.health-=50;}

                            }
                            else if (Player.water){
                            const char* line1 = "I am defeated";
                            const char* line2 = "now leave";
                            speech(line1, line2);
                            Player.health+=15;
                            }

                
                            if(ref==1){
                                 map_erase(Player.x+1, Player.y);
                                draw_game(1);
                            }
                            else{
                                map_erase(Player.x, Player.y-1);
                                draw_game(1);
                            }
                        }
                        
                    }
                }

        case MENU_BUTTON: break;
        case NO_ACTION: 
            for(int i = map_width() + 3; i < map_area(); i += 39)
            {
            if (Player.x==(i % map_width()) &&  Player.y==(i / map_width())){
                        if (Player.sh){
                        Player.health-=5;
                        }
                        else{Player.health-=10;}
                wait(.5);
                break;
            }
            }
            if (Player.x==15 && Player.y==10){
                Player.health += 5;
                wait(.5);
                break;
            }        
        break;
        case OMNIPOTENT:

            if (Player.omni) {
                Player.omni = 0;
            } else {
                Player.omni = 1;
            }
            break;
        default:        break;
    }
    return NO_RESULT;
}

/**
 * Entry point for frame drawing. This should be called once per iteration of
 * the game loop. This draws all tiles on the screen, followed by the status
 * bars. Unless init is nonzero, this function will optimize drawing by only
 * drawing tiles that have changed from the previous frame.
 */
void draw_game(int init)
{
    // Draw game border first
    if(init) draw_border();

    // Iterate over all visible map tiles
    for (int i = -5; i <= 5; i++) // Iterate over columns of tiles
    {
        for (int j = -4; j <= 4; j++) // Iterate over one column of tiles
        {
            // Here, we have a given (i,j)

            // Compute the current map (x,y) of this tile
            int x = i + Player.x;
            int y = j + Player.y;

            // Compute the previous map (px, py) of this tile
            int px = i + Player.px;
            int py = j + Player.py;

            // Compute u,v coordinates for drawing
            int u = (i+5)*11 + 3;
            int v = (j+4)*11 + 15;

            // Figure out what to draw
            DrawFunc draw = NULL;
            if (init && i == 0 && j == 0){ // Only draw the player on init
             if(Player.x==7 && Player.y==2 || Player.x==8 && Player.y==2){
               draw_player2(u,v,Player.has_key);
            } else{
                draw_player(u, v, Player.has_key);
              //  continue;}
            }
            }
            else if (x >= 0 && y >= 0 && x < map_width() && y < map_height()) // Current (i,j) in the map
            {
                MapItem* curr_item = get_here(x, y);
                MapItem* prev_item = get_here(px, py);
                if (init || curr_item != prev_item) // Only draw if they're different
                {
                    if (curr_item) // There's something here! Draw it
                    {
                        draw = curr_item->draw;
                    }
                    else // There used to be something, but now there isn't
                    {
                        draw = draw_nothing;
                    }
                }
            }
            else if (init) // If doing a full draw, but we're out of bounds, draw the walls.
            {
                draw = draw_wall;
            }

            // Actually draw the tile
            if (draw) draw(u, v);
        }
    }

    // Draw status bars
    draw_upper_status(Player.x, Player.y, Player.health);
    draw_lower_status(Player.has_key);
}

void inventory(int f, int w, int k, int s, int h){
    uLCD.cls();
    uLCD.color(RED);
    uLCD.printf("\n   Inventory \n");
    uLCD.color(GREEN);
    uLCD.printf("fire spell: %d\n", f );
    uLCD.printf("water spell: %d\n", w );  
    uLCD.printf("Key status: %d\n", k ); 
    uLCD.printf("Shield status: %d\n",s ); 
    uLCD.printf("Hammer status: %d\n", h ); 
    wait(1);
    uLCD.cls();
}

void game_over(int next_state){
    if (next_state==GAME_OVER){
        uLCD.color(RED);
        uLCD.cls();
        uLCD.text_width(1);
        uLCD.text_height(1);
        uLCD.printf("Congratulation ");
        uLCD.text_width(1);
        uLCD.text_height(1);
        uLCD.color(GREEN);
        while (true) {
        }
    }
}
void game_lose(int next_state){
    if (next_state==GAME_OVER){
        uLCD.color(RED);
        uLCD.cls();

         uLCD.text_width(2);
        uLCD.text_height(2);
        uLCD.printf("\n\n\nGAME OVER");
        uLCD.text_width(1);
        uLCD.text_height(1);
        uLCD.color(GREEN);
        while (true) {
        }
    }
}
/**
 * Initialize the main world map. Add walls around the edges, interior chambers,
 * and plants in the background so you can see motion. Note: using the similar
 * procedure you can init the secondary map(s).
 */
void init_main_map()
{
    Map* map = set_active_map(0);
    for(int i = map_width() + 3; i < map_area(); i += 39)
    {
        add_poison(i % map_width(), i / map_width());
    }
    add_cure(15, 10);
    pc.printf("plants\r\n");

    pc.printf("Adding walls!\r\n");
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    pc.printf("Walls done!\r\n");
    add_NPC(NPC_x, NPC_y);
    add_NPC2(13, 13);
    add_portal(portal_x, portal_y);
    print_map();
    add_shield(5,12);
    add_hammer (10,14);
    add_keyy(1,5);
    add_monster(m1x, m1y);
    add_monster2(m2x, m2y);
    add_plant(7,2);
    add_plant(8, 2);
    pc.printf("Added NPC, PORTAL\r\n");
}

void init_map2()
{
    
    Map* map = set_active_map(1);
    Player.x=Player.y=5;

    for(int i = map_width() + 3; i < map_area(); i += 39)
    {
        add_poison(i % map_width(), i / map_width());
    }
    /*    pc.printf("plants\r\n");
    pc.printf("Adding walls!\r\n");
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    */
    pc.printf("Walls done!\r\n");

    pc.printf("Adding escaping chamber\r\n");
    add_wall(20,              0,              VERTICAL, 12);
    add_wall(22,              11,             HORIZONTAL, 9);
    add_wall(30,              0,              VERTICAL,  12);
    pc.printf("Chamber is done!\r\n");

    add_NPC(NPC_x, NPC_y);
    add_door(21,12);
    add_crown(25,5);
    

    print_map();
    pc.printf("Added NPC, CROWN\r\n");

}

/**
 * Program entry point! This is where it all begins.
 * This function orchestrates all the parts of the game. Most of your
 * implementation should be elsewhere - this holds the game loop, and should
 * read like a road map for the rest of the code.
 */

void game_start(int statenow){
    uLCD.cls();
    uLCD.printf("\n\n   welcome to \n");
    uLCD.printf("   rpg quest \n\n\n\n\n\n");
    uLCD.printf(" press any button\n");
    uLCD.printf("     to start\n");
    while (statenow==START){
        GameInputs inputs = read_inputs();
        statenow = get_action(inputs);
    }
}
void move_npc(int tm){
    map_erase(NPC_x, NPC_y);
    xx=cc%100;
    if (xx==0){
        yy++;}
    
    if (yy%2==0){
    NPC_x++;
    }
    else{
    NPC_x--;
    } 
    add_NPC(NPC_x, NPC_y);
}
int main()
{
    GameInputs inputs;
    int action;
    int next_state;
    // First things first: initialize hardware
    ASSERT_P(hardware_init() == ERROR_NONE, "Hardware init failed!");
    game_start(gameState);

    // Initialize the maps
    maps_init();
    init_main_map();

    // Initialize game state
    set_active_map(0);
    Player.x = Player.y = 5;
    Player.health=100;

    // Initial drawing
    draw_game(1);

    // Main game loop
    while(1)
    {   cc++;
        nn=cc%10;
        if(Player.health <=0){
        game_lose(GAME_OVER);
        }
        else{
        // Timer to measure game update speed
        Timer t; t.start();

        // Actually do the game update:
        // 1. Read inputs
        inputs=read_inputs();
        // 2. Determine action (get_action)
        action=get_action(inputs);
        // 3. Update game (update_game)

        next_state=update_game(action);
        // 3b. Check for game over
        game_over(next_state);
        // 4. Draw frame (draw_game)
        draw_game(next_state);

        if (cc>20 && nn==0){
            move_npc(cc);
        }
        // 5. Frame delay
        t.stop();
        int dt = t.read_ms();
        if (dt < 100) wait_ms(100 - dt);
        }
    }

}