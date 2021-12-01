/*******************************************************************************
*
*   Stuff for sound processing
*
*******************************************************************************/

#include <stdio.h>
#include <SDL2/SDL.h> // ans Uint8, etc..
#include "audio.h"

#define BYTES_PER_SAMP 4        // assumes 16 bit 2 channel input

/******************************************************************************/
void MyAudioCallback(void* userdata, Uint8* stream, int streamLength)
{
  AudioData* audio = (AudioData*)userdata;

  if(audio->length == 0)
  {
    return;
  }

  Uint32 length = (Uint32)streamLength;
  length = (length > audio->length ? audio->length : length);

  SDL_memcpy(stream, audio->pos, length);

  audio->pos += length;
  audio->length -= length;

  // printf("reloading buffer\n");
	// 4096 samples per load (16 bit, 2 channel)
	// 44.1 samps per millisecond
	// about 93 milliseconds per load
}

/******************************************************************************/
int runAudio(GPU_Palette* P1, CPUAnimBitmap* A1, char* fileName)
{
	SDL_AudioSpec wavSpec;	// audio file header info (channels, samp rate, etc)
  Uint8* wavPtr;					// address of start of audio data
  Uint32 wavLength;				// length of music in file sample

	if(SDL_Init(SDL_INIT_AUDIO) < 0){
		printf("problem initializing SDL audio\n");
		return 1;
		}

  if(SDL_LoadWAV(fileName, &wavSpec, &wavPtr, &wavLength) == NULL){
    printf("error: file couldn't be found or isn't a proper .wav file\n");
    return 1;
  	}

	if (wavSpec.freq != 44100){
		printf(".wav frequency not 44100!\n");
		return 1;
		}

	// https://www.rubydoc.info/gems/sdl2_ffi/0.0.6/SDL2/Audio
	if (wavSpec.format != 0x8010){
		printf(".wav format is not S16LSB (signed 16 bit little endian)\n");
		return 1;
		}

	if (wavSpec.channels != 2){
		printf(".wav not 2 channel (stereo) file\n");
		return 1;
		}

	if (wavSpec.samples != 4096){
		printf(" SDL not using 4096 buffer size??!\n");
		return 1;
		}

  AudioData audio;
  audio.pos = wavPtr;				// address of where we are starting in the file
  audio.length = wavLength;		// how much data is left

  wavSpec.callback = MyAudioCallback;
  wavSpec.userdata = &audio;

  SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);

  if(device == 0){
    printf("error: audio device not found\n");
    return 1;
  }

  // 0 to play, 1 to pause
  SDL_PauseAudioDevice(device, 0);

  while(audio.length > 0)
  {
    // here is the main deal:
		myDraw(P1, A1, wavSpec.userdata);

    SDL_Delay(100);	// in ms
  }

  SDL_CloseAudioDevice(device);
  SDL_FreeWAV(wavPtr);
  SDL_Quit();
  return 0;
}



/******************************************************************************/
float getAmp(Uint8* wavPtr, int start, int end, int offset)
{
  // accumulate distance from zero across the frame
  float acc = 0;
  for (int samp = start; samp < end; samp++){ // e.g.  0 - 1023
  		acc += fabs(Get16bitAudioSample(wavPtr, (samp*BYTES_PER_SAMP)+offset));
  	}

  return acc;
}

/******************************************************************************/
double Get16bitAudioSample(Uint8* bytebuffer, int samp)
{
    Uint16 val =  0x0;

//  assumes little endian
//    if(SDL_AUDIO_ISLITTLEENDIAN(format))
    val = (uint16_t)bytebuffer[samp] | ((uint16_t)bytebuffer[samp+1] << 8);
//    else
//        val = ((uint16_t)bytebuffer[0] << 8) | (uint16_t)bytebuffer[1];

//    if(SDL_AUDIO_ISSIGNED(format))
        return ((int16_t)val)/32768.0;

//    return val/65535.0;
}
