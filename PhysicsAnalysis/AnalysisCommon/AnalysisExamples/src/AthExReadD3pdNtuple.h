///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ReadD3pdNtuple.h 
// Header file for class ReadD3pdNtuple
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ANALYSISEXAMPLES_ATHEXREADD3PDNTUPLE_H
#define ANALYSISEXAMPLES_ATHEXREADD3PDNTUPLE_H 1

// STL includes
#include <string>
#include <vector>
#include <stdint.h>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"

// fwd declares
class TTree;

namespace AthEx {

class ReadD3pdNtuple
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  ReadD3pdNtuple( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~ReadD3pdNtuple(); 

  // Assignment operator: 
  //ReadD3pdNtuple &operator=(const ReadD3pdNtuple &alg); 

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
  ReadD3pdNtuple();

  /// initialize branches
  StatusCode initBranches(TTree *tree);

  // name of the TTree to read from THistSvc
  std::string m_treeName;

  // data to read from ttree
  uint32_t m_runnbr;
  uint32_t m_evtnbr;

  int32_t m_el_n;
  std::vector<float> *m_el_eta;
  std::vector<std::vector<float> > *m_el_jetcone_dr;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthEx

#endif //> !ANALYSISEXAMPLES_ATHEXREADD3PDNTUPLE_H
