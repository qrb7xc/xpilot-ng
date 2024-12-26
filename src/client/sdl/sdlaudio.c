/*
 * XPilotNG/SDL, an SDL/OpenGL XPilot client.
 *
 * Copyright (C) 2024 Jens Krüger <qrb7xc@pobox.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
 * SDL3 audio driver.
 */
#if 1
#include "xpclient.h"
#include <stdbool.h>

#include <SDL3/SDL_audio.h>

#define MAX_SOUNDS 16
#define VOL_THRESHOLD 10

// a sound sample (loaded from an audio file)
typedef struct {
    int             type;
    SDL_AudioSpec   spec;
    Uint8           *wav_data;
    Uint32          wav_data_len;
    float           gain;
    bool            loop;
} sample_t;

// a sound connects an audio stream with a sample and a volume
typedef struct sound {
    SDL_AudioStream *stream;
    sample_t        *sample;
    int             volume;
    long            updated; // looping
    struct sound    *next;
} sound_t;

static sound_t *ring; // a ring of available sound slots
static sound_t *looping; // a ring of looping sounds
static sound_t soundinfo[MAX_SOUNDS]; // all sounds

static void sample_free(sample_t *sample)
{
    if (sample) {
        xpinfo("sample_free %i\n", sample->type);
        SDL_free(sample->wav_data);
	free(sample);
    }
}

// for example: thrust.wav,0.05,1
static void sample_parse_info(char *filename, sample_t *sample)
{
    char *token;
    
    sample->gain = 1.0;
    sample->loop = 0;

    strtok(filename, ",");
    if (!(token = strtok(NULL, ","))) return;
    sample->gain = atof(token);
    if (!(token = strtok(NULL, ","))) return;
    sample->loop = atoi(token);
}

static sample_t *sample_load(char *filename, int type)
{
    sample_t  *sample;

    if (!(sample = (sample_t*)malloc(sizeof(sample_t)))) {
	error("failed to allocate memory for a sample");
	return NULL;
    }
    sample->type = type;
    sample->wav_data = NULL;
    sample->wav_data_len = 0;

    sample_parse_info(filename, sample);

    // TODO: loading the WAV is syncronous and can hang the client
    if (!SDL_LoadWAV(filename, &sample->spec, &sample->wav_data, &sample->wav_data_len)) {
        error("Couldn't load .wav file: %s", SDL_GetError());
        sample_free(sample);
        return NULL;
    }

    return sample;
}

int audioDeviceInit(char *display)
{
    // TODO: open the audio device once, then bind all audio streams
    for (int i = 0; i < MAX_SOUNDS; i++) {
        SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL, NULL, NULL);
        if (stream == NULL) {
            error("failed to create audio stream %i, error: %s", i, SDL_GetError());
        }
        SDL_ResumeAudioStreamDevice(stream); // unpause

	soundinfo[i].stream = stream;
	soundinfo[i].sample = NULL;
	soundinfo[i].volume = 0;
	soundinfo[i].updated = 0;
	soundinfo[i].next = &soundinfo[(i + 1) % MAX_SOUNDS];
    }
    ring = soundinfo;
    looping = NULL;

    return 0;
}

static void playSample(SDL_AudioStream * stream, sample_t *sample, int volume, bool isLooping)
{
    if (!isLooping) {
        // Stop the current audio on the stream and reset the stream format
        SDL_ClearAudioStream(stream);
        if (!SDL_SetAudioStreamFormat(stream, &sample->spec, NULL)) {
            error("failed to set audio stream spec %s\n", SDL_GetError());
        }
    }
    if (!SDL_SetAudioStreamGain(stream, sample->gain * volume / 100.0f))
    {
        error("failed to set audio stream gain %s\n", SDL_GetError());
    }
    if (SDL_GetAudioStreamAvailable(stream) < (int)sample->wav_data_len) {
        /* feed more data to the stream. It will queue at the end, and trickle out as the hardware needs more data. */
        SDL_PutAudioStreamData(stream, sample->wav_data, sample->wav_data_len);
    }

}

void audioDevicePlay(char *filename, int type, int volume, void **priv)
{
    sound_t  *iter, *next;
    sample_t *sample = (sample_t *)(*priv);

    if (!sample) {
	sample = sample_load(filename, type);
	if (!sample) {
	    error("failed to load sample %s (%i)\n", filename, type);
	    return;
	}
        xpinfo("Loaded audio sample %i: %s", type, filename);
	*priv = sample;
    }


    /* if the sample is a looping one, first try to find a matching
     * sound from the list of looping sounds already playing. */
    if (sample->loop) {
	for (iter = looping; iter; iter = iter->next) {
	    if (iter->sample == sample
		&& ABS(iter->volume - volume) < VOL_THRESHOLD) {
		iter->volume = volume;
		iter->updated = loops;
                playSample(iter->stream, sample, volume, true);
		return;
	    }
	}
    }

    if (ring->next == ring) 
	return; /* only one sound left in the ring */

    /* Pick the next sound from the ring and play the sample with it.
     * If it is a looping sound move it away from the ring to the
     * looping list. Else move it to the end of the ring. */
    next = ring->next;    
    if (sample->loop) {
	ring->next = next->next;
	next->next = looping;
	looping = next;
    } else {
	ring = next;
    }
    
    next->sample  = sample;
    next->volume  = volume;
    next->updated = loops; // this is frame loops from the server

    SDL_AudioStream *stream = next->stream;
    playSample(stream, sample, volume, false);
}

void audioDeviceEvents(void)
{
    // never called for SDL client
}

void audioDeviceUpdate(void)
{
    sound_t *iter, *prev, *tmp;

    /* Go through the looping list and stop all those sounds
     * that haven't been updated during this frame. The stopped
     * sounds are moved back to the ring. */
    for (prev = NULL, iter = looping; iter;) {
	if (iter->updated < loops - 1) {
            SDL_ClearAudioStream(iter->stream);
	    if (prev) {
                prev->next = iter->next;
            }
	    else {
                looping = iter->next;
            }
	    tmp = iter;
	    iter = iter->next;
	    tmp->next = ring->next;
	    ring->next = tmp;
	} else {
	    prev = iter;
	    iter = iter->next;
	}
    }
}

void audioDeviceFree(void *priv) 
{
    if (priv) {
	sample_free((sample_t *)priv);
    }
}

void audioDeviceClose() 
{
    for (int i = 0; i < MAX_SOUNDS; i++) {
        if (soundinfo[i].stream) {
            SDL_DestroyAudioStream(soundinfo[i].stream);
        }
    }

    SDL_CloseAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);
}
#endif
