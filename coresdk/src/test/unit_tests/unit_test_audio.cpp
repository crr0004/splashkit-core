#include <catch.hpp>

#include "audio_driver.h"
#include "music.h"

TEST_CASE("Audio can be setup and shutdown", "[audio]"){
    namespace sk = splashkit_lib;
    SECTION("Setup"){
        sk::sk_init_audio();
        REQUIRE(sk::sk_audio_get_last_error() != -1);
        sk::sk_open_audio();

        REQUIRE(sk::sk_audio_is_open());

        sk::stop_music();
        sk::sk_close_audio();
        sk::free_all_music();
        REQUIRE(sk::sk_audio_get_last_error() != -1);
    }

    SECTION("Get Device Attributes"){
        sk::sk_init_audio();
        REQUIRE(sk::sk_audio_get_last_error() != -1);
        sk::sk_open_audio();

        int size = 0;
        int* attributes = sk::sk_get_device_attributes(&size);
        REQUIRE(size > 0);
        REQUIRE(attributes != nullptr);
        
        delete attributes;

        sk::sk_close_audio();
        sk::free_all_music();

    }

}

TEST_CASE("Music can be played", "[audio][music]"){
    // Setup
    namespace sk = splashkit_lib;
    sk::sk_init_audio();
    REQUIRE(sk::sk_audio_get_last_error() != -1);
    sk::sk_open_audio();

    // SECTION("Load Music"){
    //     sk::sk_init_audio();
    //     REQUIRE(sk::sk_audio_get_last_error() != -1);

    //     sk::music music = sk::load_music("magic", "magical_night_1.ogg");
    //     REQUIRE(music != nullptr);
        
    //     sk::free_all_music();
    //     sk::sk_close_audio();
    // }

    SECTION("Load and play music"){

        sk::music music = sk::load_music("magic", "magical_night.ogg");
        REQUIRE(music != nullptr);
        sk::play_music(music);
        // Uncomment to hear audio playing
        int i = 0;
        while(i < 100){
            struct timespec ts, rem;
            unsigned long nsec = 10000000;
            ts.tv_sec = (time_t)(nsec / 1000000000ul);
            ts.tv_nsec = (long)(nsec % 1000000000ul);
            while(nanosleep(&ts, &rem) == -1 && errno == EINTR){
                ts = rem;
            }
            i++;
            INFO("Volume is " << sk::music_volume());
        }

        // sk::play_music(music);


    }

    SECTION("Play pause music"){

        sk::music music = sk::load_music("magic", "magical_night.ogg");
        REQUIRE(music != nullptr);
        sk::play_music(music);
        sk::pause_music();
        REQUIRE(sk::sk_audio_get_last_error() != -1);
        sk::resume_music();
        REQUIRE(sk::sk_audio_get_last_error() != -1);
        sk::stop_music();
        REQUIRE(sk::sk_audio_get_last_error() != -1);
    }

    sk::stop_music();
    sk::free_all_music();
    sk::sk_close_audio();
    // Teardown
    // sk::sk_close_audio();
    // REQUIRE(sk::sk_audio_get_last_error() != -1);
}