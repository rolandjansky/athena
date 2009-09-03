/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// IParticleAssocFilter.h 
// Header file for class IParticleAssocFilter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ANALYSISASSOCIATION_IPARTICLEASSOCFILTER_H 
#define ANALYSISASSOCIATION_IPARTICLEASSOCFILTER_H 1

/** \class IParticleAssocFilter is a class to filter a collection of particles.
 *  One can construct a filter to associate 2 IParticles based on topological
 *  requirements.
 *  One can require associations in px, py, pz, energy, eta, phi, mass, pt,
 *  deltaR and cosTheta and specify min and max values for each of these variables.
 */

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// EventKernel includes
#include "EventKernel/IParticle.h"

// AnalysisAssociation include
#include "AnalysisAssociation/AssocFilter.h"

// Forward declaration

class IParticleAssocFilter : public AssocFilter<IParticle, IParticle>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  IParticleAssocFilter();

  /** Copy constructor: 
   */
  IParticleAssocFilter( const IParticleAssocFilter& rhs );

  // Constructor with parameters: 

  /** Destructor: 
   */
  virtual ~IParticleAssocFilter(); 

  /** Assignment operator: 
   */
  IParticleAssocFilter &operator=(const IParticleAssocFilter& rhs ); 

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

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> ANALYSISASSOCIATION_IPARTICLEASSOCFILTER_H
