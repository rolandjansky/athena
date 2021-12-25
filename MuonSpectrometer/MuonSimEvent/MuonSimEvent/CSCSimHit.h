/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMEVENT_CSCHIT_H
#define MUONSIMEVENT_CSCHIT_H 1

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include <string>

//---------------------
// back to the situation of tag MuonSimEvent-00-00-25 to avoid
// uncompatibility between 8.0.2 and 8.2.0 -----> 
// redundant time methods are present (globalTime() and meanTime() return the
// same time) --->NOT TO BE FIXED!!<----

class CSCSimHit {

public:
    //
    // construction/destruction
    //
    // needed by athenaRoot
    CSCSimHit();

    // Destructor:
    virtual ~CSCSimHit();
    
    CSCSimHit(int id, double time, double energy, 
    const Amg::Vector3D& HitStart, const Amg::Vector3D& HitEnd, int particleID);

    CSCSimHit(int id, double time, double energy, 
    const Amg::Vector3D& HitStart, const Amg::Vector3D& HitEnd, int particleID, 
    const int trackNumber);
    
    CSCSimHit(int id, double time, double energy, 
    const Amg::Vector3D& HitStart, const Amg::Vector3D& HitEnd, int particleID, 
	      const int trackNumber, double kineticEnergy);

    CSCSimHit(int id, double time, double energy, 
    const Amg::Vector3D& HitStart, const Amg::Vector3D& HitEnd, int particleID, 
	      const HepMcParticleLink& hmpl, double kineticEnergy);
    

    //
    // member functions
    //
    double            globalTime()    const { return (double) m_globalTime; }
    double            energyDeposit() const { return (double) m_energyDeposit; }
    const Amg::Vector3D& getHitStart() const { return m_hitStart; }
    const Amg::Vector3D& getHitEnd() const { return m_hitEnd; }
    int               particleID()    const  { return m_particleID; }
    int		      CSCid()	      const { return m_CSCid; }
    std::string       print()         const;
    bool operator< (const CSCSimHit& rhs) const
    	{ return m_CSCid < rhs.m_CSCid; }
    double meanTime() const;

// number of track which released this energy:
    int trackNumber() const;
    double kineticEnergy() const { return static_cast<double>(m_kineticEnergy); }
    

// link to the particle generating the hit
    const HepMcParticleLink& particleLink() const;

    void setGlobalTime( const double time ) { m_globalTime = time; }

private:    
    //
    // member data
    //
    int	m_CSCid;
    float     m_globalTime;
    float     m_energyDeposit;
    Amg::Vector3D m_hitStart;
    Amg::Vector3D m_hitEnd;
    int m_particleID;
    HepMcParticleLink m_partLink;
    float m_kineticEnergy;
    

};

//inline methods

inline double CSCSimHit::meanTime() const
	{ return (double) m_globalTime; }

inline const HepMcParticleLink& CSCSimHit::particleLink() const
 	{ return m_partLink; }

// open functions

inline float hitTime(const CSCSimHit& hit)
	{ return (float) hit.meanTime(); }

#endif
