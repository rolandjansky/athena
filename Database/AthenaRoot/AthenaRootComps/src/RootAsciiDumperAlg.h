///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RootAsciiDumperAlg.h 
// Header file for class RootAsciiDumperAlg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAROOTCOMPS_ATHENA_ROOTASCIIDUMPERALG_H
#define ATHENAROOTCOMPS_ATHENA_ROOTASCIIDUMPERALG_H 1

// STL includes
#include <string>
#include <vector>
#include <stdint.h>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"

namespace Athena {

class RootAsciiDumperAlg
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  RootAsciiDumperAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~RootAsciiDumperAlg(); 

  // Assignment operator: 
  //RootAsciiDumperAlg &operator=(const RootAsciiDumperAlg &alg); 

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
  RootAsciiDumperAlg();

  /// ASCII output file name
  std::string m_ofname;

  /// file handle to the ASCII output file
  int m_ofd;
  
  /// number of entries processed so-far
  uint64_t m_nentries;

  /// run number
  const uint32_t* m_runnbr;

  /// event number
  const uint32_t* m_evtnbr;

  /// number of electrons
  const int32_t* m_el_n;

  /// eta of electrons
  const std::vector<float> *m_el_eta;

  /// jetcone dR
  const std::vector<std::vector<float> > *m_el_jetcone_dr;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace Athena
#endif //> !ATHENAROOTCOMPS_ATHENA_ROOTASCIIDUMPERALG_H
