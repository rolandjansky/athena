/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// TruthEtIsolationsContainer.h 
// Header file for class TruthEtIsolationsContainer
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENT_TRUTHETISOLATIONSCONTAINER_H 
#define MCPARTICLEEVENT_TRUTHETISOLATIONSCONTAINER_H 

// STL includes
//#include <iosfwd>

// DataModel includes
#include "DataModel/DataVector.h"

// CLIDSvc includes (for persistency)
#include "CLIDSvc/CLASS_DEF.h"

// McParticleEvent includes
#include "McParticleEvent/TruthEtIsolations.h"

// Forward declaration

class TruthEtIsolationsContainer : public DataVector<TruthEtIsolations>
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  TruthEtIsolationsContainer();

  /** Copy constructor: 
   */
  TruthEtIsolationsContainer( const TruthEtIsolationsContainer& rhs );

  /** Assignment operator: 
   */
  TruthEtIsolationsContainer& operator=(const TruthEtIsolationsContainer& rhs);

  /** Destructor: 
   */
  ~TruthEtIsolationsContainer(); 

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

CLASS_DEF(TruthEtIsolationsContainer, 1198824686, 1)

#endif //> MCPARTICLEEVENT_TRUTHETISOLATIONSCONTAINER_H
