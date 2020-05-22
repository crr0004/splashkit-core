#include "splashkit.h"
#include <stdlib.h>

#define SCREEN_BORDER 100

//load resources
void load_resources()
{   
    load_bitmap("player", "aaa.png");
    load_bitmap("coin", "Coin.png");
    load_bitmap("fish", "Fish.png");
    load_bitmap("icecream", "Icecream.png");
    load_bitmap("pacman", "Pacman.png");
    load_bitmap("start", "Start.png");
    load_bitmap("quit", "Quit.png");
    load_bitmap("sound", "Sound.png");
}

window window_to_show;

//screen characteristics
void window_setup(string win_name, int width, int height)
{
    open_window(win_name, width, height);
}

void chance_spawn_object(std::vector<sprite> *objects_to_avoid){
    if(rand() < RAND_MAX /20){
        sprite sprite_to_add = create_sprite("coin");

        point_2d random_point = random_window_point(window_to_show);
        sprite_set_x(sprite_to_add, random_point.x);
        sprite_set_y(sprite_to_add, 0);

        // sprite_set_dy(sprite_to_add, 1);
        sprite_set_velocity(sprite_to_add, vector_to(0, 10));

        objects_to_avoid->push_back(
            sprite_to_add
        );

    }

}

int main()
{
    load_resources();
    window_to_show = open_window("Physics Collision Game", 800, 600);

    //player setup
    int sprite_num = 0;
    sprite player = create_sprite("player", "player");
    int score = 0;

    // Position setups
    sprite_set_x(player, screen_width() / 2 - 60);
    sprite_set_y(player, screen_width() / 2 - 60);

    std::vector<sprite> objects_to_avoid;

    bool quit = false;
    //Collision entity (geometry or sprite) to be loaded here

    while (quit == false)
    {
        process_events();
        clear_screen(COLOR_WHITE);
        
        update_all_sprites();
        draw_all_sprites();
        // draw_bitmap(item.bitmap, item.point.x, item.point.y);
        // draw_bitmap(player.bitmap, player.point.x, player.point.y);
        draw_text_on_window(window_to_show, "Score: " + std::to_string(score), COLOR_BLACK, 550, 550);

        //Collision check with one of the new collision functions
        //Add a function that changes which collision function is used

        chance_spawn_object(&objects_to_avoid);

        //Player controls
        point_2d pos = sprite_position(player);
        if (key_down(LEFT_KEY)){
            pos.x += -5;
        }  
        if (key_down(RIGHT_KEY)){
            pos.x += 5;
        }
        if (key_down(DOWN_KEY)){
            pos.y += 5;
        }
        if (key_down(UP_KEY)){
            pos.y += -5;
        }
        sprite_set_position(player, pos);

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
        // sprite_collision(player, coin);
        for(int i = 0; i < objects_to_avoid.size(); i++){
            auto s = objects_to_avoid[i];
            bool player_collision = sprite_collision(player, s);
            if(player_collision){
                score++;
            }
            if (player_collision || sprite_offscreen(s))
            {
                // item.point = random_window_point(window);
                // sprite_set_position(coin, random_window_point(window_to_show));
                objects_to_avoid.erase(objects_to_avoid.begin() + i);
                free_sprite(s);
            }
        }

        //Changes the player sprite
        
        refresh_screen(60);
    } 
    
    return 0;
}