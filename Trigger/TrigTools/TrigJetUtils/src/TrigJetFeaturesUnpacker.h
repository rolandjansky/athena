///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigJetFeaturesUnpacker.h 
// Header file for class TrigJetFeaturesUnpacker
// Author: B.Chapleau <bertrand.chapleau@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef TRIGJETUTILS_TRIGJETFEATURESUNACKER_H
#define TRIGJETUTILS_TRIGJETFEATURESUNACKER_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"


class TrigJetFeaturesUnpacker
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  TrigJetFeaturesUnpacker( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~TrigJetFeaturesUnpacker(); 


  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 


  /// Default constructor: 
  TrigJetFeaturesUnpacker();

  /// Containers
  

}; 


#endif //> !TRIGJETUTILS_TRIGJETFEATURESUNACKER_H
