///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ITruthParticleCnvTool.h 
// Header file for class ITruthParticleCnvTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEKERNEL_ITRUTHPARTICLECNVTOOL_H
#define MCPARTICLEKERNEL_ITRUTHPARTICLECNVTOOL_H

/** @class ITruthParticleCnvTool is the interface for AlgTools which want to convert
 *  McEventCollection objects into a TruthParticleContainer objects.
 *  It is also used by the custom POOL converter for TruthParticleContainer to
 *  recreate the TruthParticleContainers on the fly (save some disk space).
 *  ITruthParticleCnvTool also provides some helper functions related with
 *  mother/daughter relationships in the TruthParticle context.
 */

// STL includes

// HepMC / CLHEP includes

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IProperty.h"

// McParticleKernel includes
#include "McParticleKernel/ITruthParticleVisitor.h"

// Forward declaration
class McEventCollection;
class TruthParticle;
class TruthParticleContainer;

static const InterfaceID IID_ITruthParticleCnvTool("ITruthParticleCnvTool", 1, 0);

class ITruthParticleCnvTool : virtual public IAlgTool,
                              virtual public IProperty
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~ITruthParticleCnvTool();

  // Athena algorithm's Hooks
  virtual StatusCode  execute()    = 0;

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  static const InterfaceID& interfaceID();

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** Convert a @c McEventCollection into an @c TruthParticleContainer.
   *  @in  mcEvts the @c McEventCollection holding the @c HepMC::GenEvent we
   *      want to convert into a @c TruthParticleContainer
   *  @in  genEvtIndex the index to the @c HepMC::GenEvent to be converted
   *  @out mcParts a valid pointer to a @c TruthParticleContainer which will
   *       be filled with adaptors to @c HepMC::GenParticles.
   */
  virtual 
  StatusCode convert( const McEventCollection* mcEvts,
		      const unsigned int genEvtIndex,
		      TruthParticleContainer* mcParts,
                      const ITruthParticleVisitor* visitor = 0 ) const = 0;

  /** Convert the first element of a @c McEventCollection into
   *  a @c TruthParticleContainer.
   */
  StatusCode convert( const McEventCollection* mcCollection, 
		      TruthParticleContainer* container ) const;

  /** @{ deprecated
   */
  /** Convert a @c McEventCollection into an @c TruthParticleContainer.
   *  Note that the pointer to the @c TruthParticleContainer MUST be valid.
   */
  StatusCode 
  convertMcEventCollection( const McEventCollection* mcCollection, 
			    TruthParticleContainer* container  ) const;
  /** @}
   */

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline const InterfaceID& ITruthParticleCnvTool::interfaceID() 
{ 
   return IID_ITruthParticleCnvTool; 
}

inline StatusCode
ITruthParticleCnvTool::convert( const McEventCollection* mcCollection, 
				TruthParticleContainer* container  ) const
{
  const unsigned int genEvtIndex = 0;
  const ITruthParticleVisitor* dummyVisitor = 0;
  return this->convert( mcCollection, genEvtIndex, container, dummyVisitor );
}

inline StatusCode
ITruthParticleCnvTool::convertMcEventCollection( const McEventCollection* mcCollection, 
						 TruthParticleContainer* container  ) const
{
  const unsigned int genEvtIndex = 0;
  const ITruthParticleVisitor* dummyVisitor = 0;
  return this->convert( mcCollection, genEvtIndex, container, dummyVisitor );
}

#endif //> MCPARTICLEKERNEL_ITRUTHPARTICLECNVTOOL_H
