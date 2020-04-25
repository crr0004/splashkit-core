//
//  AudioDriver.cpp
//
//  Created by Andrew Cain on 28/11/2013.
//  Copyright (c) 2013 Andrew Cain. All rights reserved.
//

#ifdef __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#else
#include <SDL.h>
#include <SDL_mixer.h>
#endif

#include <iostream>
#include <sstream>
#include <AL/al.h>
#include <AL/alc.h>

extern "C"{
    #include <libavutil/frame.h>
    #include <libavutil/mem.h>
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/avutil.h>
    #include <libavutil/channel_layout.h>
    #include <libavutil/samplefmt.h>
    #include <libavutil/opt.h>
    #include <libswresample/swresample.h>
}

#include "audio_driver.h"
#include "core_driver.h"

using std::cerr;
using std::endl;

#define SG_MAX_CHANNELS 64
namespace splashkit_lib
{
    static Mix_Chunk * _sk_sound_channels[SG_MAX_CHANNELS];
    static sk_sound_data * _current_music  = nullptr;

    // Any error bits that get set by libraries
    static int audio_last_error = 0;

    // access system data from core driver
    extern sk_system_data _sk_system_data;

    static bool _sk_audio_open = false;

    // OpenAL devices
    static ALCdevice *device = nullptr;
    static ALCcontext *openal_ctx = nullptr;

    int sk_audio_get_last_error(){
        return audio_last_error;
    }

    void set_error_from_al_error(){
        ALenum error = alGetError();
        if(error != AL_NO_ERROR){
            audio_last_error = -1;
        }
    }

    void sk_init_audio()
    {
        // Mix_Init(~0);    const ALCchar *name;

        /* Open and initialize a device */
        if(_sk_audio_open){
            return;
        }
        device = nullptr;
        device = alcOpenDevice(nullptr);
        if (!device){
            // TODO Handle errors gracefully
            // fprintf(stderr, "Could not open a device!\n");
            set_error_from_al_error();
            return;
        }

        openal_ctx = alcCreateContext(device, NULL);
        if (openal_ctx == nullptr || alcMakeContextCurrent(openal_ctx) == ALC_FALSE)
        {
            if (openal_ctx != nullptr)
                alcDestroyContext(openal_ctx);
            alcCloseDevice(device);
            // fprintf(stderr, "Could not set a context!\n");
            // TODO Handle errors gracefully
            set_error_from_al_error();
            return;
        }

        const ALCchar *name;
        if (alcIsExtensionPresent(device, "ALC_ENUMERATE_ALL_EXT"))
            name = alcGetString(device, ALC_ALL_DEVICES_SPECIFIER);
        if (!name || alcGetError(device) != AL_NO_ERROR)
            name = alcGetString(device, ALC_DEVICE_SPECIFIER);
        printf("Opened \"%s\"\n", name);
    }

    bool sk_audio_is_open()
    {
        internal_sk_init();
        return _sk_audio_open;
    }

    void sk_open_audio()
    {
        // internal_sk_init();
        // if ( Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096 ) < 0 )
        // {
        //     //set_error_state("Unable to load audio. Mix_OpenAudio failed.");
        //     return;
        // }

        // Uint16 format;
        // Mix_QuerySpec(&_sk_system_data.audio_specs.audio_rate, &format, &_sk_system_data.audio_specs.audio_channels);
        // _sk_system_data.audio_specs.times_opened++;
        // _sk_system_data.audio_specs.audio_format = format;

        // Mix_AllocateChannels(SG_MAX_CHANNELS);

        _sk_audio_open = true;
    }

    void sk_close_audio()
    {
        // Mix_CloseAudio();
        // _sk_system_data.audio_specs.times_opened--;
        // if ( 0 == _sk_system_data.audio_specs.times_opened )
        // {
        //     sk_audiospec empty = { 0, 0, 0, 0 };
        //     _sk_system_data.audio_specs = empty;
        //     _sk_audio_open = false;
        // }
        ALCdevice *device;
        ALCcontext *ctx;

        ctx = alcGetCurrentContext();
        if (ctx == NULL)
            return;

        device = alcGetContextsDevice(ctx);

        alcMakeContextCurrent(NULL);
        alcDestroyContext(ctx);
        alcCloseDevice(device);
    }

    void format_av_error(int ret)
    {
        // Only want to trigger this on unhandable errors
        if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
        {
            char errbuff[1028];
            av_strerror(ret, errbuff, 1028);
            fprintf(stderr, "Error message (%d): %s\n", ret, errbuff);
            exit(ret);
        }
    }

    void format_av_error(void *pointer, const char *error_msg)
    {
        if (pointer == nullptr)
        {
            fprintf(stderr, "%s\n", error_msg);
            exit(-1);
        }
    }

    const uint8_t* decode(AVCodecContext *dec_ctx,
                 AVFormatContext *form_ctx,
                 SwrContext *swr,
                 size_t* out_mem_size)
    {
        /*
        In this context of reading a file, decoding it, resampling it, and reading it memory
        the following words have the following meanings.

        Packet: a set of data read from a file/IO stream
        Frame: Encoded/Decoded data from the library
        Format: How the data is stored in the file
        Codec: The kind of compression/decompression used on the file
        Stream: One kind of data set in the file (audio, video, subtitles)

        */

        int ret = 0;

        AVPacket *pkt = av_packet_alloc();
        AVFrame *frame = av_frame_alloc();
        /* 
        Grab our first packet from the file
        */
        ret = av_read_frame(form_ctx, pkt);
        format_av_error(ret);

        ret = avcodec_send_packet(dec_ctx, pkt);
        format_av_error(ret);

        // Pointers for our data buffers for the converter to write into
        uint8_t *outData;
        int lineSize;

        // Keeping track if the amount of frames decoded changes
        int last_nb_samples = 0;

        /* read all the output frames (in general there may be any number of them */
        std::basic_stringstream<uint8_t> *stream = new std::basic_stringstream<uint8_t>();
        bool is_eof = false;

        ret = 0; // Clear the error number so it's not confused
        while (ret >= 0 && !is_eof)
        {
            do
            {
                if (ret == AVERROR(EAGAIN))
                {
                    // We need more data from the file to decode
                    // So grab another packet and then read it into a frame
                    ret = av_read_frame(form_ctx, pkt);
                    format_av_error(ret);
                    if (ret == AVERROR_EOF)
                    {
                        // TODO
                        // This should probably be fixed.
                        // This loop needs to be elevated out
                        is_eof = true;
                        break;
                    }
                    ret = avcodec_send_packet(dec_ctx, pkt);
                    format_av_error(ret);
                }
                // This spits out EAGAIN when it needs another frame from the packet stream
                ret = avcodec_receive_frame(dec_ctx, frame);
            } while (ret == AVERROR(EAGAIN));

            if (is_eof)
            {
                // TODO Again lift this break out of the loop so the file reading is done properly
                break;
            }

            // If the frame amount has changed, we need to re-allocate our buffer
            if (frame->nb_samples != last_nb_samples)
            {
                ret = av_samples_alloc(
                    &outData,
                    &lineSize,
                    // As we are only doing alignment conversion keep we can cheat here
                    // by using the de-coded frame settings. Normally you have to figure this out
                    // by using the format changes. E.G Upsampling sample rate
                    frame->channels,
                    frame->nb_samples,
                    AV_SAMPLE_FMT_S16,
                    0);
                last_nb_samples = frame->nb_samples;
            }

            // Convert our frame to what we want through SWR
            const uint8_t **inBuf = (const uint8_t **)(frame->extended_data);
            format_av_error(ret);
            // Convert the audio to correct PCM format
            ret = swr_convert(
                swr,
                &outData,
                // Again cheating the samples because we keep the same sample rate when converting
                frame->nb_samples,
                inBuf,
                frame->nb_samples);
            format_av_error(ret);

            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                break;
            else if (ret < 0)
            {
                format_av_error(ret);
            }

            // We only care about the first channel because the data is converted to packed
            size_t size = lineSize;
            stream->write(outData, size);
        }

        stream->flush();

        uint8_t* buf = new uint8_t[stream->tellp()];
        stream->read(buf, stream->tellp());
        *out_mem_size = stream->tellp();

        delete stream;

        return buf;
    }

    /* 
    LoadBuffer loads the named audio file into an OpenAL buffer object, and
    returns the new buffer ID.
    */
    ALuint openal_load_sound(const uint8_t *membuf, size_t buf_size, size_t sample_rate)
    {

        ALenum err = AL_NO_ERROR;
        ALuint buffer;
        ALsizei num_bytes;

        buffer = 0;
        alGenBuffers(1, &buffer);
        alBufferData(
            buffer,
            AL_FORMAT_STEREO16,
            membuf,
            buf_size,
            sample_rate);

        /* Check if an error occured, and clean up if so. */
        err = alGetError();
        if (err != AL_NO_ERROR)
        {
            fprintf(stderr, "OpenAL Error: %s\n", alGetString(err));
            if (buffer && alIsBuffer(buffer))
                alDeleteBuffers(1, &buffer);
            return 0;
        }

        return buffer;
    }

    ALuint openal_play_sound(ALuint buffer)
    {
        ALuint source = 0;
        ALenum state;
        alGenSources(1, &source);
        alSourcei(source, AL_BUFFER, (ALint)buffer);
        if(alGetError()!=AL_NO_ERROR){
            exit(-1);
        }

        alSourcePlay(source);

        return source;
    }

    void *read_audio_into_buffer(const char *filename, int *out_openal_id)
    {

        AVFormatContext *avFormatContext = NULL;
        // Return code for calls
        int ret = 0;

        // Auto figure out the codec and audio settings
        ret = avformat_open_input(&avFormatContext, filename, NULL, NULL);
        format_av_error(ret);

        // Ensure the codecs are actually decoded by reading the headers
        ret = avformat_find_stream_info(avFormatContext, NULL);
        format_av_error(ret);

        // We assume the first stream is the audio stream
        // This can be replaced with a call to find_best_stream
        AVCodecParameters *codec_context = avFormatContext->streams[0]->codecpar;
        format_av_error(codec_context, "Could not find stream 0");

        // Grab the codec ID for the stream
        AVCodec *codec = avcodec_find_decoder(codec_context->codec_id);
        format_av_error(codec, "Could not find codec");

        // AVCodecParserContext *parser = av_parser_init(codec->id);
        // format_av_error(parser, "Could not create parser");

        AVCodecContext *c = avcodec_alloc_context3(codec);
        format_av_error(c, "Could not alloc codec");

        avcodec_parameters_to_context(c, codec_context);

        ret = avcodec_open2(c, codec, NULL);
        format_av_error(ret);

        /*
        Set the audio conversion options so it can be pushed into OpenAL
        OpenAL needs PCM (packed) mono or stero, 8 or 16 bit
        we just sample to stero for simplicity and keep all the incoming audio settings.
        If you wish to change the settings for sample rate changes, or layout changes,
        then changes to the decode method need to account for this
        See https://ffmpeg.org/doxygen/trunk/group__lswr.html for all the options
        */
        SwrContext *swr = swr_alloc_set_opts(
            NULL,
            AV_CH_LAYOUT_STEREO,
            AV_SAMPLE_FMT_S16,
            codec_context->sample_rate,
            codec_context->channel_layout,
            (AVSampleFormat)codec_context->format,
            codec_context->sample_rate,
            0,
            NULL);
        swr_init(swr);
        format_av_error(swr, "Something went wrong with allocating resample context");
        // dump_av_opt(swr);

        // Heap create this so we can return it out
        size_t mem_size = 0;
        const uint8_t *stream = decode(c, avFormatContext, swr, &mem_size);
        int buf_id = openal_load_sound(stream, mem_size, codec_context->sample_rate);
        *out_openal_id = buf_id;

        // Create a struct so we can return out some information
        // in a format neutral way
        // We allocate on the heap so it doesn't get deleted when we return

        swr_close(swr);
        avcodec_free_context(&c);
        avformat_free_context(avFormatContext);

        return (void*)stream;
    };



    sk_sound_data sk_load_sound_data(string filename, sk_sound_kind kind)
    {
        internal_sk_init();
        sk_sound_data result = { SGSD_UNKNOWN, nullptr } ;

        result.kind = kind;

        switch (kind)
        {
            case SGSD_SOUND_EFFECT:
            {
                result._data = Mix_LoadWAV(filename.c_str());
                break;
            }
            case SGSD_MUSIC:
            {
                result._data = read_audio_into_buffer(
                    filename.c_str(),
                    &(result.openal_id)
                );
                break;
            }

            case SGSD_UNKNOWN:
            default:
                return result;
        }

        if(result._data == nullptr)
        {
            cerr << Mix_GetError() << endl;
        }

        return result;
    }

    int sk_get_channel(sk_sound_data *sound)
    {
        if ( (!sound) || (!sound->_data) ) return -1;

        for (int i = 0; i < SG_MAX_CHANNELS; i++)
        {
            if ( _sk_sound_channels[i] == sound->_data && Mix_Playing(i) )
            {
                return i;
            }
        }
        return -1;
    }

    void sk_close_sound_data(sk_sound_data * sound )
    {
        if ( (!sound) || (!sound->_data) ) return;

        switch (sound->kind)
        {
            case SGSD_MUSIC:
                delete (const uint8_t*)sound->_data;
                sound->_data = nullptr;
                alDeleteBuffers(1, (const ALuint*)&sound->openal_id);
                break;

            case SGSD_SOUND_EFFECT:
                if (_current_music == sound)
                {
                    _current_music = nullptr;
                }
                Mix_FreeChunk(static_cast<Mix_Chunk *>(sound->_data));
                break;

            case SGSD_UNKNOWN:
                break;
        }

        sound->kind = SGSD_UNKNOWN;
        sound->_data = nullptr;
    }



    void sk_play_sound(sk_sound_data * sound, int loops, float volume)
    {
        if ( (!sound) || (!sound->_data) ) return;

        switch (sound->kind)
        {
            case SGSD_SOUND_EFFECT:
            {
                Mix_Chunk *effect = static_cast<Mix_Chunk *>(sound->_data);
                int channel = Mix_PlayChannel( -1, effect, loops);
                if (channel >= 0 && channel < SG_MAX_CHANNELS)
                {
                    Mix_Volume(channel, static_cast<int>(volume * MIX_MAX_VOLUME));
                    _sk_sound_channels[channel] = effect;   // record which channel is playing the effect
                }
                break;
            }
            case SGSD_MUSIC:
            {
                // Mix_PlayMusic(static_cast<Mix_Music *>(sound->_data), loops);
                // Mix_VolumeMusic(static_cast<int>(MIX_MAX_VOLUME * volume));

                sound->openal_source_id = openal_play_sound(
                    sound->openal_id
                ); 
                _current_music = sound;
                break;
            }
            case SGSD_UNKNOWN:
                break;
        }
    }

    float sk_sound_playing(sk_sound_data * sound)
    {
        if ( ! sound ) {
            return 0.0f;
        }

        switch (sound->kind)
        {
            case SGSD_SOUND_EFFECT:
            {
                int idx = sk_get_channel(sound);
                return ( idx >= 0 && idx < SG_MAX_CHANNELS ? 1.0f : 0.0f );
            }
            case SGSD_MUSIC:
            {
                if ( _current_music == sound && Mix_PlayingMusic() ) return 1.0f;
                break;
            }

            case SGSD_UNKNOWN:
                break;
        }

        return 0.0f;
    }

    void sk_fade_in(sk_sound_data *sound, int loops, int ms)
    {
        if ( !sound ) return;

        switch (sound->kind)
        {
            case SGSD_SOUND_EFFECT:
            {
                int channel;
                channel = Mix_FadeInChannel(-1, static_cast<Mix_Chunk *>(sound->_data), loops, ms);
                if ( channel >= 0 && channel < SG_MAX_CHANNELS )
                {
                    _sk_sound_channels[channel] = static_cast<Mix_Chunk *>(sound->_data);
                }
                break;
            }

            case SGSD_MUSIC:
            {
                Mix_FadeInMusic(static_cast<Mix_Music *>(sound->_data), loops, ms);
                _current_music = sound;
                break;
            }

            case SGSD_UNKNOWN:
                break;
        }
    }

    void sk_fade_out(sk_sound_data *sound, int ms)
    {
        if ( !sound ) return;

        switch (sound->kind)
        {
            case SGSD_SOUND_EFFECT:
            {
                int channel = sk_get_channel(sound);
                Mix_FadeOutChannel(channel, ms);
                break;
            }

            case SGSD_MUSIC:
            {
                if ( _current_music == sound )
                {
                    Mix_FadeOutMusic(ms);
                    _current_music = nullptr;
                }
                break;
            }

            case SGSD_UNKNOWN:
                break;
        }
    }

    void sk_fade_all_sound_effects_out(int ms)
    {
        internal_sk_init();
        Mix_FadeOutChannel(-1, ms);
    }

    void sk_fade_music_out(int ms)
    {
        internal_sk_init();
        Mix_FadeOutMusic(ms);
        _current_music = nullptr;
    }

    void sk_set_music_vol(float vol)
    {
        internal_sk_init();
        Mix_VolumeMusic( static_cast<int>(MIX_MAX_VOLUME * vol) );
    }

    float sk_music_vol()
    {
        internal_sk_init();
        return Mix_VolumeMusic(-1) / static_cast<float>(MIX_MAX_VOLUME);
    }

    float sk_sound_volume(sk_sound_data *sound)
    {
        if ( ! sound ) return 0.0f;

        switch (sound->kind)
        {
            case SGSD_MUSIC:
                if ( _current_music == sound ) return sk_music_vol();
                break;
            case SGSD_SOUND_EFFECT:
                return Mix_VolumeChunk(static_cast<Mix_Chunk *>(sound->_data), -1) / static_cast<float>(MIX_MAX_VOLUME);
            case SGSD_UNKNOWN:
                break;
        }

        return 0.0f;
    }

    void sk_set_sound_volume(sk_sound_data *sound, float vol)
    {
        if ( !sound ) return;

        switch (sound->kind)
        {
            case SGSD_MUSIC:
                if ( _current_music == sound )
                    sk_set_music_vol(vol);
                break;
                
            case SGSD_SOUND_EFFECT:
                Mix_VolumeChunk(static_cast<Mix_Chunk *>(sound->_data), static_cast<int>(vol * MIX_MAX_VOLUME));
                break;
                
            case SGSD_UNKNOWN:
                break;
        }
    }
    
    void sk_pause_music()
    {
        internal_sk_init();
        Mix_PauseMusic();
    }
    
    void sk_resume_music()
    {
        internal_sk_init();
        if ( Mix_PausedMusic() )
        {
            Mix_ResumeMusic();
        }
    }
    
    void sk_stop_music()
    {
        internal_sk_init();
        Mix_HaltMusic();
    }
    
    void sk_stop_sound(sk_sound_data *sound)
    {
        if ( ! sound ) return;
        
        switch (sound->kind)
        {
            case SGSD_MUSIC:
                if ( _current_music == sound ) sk_stop_music();
                break;
                
            case SGSD_SOUND_EFFECT:
            {
                for (int i = 0; i < SG_MAX_CHANNELS; i++)
                {
                    if ( _sk_sound_channels[i] == sound->_data )
                    {
                        Mix_HaltChannel(i);
                    }
                }
                break;
            }
                
            case SGSD_UNKNOWN:
                break;
        }
    }
    
    bool sk_music_playing()
    {
        internal_sk_init();
        if ( Mix_PlayingMusic() ) {
            return true;
        }
        else
        {
            return false;
        }
    }
    
    sk_sound_data * sk_current_music()
    {
        return _current_music;
    }
}