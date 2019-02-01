///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleContainer_p6.h 
// Header file for class TruthParticleContainer_p6
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENTTPCNV_TRUTHPARTICLECONTAINER_P6_H 
#define MCPARTICLEEVENTTPCNV_TRUTHPARTICLECONTAINER_P6_H 

// STL includes
#include <vector>
#include <utility>

// Persistent ElementLink
#include "DataModelAthenaPool/ElementLink_p3.h"

// Gaudi includes

// Forward declaration
class TruthParticleContainerCnv_p6;

class TruthParticleContainer_p6
{ 

  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class TruthParticleContainerCnv_p6;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  TruthParticleContainer_p6();

  /** Destructor: 
   */
  ~TruthParticleContainer_p6() = default;

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** The persistent pointer toward the @c McEventCollection the (transient)
   *  @c TruthParticleContainer is proxying
   */
  ElementLinkInt_p3 m_genEvent;

  /**
   * The persistent pointer toward the @c TruthEtIsolations container the
   * (transient) @c TruthParticleContainer may or may not be connected to.
   */
  ElementLinkInt_p3 m_etIsolations;
}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline TruthParticleContainer_p6::TruthParticleContainer_p6() :
  m_genEvent    ( ),
  m_etIsolations( )
{}

#endif //> MCPARTICLEEVENTTPCNV_TRUTHPARTICLECONTAINER_P6_H
