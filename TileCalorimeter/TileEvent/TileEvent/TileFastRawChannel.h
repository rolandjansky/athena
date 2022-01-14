/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEEVENT_TILEFASTRAWCHANNEL_H
#define TILEEVENT_TILEFASTRAWCHANNEL_H

/*
 Author : Denis Oliveira Damazio
 Date : 01/18/2006
 Very simple class to hold, during HLT operation,
 the channel output before combining into cells.
 */

#include "Identifier/HWIdentifier.h"

class TileFastRawChannel {

  public:
    TileFastRawChannel()
      : m_frag(-1)
      , m_channel(0)
      , m_adc(0)
      , m_amplitude(0.0F)
      , m_time(0.0F)
      , m_quality(0.0) {
    }

    TileFastRawChannel (unsigned int channel,
                        unsigned int adc,
                        float amplitude,
                        float time,
                        float quality)
      : m_frag(-1)
      , m_channel(channel)
      , m_adc(adc)
      , m_amplitude(amplitude)
      , m_time(time)
      , m_quality(quality)
    {
    }

    TileFastRawChannel (int frag,
                        unsigned int channel,
                        unsigned int adc,
                        float amplitude,
                        float time,
                        float quality)
      : m_frag(frag)
      , m_channel(channel)
      , m_adc(adc)
      , m_amplitude(amplitude)
      , m_time(time)
      , m_quality(quality)
    {
    }

    ~TileFastRawChannel() {
    }

    inline unsigned int frag(void) const {
      return m_frag;
    }

    inline unsigned int channel(void) const {
      return m_channel;
    }

    inline unsigned int adc(void) const {
      return m_adc;
    }

    inline float amplitude(void) const {
      return m_amplitude;
    }

    inline float time(void) const {
      return m_time;
    }

    inline float quality(void) const {
      return m_quality;
    }

    inline void set(unsigned int channel, unsigned int adc, float amplitude, float time,
        float quality) {
      m_frag = -1;
      m_channel = channel;
      m_adc = adc;
      m_amplitude = amplitude;
      m_time = time;
      m_quality = quality;
    }

    inline void set(int frag, unsigned int channel, unsigned int adc, float amplitude,
        float time, float quality) {
      m_frag = frag;
      m_channel = channel;
      m_adc = adc;
      m_amplitude = amplitude;
      m_time = time;
      m_quality = quality;
    }

  private:
    int m_frag;
    unsigned int m_channel;
    unsigned int m_adc;
    float m_amplitude;
    float m_time;
    float m_quality;
};

#endif

