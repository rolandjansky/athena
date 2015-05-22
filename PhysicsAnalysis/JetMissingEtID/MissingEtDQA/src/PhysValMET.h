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
  TH1D *m_Resolution_TruthNonInt_RefFinal_METx, *m_Resolution_TruthNonInt_RefFinal_METy;
  TH1D *m_MET_Track, *m_MET_Track_x, *m_MET_Track_y, *m_MET_Track_phi, *m_MET_Track_sum;
  TH1D *m_MET_PVTrack_Nominal, *m_MET_PVTrack_Nominal_x, *m_MET_PVTrack_Nominal_y, *m_MET_PVTrack_Nominal_phi, *m_MET_PVTrack_Nominal_sum;
  TH1D *m_MET_PVTrack_Pileup, *m_MET_PVTrack_Pileup_x, *m_MET_PVTrack_Pileup_y, *m_MET_PVTrack_Pileup_phi, *m_MET_PVTrack_Pileup_sum;
  TH1D *m_dPhi_leadJetMET, *m_dPhi_subleadJetMET, *m_dPhi_LepMET;
  TH1D *m_MET_significance;
  
  std::vector<TH1D*> m_MET_RefFinal;
  std::vector<TH1D*> m_MET_RefFinal_x;
  std::vector<TH1D*> m_MET_RefFinal_y;
  std::vector<TH1D*> m_MET_RefFinal_phi;
  std::vector<TH1D*> m_MET_RefFinal_sum;
  std::vector<TH1D*> m_MET_EM;
  std::vector<TH1D*> m_MET_EM_x;
  std::vector<TH1D*> m_MET_EM_y;
  std::vector<TH1D*> m_MET_EM_phi;
  std::vector<TH1D*> m_MET_EM_sum;
  std::vector<std::string> dir_met;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
}

#endif //> !MISSINGETDQA_PHYSVALMET_H
