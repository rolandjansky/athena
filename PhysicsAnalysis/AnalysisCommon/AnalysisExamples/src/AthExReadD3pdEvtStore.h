///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ReadD3pdEvtStore.h 
// Header file for class ReadD3pdEvtStore
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ANALYSISEXAMPLES_ATHEXREADD3PDEVTSTORE_H
#define ANALYSISEXAMPLES_ATHEXREADD3PDEVTSTORE_H 1

// STL includes
#include <string>
#include <vector>
#include <stdint.h>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"

// fwd declares

namespace AthEx {

class ReadD3pdEvtStore
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  ReadD3pdEvtStore( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~ReadD3pdEvtStore(); 

  // Assignment operator: 
  //ReadD3pdEvtStore &operator=(const ReadD3pdEvtStore &alg); 

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
  ReadD3pdEvtStore();

  // containers
  std::string m_runnbr;
  std::string m_evtnbr;
  std::string m_el_n;
  std::string m_el_eta;
  std::string m_el_jetcone_dr;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthEx

#endif //> !ANALYSISEXAMPLES_ATHEXREADD3PDEVTSTORE_H
