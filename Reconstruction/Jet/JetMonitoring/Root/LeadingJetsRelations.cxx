/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/LeadingJetsRelations.h"
#include "JetMonitoring/ToolHandleHistoHelper.h"

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"



LeadingJetsRelations::LeadingJetsRelations(const std::string &t) : JetHistoBase(t) 
                                                           , m_histoDef(this)
{
  declareProperty("HistoDef", m_histoDef, "The list of HistoDefinitionTool defining the histos to be used in this tool"); 
}



StatusCode LeadingJetsRelations::initialize() {
  CHECK( m_histoDef.retrieve() );
  return StatusCode::SUCCESS;
}

int LeadingJetsRelations::buildHistos(){
  ATH_MSG_INFO(" buildHistos num of histos : "<< m_histoDef.size() );

  ToolHandleHistoHelper::HistoRetriever hbuilder(m_histoDef);

  // Histos are defined in jobOptions !
  // For each histo, ask hbuilder if a corresponding definition exists in the jobOption list.
  //  -> if so a valid histo is returned (and booked)
  //  -> else NULL is returned
  m_histDeltaR   = bookHisto( hbuilder.build<TH1F>("ljrDeltaR") );
  m_histDeltaEta = bookHisto( hbuilder.build<TH1F>("ljrDeltaEta") );
  m_histDeltaPhi = bookHisto( hbuilder.build<TH1F>("ljrDeltaPhi") );
  m_histFrac     = bookHisto( hbuilder.build<TH1F>("ljrFrac") );
  m_histEta1Eta2 = bookHisto( hbuilder.build<TH2F>("ljrEta1Eta2") );


  return 0;
}


int LeadingJetsRelations::fillHistosFromContainer(const xAOD::JetContainer &cont, float weight){
  if( cont.size()<2) return 0;

  const xAOD::Jet * j1 = cont[0];
  const xAOD::Jet * j2 = cont[1];

  double dEta = j1->eta() - j2->eta();
  double dPhi  = fabs(j1->phi() - j2->phi() );
  if(dPhi > 2*3.14159) dPhi -= 2*3.14159;
  double dR = sqrt( dPhi*dPhi + dEta*dEta);
  double histFrac = 0.;
  if (j1->pt() > 0.) histFrac = j2->pt()/j1->pt();
  
  if(m_histDeltaEta) m_histDeltaEta->Fill( dEta, weight);
  if(m_histDeltaPhi) m_histDeltaPhi->Fill( dPhi, weight);
  if(m_histDeltaR) m_histDeltaR->Fill( dR, weight);

  if(m_histFrac) m_histFrac->Fill( histFrac, weight);

  if(m_histEta1Eta2) m_histEta1Eta2->Fill( j1->eta(), j2->eta(), weight );

  return 0;
}




