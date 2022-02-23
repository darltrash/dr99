#include "SDL2/SDL_audio.h"
#include "log.h"
#include "math.h"

unsigned int audio_pos; /* which sample we are up to */
float audio_frequency; /* audio frequency in cycles per sample */
float audio_volume; /* audio volume, 0 - ~32000 */

void fill_audio(void *udata, Uint8 *stream, int len) {
    len /= 2; 
    int i;
    Sint16* buf = (Sint16*)stream;

    for(i = 0; i < len; i++) { 
        buf[i] = audio_volume * sin(2 * 3.1415 * audio_pos * audio_frequency);
        audio_pos++;
    }
};

int au_init() {    
    SDL_AudioSpec want;
    SDL_AudioSpec have;
    want.freq = 44100;
    want.format = AUDIO_S16;
    want.channels = 1;
    want.samples = 4096;
    want.callback = fill_audio;
    want.userdata = NULL;

    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
    if (dev == 0) {
        log_fatal("Couldn't open audio: %s\n", SDL_GetError());
        return 1;
    }

    audio_pos = 0;
    audio_frequency = 1.0 * 216 / have.freq;
    audio_volume = 2000; 

    SDL_PauseAudioDevice(dev, 0);

    return 0;
}