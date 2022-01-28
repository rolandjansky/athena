/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/sTGCSimHit.h"
#include <sstream>
/*******************************************************************************/
sTGCSimHit::sTGCSimHit():
  m_sTGCId(0xffff),
  m_globalTime(0.),
  m_particleEncoding(0),// PDG_ID
  m_depositEnergy(-9999.),//Geant4 deposited energy
  m_partLink()
{
}
/*******************************************************************************/
//virtual destructor required by Pool
sTGCSimHit::~sTGCSimHit()
{
}
/*******************************************************************************/
sTGCSimHit::sTGCSimHit(HitID id, double time, 
                                     const Amg::Vector3D& position,
                                     const int particleEncoding,
                                     const Amg::Vector3D& direction,
                                     const double depositEnergy,
                                     const int trackNumber)
  : m_sTGCId(id), m_globalTime(time)
  , m_globalPosition(position)
  , m_particleEncoding(particleEncoding)
  , m_globalDirection(direction)
  , m_depositEnergy(depositEnergy)
  , m_partLink(trackNumber)
{
}

sTGCSimHit::sTGCSimHit(HitID id, double time,
                                     const Amg::Vector3D& position,
                                     const int particleEncoding,
                                     const Amg::Vector3D& direction,
                                     const double depositEnergy,
                                     const HepMcParticleLink& hmpl)
  : m_sTGCId(id), m_globalTime(time)
  , m_globalPosition(position)
  , m_particleEncoding(particleEncoding)
  , m_globalDirection(direction)
  , m_depositEnergy(depositEnergy)
  , m_partLink(hmpl)
{
}

/*******************************************************************************/
std::string sTGCSimHit::print() const {

  std::stringstream ss;

  ss << "-------------------------------------------------------------" << std::endl;
  ss << "sTGCSimHit:"<<std::endl;
  ss << "  identifier: "  << m_sTGCId;
  ss << "  t: "  << m_globalTime;
  ss << "  x : (" << m_globalPosition.x()
     << ","      << m_globalPosition.y()
     << ","      << m_globalPosition.z()
     << ")"<<std::endl;
  ss << " pdgId: "<<m_particleEncoding<<std::endl;

  return ss.str();
}
/*******************************************************************************/
int sTGCSimHit::trackNumber() const
{ return m_partLink.barcode(); }
/*******************************************************************************/
