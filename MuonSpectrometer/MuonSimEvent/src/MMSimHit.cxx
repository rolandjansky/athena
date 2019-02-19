/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/MMSimHit.h"
#include <sstream>
/*******************************************************************************/
MMSimHit::MMSimHit():
  m_MMId(0xffff),
  m_globalTime(0.),
  m_particleEncoding(0),// PDG_ID
  m_kineticEnergy(-9999.),// energy of the particle
  m_depositEnergy(-9999.),//Geant4 deposited energy
  m_partLink()
{
}
/*******************************************************************************/
//virtual destructor required by Pool
MMSimHit::~MMSimHit()
{
}
/*******************************************************************************/
MMSimHit::MMSimHit(HitID id, double time, 
                                     const Amg::Vector3D& position,
                                     const int particleEncoding,
                                     const double kineticEnergy,
                                     const Amg::Vector3D& direction,
                                     const double depositEnergy,
                                     const int trackNumber)
  : m_MMId(id), m_globalTime(time) 
  , m_globalPosition(position)
  , m_particleEncoding(particleEncoding)
  , m_kineticEnergy(kineticEnergy)
  , m_globalDirection(direction)
  , m_depositEnergy(depositEnergy)
  , m_partLink(trackNumber)
{
}

MMSimHit::MMSimHit(HitID id, double time,
                                     const Amg::Vector3D& position,
                                     const int particleEncoding,
                                     const double kineticEnergy,
                                     const Amg::Vector3D& direction,
                                     const double depositEnergy,
                                     const HepMcParticleLink hmpl)
  : m_MMId(id), m_globalTime(time)
  , m_globalPosition(position)
  , m_particleEncoding(particleEncoding)
  , m_kineticEnergy(kineticEnergy)
  , m_globalDirection(direction)
  , m_depositEnergy(depositEnergy)
  , m_partLink(hmpl)
{
}
/*******************************************************************************/
std::string MMSimHit::print() const {

  std::stringstream ss;

  ss << "-------------------------------------------------------------" << std::endl;
  ss << "MMSimHit:"<<std::endl;
  ss << "  identifier: "  << m_MMId;
  ss << "  t: "  << m_globalTime;
  ss << "  x : (" << m_globalPosition.x()
     << ","      << m_globalPosition.y()
     << ","      << m_globalPosition.z()
     << ")"<<std::endl;
  ss << " pdgId: "<<m_particleEncoding<<" kineticEnergy: "<<m_kineticEnergy<<std::endl;

  return ss.str();
}
/*******************************************************************************/
int MMSimHit::trackNumber() const
{ return m_partLink.barcode(); }
/*******************************************************************************/
