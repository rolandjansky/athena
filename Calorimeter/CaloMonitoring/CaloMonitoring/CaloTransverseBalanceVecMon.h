/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// NAME:     CaloTransverseBalanceVecMon.h
//
// AUTHORS:  FENG TIAN (COLUMBIA UNIVERSITY) 
//
//  Modified: L. Sawyer 12/2013 
//                 New ATHENA monitoring uses managed histograms - time granularity no longer needed in user code
// ********************************************************************


#ifndef CALOTRANSVERSEBALANCE_H
#define CALOTRANSVERSEBALANCE_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IToolSvc.h"
#include "AthContainers/ConstDataVector.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

#include "egammaEvent/PhotonContainer.h"
#include "MissingETEvent/MissingET.h"

#include <string>
#include "TH1D.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TCanvas.h"
#include <vector>
#include <set>
#include <utility>
#include <iostream>

class CaloTransverseBalanceVecMon : public ManagedMonitorToolBase {
 public:

  CaloTransverseBalanceVecMon(const std::string & type, const std::string& name, 
		 const IInterface* parent);
   virtual ~CaloTransverseBalanceVecMon();

   virtual StatusCode initialize();
   virtual StatusCode bookHistogramsRecurrent();
   virtual StatusCode bookHistograms();
   virtual StatusCode fillHistograms();
   virtual StatusCode procHistograms();

 private:
   ToolHandle<Trig::TrigDecisionTool> m_trigDec;  
   // time granularity
  std::string m_timeGran;
   // switch for The algorithm LArNoisyAlg 
   bool m_useLArNoisyAlg;
 
 // useful functions
  float deltaR(float a_eta,float a_phi,float b_eta,float b_phi);
  void findLeadingPhoton(const PhotonContainer* userPhotonContainer,PhotonContainer::const_iterator& leadingPhPr);
  void findleadingANDsubleadingjets
    (const std::vector<const Jet*>& userJetContainer,
     Jet const* & leadingJetPr,
     Jet const* & subleadingJetPr);
  void getmissingEt_phi(float missingEx,float missingEy,float& missingEt_phi);
  void AdjustPhi(float& deltaphi);
  void  Plotregister(MonGroup* monitorGroup,TH1F* hist);
  void  Plotregister(MonGroup* monitorGroup,TH2F* hist); 
  void  Plotregister(MonGroup* monitorGroup,TH1I* hist);
  void  Plotregister(MonGroup* monitorGroup,TProfile* hist);
///////////////////////////////////////////////////
  float m_photonPtCut;

 ConstDataVector<PhotonContainer> * m_userPhotonContainer;
/// The missing ET object
 const MissingET * m_pMissing;
 std::vector<TH1F*> m_energy_s;
 std::string m_PhotonContainerName;
 std::string m_JetContainerName;
 std::string m_missingEtObjectName;
  MonGroup * m_theTransverseBalance_shift;
// variables
  long m_cutflow_j_1;
  long m_cutflow_j_2;
  long m_cutflow_j_3;
  bool m_TriggerCut;
//histos
  TH1F* m_h_photon_pt;
  TH1F* m_h_photon_eta;
  TH1F* m_h_photon_phi;
  TH1F* m_h_jet_pt_beforeoverlap;
  TH1F* m_h_jet_eta_beforeoverlap;
  TH1F* m_h_jet_phi_beforeoverlap;
  TH1F* m_h_jet_pt_afteroverlap;
  TH1F* m_h_jet_eta_afteroverlap;
  TH1F* m_h_jet_phi_afteroverlap;
  TH1I* m_h_njet_beforeoverlap;
  TH1I* m_h_njet_afteroverlap;
  TH1F* m_h_leadingJet_pt;
  TH1F* m_h_leadingJet_eta;
  TH1F* m_h_leadingJet_phi;
  TH1F* m_h_deltaPhi;
  TH2F* m_h2_ph_leadingJet_eta;
  TProfile* m_p_ptratioVsPhi_Barrel;
  TProfile* m_p_ptratioVsPhi_ECC;
  TProfile* m_p_ptratioVsPhi_ECA;
  TProfile* m_p_ptratioVsEta;
  TH2F* m_h2_ptratioVsPhi_Barrel; 
  TH2F* m_h2_ptratioVsPhi_ECC;
  TH2F* m_h2_ptratioVsPhi_ECA;
  TH2F* m_h2_ptratioVsEta;
  TH1F* m_h_overlapped_ph_jet_ptratio;
  TH1F* m_h_pt_ratio;
//photon-MET
  TH1F* m_h_leadingPh_MET_deltaPhi;
  TH2F* m_h2_leadingPh_MET_deltaPhiVsPt;
};

#endif // CaloTransverseBalanceVecMon_H
