/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkSurfaces/Surface.h"
#include <cassert>
#include <string>

// typedef Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes nmetypes;

Trk::MaterialEffectsOnTrack::MaterialEffectsOnTrack():
        Trk::MaterialEffectsBase(),
        m_scatteringAngles(nullptr),
        m_energyLoss(nullptr)
{}

Trk::MaterialEffectsOnTrack::MaterialEffectsOnTrack(
        const double                 tInX0,
        const Trk::ScatteringAngles* scat,
        const Trk::EnergyLoss*       eloss,
        const Trk::Surface&          sf,
        const std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes> typePattern) 
  :   Trk::MaterialEffectsBase(tInX0, sf,
                               (typePattern==0 ? 3 : typePattern) ),
        m_scatteringAngles(scat), // take ownership
        m_energyLoss(eloss)       // take ownership
{ 
  assert (m_scatteringAngles!=0 || m_energyLoss!=0);
}

Trk::MaterialEffectsOnTrack::MaterialEffectsOnTrack(
        const double                 tInX0,
        const Trk::ScatteringAngles* scat, 
        const Trk::Surface&          sf,
        const std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes>  typePattern) 
  :   Trk::MaterialEffectsBase(tInX0, sf,
                               (typePattern==0?1<<static_cast<int>(Trk::MaterialEffectsBase::ScatteringEffects):typePattern) ),
        m_scatteringAngles(scat), // take ownership
        m_energyLoss(nullptr)
{
  assert (m_scatteringAngles!=0);
}

Trk::MaterialEffectsOnTrack::MaterialEffectsOnTrack(
        const double                 tInX0,
        const Trk::EnergyLoss*       eloss, 
        const Trk::Surface&          sf,
        const std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes>  typePattern )
  :   Trk::MaterialEffectsBase(tInX0, sf,
                               (typePattern==0 ? 1<<static_cast<int>(Trk::MaterialEffectsBase::EnergyLossEffects) : typePattern)),
        m_scatteringAngles(nullptr),
        m_energyLoss(eloss)       // take ownership
{
  assert (m_energyLoss!=0);
}

Trk::MaterialEffectsOnTrack::MaterialEffectsOnTrack(
        const double tInX0,
        const Trk::Surface& sf)
  :   Trk::MaterialEffectsBase(tInX0, sf,
                               1<<static_cast<int>(Trk::MaterialEffectsBase::MaterialThickness)),
      m_scatteringAngles(nullptr),
      m_energyLoss(nullptr)
{
  assert (tInX0 > 0.0);
}
                        
        
Trk::MaterialEffectsOnTrack::MaterialEffectsOnTrack(const Trk::MaterialEffectsOnTrack& emef) :
        Trk::MaterialEffectsBase(emef),
        m_scatteringAngles(emef.m_scatteringAngles ? 
            new Trk::ScatteringAngles(*emef.m_scatteringAngles) : nullptr ),
        m_energyLoss(emef.m_energyLoss ?
            emef.m_energyLoss->clone() : nullptr )
{}

Trk::MaterialEffectsOnTrack& Trk::MaterialEffectsOnTrack::operator= (const Trk::MaterialEffectsOnTrack& rhs)
{
  if (this!=&rhs) {
    Trk::MaterialEffectsBase::operator=(rhs);

    if (m_scatteringAngles) { delete m_scatteringAngles;
}
    m_scatteringAngles = rhs.m_scatteringAngles ?
      new Trk::ScatteringAngles(*rhs.m_scatteringAngles) : nullptr ;

    if (m_energyLoss) { delete m_energyLoss;
}
    m_energyLoss = rhs.m_energyLoss ?
      rhs.m_energyLoss->clone() : nullptr ;
  }
  return *this;
}


Trk::MaterialEffectsOnTrack::~MaterialEffectsOnTrack()
{
  if (m_scatteringAngles) { delete m_scatteringAngles;
}
  if (m_energyLoss) { delete m_energyLoss;
}
}

// Overload of << operator for MsgStream for debug output
MsgStream& Trk::MaterialEffectsOnTrack::dump ( MsgStream& sl ) const
{
  sl << "MaterialEffectsOnTrack based on" << endmsg;
  Trk::MaterialEffectsBase::dump(sl);
  sl <<"MaterialEffects, Scatterer? : "
     << (scatteringAngles()?"yes:":"none")<<endmsg;
  if (scatteringAngles()) { sl << *scatteringAngles() << endmsg;
}
  sl <<"MaterialEffects, E-loss ?   : "
     << (energyLoss()?"yes:":"none") <<endmsg;
  if (energyLoss()) { sl << *energyLoss() << endmsg;
}
  return sl; 
}
//Overload of << operator for std::ostream for debug output
std::ostream& Trk::MaterialEffectsOnTrack::dump ( std::ostream& sl ) const
{
  sl << "MaterialEffectsOnTrack based on" << std::endl;
  Trk::MaterialEffectsBase::dump(sl);
  sl <<"MaterialEffects, Scatterer? : "
     << (scatteringAngles()?"yes:":"none")<<std::endl;
  if (scatteringAngles()) { sl << *scatteringAngles() << std::endl;
}
  sl <<"MaterialEffects, E-loss ?   : "
     << (energyLoss()?"yes:":"none") <<std::endl;
  if (energyLoss()) { sl << *energyLoss() << std::endl;
}
  return sl; 
}




