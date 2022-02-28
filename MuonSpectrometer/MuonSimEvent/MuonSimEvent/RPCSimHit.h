/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONG4SIMEVENT_RPCSimHit_H
#define MUONG4SIMEVENT_RPCSimHit_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeneratorObjects/HepMcParticleLink.h"

#include <string>

//---------------------
// back to the situation of tag MuonSimEvent-00-00-25 to avoid
// uncompatibility between 8.0.2 and 8.2.0 -----> 
// redundant time methods are present (globalTime() and meanTime() return the
// same time) --->NOT TO BE FIXED!!<----

class RPCSimHit {

public:
    //
    // construction/destruction
    //
    // needed by athenaRoot
    RPCSimHit();

    // Destructor:
    virtual ~RPCSimHit();
    RPCSimHit(int id, double time, const Amg::Vector3D& position);
    RPCSimHit(int id, double time, const Amg::Vector3D& position, const int trackNumber);
    RPCSimHit(int id, double time, 
	      const Amg::Vector3D& prePosition,  
	      const int trackNumber, 	
	      const Amg::Vector3D& postPosition,
	      const double energyDeposit, 
	      const double stepLength, 
	      const int particleEncoding, 
	      const double kineticEnergy);
    RPCSimHit(int id, double time, 
	      const Amg::Vector3D& prePosition,  
	      const HepMcParticleLink& hmpl, 	
	      const Amg::Vector3D& postPosition,
	      const double energyDeposit, 
	      const double stepLength, 
	      const int particleEncoding, 
	      const double kineticEnergy);
    
   

    //
    // member functions
    //
    double            globalTime()    const { return (double) m_globalTime; }
    const Amg::Vector3D& localPosition()    const { return m_localPosition; }
    const Amg::Vector3D& preLocalPosition() const { return m_localPosition; }
    const Amg::Vector3D& postLocalPosition() const { return m_localPostStepPosition; }
    double            energyDeposit() const { return static_cast<double>(m_energyDeposit); }
    double            prePostStepDistance()    const { return (m_localPostStepPosition-m_localPosition).mag(); }
    double            stepLength()    const { return m_stepLength; }
    int               particleEncoding() const { return m_particleEncoding; }
    double            kineticEnergy() const { return static_cast<double>(m_kineticEnergy); }
    int		      RPCid()	      const { return m_RPCid; }
    std::string       print()         const;
    bool operator< (const RPCSimHit& rhs) const
    	{ return m_RPCid < rhs.m_RPCid; }
    double meanTime() const;

// number of track which released this energy:
    int trackNumber() const;

// link to the particle generating the hit
    const HepMcParticleLink& particleLink() const;
    
    void setGlobalTime( const double time ) { m_globalTime = time; }

private:    
    //
    // member data
    //
    int m_RPCid;
    float     m_globalTime;
    Amg::Vector3D m_localPosition; // pre-step position
    HepMcParticleLink m_partLink;
    Amg::Vector3D m_localPostStepPosition; // post-step position
    float m_energyDeposit;  //Geant4 de/dx
    float m_stepLength;     //Geant4 stepLength
    int m_particleEncoding; // PDG id 
    float m_kineticEnergy;  // kinetic energy of the particle


};
//inline methods

inline double RPCSimHit::meanTime() const
	{ return (double) m_globalTime; }

inline const HepMcParticleLink& RPCSimHit::particleLink() const
 	{ return m_partLink; }

// open functions

inline float hitTime(const RPCSimHit& hit)
	{ return (float) hit.meanTime(); }

#endif

