//  -*- c++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORING_LEADINGJETRELATION_H
#define JETMONITORING_LEADINGJETRELATION_H


#include "GaudiKernel/ToolHandle.h"

#include "JetMonitoring/JetHistoBase.h"
#include "JetMonitoring/HistoDefinitionTool.h"

class LeadingJetsRelations : public JetHistoBase {
  ASG_TOOL_CLASS0(LeadingJetsRelations);
  
public:
  
  LeadingJetsRelations(const std::string &t);
  virtual ~LeadingJetsRelations(){};

  virtual StatusCode initialize() ;
  virtual int buildHistos();
  virtual int fillHistosFromContainer(const xAOD::JetContainer &cont);

protected:
  ToolHandleArray<HistoDefinitionTool> m_histoDef;
  TH1F * m_histDeltaR;
  TH1F * m_histDeltaEta;
  TH1F * m_histDeltaPhi;
  TH1F * m_histFrac;

  TH2F* m_histEta1Eta2;

};


#endif
