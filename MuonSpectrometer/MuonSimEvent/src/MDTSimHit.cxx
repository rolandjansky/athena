/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/MDTSimHit.h"


#include <sstream>
// construction/destruction

MDTSimHit::MDTSimHit() :
  m_MDTid(0xffff),
  m_globalTime(0.), 
  m_driftRadius(0.),
  m_partLink(),
  m_stepLength(0.),
  m_energyDeposit(0.),
  m_particleEncoding(0),
  m_kineticEnergy(0.)

{
}

//virtual destructor required by Pool
MDTSimHit::~MDTSimHit() {} 


MDTSimHit::MDTSimHit(HitID id, 
		     double time, 
	             double radius, 
	             const Amg::Vector3D& position)
    : m_MDTid(id), m_globalTime(time)
    , m_driftRadius(radius)
    , m_localPosition(position)
{
 m_stepLength       = -9999999.; //total lenght of particle
 m_energyDeposit    = -1.;   //Geant4 deposited energy
 m_particleEncoding =  0; // PDG_ID
 m_kineticEnergy    = -1.; // energy of the particle
}

MDTSimHit::MDTSimHit(HitID id, 
		     double time, 
	             double radius, 
	             const Amg::Vector3D& position, 
		     const int trackNumber)
    : m_MDTid(id), m_globalTime(time)
    , m_driftRadius(radius)
    , m_localPosition(position)
    , m_partLink(trackNumber)
{
 m_stepLength       = -9999999.; //total lenght of particle
 m_energyDeposit    = -1.;   //Geant4 deposited energy
 m_particleEncoding =  0; // PDG_ID
 m_kineticEnergy    = -1.; // energy of the particle
}

MDTSimHit::MDTSimHit(HitID id,
		     double time,
		     double radius, 
		     const Amg::Vector3D& position, 
		     const int trackNumber,
		     const double stepLength,
		     const double energyDeposit,
		     const int particleEncoding, 
                      const double kineticEnergy)
    : m_MDTid(id), m_globalTime(time)
    , m_driftRadius(radius)
    , m_localPosition(position)
    , m_partLink(trackNumber)
    , m_stepLength(static_cast<float>(stepLength))
    , m_energyDeposit(static_cast<float>(energyDeposit))
    , m_particleEncoding(particleEncoding)
    , m_kineticEnergy(static_cast<float>(kineticEnergy))
{
}

MDTSimHit::MDTSimHit(HitID id,
		     double time,
		     double radius, 
		     const Amg::Vector3D& position, 
		     const HepMcParticleLink& hmpl,
		     const double stepLength,
		     const double energyDeposit,
		     const int particleEncoding, 
                      const double kineticEnergy)
    : m_MDTid(id), m_globalTime(time)
    , m_driftRadius(radius)
    , m_localPosition(position)
    , m_partLink(hmpl)
    , m_stepLength(static_cast<float>(stepLength))
    , m_energyDeposit(static_cast<float>(energyDeposit))
    , m_particleEncoding(particleEncoding)
    , m_kineticEnergy(static_cast<float>(kineticEnergy))
{
}


// Implementation of member functions
std::string MDTSimHit::print() const {
    
    std::stringstream ss;

    ss << "MDTSimHit:";
    ss << "  identifier: "  << m_MDTid;
    ss << "  t: "  << m_globalTime;
    ss << "  r: "  << m_driftRadius;
    ss << "  x: (" << m_localPosition.x()
       << ","      << m_localPosition.y()
       << ","      << m_localPosition.z()
       << ")";
    ss << " EnergyDeposit: "<<m_energyDeposit<<" pdgId: "<<m_particleEncoding<<" kineticEnergy: "<<m_kineticEnergy;
    ss << " StepLength:  " << m_stepLength;

    return ss.str();
}

int MDTSimHit::trackNumber() const
 { return m_partLink.barcode(); }
 
