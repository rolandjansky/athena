/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMEVENT_GenericMuonSimHit_H
#define MUONSIMEVENT_GenericMuonSimHit_H 1

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeneratorObjects/HepMcParticleLink.h"

#include <string>

typedef int HitID;

class GenericMuonSimHit {
 public:
  //
  // construction/destruction
  //
  // needed by athenaRoot
  GenericMuonSimHit();

  // Destructor:
  virtual ~GenericMuonSimHit();
   GenericMuonSimHit(HitID id, double time, double pretime, const Amg::Vector3D& position, const Amg::Vector3D& local_position, const Amg::Vector3D& preposition, const Amg::Vector3D& local_preposition, const int particleEncoding, const double kineticEnergy, const Amg::Vector3D& direction, const double depositEnergy, const double StepLength, const int trackNumber);
      GenericMuonSimHit(HitID id, double time, double pretime, const Amg::Vector3D& position, const Amg::Vector3D& local_position, const Amg::Vector3D& preposition, const Amg::Vector3D& local_preposition, const int particleEncoding, const double kineticEnergy, const Amg::Vector3D& direction, const double depositEnergy, const double StepLength, const HepMcParticleLink& hmpl);
 
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
    
  HitID  GenericId() const { return m_GenericId; }
  std::string  print() const;
    
  bool operator< (const GenericMuonSimHit& rhs) const
  { return m_GenericId < rhs.m_GenericId; }
	
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
  HitID m_GenericId;
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

inline const HepMcParticleLink& GenericMuonSimHit::particleLink() const
{ return m_partLink; }
inline double GenericMuonSimHit::meanTime() const
{ return (double) m_globalTime; }
inline float hitTime(const GenericMuonSimHit& hit)
{ return (float) hit.meanTime(); }	

#endif
