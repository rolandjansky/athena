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

#include "MuonSelectorTools/IMuonSelectionTool.h"
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"
#include "ElectronPhotonSelectorTools/IAsgPhotonIsEMSelector.h"
#include "JetInterface/IJetUpdateJvt.h"
#include "METInterface/IMETMaker.h"
#include "TauAnalysisTools/ITauSelectionTool.h"

#include "AsgTools/ToolHandle.h"

// Root includes
#include "TH1.h"

// Forward declaration
class IMETMaker;
class IAsgElectronLikelihoodTool;
class IAsgPhotonIsEMSelector;
namespace CP {
  class IMuonSelectionTool;
}
namespace TauAnalysisTools {
  class ITauSelectionTool;
}

using namespace xAOD;

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
	      const IInterface* parent);

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
  bool m_doTruth;

  /// Default constructor: 
  PhysValMET();

  // Containers
  std::string m_metName;
  std::string m_jetColl;
  std::string m_eleColl;
  std::string m_gammaColl;
  std::string m_tauColl;
  std::string m_muonColl;
  std::string m_mapname;
  std::string m_corename;

  // Methods
  bool Accept(const xAOD::Electron* el);
  bool Accept(const xAOD::Photon* ph);
  bool Accept(const xAOD::TauJet* tau);
  bool Accept(const xAOD::Muon* muon);
  bool Accept(const xAOD::Jet* jet);

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
  std::vector<TH1D*> m_MET_RebLC;
  std::vector<TH1D*> m_MET_RebLC_x;
  std::vector<TH1D*> m_MET_RebLC_y;
  std::vector<TH1D*> m_MET_RebLC_phi;
  std::vector<TH1D*> m_MET_RebLC_sum;
  std::vector<TH1D*> m_MET_RebEM;
  std::vector<TH1D*> m_MET_RebEM_x;
  std::vector<TH1D*> m_MET_RebEM_y;
  std::vector<TH1D*> m_MET_RebEM_phi;
  std::vector<TH1D*> m_MET_RebEM_sum;
  std::vector<TH1D*> m_MET_DiffRef;
  std::vector<TH1D*> m_MET_DiffRef_x;
  std::vector<TH1D*> m_MET_DiffRef_y;
  std::vector<TH1D*> m_MET_DiffRef_phi;
  std::vector<TH1D*> m_MET_DiffRef_sum;
  std::vector<TH1D*> m_MET_DiffReb;
  std::vector<TH1D*> m_MET_DiffReb_x;
  std::vector<TH1D*> m_MET_DiffReb_y;
  std::vector<TH1D*> m_MET_DiffReb_phi;
  std::vector<TH1D*> m_MET_DiffReb_sum;
  std::vector<std::string> dir_met;

  ToolHandle<CP::IMuonSelectionTool> m_muonSelTool;
  ToolHandle<IAsgElectronLikelihoodTool> m_elecSelLHTool;
  ToolHandle<IAsgPhotonIsEMSelector>     m_photonSelIsEMTool;
  ToolHandle<IJetUpdateJvt> m_jvtTool;
  ToolHandle<IMETMaker> m_metmaker;
  ToolHandle<TauAnalysisTools::ITauSelectionTool> m_tauSelTool;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
}

#endif //> !MISSINGETDQA_PHYSVALMET_H
