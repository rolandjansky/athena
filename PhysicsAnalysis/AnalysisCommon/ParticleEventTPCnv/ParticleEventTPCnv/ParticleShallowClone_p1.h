///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ParticleShallowClone_p1.h 
// Header file for class ParticleShallowClone_p1
// Author: Karsten Koeneke
// Date:   February 2013
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_PARTICLESHALLOWCLONE_P1_H 
#define PARTICLEEVENTTPCNV_PARTICLESHALLOWCLONE_P1_H 

// STL includes
//#include <vector>

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLink_p3.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4PxPyPzE_p1.h"

// ParticleEventTPCnv includes
//#include "ParticleEventTPCnv/ParticleBase_p1.h"

// forward declarations
class ParticleShallowCloneCnv_p1;

class ParticleShallowClone_p1 
{
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class ParticleShallowCloneCnv_p1;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /** Default constructor: 
   */
  ParticleShallowClone_p1();

  /** Destructor: 
   */
  ~ParticleShallowClone_p1() = default;

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
private: 

  /// the 4-mom part
  P4PxPyPzE_p1 m_momentum;

  /// The persistent pointer (ElementLink) to the original "master" particle
  ElementLinkInt_p3 m_masterLink;
    
  /// Persistent pointer (ElementLink) to the origin of this particle
  ElementLinkInt_p3 m_originLink;

  /// Switch to tell if this particle has a charge
  bool m_hasCharge;

  /** Value of the charge of the particle (this has real meaning, only
   *  if hasCharge() returns true)
   */
  float m_charge;



}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline ParticleShallowClone_p1::ParticleShallowClone_p1()
  : m_hasCharge(false),
    m_charge(0)
{}

#endif //> PARTICLEEVENTTPCNV_PARTICLESHALLOWCLONE_P1_H
