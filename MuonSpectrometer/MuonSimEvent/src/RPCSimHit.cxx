/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonSimEvent/RPCSimHit.h"


#include <sstream>

// construction/destruction

RPCSimHit::RPCSimHit( ) :
  m_RPCid(0xffff),
  m_globalTime(0.), 
  m_partLink() 
{
  m_localPosition=Amg::Vector3D(0.,0.,0.);         // pre-step position
  m_localPostStepPosition=Amg::Vector3D(0.,0.,0.); // post-step position
  m_energyDeposit = -1.;                        //Geant4 de/dx
  m_stepLength    = -1.;                        //Geant4 step Length  
  m_particleEncoding = 0;                       // PDG id
  m_kineticEnergy = -1.;                        // kinetic energy of the particle
}
//virtual destructor required by Pool
RPCSimHit::~RPCSimHit( ) {}

RPCSimHit::RPCSimHit(int id, 
		     double time, 
	             const Amg::Vector3D& position)
    : m_RPCid(id), m_globalTime(time)
    , m_localPosition(position)
{
  m_localPostStepPosition=Amg::Vector3D(0.,0.,0.); // post-step position
  m_energyDeposit = -1.;                        //Geant4 de/dx
  m_stepLength    = -1.;                        //Geant4 step Length  
  m_particleEncoding = 0;                       // PDG id
  m_kineticEnergy = -1.;                        // kinetic energy of the particle
}

RPCSimHit::RPCSimHit(int id, 
		     double time, 
	             const Amg::Vector3D& position, 
		     const int trackNumber)
    : m_RPCid(id), m_globalTime(time)
    , m_localPosition(position)
    , m_partLink(trackNumber)
{
  m_localPostStepPosition=Amg::Vector3D(0.,0.,0.); // post-step position
  m_energyDeposit = -1.;                        //Geant4 de/dx
  m_stepLength    = -1.;                        //Geant4 step Length  
  m_particleEncoding = 0;                      // PDG id
  m_kineticEnergy = -1.;                        // kinetic energy of the particle
}

RPCSimHit::RPCSimHit(int id, 
		     double time, 
	             const Amg::Vector3D& prePosition,
		     const int trackNumber, 
	             const Amg::Vector3D& postPosition,
		     const double energyDeposit, const double stepLength, 
		     const int particleEncoding, const double kineticEnergy)
    : m_RPCid(id), m_globalTime(time)
    , m_localPosition(prePosition)
    , m_partLink(trackNumber)
    , m_localPostStepPosition(postPosition)
    , m_energyDeposit(static_cast<float>(energyDeposit))
    , m_stepLength(static_cast<float>(stepLength))
    , m_particleEncoding(particleEncoding) 
    , m_kineticEnergy(static_cast<float>(kineticEnergy))
{
}

RPCSimHit::RPCSimHit(int id, 
		     double time, 
	             const Amg::Vector3D& prePosition,
		     const HepMcParticleLink& hmpl, 
	             const Amg::Vector3D& postPosition,
		     const double energyDeposit, const double stepLength, 
		     const int particleEncoding, const double kineticEnergy)
    : m_RPCid(id), m_globalTime(time)
    , m_localPosition(prePosition)
    , m_partLink(hmpl)
    , m_localPostStepPosition(postPosition)
    , m_energyDeposit(static_cast<float>(energyDeposit))
    , m_stepLength(static_cast<float>(stepLength))
    , m_particleEncoding(particleEncoding) 
    , m_kineticEnergy(static_cast<float>(kineticEnergy))
{
}

// Implemenation of member functions
std::string RPCSimHit::print() const {

    std::stringstream ss;

    ss << "RPCSimHit:";
    ss << "  identifier: "  << m_RPCid;
    ss << "  t:  " << m_globalTime;
    ss << "  startPoint: (" << m_localPosition.x()
       << ","      << m_localPosition.y()
       << ","      << m_localPosition.z()
       << ")";
    ss << "  endPoint: ("<< m_localPostStepPosition.x()
       << ","      << m_localPostStepPosition.y()
       << ","      << m_localPostStepPosition.z()
       << ")";
    ss << " EnergyDeposit: "<<m_energyDeposit<<" StepLength: "<<m_stepLength
       <<" pdgId: "<<m_particleEncoding<<" kineticEnergy: "<<m_kineticEnergy;
    
    return ss.str();
}

int RPCSimHit::trackNumber() const
 { return m_partLink.barcode(); }
 
