/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "JetMonitoring/JetHistoLeadingJetsRelations.h"

#include "JetMonitoring/JetMonitoringAlg.h"


JetHistoLeadingJetsRelations::JetHistoLeadingJetsRelations( const std::string& type,  const std::string & name ,const IInterface* parent):
  AthAlgTool( type, name, parent )
{
  declareInterface<IJetHistoFiller>(this);
  
}


StatusCode JetHistoLeadingJetsRelations::initialize() {

  return StatusCode::SUCCESS;
}

StatusCode JetHistoLeadingJetsRelations::processJetContainer(const JetMonitoringAlg& parentAlg, const xAOD::JetContainer & jets, const EventContext& ) const {
  if(jets.empty()) return StatusCode::SUCCESS;
  //
  
  if(jets.size()==1) return StatusCode::SUCCESS;


  // consider the two jets with the highest pt
  const xAOD::Jet* j1 = jets[0];
  const xAOD::Jet* j2 = jets[1];

  // define deta, dphi  and dr so that they are known to be monitored
  auto dEta= Monitored::Scalar<float>("dEta",0.0);
  // alternative way
  //Monitored::Scalar<float>dEta("dEta")
  auto dPhi= Monitored::Scalar<float>("dPhi",0.0);
  // alternative way
  //Monitored::Scalar<float>dEta("dPhi") 
  auto dR = Monitored::Scalar<float>("dR",0.0);
  //  Monitored::Scalar<float>dR("dR") 

  dEta= j1->eta() - j2->eta();
  dPhi  = fabs(j1->phi() - j2->phi() );
  if(dPhi > 2*3.14159) dPhi -= 2*3.14159;
  dR = sqrt( dPhi*dPhi + dEta*dEta);
  
  //  ToolHandle<GenericMonitoringTool> grpT = 
  parentAlg.fill(m_group,dEta,dPhi,dR); 
  
  return StatusCode::SUCCESS;
}


