///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhysValMET.h 
// Header file for class PhysValMET
// Author: Daniel Buescher <daniel.buescher@cern.ch>, Philipp Mogg <philipp.mogg@cern.ch>
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
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "EgammaAnalysisInterfaces/IAsgPhotonIsEMSelector.h"
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
  bool Accept(const xAOD::Jet* jet, double JvtCut);

  // vector of collections
  std::vector <std::string> types;

  // vector of terms
  std::vector <std::string> terms;

  // Map for names
  std::map <std::string,std::string> names;

  // Hists
  TH1D *m_MET_Track, *m_MET_Track_x, *m_MET_Track_y, *m_MET_Track_phi, *m_MET_Track_sum;
  TH1D *m_MET_PVTrack_Nominal, *m_MET_PVTrack_Nominal_x, *m_MET_PVTrack_Nominal_y, *m_MET_PVTrack_Nominal_phi, *m_MET_PVTrack_Nominal_sum;
  TH1D *m_MET_PVTrack_Pileup, *m_MET_PVTrack_Pileup_x, *m_MET_PVTrack_Pileup_y, *m_MET_PVTrack_Pileup_phi, *m_MET_PVTrack_Pileup_sum;

  //Maps
  std::map<std::string,std::vector<TH1D*> > m_MET_Ref;
  std::map<std::string,std::vector<TH1D*> > m_MET_Ref_x;
  std::map<std::string,std::vector<TH1D*> > m_MET_Ref_y;
  std::map<std::string,std::vector<TH1D*> > m_MET_Ref_phi;
  std::map<std::string,std::vector<TH1D*> > m_MET_Ref_sum;
  std::map<std::string,std::vector<TH1D*> > m_MET_Diff_Ref;
  std::map<std::string,std::vector<TH1D*> > m_MET_Diff_Ref_x;
  std::map<std::string,std::vector<TH1D*> > m_MET_Diff_Ref_y;
  std::map<std::string,std::vector<TH1D*> > m_MET_Diff_Ref_phi;
  std::map<std::string,std::vector<TH1D*> > m_MET_Diff_Ref_sum;
  std::map<std::string,std::vector<TH1D*> > m_MET_Cumu_Ref;
  std::map<std::string,std::vector<TH1D*> > m_MET_Resolution_Ref;
  std::map<std::string,std::vector<TH1D*> > m_MET_Significance_Ref;
  std::map<std::string,std::vector<TH1D*> > m_MET_dPhi_Ref;
  std::map<std::string,std::vector<TH2D*> > m_MET_CorrFinalTrk_Ref;
  std::map<std::string,std::vector<TH2D*> > m_MET_CorrFinalClus_Ref;
  std::map<std::string,std::vector<TH1D*> > m_MET_Reb;
  std::map<std::string,std::vector<TH1D*> > m_MET_Reb_x;
  std::map<std::string,std::vector<TH1D*> > m_MET_Reb_y;
  std::map<std::string,std::vector<TH1D*> > m_MET_Reb_phi;
  std::map<std::string,std::vector<TH1D*> > m_MET_Reb_sum;
  std::map<std::string,std::vector<TH1D*> > m_MET_Diff_Reb;
  std::map<std::string,std::vector<TH1D*> > m_MET_Diff_Reb_x;
  std::map<std::string,std::vector<TH1D*> > m_MET_Diff_Reb_y;
  std::map<std::string,std::vector<TH1D*> > m_MET_Diff_Reb_phi;
  std::map<std::string,std::vector<TH1D*> > m_MET_Diff_Reb_sum;
  std::map<std::string,std::vector<TH1D*> > m_MET_Cumu_Reb;
  std::map<std::string,std::vector<TH1D*> > m_MET_Resolution_Reb;
  std::map<std::string,std::vector<TH1D*> > m_MET_Significance_Reb;
  std::map<std::string,std::vector<TH1D*> > m_MET_dPhi_Reb;
  std::map<std::string,std::vector<TH2D*> > m_MET_CorrFinalTrk_Reb;
  std::map<std::string,std::vector<TH2D*> > m_MET_CorrFinalClus_Reb;

  std::vector<std::string> dir_met;

  ToolHandle<CP::IMuonSelectionTool> m_muonSelTool;
  ToolHandle<IAsgElectronLikelihoodTool> m_elecSelLHTool;
  ToolHandle<IAsgPhotonIsEMSelector>     m_photonSelIsEMTool;
  ToolHandle<IJetUpdateJvt> m_jvtTool;
  ToolHandle<IMETMaker> m_metmaker;
  ToolHandle<IMETMaker> m_metmakerTopo;
  ToolHandle<IMETMaker> m_metmakerPFlow;
  ToolHandle<TauAnalysisTools::ITauSelectionTool> m_tauSelTool;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
}

#endif //> !MISSINGETDQA_PHYSVALMET_H
