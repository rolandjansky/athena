///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ReadThinnedData.h 
// Header file for class ReadThinnedData
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_READTHINNEDDATA_H 
#define ATHEXTHINNING_READTHINNEDDATA_H 

/** @class AthExThinning::ReadThinnedData A simple algorithm to read back data
 *  which contains container whose elements have been "thinned away".
 *  This is to make sure that the elements which have been removed are indeed
 *  not there anymore and that @c ElementLink pointing to the remaining 
 *  elements are still valid and pointing to the good elements.
 */

// STL includes
#include <string>
#include <vector>

// HepMC / CLHEP includes

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"

// Forward declaration

namespace AthExThinning {

class ReadThinnedData : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  ReadThinnedData( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~ReadThinnedData(); 

  // Assignment operator: 
  //ReadThinnedData &operator=(const ReadThinnedData &alg); 

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
  ReadThinnedData();

  /// read the output of a given test (test1, test2, test3)
  StatusCode checkTest( const std::string& testName );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// Containers
  
  /// Particles input location
  StringProperty m_particlesName;

  /// Decay input location
  StringProperty m_decayName;

  /// Elephantino input location
  StringProperty m_elephantinoName;

  /// list of test names
  std::vector<std::string> m_testNames;
}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthExThinning

#endif //> ATHEXTHINNING_READTHINNEDDATA_H
