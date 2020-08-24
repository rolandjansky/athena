/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


This tool is intended to be able to make histograms of event level variables

*/

#ifndef JETHISTOEVENTLEVELFILLER_H
#define JETHISTOEVENTLEVELFILLER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "JetMonitoring/IJetHistoFiller.h"
#include "JetMonitoring/EventHistoVarTool.h"

class JetHistoEventLevelFiller : public AthAlgTool, virtual public IJetHistoFiller {
public:

  JetHistoEventLevelFiller( const std::string& type,  const std::string & name ,const IInterface* parent);
  virtual ~JetHistoEventLevelFiller(){}
  virtual StatusCode processJetContainer(const JetMonitoringAlg& parentAlg, const xAOD::JetContainer & jets, const EventContext& ctx) const ;  
  
private:

  Gaudi::Property<std::string> m_group {this,"Group", "undefined"};
  ToolHandle<IEventHistoVarTool> m_var;

};
#endif

