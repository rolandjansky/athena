/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TOOL_H 
#define MUON_TOOL_H 

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "MissingETPerformance/MissingETMuonData.h"

#include <string>
#include <sstream>
#include "TH1.h"
#include "TH2.h"

static const InterfaceID IID_MuonTool("MuonTool", 1, 0);

class MuonTool : public AthAlgTool {

public:

  MuonTool(const std::string& type, const std::string& name,
	           const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_MuonTool; };

  StatusCode CBNT_initialize();

  /** Overriding initialize and finalize */
  virtual StatusCode initialize();
  /** finalization */
  virtual StatusCode finalize();
  virtual StatusCode execute(MissingETMuonData *muondata);

  StatusCode muonSetup(MissingETMuonData *muondata);

  inline void SetFolderName(const std::string& folderName) { m_folderName=folderName; }

protected:

   /** Standard destructor */
   virtual ~MuonTool( );

 private:
  std::string m_folderName;

  //storegate key names
  std::string _JetCollectionKey;
  std::string _CaloMuonCollectionKey;

  /** a handle on the Hist/TTree registration service */
  ITHistSvc * m_thistSvc;

  //histograms 

  //[0] = MuonBoy, [1] = MuTag [2] = CaloMuons

  TH2D *h_MC_2D_truthMatched_IsolationVsPt_LogLogMuonBoy;
  TH2D *h_MC_2D_truthMatched_IsolationVsPtMuonBoy;
  TH2D *h_MC_2D_truthUnmatched_IsolationVsPt_LogLogMuonBoy;
  TH2D *h_MC_2D_truthUnmatched_IsolationVsPtMuonBoy;
  TH2D *h_2D_jetMatched_IsolationVsPt_LogLogMuonBoy;
  TH2D *h_2D_jetMatched_IsolationVsPtMuonBoy;

  TH1D *h_eta_dist_allMuonBoy;
  TH1D *h_phi_dist_allMuonBoy;
  TH1D *h_Pt_dist_allMuonBoy;
  TH2D *h_2D_phiVsEta_allMuonBoy;

  TH1D *h_MC_eta_dist_truthMatchedMuonBoy;
  TH1D *h_MC_phi_dist_truthMatchedMuonBoy;
  TH1D *h_MC_Pt_dist_truthMatchedMuonBoy;
  TH2D *h_MC_2D_phiVsEta_truthMatchedMuonBoy;

  TH1D *h_MC_eta_dist_truthUnmatchedMuonBoy;
  TH1D *h_MC_phi_dist_truthUnmatchedMuonBoy;
  TH1D *h_MC_Pt_dist_truthUnmatchedMuonBoy;
  TH2D *h_MC_2D_phiVsEta_truthUnmatchedMuonBoy;

  TH1D *h_eta_dist_jetMatchedMuonBoy;
  TH1D *h_phi_dist_jetMatchedMuonBoy;
  TH1D *h_Pt_dist_jetMatchedMuonBoy;
  TH2D *h_2D_phiVsEta_jetMatchedMuonBoy;

  TH2D *h_MC_2D_truthMatched_IsolationVsPt_LogLogMutag;
  TH2D *h_MC_2D_truthMatched_IsolationVsPtMutag;
  TH2D *h_MC_2D_truthUnmatched_IsolationVsPt_LogLogMutag;
  TH2D *h_MC_2D_truthUnmatched_IsolationVsPtMutag;
  TH2D *h_2D_jetMatched_IsolationVsPt_LogLogMutag;
  TH2D *h_2D_jetMatched_IsolationVsPtMutag;

  TH1D *h_eta_dist_allMutag;
  TH1D *h_phi_dist_allMutag;
  TH1D *h_Pt_dist_allMutag;
  TH2D *h_2D_phiVsEta_allMutag;

  TH1D *h_MC_eta_dist_truthMatchedMutag;
  TH1D *h_MC_phi_dist_truthMatchedMutag;
  TH1D *h_MC_Pt_dist_truthMatchedMutag;
  TH2D *h_MC_2D_phiVsEta_truthMatchedMutag;

  TH1D *h_MC_eta_dist_truthUnmatchedMutag;
  TH1D *h_MC_phi_dist_truthUnmatchedMutag;
  TH1D *h_MC_Pt_dist_truthUnmatchedMutag;
  TH2D *h_MC_2D_phiVsEta_truthUnmatchedMutag;

  TH1D *h_eta_dist_jetMatchedMutag;
  TH1D *h_phi_dist_jetMatchedMutag;
  TH1D *h_Pt_dist_jetMatchedMutag;
  TH2D *h_2D_phiVsEta_jetMatchedMutag;

  TH2D *h_MC_2D_truthMatched_IsolationVsPt_LogLogCaloMuons;
  TH2D *h_MC_2D_truthMatched_IsolationVsPtCaloMuons;
  TH2D *h_MC_2D_truthUnmatched_IsolationVsPt_LogLogCaloMuons;
  TH2D *h_MC_2D_truthUnmatched_IsolationVsPtCaloMuons;
  TH2D *h_2D_jetMatched_IsolationVsPt_LogLogCaloMuons;
  TH2D *h_2D_jetMatched_IsolationVsPtCaloMuons;

  TH1D *h_eta_dist_allCaloMuons;
  TH1D *h_phi_dist_allCaloMuons;
  TH1D *h_Pt_dist_allCaloMuons;
  TH2D *h_2D_phiVsEta_allCaloMuons;

  TH1D *h_MC_eta_dist_truthMatchedCaloMuons;
  TH1D *h_MC_phi_dist_truthMatchedCaloMuons;
  TH1D *h_MC_Pt_dist_truthMatchedCaloMuons;
  TH2D *h_MC_2D_phiVsEta_truthMatchedCaloMuons;

  TH1D *h_MC_eta_dist_truthUnmatchedCaloMuons;
  TH1D *h_MC_phi_dist_truthUnmatchedCaloMuons;
  TH1D *h_MC_Pt_dist_truthUnmatchedCaloMuons;
  TH2D *h_MC_2D_phiVsEta_truthUnmatchedCaloMuons;

  TH1D *h_eta_dist_jetMatchedCaloMuons;
  TH1D *h_phi_dist_jetMatchedCaloMuons;
  TH1D *h_Pt_dist_jetMatchedCaloMuons;
  TH2D *h_2D_phiVsEta_jetMatchedCaloMuons;

  TH1D *h_MC_eta_dist_Truth;
  TH1D *h_MC_phi_dist_Truth;
  TH1D *h_MC_Pt_dist_Truth;
  TH2D *h_MC_2D_phiVsEta_Truth;

  TH1D *h_eta_dist_Jets;
  TH1D *h_phi_dist_Jets;
  TH1D *h_Pt_dist_Jets;
  TH2D *h_2D_phiVsEta_Jets;
};
#endif // MUON_TOOL_H
