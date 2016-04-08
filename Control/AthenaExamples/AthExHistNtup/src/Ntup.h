///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Ntup.h 
// Header file for class AthEx::Ntup
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXHISTNTUP_ATHEXNTUP_H
#define ATHEXHISTNTUP_ATHEXNTUP_H 1

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"

//#include "TTree.h"
// fwd declares
class TTree;

namespace AthEx {

class Ntup
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  Ntup( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~Ntup(); 

  // Assignment operator: 
  //Ntup &operator=(const Ntup &alg); 

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
  Ntup();

  /// handle to the n-tuple svc
  ServiceHandle<ITHistSvc> m_ntsvc;

  // n-tuple data members
  unsigned int m_size;
  unsigned int m_run;
  unsigned int m_event;
  std::vector<long> m_rundata;

  /// pointer to the n-tuple
  TTree* m_ntuple;
  
  /// key to the event-info
  std::string m_evtInfoName;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> namespace AthEx

#endif //> !ATHEXHISTNTUP_ATHEXNTUP_H
