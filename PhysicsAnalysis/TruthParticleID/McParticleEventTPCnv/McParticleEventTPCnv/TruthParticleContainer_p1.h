///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleContainer_p1.h 
// Header file for class TruthParticleContainer_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENTTPCNV_TRUTHPARTICLECONTAINER_P1_H 
#define MCPARTICLEEVENTTPCNV_TRUTHPARTICLECONTAINER_P1_H 

// STL includes
#include <string>

// HepMC / CLHEP includes

// Gaudi includes

// Forward declaration
class TruthParticleContainerCnv_p1;

class TruthParticleContainer_p1
{ 

  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class TruthParticleContainerCnv_p1;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  TruthParticleContainer_p1();

  /** Constructor with parameters: we give the key of the 
   *  parent @c McEventCollection.
   */
  TruthParticleContainer_p1( const std::string& parentKey );

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** Set the key of the @c McEventCollection from which the (transient)
   *  @c TruthParticleContainer has been made.
   */
  void setParentKey( const std::string& parentKey );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** The key of the @c McEventCollection from which the (transient)
   *  @c TruthParticleContainer has been made.
   */
  std::string m_parentKey;
  
}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline TruthParticleContainer_p1::TruthParticleContainer_p1() :
  m_parentKey( "NO_KEY" )
{}

inline 
TruthParticleContainer_p1::TruthParticleContainer_p1( const std::string& key ):
  m_parentKey( key )
{}

inline void 
TruthParticleContainer_p1::setParentKey( const std::string& parentKey )
{
  m_parentKey = parentKey;
}

#endif //> MCPARTICLEEVENTTPCNV_TRUTHPARTICLECONTAINER_P1_H
