/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//********************************************************************
// Filename : TileSimHit.cxx
// Author   : Alexander Solodkov
// Created  : June, 2009
//
// DESCRIPTION:
//    A TileSimHit represents the total energy deposited in scintillator
//    which is visible by one PMT
//    Energy and time is kept as double 
//
// HISTORY:
//    04Jun09: Copied from TileHit class
//    
// BUGS:
//
// *******************************************************************

#include "TileSimEvent/TileSimHit.h"
#include <cmath>


TileSimHit::TileSimHit(Identifier & id, double energy, double time, double delta)
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

int TileSimHit::add(double energy, double time, double delta)
{
  if(delta>0) {
    int size=m_time.size();
    for (int i=0; i<size; ++i) {
      double dt = time-m_time[i];
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
