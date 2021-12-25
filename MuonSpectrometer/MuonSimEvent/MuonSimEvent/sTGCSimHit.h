/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMEVENT_sTGCSimHit_H
#define MUONSIMEVENT_sTGCSimHit_H 1

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeneratorObjects/HepMcParticleLink.h"

#include <string>

typedef int HitID;

class sTGCSimHit {
 public:
  //
  // construction/destruction
  //
  // needed by athenaRoot
  sTGCSimHit();

  // Destructor:
  virtual ~sTGCSimHit();
   sTGCSimHit(HitID id, double time, const Amg::Vector3D& position, const int particleEncoding, const Amg::Vector3D& direction, const double depositEnergy, const int trackNumber);
   sTGCSimHit(HitID id, double time, const Amg::Vector3D& position, const int particleEncoding, const Amg::Vector3D& direction, const double depositEnergy, const HepMcParticleLink& hmpl);
 
  //
  // member functions
  //
  
  //postStep globalTime
  double  globalTime()    const { return (double) m_globalTime; }
  
  //postStep Global and Local Position
  const Amg::Vector3D& globalPosition() const { return m_globalPosition; }
  int particleEncoding() const { return m_particleEncoding; }
  const Amg::Vector3D& globalDirection() const { return m_globalDirection; }
  double depositEnergy() const { return (double) m_depositEnergy; }
    
  HitID  sTGCId() const { return m_sTGCId; }
  std::string  print() const;
    
  bool operator< (const sTGCSimHit& rhs) const
  { return m_sTGCId < rhs.m_sTGCId; }
	
  double meanTime() const;

  void setGlobalTime( const double time ) { m_globalTime = time; }
  void setDepositEnergy ( const double depositEnergy ) { m_depositEnergy = depositEnergy ;}
  void setGlobalPosition ( const Amg::Vector3D& globalPosition ) { m_globalPosition = globalPosition ;} //!< sets the position expressed in global coordinates
  void setGlobalDirection ( const Amg::Vector3D& globalDirection ) { m_globalDirection = globalDirection ;} //!< sets the direction expressed in global coordinates
    
  // number of track which released this energy:
  int trackNumber() const;

  // link to the particle generating the hit
  const HepMcParticleLink& particleLink() const;
    
 private:    
  //
  // member data
  //
  HitID m_sTGCId;
  float m_globalTime;
  // postStep  
  Amg::Vector3D m_globalPosition;

  int m_particleEncoding; // PDG id 

  Amg::Vector3D m_globalDirection;
  float m_depositEnergy;
	
  HepMcParticleLink m_partLink;
};

//inline methods

inline const HepMcParticleLink& sTGCSimHit::particleLink() const
{ return m_partLink; }
inline double sTGCSimHit::meanTime() const
{ return (double) m_globalTime; }
inline float hitTime(const sTGCSimHit& hit)
{ return (float) hit.meanTime(); }	

#endif
