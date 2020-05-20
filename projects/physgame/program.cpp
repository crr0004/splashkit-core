#include "splashkit.h"
<<<<<<< HEAD

=======
>>>>>>> c43b1aa... I have created a small game that draws a player to a window. It can be controlled with the arrow keys. The aim is to build off this game and implement the new collision functions.
#define SCREEN_BORDER 100

//load resources
void load_resources()
<<<<<<< HEAD
{   
    load_bitmap("player", "aaa.png");
    load_bitmap("coin", "Coin.png");
    load_bitmap("fish", "Fish.png");
    load_bitmap("icecream", "Icecream.png");
    load_bitmap("pacman", "Pacman.png");
    load_bitmap("start", "Start.png");
    load_bitmap("quit", "Quit.png");
    load_bitmap("sound", "Sound.png");
=======
{
    load_bitmap("player", "Player.png");
>>>>>>> c43b1aa... I have created a small game that draws a player to a window. It can be controlled with the arrow keys. The aim is to build off this game and implement the new collision functions.
}

//defines player attributes
struct Player
{
<<<<<<< HEAD
    int sprite = 0;
    bitmap bitmap;
    point_2d point;
};

Player player;

//defines player attributes
struct Item
{
    int sprite = 0;
    bitmap bitmap;
    point_2d point;
};

Item item;

=======
    bitmap bitmap = bitmap_named("player");
    float x = screen_width() / 2;
    float y = screen_height() / 2;
};

>>>>>>> c43b1aa... I have created a small game that draws a player to a window. It can be controlled with the arrow keys. The aim is to build off this game and implement the new collision functions.
//screen characteristics
void window_setup(string win_name, int width, int height)
{
    open_window(win_name, width, height);
}

int main()
{
    load_resources();
<<<<<<< HEAD
    window window = open_window("Physics Collision Game", 800, 600);

    //player setup
    player.bitmap = bitmap_named("player");
    player.point.x = screen_width() / 2 - 60;
    player.point.y = screen_height() / 2 - 60;
    int sprite_num = 0;

    //item setup
    item.bitmap = bitmap_named("coin");
    item.point.x = screen_width() / 2 - 200;
    item.point.y = screen_height() / 2 - 200;

    bool quit = false;

    //Collision entity (geometry or sprite) to be loaded here

    while (quit == false)
    {
        process_events();
        clear_screen(COLOR_WHITE);
        
        draw_bitmap(item.bitmap, item.point.x, item.point.y);
        draw_bitmap(player.bitmap, player.point.x, player.point.y);
        draw_text_on_window(window, "Press c to change sprite", COLOR_BLACK, 550, 550);

        //Collision check with one of the new collision functions
        //Add a function that changes which collision function is used

        //Player controls
        if (key_down(LEFT_KEY))  player.point.x -= 5;
        if (key_down(RIGHT_KEY))  player.point.x += 5;
        if (key_down(DOWN_KEY))  player.point.y += 5;
        if (key_down(UP_KEY))  player.point.y -= 5;

        //Quit options
        if (key_down(ESCAPE_KEY)) 
            {
                quit = true;
            }
        if (quit_requested()) 
            {
                quit = true;
            }

        //Collision
        //Bitmap / sprite based 
        if (bitmap_collision(player.bitmap, player.point, item.bitmap, item.point) == true)
        {
            item.point = random_window_point(window);
        }

        //Changes the player sprite
        if (key_released(C_KEY))
        {
            sprite_num = ((sprite_num + 1) % 3);
            write_line(sprite_num);
            if (sprite_num == 0)
            {
                player.bitmap = bitmap_named("player");
            }
            if (sprite_num == 1)
            {
                player.bitmap = bitmap_named("pacman");
            }
            if (sprite_num == 2)
            {
                player.bitmap = bitmap_named("fish");
            }

        }
=======
    window_setup("Test program", 800, 600);
    Player player;

    while (! quit_requested())
    {
        process_events();
        clear_screen(COLOR_WHITE);
        draw_bitmap(player.bitmap, player.x, player.y);

        if (key_down(LEFT_KEY))  player.x -= 5;
        if (key_down(RIGHT_KEY))  player.x += 5;
        if (key_down(DOWN_KEY))  player.y += 5;
        if (key_down(UP_KEY))  player.y -= 5;

>>>>>>> c43b1aa... I have created a small game that draws a player to a window. It can be controlled with the arrow keys. The aim is to build off this game and implement the new collision functions.
        refresh_screen(60);
    } 
    
    return 0;
}

<<<<<<< HEAD
=======







>>>>>>> c43b1aa... I have created a small game that draws a player to a window. It can be controlled with the arrow keys. The aim is to build off this game and implement the new collision functions.
// ============== Below is the old code written by Sunny (?)

//declares rectangles and their attributes
/*
void rect_setup()
{
    rectangle rect;
    rect.x = 100, rect.y = 300;
    rect.width = 100, rect.height = 100;

    rectangle rect2;
    rect2.x = player2_x, rect2.y = player2_y;
    rect2.width = 200, rect2.height = 200;
}
*/

/*
int main()
{
    //player initial start position
    double player_x = 50, player_y = 50;

    bitmap_set_cell_details(player, 73, 105, 4, 4 ,16);

    //@Point2D for Player
    point_2d PYpoint2d;
    PYpoint2d.x = player_x, PYpoint2d.y = player_y;


    while ( not quit_requested() || key_typed(ESCAPE_KEY) )
    {
        
        process_events();
        
        if (key_down(LEFT_KEY))  PYpoint2d.x -= 5;
        if (key_down(RIGHT_KEY))  PYpoint2d.x += 5;
        if (key_down(DOWN_KEY))  PYpoint2d.y += 5;
        if (key_down(UP_KEY))  PYpoint2d.y -= 5;

        update_camera_position(player_x, player_y);

        clear_screen(COLOR_WHITE);

        // Draw bitmap and rect
        draw_bitmap(player, PYpoint2d.x, PYpoint2d.y);
        draw_rectangle(COLOR_BLUE, rect); 

    
        //Bitmap collision with rec
       if(bitmap_rectangle_collision(player, PYpoint2d, rect)) 
       {
           rect.x +=1;
       }

        refresh_screen(60);
    }

    open_window("Collision Test", 800, 800);

    double player_x = 50, player_y = 50;
    double player2_x = 150, player2_y = 150;


    rectangle rect;
    rect.x = player_x, rect.y = player_y;
    rect.width = 100, rect.height = 100;

    


    while ( not quit_requested() || key_typed(ESCAPE_KEY) )
    {
        
        process_events();
        
        if (key_down(LEFT_KEY))  rect.x -= 5;
        if (key_down(RIGHT_KEY))  rect.x += 5;
        if (key_down(DOWN_KEY))  rect.y += 5;
        if (key_down(UP_KEY))  rect.y -= 5;

        update_camera_position(player_x, player_y);

        clear_screen(COLOR_WHITE);

        draw_rectangle(COLOR_RED, rect);
        draw_rectangle(COLOR_BLUE, rect2);

       
        //if(rect_rect_collision(rect, rect2))
        //{
            //rect2.x +=1;
        //}
        bitmap_collision(player, )


        refresh_screen(60);
    }


    return 0;
}
*/