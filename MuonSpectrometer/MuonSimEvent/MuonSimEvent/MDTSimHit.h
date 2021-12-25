/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMEVENT_MDTSimHit_H
#define MUONSIMEVENT_MDTSimHit_H 1

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeneratorObjects/HepMcParticleLink.h"

#include <string>

//---------------------
// back to the situation of tag MuonSimEvent-00-00-25 to avoid
// uncompatibility between 8.0.2 and 8.2.0 -----> 
// redundant time methods are present (globalTime() and meanTime() return the
// same time) --->NOT TO BE FIXED!!<----

typedef int HitID;

class MDTSimHit {
public:
    //
    // construction/destruction
    //
    // needed by athenaRoot
    MDTSimHit();

    // Destructor:
    virtual ~MDTSimHit();
    
    MDTSimHit(HitID id, double time, double radius, const Amg::Vector3D& position);
    MDTSimHit(HitID id, double time, double radius, const Amg::Vector3D& position, const int trackNumber);
    MDTSimHit(HitID id, double time, double radius, const Amg::Vector3D& position, const int trackNumber, const double stepLength, const double energyDeposit, const int particleEncoding, const double kineticEnergy);
    MDTSimHit(HitID id, double time, double radius, const Amg::Vector3D& position, const HepMcParticleLink& hmpl, const double stepLength, const double energyDeposit, const int particleEncoding, const double kineticEnergy);
     
    
    //
    // member functions
    //
    double            globalTime()    const { return (double) m_globalTime; }
    double            driftRadius()   const { return (double) m_driftRadius; }
    const Amg::Vector3D& localPosition() const { return m_localPosition; }

    double            stepLength()    const { return static_cast<double>(m_stepLength); }
    double            energyDeposit()    const { return static_cast<double>(m_energyDeposit); }
    int               particleEncoding() const { return m_particleEncoding; }
    double            kineticEnergy()    const { return static_cast<double>(m_kineticEnergy); }
    
    HitID	      MDTid()	      const { return m_MDTid; }
    std::string       print()         const;
    bool operator< (const MDTSimHit& rhs) const
    	{ return m_MDTid < rhs.m_MDTid; }
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
    HitID m_MDTid;
    float     m_globalTime;
    float     m_driftRadius;
    Amg::Vector3D m_localPosition;
    HepMcParticleLink m_partLink;
    
    float m_stepLength;
    float m_energyDeposit;  //Geant4 de/dx
    int m_particleEncoding; // PDG id 
    float m_kineticEnergy;  // kinetic energy of the particle


};
//inline methods

inline double MDTSimHit::meanTime() const
	{ return (double) m_globalTime; }

inline const HepMcParticleLink& MDTSimHit::particleLink() const
 	{ return m_partLink; }
 

// open functions

inline float hitTime(const MDTSimHit& hit)
	{ return (float) hit.meanTime(); }

#endif
