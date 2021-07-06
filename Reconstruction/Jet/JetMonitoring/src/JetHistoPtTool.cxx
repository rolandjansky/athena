/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoringKernel/Monitored.h"
#include "JetMonitoring/JetHistoPtTool.h"
#include "JetMonitoring/JetMonitoringAlg.h"


JetHistoPtTool::JetHistoPtTool( const std::string& type,  const std::string & name ,const IInterface* parent):
  AthAlgTool( type, name, parent )

{
  declareInterface<IJetHistoFiller>(this);

  
}

StatusCode JetHistoPtTool::processJetContainer(const JetMonitoringAlg& parentAlg, const xAOD::JetContainer & jets, const EventContext& ) const {

  std::vector<float> jetPt_v;      
  for (const auto jetItr : jets) {
    jetPt_v.push_back(jetItr->pt()/1000.);        
  }
  auto jetPt = Monitored::Collection("jetPt", jetPt_v);      
  parentAlg.fill(m_group, jetPt);

  return StatusCode::SUCCESS;
}
