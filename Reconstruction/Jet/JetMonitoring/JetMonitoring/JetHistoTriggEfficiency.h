// this file is -*- C++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTRIGGEFFICIENCY_H
#define JETTRIGGEFFICIENCY_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "JetMonitoring/IJetHistoFiller.h"
#include "JetMonitoring/JetHistoVarTool.h"
#include "JetInterface/IJetSelector.h"

////////////////////////////////////////////////////
/// \class JetHistoTriggEfficiency
///
///
class JetHistoTriggEfficiency : public AthAlgTool, virtual public IJetHistoFiller {
public:
  JetHistoTriggEfficiency( const std::string& type,  const std::string & name ,const IInterface* parent);
  virtual ~JetHistoTriggEfficiency(){}

  virtual StatusCode initialize() ;
  virtual StatusCode processJetContainer(const JetMonitoringAlg& parentAlg, const xAOD::JetContainer & jets, const EventContext& ctx) const;  

  
private:


  Gaudi::Property<std::string> m_group {this,"Group", "undefined"};
  Gaudi::Property<std::string> m_probeTrigChain {this, "ProbeTrigChain", "_undefined_"};
  Gaudi::Property<size_t> m_jetIndex {this, "Index",0};
  Gaudi::Property<bool> m_sortJets {this,"SortJets", false};
  ToolHandle<IJetSelector> m_selectTool;
  ToolHandle<IJetHistoVarTool> m_jetVar;
  
  
};
#endif
