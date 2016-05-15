//  -*- c++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORING_EFFICIENCYRESPONSEHISTOSHI_H
#define JETMONITORING_EFFICIENCYRESPONSEHISTOSHI_H

#include "GaudiKernel/ToolHandle.h"

#include "JetMonitoring/JetHistoBase.h"
#include "JetMonitoring/HistoDefinitionTool.h"

#include "xAODHIEvent/HIEventShapeContainer.h"

class HIEfficiencyResponseHistos : public JetHistoBase {
  ASG_TOOL_CLASS0(HIEfficiencyResponseHistos);
  
public:
  
  HIEfficiencyResponseHistos(const std::string &t);
  virtual ~HIEfficiencyResponseHistos(){};

  virtual StatusCode initialize() ;
  virtual int buildHistos();
  virtual int fillHistosFromContainer(const xAOD::JetContainer &cont);

  ///HI
private:
  std::string container_key;
  const xAOD::HIEventShapeContainer* m_eventShape;
  const xAOD::Jet* refContainer;

  /// Centrality
  float m_FCalET;
  float minCut;
  float maxCut;
  float m_psiN_FCal;
  float m_vN_fcal;
  unsigned int n;
  unsigned int harmonic;

protected:
  ToolHandleArray<HistoDefinitionTool> m_histoDef;
  std::string m_refContainerName;


  TProfile * m_eff1;
  TProfile * m_eff2;
  TProfile * m_eff3;

  TH1F* m_etres;
  TProfile* m_etres_eta;
  TProfile* m_etres_eta_hpt;
  TProfile* m_etres_pt;

  TProfile * m_eff1_0_10;
  TProfile * m_eff2_0_10;
  TProfile * m_eff3_0_10;

  TProfile * m_eff1_10_20;
  TProfile * m_eff2_10_20;
  TProfile * m_eff3_10_20;

  TProfile * m_eff1_20_40;
  TProfile * m_eff2_20_40;
  TProfile * m_eff3_20_40;

  TProfile * m_eff1_60_100;
  TProfile * m_eff2_60_100;
  TProfile * m_eff3_60_100;

  TH1F* m_etres_0_10;
  TProfile* m_etres_eta_0_10;
  TProfile* m_etres_eta_hpt_0_10;
  TProfile* m_etres_pt_0_10;

  TH1F* m_etres_10_20;
  TProfile* m_etres_eta_10_20;
  TProfile* m_etres_eta_hpt_10_20;
  TProfile* m_etres_pt_10_20;

  TH1F* m_etres_20_40;
  TProfile* m_etres_eta_20_40;
  TProfile* m_etres_eta_hpt_20_40;
  TProfile* m_etres_pt_20_40;

  TH1F* m_etres_60_100;
  TProfile* m_etres_eta_60_100;
  TProfile* m_etres_eta_hpt_60_100;
  TProfile* m_etres_pt_60_100;

  TProfile* m_etres_pt_RP;
  TProfile* m_etres_pt_hpt_RP;
  TProfile* m_etres_pt_2Dphi;
  TProfile* m_etres_pt_hpt_2Dphi;
  TH1F* m_deltaRclosest;

};


#endif
