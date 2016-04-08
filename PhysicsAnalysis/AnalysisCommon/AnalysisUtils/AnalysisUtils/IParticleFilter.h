///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IParticleFilter.h 
// Header file for class IParticleFilter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ANALYSISUTILS_IPARTICLEFILTER_H 
#define ANALYSISUTILS_IPARTICLEFILTER_H 

/** IParticleFilter : this is the filter for IParticles. It resolves the
 *  templated class of IIParticleFilter to IParticle.
 *  So each class within the AOD packages should be compliant with this
 *  filter.
 *  The IParticleFilter can select particles from their momentum, pdg ID and charge.
 *  A basic instantiation could look like :
 *
 * @code
 * IParticleFilter filter;
 * //> to select only particles whith an energy above 100 GeV.
 * filter.set_eneMin( 100.*GeV );
 * filter.set_pdgId( PDG::Z0 ); //> to select only Z-bosons
 *
 * //> to select a 40*GeV mass window around the Z invariant mass peak :
 * filter.set_massMin( (91.-20.)*GeV );
 * filter.set_massMax( (91.+20.)*GeV );
 * @endcode
 */

// FrameWork includes
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

// EventKernel includes
#include "EventKernel/IParticle.h"

// AnalysisUtils includes
#include "AnalysisUtils/IIParticleFilter.h"

class IParticleFilter : public IIParticleFilter<IParticle>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  IParticleFilter();

  /** Copy constructor: 
   */
  IParticleFilter( const IParticleFilter& rhs );

  // Constructor with parameters: 

  /** Destructor: 
   */
  virtual ~IParticleFilter(); 

  /** Assignment operator: 
   */
  IParticleFilter& operator=( const IParticleFilter& rhs ); 

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

}; 

#endif //> ANALYSISUTILS_IPARTICLEFILTER_H
