///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CreateData.h 
// Header file for class CreateData
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_CREATEDATA_H 
#define ATHEXTHINNING_CREATEDATA_H 

/** @class AthExThinning::CreateData A simple algorithm to create dummy 
 *  4-momentum-like data classes: @c AthExParticle as well as dummy decays
 *  (@c AthExDecay) made up from 2 @c AthExParticle and a more involved class 
 *  (@c AthExElephantino) holding 4 pointers to some elements of the 
 *  @c AthExParticles container.
 */

// STL includes
#include <string>

// HepMC / CLHEP includes

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"

// Forward declaration

namespace AthExThinning {

class CreateData : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  CreateData( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~CreateData(); 

  // Assignment operator: 
  //CreateData &operator=(const CreateData &alg); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// Default constructor: 
  CreateData();

  /// make data for a test
  StatusCode makeData( const std::string& testName );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// list of test-names to create data for (default: test1,test2,test3)
  std::vector<std::string> m_testNames;

  // Containers
  
  /// Particles output location
  StringProperty m_particlesOutputName;

  /// Number of particles to generate
  UnsignedIntegerProperty m_nbrParticles;

  /// Decay object output location
  StringProperty m_decayOutputName;

  /// Elephantino object output location
  StringProperty m_elephantinoOutputName;


}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthExThinning

#endif //> ATHEXTHINNING_CREATEDATA_H
