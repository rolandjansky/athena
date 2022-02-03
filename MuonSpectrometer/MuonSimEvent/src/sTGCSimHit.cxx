/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/sTGCSimHit.h"
#include <sstream>
/*******************************************************************************/
sTGCSimHit::sTGCSimHit():
  m_sTGCId(0xffff),
  m_globalTime(0.),
  m_particleEncoding(0),// PDG_ID
  m_depositEnergy(-9999.),//Geant4 deposited energy
  m_partLink(),
  m_kineticEnergy(-1.)
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
  , m_kineticEnergy(-1.0)
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
  , m_kineticEnergy(-1.0)
{
}

sTGCSimHit::sTGCSimHit(HitID id, double time,
                                     const Amg::Vector3D& position,
                                     const int particleEncoding,
                                     const Amg::Vector3D& direction,
                                     const double depositEnergy,
                                     const HepMcParticleLink& hmpl,
                                     const double kineticEnergy,
                                     const Amg::Vector3D& preposition)
  : m_sTGCId(id)
  , m_globalTime(static_cast<float>(time))
  , m_globalPosition(position)
  , m_particleEncoding(particleEncoding)
  , m_globalDirection(direction)
  , m_depositEnergy(static_cast<float>(depositEnergy))
  , m_partLink(hmpl)
  , m_kineticEnergy(static_cast<float>(kineticEnergy))
  , m_globalPrePosition(preposition)
{
}

/*******************************************************************************/
std::string sTGCSimHit::print() const {

  std::stringstream ss;

  ss << "-------------------------------------------------------------" << std::endl;
  ss << "sTGCSimHit:";
  ss << "  identifier: "  << m_sTGCId;
  ss << "  t: "  << m_globalTime;
  ss << "  x : (" << m_globalPosition.x()
     << ","      << m_globalPosition.y()
     << ","      << m_globalPosition.z()
     << ")";
  ss << "  pdgId: "<< m_particleEncoding;
  ss << "  KineticEnergy: " << m_kineticEnergy;
  ss << "  x(pre) : (" << m_globalPrePosition.x()
     << ","            << m_globalPrePosition.y()
     << ","            << m_globalPrePosition.z()
     << ")";
  ss << "  dir : (" << m_globalDirection.x()
     << ","         << m_globalDirection.y()
     << ","         << m_globalDirection.z()
     << ")";
  ss << std::endl;

  return ss.str();
}
/*******************************************************************************/
int sTGCSimHit::trackNumber() const
{ return m_partLink.barcode(); }
/*******************************************************************************/
