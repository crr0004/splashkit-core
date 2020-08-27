#include "box2d/box2d.h"
#include <stdlib.h>
#include "sprites.h"
#include "graphics.h"
#include "input.h"
#include "physics.h"

int main(){
    
    
    using namespace splashkit_lib;
    auto physics_context = Box2DContext{};
    auto player_bitmap = load_bitmap("player", "player.png");
    sprite player = create_sprite("player", player_bitmap);
    auto player_2_sprite = create_sprite("player_block", player_bitmap);

    auto window_to_show = open_window("Physics Collision Game", 800, 600);

    sprite_set_position(player_2_sprite, {100.0f, 50.0f});
    sprite_set_position(player, {700.0f, 50.0f});

    auto body_def = physics_context.get_default_body_definition();
    body_def.type = b2_dynamicBody;

    auto player_body = physics_context.add_sprite_to_world(&player, body_def);
    auto player_2_body = physics_context.add_sprite_to_world(&player_2_sprite, body_def);

    player_body.second->ApplyLinearImpulseToCenter({-10.0f, 0.0f}, true);
    player_2_body.second->ApplyLinearImpulseToCenter({5.0f, 0.0f}, true);


    




    bool quit = false;
    while(!quit){
        process_events();
        clear_screen(COLOR_WHITE);
        physics_context.step();


        // box2dWorld.world.Step(1.0f/60.0f, 6, 2);

        // auto groudExtents = groundBody->GetFixtureList()->GetAABB(0).GetExtents();
        // draw_rectangle(
        //     COLOR_BLACK,
        //     (groundBody->GetPosition().x - groudExtents.x) * box2dWorld.options.scale_x,
        //     (groundBody->GetPosition().y - groudExtents.y) * box2dWorld.options.scale_y,
        //     groudExtents.x * 2 * box2dWorld.options.scale_x,
        //     groudExtents.y * 2 * box2dWorld.options.scale_y, box2dWorld.options);

        // auto bodyExtents = body->GetFixtureList()->GetAABB(0).GetExtents();
        // // draw_rectangle(
        // //     COLOR_BLUE,
        // //     (body->GetPosition().x - bodyExtents.x) * box2dWorld.options.scale_x,
        // //     (body->GetPosition().y - bodyExtents.y) * box2dWorld.options.scale_y,
        // //     bodyExtents.x * 2 * box2dWorld.options.scale_x,
        // //     bodyExtents.y * 2 * box2dWorld.options.scale_y, box2dWorld.options);

        // sprite_set_position(player, {(body->GetPosition().x - bodyExtents.x) * box2dWorld.options.scale_x,
        //                              (body->GetPosition().y - bodyExtents.y) * box2dWorld.options.scale_y});

        // sprite_set_rotation(player, body->GetAngle()*(180.0f/M_PI));

        update_all_sprites();
        draw_all_sprites();

        if (key_down(ESCAPE_KEY)) {
                quit = true;
        }

        if (quit_requested()) {
            quit = true;
        }

        refresh_screen(60);
    }
    return 0;
}