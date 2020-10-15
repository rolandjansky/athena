/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "JetMonitoring/JetHistoMatchedFiller.h"
#include "JetMonitoring/JetMonitoringAlg.h"
#include "AsgDataHandles/ReadDecorHandle.h"

JetHistoMatchedFiller::JetHistoMatchedFiller( const std::string& type,  const std::string & name ,const IInterface* parent):
  AthAlgTool( type, name, parent )
{
  declareInterface<IJetHistoFiller>(this);
  declareProperty("JetMatchedKey",m_matchedKey="NONE");
  declareProperty("JetPtDiffKey", m_ptdiffKey="NONE");
}


StatusCode JetHistoMatchedFiller::initialize() {
  ATH_CHECK( m_matchedKey.initialize() );
  ATH_CHECK( m_ptdiffKey.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode JetHistoMatchedFiller::processJetContainer(const JetMonitoringAlg& parentAlg, const xAOD::JetContainer & jets, const EventContext& ctx ) const {
  if(jets.empty()) return StatusCode::SUCCESS;

  SG::ReadDecorHandle<xAOD::JetContainer, char>  matchedHandle(m_matchedKey, ctx);
  SG::ReadDecorHandle<xAOD::JetContainer, double> ptdiffHandle(m_ptdiffKey, ctx);
  
  auto dPt = Monitored::Scalar<double>("ptdiff",0.0);

  // Loop over jets and fill pt difference between matched jets
  for(const xAOD::Jet* jet : jets){
    bool matched = matchedHandle(*jet);
    if(matched) dPt = ptdiffHandle(*jet);
  }
  
  parentAlg.fill(m_group,dPt); 
  
  return StatusCode::SUCCESS;
}


