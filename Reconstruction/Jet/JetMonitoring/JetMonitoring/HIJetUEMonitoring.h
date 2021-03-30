/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//HIJetUEMonitoring.h
#ifndef JetMonitoring_HIJetUEMonitoring_h
#define JetMonitoring_HIJetUEMonitoring_h

#include "GaudiKernel/ToolHandle.h"
//#include "AthenaBaseComps/AthAlgTool.h"

#include "JetMonitoring/JetHistoBase.h"
#include "JetMonitoring/HistoDefinitionTool.h"

#include "xAODHIEvent/HIEventShapeContainer.h"

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

class HIJetUEMonitoring : public JetHistoBase {
  ASG_TOOL_CLASS0(HIJetUEMonitoring);
 public:
  HIJetUEMonitoring(const std::string& name);
  ~HIJetUEMonitoring(){};
  virtual StatusCode initialize() ;
  virtual int buildHistos();
  virtual int fillHistosFromJet(const xAOD::Jet &j, float weight);
  //  virtual int getEventShape();

 private:
  std::string m_container_key;
  const xAOD::HIEventShapeContainer* m_eventShape;

  /// Centrality
  float m_FCalET;
  float m_psiN_FCal;
  float m_vN_fcal;
  unsigned int m_n;
  unsigned int m_harmonic;
  float m_ptcut=60.0;

 protected:
  ToolHandleArray<HistoDefinitionTool> m_histoDef;
  std::string m_refContainerName;
  int m_jetScale;

  TH1F* m_FCALET;

  TProfile * m_SubtractedET_Centrality;
  TH2F * m_2dSubtractedET_Centrality;
  TH2F * m_2dSubtractedET_Expected_Centrality;
  TH2F * m_2dSubtractedET_pT;

  TH2F * m_2dSubtractedET_Expected_eta;
  TProfile * m_SubtractedET_Expected_eta;
  TH2F *     m_2dSubtractedET_2Dphi;
  TProfile * m_SubtractedET_eta;
  TProfile * m_SubtractedET_pt;
  TProfile * m_SubtractedET_2Dphi;

  //0-10%
  TH1F* m_JetUnsubtractedScaleMomentum_pt_0_10; 
  TH1F* m_JetUnsubtractedScaleMomentum_eta_0_10;
  TH1F* m_JetUnsubtractedScaleMomentum_phi_0_10;
  TH1F* m_JetUnsubtractedScaleMomentum_m_0_10;
  TH1F* m_JetSubtractedScaleMomentum_pt_0_10; 
  TH1F* m_JetSubtractedScaleMomentum_eta_0_10;
  TH1F* m_JetSubtractedScaleMomentum_phi_0_10;
  TH1F* m_JetSubtractedScaleMomentum_m_0_10;

  TH2F * m_2dSubtractedET_Expected_eta_0_10;
  TProfile * m_SubtractedET_Expected_eta_0_10;

  TH2F *     m_2dSubtractedET_2Dphi_0_10;

  TProfile * m_SubtractedET_eta_0_10;
  TProfile * m_SubtractedET_pt_0_10;

  TProfile * m_SubtractedET_2Dphi_0_10;

  /* //10-20% */
  TH1F* m_JetUnsubtractedScaleMomentum_pt_10_20; 
  TH1F* m_JetUnsubtractedScaleMomentum_eta_10_20;
  TH1F* m_JetUnsubtractedScaleMomentum_phi_10_20;
  TH1F* m_JetUnsubtractedScaleMomentum_m_10_20;
  TH1F* m_JetSubtractedScaleMomentum_pt_10_20; 
  TH1F* m_JetSubtractedScaleMomentum_eta_10_20;
  TH1F* m_JetSubtractedScaleMomentum_phi_10_20;
  TH1F* m_JetSubtractedScaleMomentum_m_10_20;

  TH2F * m_2dSubtractedET_Expected_eta_10_20;
  TProfile * m_SubtractedET_Expected_eta_10_20;

  TH2F *     m_2dSubtractedET_2Dphi_10_20;

  TProfile * m_SubtractedET_eta_10_20;
  TProfile * m_SubtractedET_pt_10_20;
  TProfile * m_SubtractedET_2Dphi_10_20;

  /* //20-40% */
  TH1F* m_JetUnsubtractedScaleMomentum_pt_20_40; 
  TH1F* m_JetUnsubtractedScaleMomentum_eta_20_40;
  TH1F* m_JetUnsubtractedScaleMomentum_phi_20_40;
  TH1F* m_JetUnsubtractedScaleMomentum_m_20_40;
  TH1F* m_JetSubtractedScaleMomentum_pt_20_40; 
  TH1F* m_JetSubtractedScaleMomentum_eta_20_40;
  TH1F* m_JetSubtractedScaleMomentum_phi_20_40;
  TH1F* m_JetSubtractedScaleMomentum_m_20_40;

  TH2F * m_2dSubtractedET_Expected_eta_20_40;
  TProfile * m_SubtractedET_Expected_eta_20_40;

  TH2F *     m_2dSubtractedET_2Dphi_20_40;

  TProfile * m_SubtractedET_eta_20_40;
  TProfile * m_SubtractedET_pt_20_40;
  TProfile * m_SubtractedET_2Dphi_20_40;

  /* //60-100% */
  TH1F* m_JetUnsubtractedScaleMomentum_pt_60_100; 
  TH1F* m_JetUnsubtractedScaleMomentum_eta_60_100;
  TH1F* m_JetUnsubtractedScaleMomentum_phi_60_100;
  TH1F* m_JetUnsubtractedScaleMomentum_m_60_100;
  TH1F* m_JetSubtractedScaleMomentum_pt_60_100; 
  TH1F* m_JetSubtractedScaleMomentum_eta_60_100;
  TH1F* m_JetSubtractedScaleMomentum_phi_60_100;
  TH1F* m_JetSubtractedScaleMomentum_m_60_100;

  TH2F * m_2dSubtractedET_Expected_eta_60_100;
  TProfile * m_SubtractedET_Expected_eta_60_100;

  TH2F *     m_2dSubtractedET_2Dphi_60_100;

  TProfile * m_SubtractedET_eta_60_100;
  TProfile * m_SubtractedET_pt_60_100;
  TProfile * m_SubtractedET_2Dphi_60_100;

};
	
	
#endif //JetMonitoring_HIJetUEMonitoring_h
