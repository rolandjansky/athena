/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/GenericMuonSimHit.h"
#include <sstream>
/*******************************************************************************/
GenericMuonSimHit::GenericMuonSimHit():
  m_GenericId(0xffff),
  m_globalTime(0.), 
  m_globalpreTime(0.), 
  m_particleEncoding(0),// PDG_ID  
  m_kineticEnergy(-9999.),// energy of the particle
  m_depositEnergy(-9999.),//Geant4 deposited energy 
  m_StepLength(-9999.),//total lenght of particle
  m_partLink()
{
}
/*******************************************************************************/
//virtual destructor required by Pool
GenericMuonSimHit::~GenericMuonSimHit()
{
}
/*******************************************************************************/
GenericMuonSimHit::GenericMuonSimHit(HitID id, double time, double pretime,
                                     const Amg::Vector3D& position,
                                     const Amg::Vector3D& lposition,
                                     const Amg::Vector3D& preposition,
                                     const Amg::Vector3D& lpreposition, 
                                     const int particleEncoding,
                                     const double kineticEnergy,
                                     const Amg::Vector3D& direction,
                                     const double depositEnergy,
                                     const double StepLength,
                                     const int trackNumber)
  : m_GenericId(id), m_globalTime(time), m_globalpreTime(pretime)
  , m_globalPosition(position)
  , m_localPosition(lposition)
  , m_globalPrePosition(preposition)
  , m_localPrePosition(lpreposition)
  , m_particleEncoding(particleEncoding)
  , m_kineticEnergy(kineticEnergy)
  , m_globalDirection(direction)
  , m_depositEnergy(depositEnergy)
  , m_StepLength(StepLength)
  , m_partLink(trackNumber)
{
}

GenericMuonSimHit::GenericMuonSimHit(HitID id, double time, double pretime,
                                     const Amg::Vector3D& position,
                                     const Amg::Vector3D& lposition,
                                     const Amg::Vector3D& preposition,
                                     const Amg::Vector3D& lpreposition, 
                                     const int particleEncoding,
                                     const double kineticEnergy,
                                     const Amg::Vector3D& direction,
                                     const double depositEnergy,
                                     const double StepLength,
                                     const HepMcParticleLink& hmpl)
  : m_GenericId(id), m_globalTime(time), m_globalpreTime(pretime)
  , m_globalPosition(position)
  , m_localPosition(lposition)
  , m_globalPrePosition(preposition)
  , m_localPrePosition(lpreposition)
  , m_particleEncoding(particleEncoding)
  , m_kineticEnergy(kineticEnergy)
  , m_globalDirection(direction)
  , m_depositEnergy(depositEnergy)
  , m_StepLength(StepLength)
  , m_partLink(hmpl)
{
}
/*******************************************************************************/
std::string GenericMuonSimHit::print() const {
    
  std::stringstream ss;

  ss << "-------------------------------------------------------------" << std::endl;
  ss << "GenericMuonSimHit:"<<std::endl;
  ss << "  identifier: "  << m_GenericId;
  ss << "  t (pre) : "  << m_globalpreTime;
  ss << "  t: "  << m_globalTime;
  ss << "  x (pre) : (" << m_globalPrePosition.x()
     << ","      << m_globalPrePosition.y()
     << ","      << m_globalPrePosition.z()
     << ")";
  ss << "  x : (" << m_globalPosition.x()
     << ","      << m_globalPosition.y()
     << ","      << m_globalPosition.z()
     << ")"<<std::endl;
  ss << " pdgId: "<<m_particleEncoding<<" kineticEnergy: "<<m_kineticEnergy<<std::endl;

  return ss.str();
}
/*******************************************************************************/
int GenericMuonSimHit::trackNumber() const
{ return m_partLink.barcode(); }
/*******************************************************************************/
