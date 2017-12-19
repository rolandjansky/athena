/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/HIEfficiencyResponseHistos.h"

#include "JetMonitoring/ToolHandleHistoHelper.h"
#include "JetUtils/JetDistances.h"

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"


#define toGeV 1/1000.
#define toTeV 1/1000000.

HIEfficiencyResponseHistos::HIEfficiencyResponseHistos(const std::string &t) : JetHistoBase(t) 
									     , m_histoDef(this)
{
  declareProperty("HistoDef", m_histoDef, "The list of HistoDefinitionTool defining the histos to be used in this tool"); 
  declareProperty("RefContainer", m_refContainerName);
  declareProperty("HIEventShapeContainerName", container_key="CaloSums");
}



StatusCode HIEfficiencyResponseHistos::initialize() {
  CHECK( m_histoDef.retrieve() );
  return StatusCode::SUCCESS;
}

int HIEfficiencyResponseHistos::buildHistos(){
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
  m_etres_eta_hpt =bookHisto( hbuilder.build<TProfile>("erhResponseVsEta_highpT") );
  m_etres_pt =bookHisto( hbuilder.build<TProfile>("erhResponseVsPt") );

  m_deltaRclosest = bookHisto( hbuilder.build<TH1F>("erhDeltaR") );
  ///////////////HI
  
  m_eff1_0_10 = bookHisto( hbuilder.build<TProfile>("erhEfficiencyR1_0_10") );
  m_eff2_0_10 = bookHisto( hbuilder.build<TProfile>("erhEfficiencyR2_0_10") );
  m_eff3_0_10 = bookHisto( hbuilder.build<TProfile>("erhEfficiencyR3_0_10") );

  m_eff1_10_20 = bookHisto( hbuilder.build<TProfile>("erhEfficiencyR1_10_20") );
  m_eff2_10_20 = bookHisto( hbuilder.build<TProfile>("erhEfficiencyR2_10_20") );
  m_eff3_10_20 = bookHisto( hbuilder.build<TProfile>("erhEfficiencyR3_10_20") );

  m_eff1_20_40 = bookHisto( hbuilder.build<TProfile>("erhEfficiencyR1_20_40") );
  m_eff2_20_40 = bookHisto( hbuilder.build<TProfile>("erhEfficiencyR2_20_40") );
  m_eff3_20_40 = bookHisto( hbuilder.build<TProfile>("erhEfficiencyR3_20_40") );

  m_eff1_60_100 = bookHisto( hbuilder.build<TProfile>("erhEfficiencyR1_60_100") );
  m_eff2_60_100 = bookHisto( hbuilder.build<TProfile>("erhEfficiencyR2_60_100") );
  m_eff3_60_100 = bookHisto( hbuilder.build<TProfile>("erhEfficiencyR3_60_100") );

  m_etres_0_10 = bookHisto( hbuilder.build<TH1F>("erhResponse_0_10") );
  m_etres_eta_0_10 =bookHisto( hbuilder.build<TProfile>("erhResponseVsEta_0_10") );
  m_etres_eta_hpt_0_10 =bookHisto( hbuilder.build<TProfile>("erhResponseVsEta_highpT_0_10") );
  m_etres_pt_0_10 =bookHisto( hbuilder.build<TProfile>("erhResponseVsPt_0_10") );

  m_etres_10_20 = bookHisto( hbuilder.build<TH1F>("erhResponse_10_20") );
  m_etres_eta_10_20 =bookHisto( hbuilder.build<TProfile>("erhResponseVsEta_10_20") );
  m_etres_eta_hpt_10_20 =bookHisto( hbuilder.build<TProfile>("erhResponseVsEta_highpT_10_20") );
  m_etres_pt_10_20 =bookHisto( hbuilder.build<TProfile>("erhResponseVsPt_10_20") );

  m_etres_20_40 = bookHisto( hbuilder.build<TH1F>("erhResponse_20_40") );
  m_etres_eta_20_40 =bookHisto( hbuilder.build<TProfile>("erhResponseVsEta_20_40") );
  m_etres_eta_hpt_20_40 =bookHisto( hbuilder.build<TProfile>("erhResponseVsEta_highpT_20_40") );
  m_etres_pt_20_40 =bookHisto( hbuilder.build<TProfile>("erhResponseVsPt_20_40") );

  m_etres_60_100 = bookHisto( hbuilder.build<TH1F>("erhResponse_60_100") );
  m_etres_eta_60_100 =bookHisto( hbuilder.build<TProfile>("erhResponseVsEta_60_100") );
  m_etres_eta_hpt_60_100 =bookHisto( hbuilder.build<TProfile>("erhResponseVsEta_highpT_60_100") );
  m_etres_pt_60_100 =bookHisto( hbuilder.build<TProfile>("erhResponseVsPt_60_100") );
  
  m_etres_pt_RP = bookHisto( hbuilder.build<TProfile>("erhResponse_RP") );
  m_etres_pt_2Dphi = bookHisto( hbuilder.build<TProfile>("erhResponse_2Dphi") );

  m_etres_pt_hpt_RP = bookHisto( hbuilder.build<TProfile>("erhResponse_highpT_RP") );
  m_etres_pt_hpt_2Dphi = bookHisto( hbuilder.build<TProfile>("erhResponse_highpT_2Dphi") );
  return 0;
}


int HIEfficiencyResponseHistos::fillHistosFromContainer(const xAOD::JetContainer &cont){
  n=2;
  harmonic=n-1;
  m_eventShape=nullptr;
  evtStore()->retrieve(m_eventShape,container_key);
  m_FCalET=0;
  m_psiN_FCal=0;
  //  m_vN_fcal=0;
  for(const xAOD::HIEventShape* sh : *m_eventShape){
    std::string summary;	
    if(sh->isAvailable<std::string>("Summary")) summary=sh->auxdata<std::string>("Summary");
    if(summary.compare("FCal")==0){
      m_FCalET=sh->et()*toTeV;
      float qx=sh->etCos().at(harmonic);
      float qy=sh->etSin().at(harmonic);
      m_psiN_FCal=std::atan2(qy,qx)/float(n); // Psi2 (n=2)
      //     vN_fcal=std::sqrt(qx+qx+qy*qy)/m_FCalET;
      break;
    }
  }

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

    if (fabs(refjet->eta()) > 2.1 ) continue;
    //   if (refPt<100 ) continue;
    m_eff1->Fill(refPt, dr<0.1 ?  1 : 0 ); // 0 weight if not matching close enough
    m_eff2->Fill(refPt, dr<0.2 ?  1 : 0 ); // 0 weight if not matching close enough
    m_eff3->Fill(refPt, dr<0.3 ?  1 : 0 ); // 0 weight if not matching close enough

    if (m_FCalET > 2.7){
      m_eff1_0_10->Fill(refPt, dr<0.1 ?  1 : 0 ); // 0 weight if not matching close enough
      m_eff2_0_10->Fill(refPt, dr<0.2 ?  1 : 0 ); // 0 weight if not matching close enough
      m_eff3_0_10->Fill(refPt, dr<0.3 ?  1 : 0 ); // 0 weight if not matching close enough
    }
    if (m_FCalET < 2.7 && m_FCalET > 1.75 ){//10-20%
      m_eff1_10_20->Fill(refPt, dr<0.1 ?  1 : 0 ); // 0 weight if not matching close enough
      m_eff2_10_20->Fill(refPt, dr<0.2 ?  1 : 0 ); // 0 weight if not matching close enough
      m_eff3_10_20->Fill(refPt, dr<0.3 ?  1 : 0 ); // 0 weight if not matching close enough
    }
    if (m_FCalET < 1.75 && m_FCalET > 0.65 ){//20-40%
      m_eff1_20_40->Fill(refPt, dr<0.1 ?  1 : 0 ); // 0 weight if not matching close enough
      m_eff2_20_40->Fill(refPt, dr<0.2 ?  1 : 0 ); // 0 weight if not matching close enough
      m_eff3_20_40->Fill(refPt, dr<0.3 ?  1 : 0 ); // 0 weight if not matching close enough
    }
    if (m_FCalET < 0.20 ){//60-100%
      m_eff1_60_100->Fill(refPt, dr<0.1 ?  1 : 0 ); // 0 weight if not matching close enough
      m_eff2_60_100->Fill(refPt, dr<0.2 ?  1 : 0 ); // 0 weight if not matching close enough
      m_eff3_60_100->Fill(refPt, dr<0.3 ?  1 : 0 ); // 0 weight if not matching close enough
    }
    m_deltaRclosest->Fill( dr );
    float Acos = std::acos(std::cos(2*(matched->getAttribute<float>("JetEtaJESScaleMomentum_phi") - m_psiN_FCal)));
    // float diff = fabs(matched->phi() - m_psiN_FCal);
    // while (diff > TMath::Pi()/2. ) diff = TMath::Pi() - diff;

    if( dr < 0.2) {
      double relDiff = ( matched->pt()* toGeV - refPt )/refPt;
      m_etres->Fill( relDiff );
      m_etres_eta->Fill( refjet->eta(), relDiff);
      if (matched->pt()* toGeV > 100) {
	m_etres_eta_hpt->Fill( refjet->eta(), relDiff);
	m_etres_pt_hpt_RP->Fill( m_psiN_FCal, relDiff );
	m_etres_pt_hpt_2Dphi->Fill( Acos, relDiff );
      }
      m_etres_pt->Fill( refPt, relDiff);
      m_etres_pt_2Dphi->Fill( Acos, relDiff );
      m_etres_pt_RP->Fill( m_psiN_FCal, relDiff );
      if (m_FCalET > 2.7){
	m_etres_0_10->Fill( relDiff );
	m_etres_eta_0_10->Fill( refjet->eta(), relDiff);
	if (matched->pt()* toGeV > 100) m_etres_eta_hpt_0_10->Fill( refjet->eta(), relDiff);
	m_etres_pt_0_10->Fill( refPt, relDiff);
      }
      if (m_FCalET < 2.7 && m_FCalET > 1.75 ){//10-20%
	m_etres_10_20->Fill( relDiff );
	m_etres_eta_10_20->Fill( refjet->eta(), relDiff);
	if (matched->pt()* toGeV > 100) m_etres_eta_hpt_10_20->Fill( refjet->eta(), relDiff);
	m_etres_pt_10_20->Fill( refPt, relDiff);
      }
      if (m_FCalET < 1.75 && m_FCalET > 0.65 ){//20-40%
	m_etres_20_40->Fill( relDiff );
	m_etres_eta_20_40->Fill( refjet->eta(), relDiff);
	if (matched->pt()* toGeV > 100) m_etres_eta_hpt_20_40->Fill( refjet->eta(), relDiff);
	m_etres_pt_20_40->Fill( refPt, relDiff);
      }
      if (m_FCalET < 0.20 ){//60-100%
	m_etres_60_100->Fill( relDiff );
	m_etres_eta_60_100->Fill( refjet->eta(), relDiff);
	if (matched->pt()* toGeV > 100) m_etres_eta_hpt_60_100->Fill( refjet->eta(), relDiff);
	m_etres_pt_60_100->Fill( refPt, relDiff);
      }

    }

  }

  
  return 0;
}
