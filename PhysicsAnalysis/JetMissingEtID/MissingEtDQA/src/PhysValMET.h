///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhysValMET.h 
// Header file for class PhysValMET
// Author: Daniel Buescher <daniel.buescher@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MISSINGETDQA_PHYSVALMET_H
#define MISSINGETDQA_PHYSVALMET_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"

// Local includes
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

// Root includes
#include "TH1.h"

// Forward declaration

namespace MissingEtDQA {

class PhysValMET
  : public ManagedMonitorToolBase
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  PhysValMET( const std::string& type,
		  const std::string& name, 
		  const IInterface* parent );

  /// Destructor: 
  virtual ~PhysValMET(); 

  // Athena algtool's Hooks
  virtual StatusCode initialize();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();


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
  PhysValMET();

  // Containers
  std::string m_metName; 

  // Hists
  // TODO: use map (each one for met, set, mpx, ...)
  TH1D *m_MET_RefFinal, *m_MET_RefEle, *m_MET_RefGamma, *m_MET_RefTau, *m_MET_RefJet, *m_MET_Muons, *m_MET_SoftClus, *m_MET_PVSoftTrk; 
  TH1D *m_MET_RefFinal_x, *m_MET_RefEle_x, *m_MET_RefGamma_x, *m_MET_RefTau_x, *m_MET_RefJet_x, *m_MET_Muons_x, *m_MET_SoftClus_x, *m_MET_PVSoftTrk_x;
  TH1D *m_MET_RefFinal_y, *m_MET_RefEle_y, *m_MET_RefGamma_y, *m_MET_RefTau_y, *m_MET_RefJet_y, *m_MET_Muons_y, *m_MET_SoftClus_y, *m_MET_PVSoftTrk_y;
  TH1D *m_MET_RefFinal_phi, *m_MET_RefEle_phi, *m_MET_RefGamma_phi, *m_MET_RefTau_phi, *m_MET_RefJet_phi, *m_MET_Muons_phi, *m_MET_SoftClus_phi, *m_MET_PVSoftTrk_phi;
  TH1D *m_SET_RefFinal, *m_SET_RefEle, *m_SET_RefGamma, *m_SET_RefTau, *m_SET_RefJet, *m_SET_Muons, *m_SET_SoftClus, *m_SET_PVSoftTrk;
  TH1D *m_Resolution_TruthNonInt_RefFinal_METx, *m_Resolution_TruthNonInt_RefFinal_METy;
  
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
}

#endif //> !MISSINGETDQA_PHYSVALMET_H
