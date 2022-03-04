/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileRawChannel.cxx
//  Author   : Ed Frank, Ambreesh Gupta, Frank Merritt
//  Created  : Jan, 2001
//
//  DESCRIPTION:
//     Implementation comments only.  Class level comments go in .h file.
//
//  HISTORY:
//    02Mar02: Created to replace TileRawCell
//
//  BUGS:
//    
//    
//
//*****************************************************************************

#include "TileEvent/TileRawChannel.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

TileRawChannel::TileRawChannel( const Identifier& id,
				float amplitude, float time, float quality, float ped )
  : TileRawData( id )
  , m_amplitude(1,amplitude)
  , m_time(1,time)
  , m_quality(1,quality)
  , m_pedestal(ped)
{
}

TileRawChannel::TileRawChannel( const HWIdentifier& HWid,
				float amplitude, float time, float quality, float ped )
  : TileRawData( HWid )
  , m_amplitude(1,amplitude)
  , m_time(1,time)
  , m_quality(1,quality)
  , m_pedestal(ped)
{
}

TileRawChannel::TileRawChannel(const HWIdentifier& HWid, 
                               std::vector<float>&& amplitude,
                               std::vector<float>&& time,
                               std::vector<float>&& quality,
                               float ped /*= 0.0*/)
  : TileRawData( HWid ),
    m_amplitude (std::move(amplitude)),
    m_time (std::move(time)),
    m_quality (std::move(quality)),
    m_pedestal (ped)
{
}

int TileRawChannel::add(float amplitude, float time, float quality) {
  m_amplitude.push_back(amplitude);
  m_time.push_back(time);
  m_quality.push_back(quality);
  return m_amplitude.size();
}

void TileRawChannel::scaleAmplitude(float scale) {
    using std::placeholders::_1;
    std::transform(m_amplitude.begin(), m_amplitude.end(), 
                   m_amplitude.begin(),
                   std::bind(std::multiplies<float>(),scale,_1));
}

void TileRawChannel::insertTime(float time) {
  m_time.insert(m_time.begin(), time); // put new time in front
}

int TileRawChannel::insert(float amplitude, float time, float quality) {
  m_amplitude.insert(m_amplitude.begin(), amplitude);
  m_time.insert(m_time.begin(), time);
  m_quality.insert(m_quality.begin(), quality);
  return m_amplitude.size();
}

void TileRawChannel::print() const {
  std::cout << (std::string) (*this) << std::endl;
}

TileRawChannel::operator std::string() const {
  std::ostringstream text(std::ostringstream::out);

  text << TileRawData::operator std::string();
  print_to_stream(m_amplitude, " ampl =", text);
  print_to_stream(m_time, " time =", text);
  print_to_stream(m_quality, " qual =", text);

  return text.str();
}
