/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/CSCSimHit.h"


#include <sstream>

// construction/destruction
//
CSCSimHit::CSCSimHit( ) :
    m_CSCid(0xffff),
    m_globalTime(0.), 
    m_energyDeposit(0),
    m_particleID(0),
    m_partLink(), 
    m_kineticEnergy(-1.)
{
}
//virtual destructor required by Pool
CSCSimHit::~CSCSimHit( ){}


CSCSimHit::CSCSimHit(int id, 
		     double time, 
		     double  energy,
	       	     const Amg::Vector3D& HitStart, 
		     const Amg::Vector3D& HitEnd, 
	       	     int particleID)
    : m_CSCid(id), m_globalTime(time)
    , m_energyDeposit(energy)
    , m_hitStart(HitStart)
    , m_hitEnd(HitEnd)
    , m_particleID(particleID)
    , m_kineticEnergy(-1.)
{

}

CSCSimHit::CSCSimHit(int id, 
		     double time, 
		     double  energy,
	             const Amg::Vector3D& HitStart, 
		     const Amg::Vector3D& HitEnd, 
	             int particleID, 
		     const int trackNumber)
    : m_CSCid(id), m_globalTime(time)
    , m_energyDeposit(energy)
    , m_hitStart(HitStart)
    , m_hitEnd(HitEnd)
    , m_particleID(particleID)
    , m_partLink(trackNumber)
    , m_kineticEnergy(-1.)
{

}

CSCSimHit::CSCSimHit(int id, 
		     double time, 
		     double  energy,
	             const Amg::Vector3D& HitStart, 
		     const Amg::Vector3D& HitEnd, 
	             int particleID, 
		     const int trackNumber, 
                     const double kineticEnergy)
    : m_CSCid(id), m_globalTime(time)
    , m_energyDeposit(energy)
    , m_hitStart(HitStart)
    , m_hitEnd(HitEnd)
    , m_particleID(particleID)
    , m_partLink(trackNumber)
    , m_kineticEnergy(static_cast<float>(kineticEnergy))
{

}

CSCSimHit::CSCSimHit(int id, 
		     double time, 
		     double  energy,
	             const Amg::Vector3D& HitStart, 
		     const Amg::Vector3D& HitEnd, 
	             int particleID, 
		     const HepMcParticleLink& hmpl, 
                     const double kineticEnergy)
    : m_CSCid(id), m_globalTime(time)
    , m_energyDeposit(energy)
    , m_hitStart(HitStart)
    , m_hitEnd(HitEnd)
    , m_particleID(particleID)
    , m_partLink(hmpl)
    , m_kineticEnergy(static_cast<float>(kineticEnergy))
{

}


// Implementation of member functions

std::string CSCSimHit::print() const {
    
    std::string        str;
    std::stringstream  ss;

    ss << "CSCSimHit:";
    ss << "  identifier: "  << m_CSCid;
    ss << "  t:  "    << m_globalTime;
    ss << "  E:  "    << m_energyDeposit;
    ss << "  kinE:  " << m_kineticEnergy;
    ss << "  Hit start position: ("    << m_hitStart.x()
	      << ","  << m_hitStart.y()
	      << ","  << m_hitStart.z()
	      << ")";  
    ss << "  Hit end position: ("    << m_hitEnd.x()
	      << ","  << m_hitEnd.y()
	      << ","  << m_hitEnd.z()
	      << ")"; 	      

    float distance = pow( m_hitEnd.x()-m_hitStart.x() , 2);
    distance += pow( m_hitEnd.y()-m_hitStart.y() , 2);
    distance += pow( m_hitEnd.z()-m_hitStart.z() , 2);
    distance = sqrt(distance);
    
    ss << " Distance: "   << distance;
    ss << " Particle ID: "   << m_particleID;
    ss << " genTrackNumber: "   << m_partLink.barcode();
    
    str = ss.str();
    
    return str;

}

int CSCSimHit::trackNumber() const
 { return m_partLink.barcode(); }
