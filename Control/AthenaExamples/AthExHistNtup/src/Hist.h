///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Hist.h 
// Header file for class AthEx::Hist
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXHISTNTUP_ATHEXHIST_H
#define ATHEXHISTNTUP_ATHEXHIST_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"

// fwd declares
class TH1F;

namespace AthEx {

class Hist
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  Hist( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~Hist(); 

  // Assignment operator: 
  //Hist &operator=(const Hist &alg); 

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
  Hist();

  /// handle to the n-tuple svc
  ServiceHandle<ITHistSvc> m_ntsvc;

  /// pointer to our histogram
  TH1F *m_hist;

  /// key to the event-info
  std::string m_evtInfoName;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> namespace AthEx

#endif //> !ATHEXHISTNTUP_ATHEXHIST_H
