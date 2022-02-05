//  -*- c++ -*- 

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORING_EFFICIENCYRESPONSEHISTOS_H
#define JETMONITORING_EFFICIENCYRESPONSEHISTOS_H

#include "GaudiKernel/ToolHandle.h"

#include "JetMonitoring/JetHistoBase.h"
#include "JetMonitoring/HistoDefinitionTool.h"

class EfficiencyResponseHistos : public JetHistoBase {
  ASG_TOOL_CLASS0(EfficiencyResponseHistos);
  
public:
  
  EfficiencyResponseHistos(const std::string &t);
  virtual ~EfficiencyResponseHistos(){};

  virtual StatusCode initialize() ;
  virtual int buildHistos();
  virtual int fillHistosFromContainer(const xAOD::JetContainer &cont, float weight);

protected:
  ToolHandleArray<HistoDefinitionTool> m_histoDef;
  std::string m_refContainerName;


  TProfile * m_eff1;
  TProfile * m_eff2;
  TProfile * m_eff3;

  TH1F* m_etres;
  TProfile* m_etres_eta;
  TProfile* m_etres_pt;

  TH1F* m_etres_noShift;
  TProfile* m_etres_noShift_eta;
  TProfile* m_etres_noShift_pt;

  TH1F* m_deltaRclosest;

};


#endif
