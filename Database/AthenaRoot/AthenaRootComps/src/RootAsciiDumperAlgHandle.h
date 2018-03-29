///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RootAsciiDumperAlgHandle.h 
// Header file for class RootAsciiDumperAlgHandle
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAROOTCOMPS_ATHENA_ROOTASCIIDUMPERALGHANDLE_H
#define ATHENAROOTCOMPS_ATHENA_ROOTASCIIDUMPERALGHANDLE_H 1

// STL includes
#include <string>
#include <vector>
#include <stdint.h>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

namespace Athena {

class RootAsciiDumperAlgHandle
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  RootAsciiDumperAlgHandle( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~RootAsciiDumperAlgHandle(); 

  // Assignment operator: 
  //RootAsciiDumperAlgHandle &operator=(const RootAsciiDumperAlgHandle &alg); 

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
  RootAsciiDumperAlgHandle();

  /// ASCII output file name
  std::string m_ofname;

  /// file handle to the ASCII output file
  int m_ofd;
  
  /// number of entries processed so-far
  uint64_t m_nentries;

  /// run number
  SG::RVar<uint32_t> m_runnbr;

  /// event number
  SG::RVar<uint32_t> m_evtnbr;

  /// number of electrons
  SG::RVar<int32_t> m_el_n;

  /// eta of electrons
  SG::RVar<std::vector<float> > m_el_eta;

  /// jetcone dR
  SG::RVar<std::vector<std::vector<float> > > m_el_jetcone_dr;

  SG::ReadHandleKey<xAOD::EventInfo> m_eiKey;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace Athena
#endif //> !ATHENAROOTCOMPS_ATHENA_ROOTASCIIDUMPERALGHANDLE_H
