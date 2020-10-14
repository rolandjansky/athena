/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//********************************************************************
// Filename : TileHit.cxx
// Author   : UC-ATLAS TileCal Group
// Created  : April, 2002
//
// DESCRIPTION:
//    A TileHit is from simulation(G3), and represents the
//    total energy visible by one PMT
//
// HISTORY:
//    02Feb03: Inherit from TileSimData now
//    09Nov03: Inheritace removed, class moved to TileSimEvent
//    
// BUGS:
//
// *******************************************************************

#include "TileSimEvent/TileHit.h"
#include "TileSimEvent/TileSimHit.h"
#include "TileConditions/TileCablingService.h"
#include "CaloIdentifier/TileID.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>


namespace {
// get cabling
const TileCablingService * const s_cabling = TileCablingService::getInstance();
}

TileHit::TileHit(const Identifier & id, float energy, float time)
    : m_pmt_id( id )
    , m_energy(1)
    , m_time(1)
{
  m_energy[0] = energy;
  m_time[0] = time;
}

TileHit::TileHit(const Identifier & id, float energy, float time, float delta)
    : m_pmt_id( id )
    , m_energy(1)
    , m_time(1)
{
  m_energy[0] = energy;
  if(delta>0){ 
    m_time[0] = delta*nearbyint(time/delta);
  }
  else{
    // ignore time completely if delta is zero
    m_time[0]=0.0;
  }
}

TileHit::TileHit( const TileSimHit * hitInp ) 
    : m_pmt_id( hitInp->identify() )
{
  int size = hitInp->size();
  m_energy.reserve(size);
  m_time.reserve(size);
  for (int i=0; i<size; ++i) {
    m_energy.push_back((float)hitInp->energy(i));
    m_time.push_back((float)hitInp->time(i));
  }
}
  
int TileHit::add(float energy, float time)
{
  m_energy.push_back(energy);
  m_time.push_back(time);
  return 1;
}

int TileHit::add(float energy, float time, float delta)
{
  if(delta>0) {
    int size=m_time.size();
    for (int i=0; i<size; ++i) {
      float dt = time-m_time[i];
      if (fabs(dt)<delta/2.) {
        m_energy[i]+=energy;
        //if ( (m_energy[i] += energy) > 0.0) 
        //  m_time[i] += dt * energy / m_energy[i];
        return 0;
      }
    }
  
    m_energy.push_back(energy);
    m_time.push_back(delta*nearbyint(time/delta));

  } else{
    // ignore time completely if delta is zero
    m_energy[0] += energy;
    return 0;
  }

  return 1;
}

int TileHit::add(const TileHit * hitInp, float delta)
{
  int result=0;
  int sizeInp = hitInp->size();
  for (int i=0; i<sizeInp; ++i) {
    result += add(hitInp->energy(i),hitInp->time(i),delta);
  }
  return result;
}

void TileHit::scale(float coeff)
{
  int i,s;
  s = m_energy.size();
  for(i=0; i<s; i++)
    m_energy[i] = coeff*m_energy[i];
}

HWIdentifier TileHit::pmt_HWID(void)   const {
    return s_cabling->s2h_channel_id(m_pmt_id);
}

Identifier TileHit::tt_ID(void)   const {
    return s_cabling->pmt2tt_id(m_pmt_id);
}

Identifier TileHit::mt_ID(void)   const {
    return s_cabling->pmt2mt_id(m_pmt_id);
}

int TileHit::frag_ID(void)   const {
    return s_cabling->frag(m_pmt_id);
}

void TileHit::print() const
{
    std::cout << (std::string) (*this) << std::endl;
}

TileHit::operator std::string() const
{
    const int MAX_BUF = 1024;
    char buf[MAX_BUF];
    std::ostringstream text(buf);

    text << whoami();
    text << " Id = " + s_cabling->getTileID()->to_string(m_pmt_id,-1); // pmt_id => -1 level from adc_id
    print_to_stream(m_energy, " ener =", text);
    print_to_stream(m_time,   " time =", text);
    text << '\0';
    return text.str();
}

void TileHit::print_to_stream ( const std::vector<float>& val,
                                const std::string & label,
                                std::ostringstream & text)
{
    text << label;

    std::vector<float>::const_iterator it1=val.begin();
    std::vector<float>::const_iterator it2=val.end();

    for ( ; it1!=it2; ++it1) text << " " << (*it1);
}
