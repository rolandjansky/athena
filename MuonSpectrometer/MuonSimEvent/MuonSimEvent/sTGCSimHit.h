/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
   sTGCSimHit(HitID id, double time, const Amg::Vector3D& position, const int particleEncoding, const Amg::Vector3D& direction, const double depositEnergy);
   //sTGCSimHit(HitID id, double time, const Amg::Vector3D& position, const int particleEncoding, const Amg::Vector3D& direction, const double depositEnergy);
 
  //
  // member functions
  //
  
  //preStep globalTime
  double  globalpreTime()    const { return (double) m_globalpreTime; }
  //postStep globalTime
  double  globalTime()    const { return (double) m_globalTime; }
  
  //preStep Global and Local position
  const Amg::Vector3D& globalPrePosition() const { return m_globalPrePosition; }
  const Amg::Vector3D& localPrePosition() const { return m_localPrePosition; }
  //postStep Global and Local Position
  const Amg::Vector3D& globalPosition() const { return m_globalPosition; }
  const Amg::Vector3D& localPosition() const { return m_localPosition; }
  int particleEncoding() const { return m_particleEncoding; }
  double kineticEnergy() const { return (double) m_kineticEnergy; }
  const Amg::Vector3D& globalDirection() const { return m_globalDirection; }
  double depositEnergy() const { return (double) m_depositEnergy; }
  double StepLength() const { return (double) m_StepLength; }
    
  HitID  sTGCId() const { return m_sTGCId; }
  std::string  print() const;
    
  bool operator< (const sTGCSimHit& rhs) const
  { return m_sTGCId < rhs.m_sTGCId; }
	
  double meanTime() const;

  void setGlobalTime( const double time ) { m_globalTime = time; }
  void setDepositEnergy ( const double depositEnergy ) { m_depositEnergy = depositEnergy ;}
  void setGlobalPosition ( const Amg::Vector3D& globalPosition ) { m_globalPosition = globalPosition ;} //!< sets the position expressed in global coordinates
  void setGlobalDirection ( const Amg::Vector3D& globalDirection ) { m_globalDirection = globalDirection ;} //!< sets the direction expressed in global coordinates
  void setKineticEnergy ( const double kineticEnergy ) { m_kineticEnergy = kineticEnergy ;} 
    
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
  float m_globalpreTime;
  // postStep  
  Amg::Vector3D m_globalPosition;
  Amg::Vector3D m_localPosition;
  //preStep
  Amg::Vector3D m_globalPrePosition;
  Amg::Vector3D m_localPrePosition;

  int m_particleEncoding; // PDG id 
  float m_kineticEnergy;  // kinetic energy of the particle

  Amg::Vector3D m_globalDirection;
  float m_depositEnergy;
  float m_StepLength;
	
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
