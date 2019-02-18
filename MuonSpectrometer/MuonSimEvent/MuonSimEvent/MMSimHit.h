/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMEVENT_MMSimHit_H
#define MUONSIMEVENT_MMSimHit_H 1

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeneratorObjects/HepMcParticleLink.h"

#include <string>

typedef int HitID;

class MMSimHit {
 public:
  //
  // construction/destruction
  //
  // needed by athenaRoot
  MMSimHit();

  // Destructor:
  virtual ~MMSimHit();
   MMSimHit(HitID id, double time, const Amg::Vector3D& position, const int particleEncoding, const double kineticEnergy, const Amg::Vector3D& direction, const double depositEnergy, const int trackNumber);
      MMSimHit(HitID id, double time, const Amg::Vector3D& position, const int particleEncoding, const double kineticEnergy, const Amg::Vector3D& direction, const double depositEnergy, const HepMcParticleLink hmpl);
 
  //
  // member functions
  //
  
  //postStep globalTime
  double  globalTime()    const { return (double) m_globalTime; }
  
  //postStep Global and Local Position
  const Amg::Vector3D& globalPosition() const { return m_globalPosition; }
  int particleEncoding() const { return m_particleEncoding; }
  double kineticEnergy() const { return (double) m_kineticEnergy; }
  const Amg::Vector3D& globalDirection() const { return m_globalDirection; }
  double depositEnergy() const { return (double) m_depositEnergy; }
    
  HitID  MMId() const { return m_MMId; }
  std::string  print() const;
    
  bool operator< (const MMSimHit& rhs) const
  { return m_MMId < rhs.m_MMId; }
	
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
  HitID m_MMId;
  double m_globalTime;
  // postStep  
  Amg::Vector3D m_globalPosition;

  int m_particleEncoding; // PDG id 
  double m_kineticEnergy;  // kinetic energy of the particle

  Amg::Vector3D m_globalDirection;
  double m_depositEnergy;
	
  HepMcParticleLink m_partLink;
};

//inline methods

inline const HepMcParticleLink& MMSimHit::particleLink() const
{ return m_partLink; }
inline double MMSimHit::meanTime() const
{ return (double) m_globalTime; }
inline float hitTime(const MMSimHit& hit)
{ return (float) hit.meanTime(); }	

#endif
