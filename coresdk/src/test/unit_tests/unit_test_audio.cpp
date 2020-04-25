#include <catch.hpp>

#include "audio_driver.h"
#include "music.h"

// TEST_CASE("Audio can be setup and shutdown", "[audio]"){
//     namespace sk = splashkit_lib;
//     SECTION("Setup"){
//         sk::sk_init_audio();
//         REQUIRE(sk::sk_audio_get_last_error() != -1);
//         sk::sk_open_audio();

//         REQUIRE(sk::sk_audio_is_open());

//         sk::sk_close_audio();
//         REQUIRE(sk::sk_audio_get_last_error() != -1);

//     }

// }
TEST_CASE("Music can be played", "[audio]"){
    // Setup
    namespace sk = splashkit_lib;

    // SECTION("Load Music"){
    //     sk::sk_init_audio();
    //     REQUIRE(sk::sk_audio_get_last_error() != -1);

    //     sk::music music = sk::load_music("magic", "magical_night_1.ogg");
    //     REQUIRE(music != nullptr);
        
    //     sk::free_all_music();
    //     sk::sk_close_audio();
    // }

    SECTION("Load and play music"){
        sk::sk_init_audio();
        REQUIRE(sk::sk_audio_get_last_error() != -1);
        sk::sk_open_audio();

        sk::music music = sk::load_music("magic", "test.ogg");
        REQUIRE(music != nullptr);
        sk::play_music(music);
        int i = 0;
        while(i < 10000){
            struct timespec ts, rem;
            unsigned long nsec = 10000000;
            ts.tv_sec = (time_t)(nsec / 1000000000ul);
            ts.tv_nsec = (long)(nsec % 1000000000ul);
            while(nanosleep(&ts, &rem) == -1 && errno == EINTR){
                ts = rem;
            }
            i++;
        }

        sk::free_all_music();
        sk::sk_close_audio();
        // sk::play_music(music);


    }

    // Teardown
    // sk::sk_close_audio();
    // REQUIRE(sk::sk_audio_get_last_error() != -1);
}