///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ITruthParticleVisitor.h 
// Header file for class ITruthParticleVisitor
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEKERNEL_ITRUTHPARTICLEVISITOR_H 
#define MCPARTICLEKERNEL_ITRUTHPARTICLEVISITOR_H 

/** @class ITruthParticleVisitor to "visit" TruthParticles.
 *  Its main purpose is to be used by concrete @c TruthParticleCnvTools and
 *  apply additional transformations to the @c TruthParticle at hand.
 *  It is currently used by the T/P separation to speed-up the (re)creation of
 *  the transient class by setting on the fly the vector of Et isolations
 *  without requiring an additional for-loop over the whole container.
 */

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// Forward declaration
class TruthParticle;

class ITruthParticleVisitor
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  ITruthParticleVisitor();

  /** Copy constructor: 
   */
  ITruthParticleVisitor( const ITruthParticleVisitor& rhs );

  /** Assignment operator: 
   */
  ITruthParticleVisitor& operator=( const ITruthParticleVisitor& rhs ); 

  /** Constructor with parameters: 
   */

  /** Destructor: 
   */
  virtual ~ITruthParticleVisitor(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** The method to visit a @c TruthParticle to apply further modifications
   *  to the instance at hand.
   */
  virtual 
  void visit( TruthParticle* truthParticle ) const = 0;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> MCPARTICLEKERNEL_ITRUTHPARTICLEVISITOR_H
