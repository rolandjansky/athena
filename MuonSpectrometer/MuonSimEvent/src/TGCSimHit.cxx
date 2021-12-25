/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/TGCSimHit.h"

#include <sstream>

// construction/destruction
TGCSimHit::TGCSimHit( ) :
  m_TGCid(0xffff),
  m_globalTime(0.), 
  m_partLink(), 
  m_energyDeposit(-9999.), 
  m_stepLength(-9999.),
  m_particleEncoding(0), 
  m_kineticEnergy(-9999.)
{
}

//virtual destructor required by Pool
TGCSimHit::~TGCSimHit() {}

TGCSimHit::TGCSimHit(const int id,
		     const double time, 
		     const Amg::Vector3D& position,
		     const Amg::Vector3D& direcos) 
    : m_TGCid(id)
    , m_globalTime(static_cast<float>(time))
    , m_localPosition(position)
    , m_localDireCos(direcos)
    , m_partLink()
    , m_energyDeposit(-9999.)
    , m_stepLength(-9999.)
    , m_particleEncoding(0) 
    , m_kineticEnergy(-9999.)
{
  
}
 
TGCSimHit::TGCSimHit(const int id,
		     const double time, 
		     const Amg::Vector3D& position,
		     const Amg::Vector3D& direcos,
		     const int trackNumber,
		     const double energyDeposit,
		     const double stepLength,
		     const int particleEncoding,
		     const double kineticEnergy)
    : m_TGCid(id)
    , m_globalTime(static_cast<float>(time))
    , m_localPosition(position)
    , m_localDireCos(direcos)
    , m_partLink(trackNumber)
    , m_energyDeposit(static_cast<float>(energyDeposit))
    , m_stepLength(static_cast<float>(stepLength))
    , m_particleEncoding(particleEncoding) 
    , m_kineticEnergy(static_cast<float>(kineticEnergy))
{
    
}

TGCSimHit::TGCSimHit(const int id,
		     const double time, 
		     const Amg::Vector3D& position,
		     const Amg::Vector3D& direcos,
		     const HepMcParticleLink& hmpl,
		     const double energyDeposit,
		     const double stepLength,
		     const int particleEncoding,
		     const double kineticEnergy)
    : m_TGCid(id)
    , m_globalTime(static_cast<float>(time))
    , m_localPosition(position)
    , m_localDireCos(direcos)
    , m_partLink(hmpl)
    , m_energyDeposit(static_cast<float>(energyDeposit))
    , m_stepLength(static_cast<float>(stepLength))
    , m_particleEncoding(particleEncoding) 
    , m_kineticEnergy(static_cast<float>(kineticEnergy))
{
    
}

////////////////////////////////////////////////////////////////////////////////
// Implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
std::string TGCSimHit::print() const {
    
    std::stringstream ss;

    ss << "TGCSimHit:";
    ss << "  identifier: "  << m_TGCid;
    ss << "  t:  " << m_globalTime;
    ss << "  x: (" << m_localPosition.x()
       << ","      << m_localPosition.y()
       << ","      << m_localPosition.z()
       << ")";
    ss << "  dir: (" << m_localDireCos.x()
       << ","        << m_localDireCos.y()
       << ","        << m_localDireCos.z()
       << ")";
    ss << "  dE: " << m_energyDeposit;
    ss << "  l:  " << m_stepLength;
            
    return ss.str();
}

int TGCSimHit::trackNumber() const
 { return m_partLink.barcode(); }
