///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PerfMonTestBasicAlg.h 
// Header file for class PerfMonTest::BasicAlg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONTESTS_PERFMONTESTBASICALG_H 
#define PERFMONTESTS_PERFMONTESTBASICALG_H 

// STL includes
#include <string>
#include <vector>
#include <utility> // for std::pair

// HepMC / CLHEP includes

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

namespace PerfMonTest {

class BasicAlg : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  BasicAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~BasicAlg(); 

  // Assignment operator: 
  //BasicAlg &operator=(const BasicAlg &alg); 

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
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  BasicAlg();

  /// Property to setup the size of the @c PerfMonTest::Data
  int m_dataSize;

  /// @c PerfMonTest::Data output location
  std::string m_dataOutputName;

  /// dummy boolean to exercize @c IPerfMonSvc::declareInfo
  bool m_my_bool;

  /// dummy int to exercize @c IPerfMonSvc::declareInfo
  int m_my_int;

  /// dummy long to exercize @c IPerfMonSvc::declareInfo
  long m_my_long;

  /// dummy double to exercize @c IPerfMonSvc::declareInfo
  double m_my_double;

  /// dummy string to exercize @c IPerfMonSvc::declareInfo
  std::string m_my_string;

  /// dummy pair to exercize @c IPerfMonSvc::declareInfo
  std::pair<double, double> m_my_pair;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace PerfMonTest

#endif //> PERFMONTESTS_PERFMONTESTBASICALG_H
