///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ITruthParticleCnvTool.h 
// Header file for class ITruthParticleCnvTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEKERNEL_ITRUTHISOLATIONTOOL_H
#define MCPARTICLEKERNEL_ITRUTHISOLATIONTOOL_H

/** @class ITruthIsolationTool is the interface for AlgTools which want to 
 *  compute and/or retrieve isolation energies for/from @c HepMC::GenEvent.
 *  This information is then stored into a @c TruthEtIsolations container,
 *  put into @c StoreGate with the key "TruthEtIsolations_<key-of-GenEvent>".
 *  See McParticleEvent/TruthParticleParameters.h for more informations about
 *  these transverse energy isolations.
 */

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"

// Forward declaration

static const InterfaceID IID_ITruthIsolationTool("ITruthIsolationTool", 1, 0);

class ITruthIsolationTool : virtual public IAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public enums: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** enumerator to decide which strategy to apply in the selection
   *  of particles during the Et-isolation computation
   */
  enum ParticleSelect {
    UseChargedOnly = 0,
    UseAll
  };

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~ITruthIsolationTool();

  // Athena algorithm's Hooks
  virtual StatusCode  initialize()    = 0;

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  static const InterfaceID& interfaceID();

  /**
   * Return the name of the @c TruthEtIsolations container (ie: its 
   * @c StoreGate location) given the @c StoreGate location of a 
   * @c McEventCollection.
   * It returns an empty string if no @c TruthEtIsolations container has been
   * processed by the isolation tool.
   */
  virtual const std::string&
  etIsolationsName( const std::string& mcEvtName ) const = 0;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** Computes the isolation energies for each of the @c HepMC::GenEvent 
   *  contained into the @c McEventCollection.
   *  One can switch b/w using all particles or only the charged ones.
   *  Default is to use all
   */
  virtual 
    StatusCode buildEtIsolations( const std::string& mcEvtName, 
				  ParticleSelect c=UseAll ) = 0;

  /** Make an alias in the map of isolation energies
   */
  virtual
    StatusCode registerAlias( const std::string& originalMcEvtColl,
			      const std::string& aliasMcEvtColl ) = 0;

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
inline const InterfaceID& ITruthIsolationTool::interfaceID() 
{ 
   return IID_ITruthIsolationTool; 
}


#endif //> MCPARTICLEKERNEL_ITRUTHISOLATIONTOOL_H
