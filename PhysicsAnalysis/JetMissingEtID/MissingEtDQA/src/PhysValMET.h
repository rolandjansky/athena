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
  TH1D *m_Resolution_TruthNonInt_RebLC_METx, *m_Resolution_TruthNonInt_RebLC_METy, *m_Resolution_TruthNonInt_RebEM_METx, *m_Resolution_TruthNonInt_RebEM_METy, *m_Resolution_TruthNonInt_RebEMPFlow_METx, *m_Resolution_TruthNonInt_RebEMPFlow_METy;
  TH1D *m_MET_Track, *m_MET_Track_x, *m_MET_Track_y, *m_MET_Track_phi, *m_MET_Track_sum;
  TH1D *m_MET_PVTrack_Nominal, *m_MET_PVTrack_Nominal_x, *m_MET_PVTrack_Nominal_y, *m_MET_PVTrack_Nominal_phi, *m_MET_PVTrack_Nominal_sum;
  TH1D *m_MET_PVTrack_Pileup, *m_MET_PVTrack_Pileup_x, *m_MET_PVTrack_Pileup_y, *m_MET_PVTrack_Pileup_phi, *m_MET_PVTrack_Pileup_sum;
  TH1D *m_dPhi_leadJetMET, *m_dPhi_subleadJetMET, *m_dPhi_LepMET;
  TH1D *m_MET_significance_RebLC, *m_MET_significance_RebEM, *m_MET_significance_RebEMPFlow;

  std::vector<TH1D*> m_MET_RefLC;
  std::vector<TH1D*> m_MET_RefLC_x;
  std::vector<TH1D*> m_MET_RefLC_y;
  std::vector<TH1D*> m_MET_RefLC_phi;
  std::vector<TH1D*> m_MET_RefLC_sum;
  std::vector<TH1D*> m_MET_RefEM;
  std::vector<TH1D*> m_MET_RefEM_x;
  std::vector<TH1D*> m_MET_RefEM_y;
  std::vector<TH1D*> m_MET_RefEM_phi;
  std::vector<TH1D*> m_MET_RefEM_sum;
  std::vector<TH1D*> m_MET_RefEMPFlow;
  std::vector<TH1D*> m_MET_RefEMPFlow_x;
  std::vector<TH1D*> m_MET_RefEMPFlow_y;
  std::vector<TH1D*> m_MET_RefEMPFlow_phi;
  std::vector<TH1D*> m_MET_RefEMPFlow_sum;
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
  std::vector<TH1D*> m_MET_RebEMPFlow;
  std::vector<TH1D*> m_MET_RebEMPFlow_x;
  std::vector<TH1D*> m_MET_RebEMPFlow_y;
  std::vector<TH1D*> m_MET_RebEMPFlow_phi;
  std::vector<TH1D*> m_MET_RebEMPFlow_sum;
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
  std::vector<TH2D*> m_MET_LC_CorrFinalTrk;
  std::vector<TH2D*> m_MET_EM_CorrFinalTrk;
  std::vector<TH2D*> m_MET_EMPFlow_CorrFinalTrk;
  std::vector<TH2D*> m_MET_LC_CorrFinalClus;
  std::vector<TH2D*> m_MET_EM_CorrFinalClus;
  std::vector<TH2D*> m_MET_EMPFlow_CorrFinalClus;
  std::vector<TH1D*> m_MET_Cumu;
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
