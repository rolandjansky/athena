/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/EfficiencyResponseHistos.h"

#include "JetMonitoring/ToolHandleHistoHelper.h"
#include "JetUtils/JetDistances.h"

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"


#define toGeV 1/1000.


EfficiencyResponseHistos::EfficiencyResponseHistos(const std::string &t) : JetHistoBase(t) 
                                                           , m_histoDef(this)
{
  declareProperty("HistoDef", m_histoDef, "The list of HistoDefinitionTool defining the histos to be used in this tool"); 
  declareProperty("RefContainer", m_refContainerName);
}



StatusCode EfficiencyResponseHistos::initialize() {
  CHECK( m_histoDef.retrieve() );
  return StatusCode::SUCCESS;
}

int EfficiencyResponseHistos::buildHistos(){
  ATH_MSG_INFO(" buildHistos num of histos : "<< m_histoDef.size() );

  ToolHandleHistoHelper::HistoRetriever hbuilder(m_histoDef);

  // Histos are defined in jobOptions !
  // For each histo, ask hbuilder if a corresponding definition exists in the jobOption list.
  //  -> if so a valid histo is returned (and booked)
  //  -> else NULL is returned
  m_eff1 = bookHisto( hbuilder.build<TProfile>("erhEfficiencyR1") );
  m_eff2 = bookHisto( hbuilder.build<TProfile>("erhEfficiencyR2") );
  m_eff3 = bookHisto( hbuilder.build<TProfile>("erhEfficiencyR3") );

  m_etres = bookHisto( hbuilder.build<TH1F>("erhResponse") );
  m_etres_eta =bookHisto( hbuilder.build<TProfile>("erhResponseVsEta") );
  m_etres_pt =bookHisto( hbuilder.build<TProfile>("erhResponseVsPt") );

  m_deltaRclosest = bookHisto( hbuilder.build<TH1F>("erhDeltaR") );


  return 0;
}


int EfficiencyResponseHistos::fillHistosFromContainer(const xAOD::JetContainer &cont){

  const xAOD::JetContainer* refContainer = 0;
  CHECK( evtStore()->retrieve( refContainer, m_refContainerName), 1 );
  /// use a list to be a bit more efficient.
  std::list<const xAOD::Jet*> listJets(cont.begin(), cont.end());

  for ( const xAOD::Jet* refjet : *refContainer ){
    double dr2min = 500000;

    if (listJets.empty() ) break;
    // find the min match
    std::list<const xAOD::Jet*>::iterator it=listJets.begin();
    std::list<const xAOD::Jet*>::iterator itmin=listJets.end();
    for( ; it != listJets.end(); ++it) {
      double dr2 = jet::JetDistances::deltaR2(*(*it),*refjet);
      if(dr2 < dr2min) { dr2min = dr2; itmin = it ;}
    }
    const xAOD::Jet* matched = *itmin;
    listJets.erase(itmin);
    
    double dr = sqrt(dr2min);
    double refPt = refjet->pt() * toGeV;

    m_eff1->Fill(refPt, dr<0.1 ?  1 : 0 ); // 0 weight if not matching close enough
    m_eff2->Fill(refPt, dr<0.2 ?  1 : 0 ); // 0 weight if not matching close enough
    m_eff3->Fill(refPt, dr<0.3 ?  1 : 0 ); // 0 weight if not matching close enough
    
    m_deltaRclosest->Fill( dr );

    if( dr < 0.3) {
      double relDiff = ( matched->pt()* toGeV - refPt )/refPt;
      m_etres->Fill( relDiff );
      m_etres_eta->Fill( refjet->eta(), relDiff);
      m_etres_pt->Fill( refPt, relDiff);

    }

  }

  
  return 0;
}
