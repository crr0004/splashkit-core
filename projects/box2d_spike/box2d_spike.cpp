#include "splashkit.h"
#include "box2d/box2d.h"
#include <stdlib.h>

class Box2DContext {
    public:
        b2World world;
        drawing_options options = option_defaults();
};
Box2DContext setUpBox2D(){

    auto world = Box2DContext{b2World{{0.0f, 10.0f}}};
    world.options.scale_x = 100;
    world.options.scale_y = 100;

    return world;
}
int main(){
    
    auto player_bitmap = load_bitmap("player", "player.png");
    sprite player = create_sprite("player", player_bitmap);

    auto window_to_show = open_window("Physics Collision Game", 800, 600);

    auto box2dWorld = setUpBox2D();

    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(600.0f/box2dWorld.options.scale_x, 200.0f/box2dWorld.options.scale_y);
	b2Body* groundBody = box2dWorld.world.CreateBody(&groundBodyDef);
	b2PolygonShape groundBox;
	groundBox.SetAsBox(300.0f/box2dWorld.options.scale_y/2, 50.0f/box2dWorld.options.scale_y/2);
	groundBody->CreateFixture(&groundBox, 0.0f);


    b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(400.0f/box2dWorld.options.scale_y, 50.0f/box2dWorld.options.scale_y);
	b2Body* body = box2dWorld.world.CreateBody(&bodyDef);

	// Define another box shape for our dynamic body.
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(sprite_width(player)/box2dWorld.options.scale_y/2, sprite_height(player)/box2dWorld.options.scale_y/2);

	// Define the dynamic body fixture.
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;

	// Set the box density to be non-zero, so it will be dynamic.
	fixtureDef.density = 1.0f;

	// Override the default friction.
	fixtureDef.friction = 0.3f;

	// Add the shape to the body.
	body->CreateFixture(&fixtureDef);

    sprite_set_position(player, {screen_center().x, 10});



    bool quit = false;
    while(!quit){
        process_events();
        clear_screen(COLOR_WHITE);


        box2dWorld.world.Step(1.0f/60.0f, 6, 2);

        auto groudExtents = groundBody->GetFixtureList()->GetAABB(0).GetExtents();
        draw_rectangle(
            COLOR_BLACK,
            (groundBody->GetPosition().x - groudExtents.x) * box2dWorld.options.scale_x,
            (groundBody->GetPosition().y - groudExtents.y) * box2dWorld.options.scale_y,
            groudExtents.x * 2 * box2dWorld.options.scale_x,
            groudExtents.y * 2 * box2dWorld.options.scale_y, box2dWorld.options);

        auto bodyExtents = body->GetFixtureList()->GetAABB(0).GetExtents();
        // draw_rectangle(
        //     COLOR_BLUE,
        //     (body->GetPosition().x - bodyExtents.x) * box2dWorld.options.scale_x,
        //     (body->GetPosition().y - bodyExtents.y) * box2dWorld.options.scale_y,
        //     bodyExtents.x * 2 * box2dWorld.options.scale_x,
        //     bodyExtents.y * 2 * box2dWorld.options.scale_y, box2dWorld.options);

        sprite_set_position(player, {(body->GetPosition().x - bodyExtents.x) * box2dWorld.options.scale_x,
                                     (body->GetPosition().y - bodyExtents.y) * box2dWorld.options.scale_y});

        sprite_set_rotation(player, body->GetAngle()*(180.0f/M_PI));

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