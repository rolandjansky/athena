/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "JetMonitoring/JetHistoResponseAndEff.h"

#include "JetMonitoring/JetMonitoringAlg.h"
#include "FourMomUtils/xAODP4Helpers.h"

JetHistoResponseAndEff::JetHistoResponseAndEff( const std::string& type,  const std::string & name ,const IInterface* parent):
  AthAlgTool( type, name, parent ),
  m_refContainerKey("REFCONTAINER")  
{
  declareInterface<IJetHistoFiller>(this);
  declareProperty("RefContainerName",m_refContainerKey);  
}


StatusCode JetHistoResponseAndEff::initialize() {
  ATH_CHECK( m_refContainerKey.initialize() );
  ATH_MSG_DEBUG(" initialize  "<< m_refContainerKey.key());
  return StatusCode::SUCCESS;
}


StatusCode JetHistoResponseAndEff::processJetContainer(const JetMonitoringAlg& parentAlg, const xAOD::JetContainer & jets, const EventContext& ctx ) const {
  if(jets.empty()) return StatusCode::SUCCESS;
  
  // retrieve the reference jet container (typically : truth jets)
  SG::ReadHandle<xAOD::JetContainer> refJets(m_refContainerKey, ctx);    
  if (! refJets.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain jet Collection with name "<< m_refContainerKey.key());
    return StatusCode::FAILURE;
  }


  // declare the variables to be monitored by the framework
  Monitored::Scalar<float> refPt("refPt");  
  Monitored::Scalar<float> refEta("refEta");  
  Monitored::Scalar<float> relDiff("relDiff");  
  Monitored::Scalar<float> closestDr("closestDr");  
  Monitored::Scalar<bool> passDr1("passDr1");  
  Monitored::Scalar<bool> passDr2("passDr2");  
  Monitored::Scalar<bool> passDr3("passDr3");  

  
  // use a list to be a bit more efficient.
  std::list<const xAOD::Jet*> listJets(jets.begin(), jets.end());

  for ( const xAOD::Jet* refjet : *refJets ){
    double dr2min = 500000;

    if (listJets.empty() ) break;
    // find the min match
    std::list<const xAOD::Jet*>::iterator it=listJets.begin();
    std::list<const xAOD::Jet*>::iterator itmin=listJets.end();
    for( ; it != listJets.end(); ++it) {
      double dr2 = xAOD::P4Helpers::deltaR2(*(*it),*refjet);
      if(dr2 < dr2min) { dr2min = dr2; itmin = it ;}
    }

    // calculate efficiency and response from matched jet
    const xAOD::Jet* matched = *itmin;
    listJets.erase(itmin);
    
    double dr = sqrt(dr2min);
    refPt = refjet->pt() * m_energyScale;
    closestDr = dr;

    // Set flag passDrX if (reco,truth) jets are closer than deltaR=0.X 
    passDr1 = dr<0.1;
    passDr2 = dr<0.2;
    passDr3 = dr<0.3;
    // using these flags, the framework will fill 3 TEfficiency to draw ( efficiency_at_drX vs pT) (see also configuration) :
    parentAlg.fill(m_group, refPt , passDr1, passDr2, passDr3 , closestDr);         

    if( dr < 0.3) {
      if (refPt > 0.) relDiff = ( matched->pt()* m_energyScale - refPt )/refPt;
      else relDiff = -999;
      refEta = refjet->eta();
      parentAlg.fill(m_group, refPt , refEta, relDiff ); 
    }

  }
  
  return StatusCode::SUCCESS;
}


