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

using namespace xAOD;

class HIJetUEMonitoring : public JetHistoBase {
  ASG_TOOL_CLASS0(HIJetUEMonitoring);
 public:
  HIJetUEMonitoring(const std::string& name);
  ~HIJetUEMonitoring(){};
  virtual StatusCode initialize() ;
  virtual int buildHistos();
  virtual int fillHistosFromJet(const xAOD::Jet &j);
  //  virtual int getEventShape();

 private:
  std::string container_key;
  const xAOD::HIEventShapeContainer* m_eventShape;
  const xAOD::Jet* refContainer;

  /// Centrality
  float m_FCalET;
  float m_psiN_FCal;
  float m_vN_fcal;
  unsigned int n;
  unsigned int harmonic;
  float minCut;
  float maxCut;

 protected:
  ToolHandleArray<HistoDefinitionTool> m_histoDef;
  std::string m_refContainerName;

  TH1F* m_MaxOverMean_CentralColl;
  TH1F* m_MaxConstituentET_CentralColl;
  TH1F* m_JetUnsubtractedScaleMomentum_pt_CentralColl; 
  TH1F* m_JetUnsubtractedScaleMomentum_eta_CentralColl;
  TH1F* m_JetUnsubtractedScaleMomentum_phi_CentralColl;
  TH1F* m_JetUnsubtractedScaleMomentum_m_CentralColl;
  TH1F* m_JetSubtractedScaleMomentum_pt_CentralColl; 
  TH1F* m_JetSubtractedScaleMomentum_eta_CentralColl;
  TH1F* m_JetSubtractedScaleMomentum_phi_CentralColl;
  TH1F* m_JetSubtractedScaleMomentum_m_CentralColl;
  TH1F* m_JetSubtractedScaleNoVnMomentum_pt_CentralColl; 
  TH1F* m_JetSubtractedScaleNoVnMomentum_eta_CentralColl;
  TH1F* m_JetSubtractedScaleNoVnMomentum_phi_CentralColl;
  TH1F* m_JetSubtractedScaleNoVnMomentum_m_CentralColl;

  TProfile * m_SubtractedET_Centrality;
  TH2F * m_2d_SubtractedET_Centrality;
  TH2F * m_2d_SubtractedETNoVN_Centrality; 
  TH2F * m_2d_UnSubtractedETNoVN_Centrality; 
  TH2F * m_2d_SubtractedET_Expected_Centrality;
  TH2F * m_2d_SubtractedETNoVN_Expected_Centrality; 
  TH2F * m_2d_UnSubtractedETNoVN_Expected_Centrality; 
  TH2F * m_2d_SubtractedET_Expected_eta;
  TH2F * m_2d_SubtractedETNoVN_Expected_eta; 
  TH2F * m_2d_UnSubtractedETNoVN_Expected_eta; 
  TProfile * m_SubtractedET_Expected_eta;
  TProfile * m_SubtractedETNoVN_Expected_eta;
  TProfile * m_UnSubtractedETNoVN_Expected_eta;

  TProfile * m_SubtractedET_eta;
  TProfile * m_SubtractedET_RP;
  TProfile * m_SubtractedET_2Dphi;
  TH2F * m_2d_SubtractedET_2Dphi;
  TProfile * m_SubtractedETNoVN_2Dphi;
  TH2F * m_2d_SubtractedETNoVN_2Dphi;
  TProfile * m_SubtractedET_eta_CentralColl;
  TProfile * m_SubtractedET_RP_CentralColl;
  TProfile * m_SubtractedET_2Dphi_CentralColl;
  TH2F * m_2d_SubtractedET_2Dphi_CentralColl;
  TProfile * m_SubtractedETNoVN_2Dphi_CentralColl;
  TH2F * m_2d_SubtractedETNoVN_2Dphi_CentralColl;
  int m_jetScale;
};
	
	
#endif //JetMonitoring_HIJetUEMonitoring_h
