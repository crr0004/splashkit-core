#include "splashkit.h"
#define SCREEN_BORDER 100

//load resources
void load_resources()
{
    load_bitmap("player", "Player.png");
}

//defines player attributes
struct Player
{
    bitmap bitmap = bitmap_named("player");
    float x = screen_width() / 2;
    float y = screen_height() / 2;
};

//screen characteristics
void window_setup(string win_name, int width, int height)
{
    open_window(win_name, width, height);
}

int main()
{
    load_resources();
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

        refresh_screen(60);
    } 
    
    return 0;
}








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