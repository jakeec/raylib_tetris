#ifndef SOUND_H
#define SOUND_H

#include <math.h>
#include <raylib.h>

Wave GenBwoopWave(float startFreq, float endFreq, float duration,
                  int sampleRate) {
  int sampleCount = (int)(duration * sampleRate);
  short *samples  = (short *)MemAlloc(sampleCount * sizeof(short));

  for (int i = 0; i < sampleCount; i++) {
    float t         = (float)i / sampleRate;
    float freq      = startFreq + (endFreq - startFreq) * (t / duration);
    float value     = sinf(2.0f * PI * freq * t);
    float amplitude = 1.0f - (t / duration);
    samples[i]      = (short)(value * amplitude * 16000);
  }

  Wave wave = {.frameCount = sampleCount,
               .sampleRate = sampleRate,
               .sampleSize = 16,
               .channels   = 1,
               .data       = samples};

  return wave;
}

#endif  // SOUND_H