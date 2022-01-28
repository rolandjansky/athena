/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMEVENT_TGCSimHit_H
#define MUONSIMEVENT_TGCSimHit_H 1

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeneratorObjects/HepMcParticleLink.h"

#include <string>

//---------------------
// back to the situation of tag MuonSimEvent-00-00-25 to avoid
// uncompatibility between 8.0.2 and 8.2.0 -----> 
// redundant time methods are present (globalTime() and meanTime() return the
// same time) --->NOT TO BE FIXED!!<----

class TGCSimHit{

public:
    //
    // construction/destruction
    //
    // needed by athenaRoot
    TGCSimHit();

    // Destructor:
    virtual ~TGCSimHit();
    TGCSimHit(const int id, const double time, const Amg::Vector3D& position, const Amg::Vector3D& direcos);
    TGCSimHit(const int id, const double time, const Amg::Vector3D& position, const Amg::Vector3D& direcos,
	      const int trackNumber, const double energyDeposit, const double stepLength,
	      const int particleEncoding=0, const double kineticEnergy=-9999.);
    TGCSimHit(const int id, const double time, const Amg::Vector3D& position, const Amg::Vector3D& direcos,
	      const HepMcParticleLink& hmpl, const double energyDeposit, const double stepLength,
	      const int particleEncoding=0, const double kineticEnergy=-9999.);
    

    //
    // member functions
    //
    double            globalTime()    const { return static_cast<double>(m_globalTime); }
    const Amg::Vector3D& localPosition() const { return m_localPosition; }
    const Amg::Vector3D& localDireCos()  const { return m_localDireCos; }
    int		      TGCid()	      const { return m_TGCid; }
    double            energyDeposit() const { return static_cast<double>(m_energyDeposit); }
    double            stepLength()    const { return static_cast<double>(m_stepLength); }
    int               particleEncoding() const { return m_particleEncoding; }
    double            kineticEnergy() const { return static_cast<double>(m_kineticEnergy); }

    std::string       print()         const;
    bool operator< (const TGCSimHit& rhs) const
    	{ return m_TGCid < rhs.m_TGCid; }
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
    int m_TGCid;
    float     m_globalTime;
    Amg::Vector3D m_localPosition;
    Amg::Vector3D m_localDireCos;
    HepMcParticleLink m_partLink;
    float m_energyDeposit;
    float m_stepLength;
    int m_particleEncoding; // PDG id 
    float m_kineticEnergy; // kinetic energy of the particle

};
//inline methods

inline double TGCSimHit::meanTime() const
	{ return (double) m_globalTime; }

inline const HepMcParticleLink& TGCSimHit::particleLink() const
 	{ return m_partLink; }
 

// open functions

inline float hitTime(const TGCSimHit& hit)
	{ return (float) hit.meanTime(); }


#endif
