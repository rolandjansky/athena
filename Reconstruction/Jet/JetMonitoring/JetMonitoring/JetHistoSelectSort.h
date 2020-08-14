//  -*- c++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORING_JETHISTOSELECTOR_H
#define JETMONITORING_JETHISTOSELECTOR_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "JetInterface/IJetSelector.h"
#include "JetInterface/IJetEventSelector.h"

#include "JetMonitoring/IJetHistoFiller.h"
#include "JetMonitoring/JetHistoVarTool.h"



class JetHistoSelectSort : public AthAlgTool, virtual public IJetHistoFiller {
public:
  JetHistoSelectSort( const std::string& type,  const std::string & name ,const IInterface* parent);
  virtual ~JetHistoSelectSort(){}

  virtual StatusCode initialize() ;
  virtual StatusCode processJetContainer(const JetMonitoringAlg& parentAlg, const xAOD::JetContainer & jets, const EventContext& ctx) const;  

  
private:


  ToolHandleArray<IJetHistoFiller> m_jetFillerTools;

  ToolHandle<IJetSelector> m_selectTool;
  ToolHandle<IJetEventSelector> m_eventSelTool;
  Gaudi::Property<bool> m_inverseJetSel {this,"InverseJetSel", false};
  Gaudi::Property<int> m_selIndex {this,"SelectedIndex", -1};
  ToolHandle<IJetHistoVarTool> m_sortVar;

  
};
#endif
