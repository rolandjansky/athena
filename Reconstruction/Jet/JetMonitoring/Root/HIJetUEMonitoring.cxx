/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//HIJetUEMonitoring.cxx

#include "JetMonitoring/HIJetUEMonitoring.h"
#include "JetMonitoring/ToolHandleHistoHelper.h"

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

#define toGeV 1/1000.
#define toTeV 1/1000000.

HIJetUEMonitoring::HIJetUEMonitoring(const std::string& name)
  : JetHistoBase(name), m_histoDef(this), m_jetScale(xAOD::JetAssignedScaleMomentum) {
  declareProperty("HIEventShapeContainerName", container_key="CaloSums");
  declareProperty("HistoDef", m_histoDef, "The list of HistoDefinitionTool defining the histos to be used in this tool"); 
  declareProperty("RefContainer", m_refContainerName);
}

StatusCode HIJetUEMonitoring::initialize() {
  return StatusCode::SUCCESS;
}

int HIJetUEMonitoring::buildHistos(){
  ATH_MSG_INFO(" buildHistos num of histos : "<< m_histoDef.size() );

  ToolHandleHistoHelper::HistoRetriever hbuilder(m_histoDef);

  // Histos are defined in jobOptions !
  // For each histo, ask hbuilder if a corresponding definition exists in the jobOption list.
  //  -> if so a valid histo is returned (and booked)
  //  -> else NULL is returned
  //  m_peripheral = bookHisto( hbuilder.build<TH1F>("peripheral") );

  m_MaxOverMean_CentralColl = bookHisto( hbuilder.build<TH1F>("MaxOverMean_CentralColl") );
  m_MaxConstituentET_CentralColl = bookHisto( hbuilder.build<TH1F>("MaxConstituentET_CentralColl") );
  m_JetUnsubtractedScaleMomentum_pt_CentralColl = bookHisto( hbuilder.build<TH1F>("JetUnsubtractedScaleMomentum_pt_CentralColl") );
  m_JetUnsubtractedScaleMomentum_eta_CentralColl = bookHisto( hbuilder.build<TH1F>("JetUnsubtractedScaleMomentum_eta_CentralColl") );
  m_JetUnsubtractedScaleMomentum_phi_CentralColl = bookHisto( hbuilder.build<TH1F>("JetUnsubtractedScaleMomentum_phi_CentralColl") ); 
  m_JetUnsubtractedScaleMomentum_m_CentralColl = bookHisto( hbuilder.build<TH1F>("JetUnsubtractedScaleMomentum_m_CentralColl") ); 
  m_JetSubtractedScaleMomentum_pt_CentralColl = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleMomentum_pt_CentralColl") );
  m_JetSubtractedScaleMomentum_eta_CentralColl = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleMomentum_eta_CentralColl") );
  m_JetSubtractedScaleMomentum_phi_CentralColl = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleMomentum_phi_CentralColl") ); 
  m_JetSubtractedScaleMomentum_m_CentralColl = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleMomentum_m_CentralColl") ); 
  m_JetSubtractedScaleNoVnMomentum_pt_CentralColl = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleNoVnMomentum_pt_CentralColl") );
  m_JetSubtractedScaleNoVnMomentum_eta_CentralColl = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleNoVnMomentum_eta_CentralColl") );
  m_JetSubtractedScaleNoVnMomentum_phi_CentralColl = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleNoVnMomentum_phi_CentralColl") ); 
  m_JetSubtractedScaleNoVnMomentum_m_CentralColl = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleNoVnMomentum_m_CentralColl") ); 


  m_SubtractedET_Centrality = bookHisto( hbuilder.build<TProfile>("SubtractedET_Centrality") ); 
  m_2d_SubtractedET_Centrality = bookHisto( hbuilder.build<TH2F>("2dSubtractedET_Centrality") ); 
  m_2d_SubtractedETNoVN_Centrality = bookHisto( hbuilder.build<TH2F>("2dSubtractedETNoVn_Centrality") ); 
  m_2d_UnSubtractedETNoVN_Centrality = bookHisto( hbuilder.build<TH2F>("2dUnSubtractedETNoVN_Centrality") );
  m_2d_SubtractedET_Expected_Centrality = bookHisto( hbuilder.build<TH2F>("2dSubtractedET_Expected_Centrality") ); 
  m_2d_SubtractedETNoVN_Expected_Centrality = bookHisto( hbuilder.build<TH2F>("2dSubtractedETNoVn_Expected_Centrality") ); 
  m_2d_UnSubtractedETNoVN_Expected_Centrality = bookHisto( hbuilder.build<TH2F>("2dUnSubtractedETNoVN_Expected_Centrality") );

  m_2d_SubtractedET_Expected_eta = bookHisto( hbuilder.build<TH2F>("2dSubtractedET_Expected_eta") ); 
  m_2d_SubtractedETNoVN_Expected_eta = bookHisto( hbuilder.build<TH2F>("2dSubtractedETNoVn_Expected_eta") ); 
  m_2d_UnSubtractedETNoVN_Expected_eta = bookHisto( hbuilder.build<TH2F>("2dUnSubtractedETNoVN_Expected_eta") );
  m_SubtractedET_Expected_eta = bookHisto( hbuilder.build<TProfile>("SubtractedET_Expected_eta") ); 
  m_SubtractedETNoVN_Expected_eta = bookHisto( hbuilder.build<TProfile>("SubtractedETNoVn_Expected_eta") ); 
  m_UnSubtractedETNoVN_Expected_eta = bookHisto( hbuilder.build<TProfile>("UnSubtractedETNoVN_Expected_eta") );

  m_SubtractedET_eta = bookHisto( hbuilder.build<TProfile>("SubtractedET_eta") ); 
  m_SubtractedET_RP = bookHisto( hbuilder.build<TProfile>("SubtractedET_RP") ); 
  m_SubtractedET_2Dphi = bookHisto( hbuilder.build<TProfile>("SubtractedET_2Dphi") ); 
  m_2d_SubtractedET_2Dphi = bookHisto( hbuilder.build<TH2F>("2dSubtractedET_2Dphi") ); 
  m_SubtractedETNoVN_2Dphi = bookHisto( hbuilder.build<TProfile>("SubScMom-NoVn_2Dphi") ); 
  m_2d_SubtractedETNoVN_2Dphi = bookHisto( hbuilder.build<TH2F>("2dSubScMom-NoVn_2Dphi") ); 
  m_SubtractedET_eta_CentralColl = bookHisto( hbuilder.build<TProfile>("SubtractedET_eta_CentralColl") ); 
  m_SubtractedET_RP_CentralColl = bookHisto( hbuilder.build<TProfile>("SubtractedET_RP_CentralColl") ); 
  m_SubtractedET_2Dphi_CentralColl = bookHisto( hbuilder.build<TProfile>("SubtractedET_2Dphi_CentralColl") ); 
  m_2d_SubtractedET_2Dphi_CentralColl = bookHisto( hbuilder.build<TH2F>("2dSubtractedET_2Dphi_CentralColl") ); 
  m_SubtractedETNoVN_2Dphi_CentralColl = bookHisto( hbuilder.build<TProfile>("SubScMom-NoVn_2Dphi_CentralColl") ); 
  m_2d_SubtractedETNoVN_2Dphi_CentralColl = bookHisto( hbuilder.build<TH2F>("2dSubScMom-NoVn_2Dphi_CentralColl") ); 

  return 0;
}

int HIJetUEMonitoring::fillHistosFromJet(const xAOD::Jet &j){
  maxCut=2; //This must be changed.
  minCut=1;
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
      m_vN_fcal=std::sqrt(qx+qx+qy*qy)/m_FCalET;
      break;
    }
  }

  // m_jetScale is a property of the base tool
  //const xAOD::JetFourMom_t p4 = j.jetP4( (xAOD::JetScale) m_jetScale);

  float SubtractedET =  j.getAttribute<float>("JetUnsubtractedScaleMomentum_pt")*toGeV -  j.getAttribute<float>("JetSubtractedScaleMomentum_pt")*toGeV;
  float SubtractedETNoVN = j.getAttribute<float>("JetSubtractedScaleNoVnMomentum_pt")*toGeV - j.getAttribute<float>("JetSubtractedScaleMomentum_pt")*toGeV;
  float UnSubtractedETNoVN = j.getAttribute<float>("JetUnsubtractedScaleMomentum_pt")*toGeV - j.getAttribute<float>("JetSubtractedScaleNoVnMomentum_pt")*toGeV;

  float Acos = std::acos(std::cos(2*(j.getAttribute<float>("JetEtaJESScaleMomentum_phi") - m_psiN_FCal)));
  // float diff = fabs(j.getAttribute<float>("JetSubtractedScaleMomentum_phi") - m_psiN_FCal);
  // while (diff > TMath::Pi()/2. ) diff = TMath::Pi() - diff;

  if (j.getAttribute<float>("JetEtaJESScaleMomentum_pt")*toGeV > 80){
    m_SubtractedET_Centrality->Fill( m_FCalET, SubtractedET);
    m_2d_SubtractedET_Centrality->Fill( m_FCalET, SubtractedET);
    m_2d_SubtractedET_Expected_Centrality->Fill( m_FCalET, SubtractedET/(m_FCalET*0.025));
    m_2d_SubtractedET_Expected_eta->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_eta") , SubtractedET/(m_FCalET*0.025));
    m_SubtractedET_Expected_eta->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_eta") , SubtractedET/(m_FCalET*0.025));
    
    m_2d_SubtractedETNoVN_Centrality->Fill( m_FCalET, SubtractedETNoVN);
    m_2d_SubtractedETNoVN_Expected_Centrality->Fill( m_FCalET, SubtractedETNoVN/(m_FCalET*0.025));
    m_2d_SubtractedETNoVN_Expected_eta->Fill( j.getAttribute<float>("JetEtaJESScaleMomentum_eta"), SubtractedETNoVN/(m_FCalET*0.025));
    m_SubtractedETNoVN_Expected_eta->Fill( j.getAttribute<float>("JetEtaJESScaleMomentum_eta"), SubtractedETNoVN/(m_FCalET*0.025));

    m_2d_UnSubtractedETNoVN_Centrality->Fill( m_FCalET, UnSubtractedETNoVN);
    m_2d_UnSubtractedETNoVN_Expected_Centrality->Fill( m_FCalET, UnSubtractedETNoVN/(m_FCalET*0.025));
    m_2d_UnSubtractedETNoVN_Expected_eta->Fill( j.getAttribute<float>("JetEtaJESScaleMomentum_eta"), UnSubtractedETNoVN/(m_FCalET*0.025));
    m_UnSubtractedETNoVN_Expected_eta->Fill( j.getAttribute<float>("JetEtaJESScaleMomentum_eta"), UnSubtractedETNoVN/(m_FCalET*0.025));
    
    m_SubtractedET_RP->Fill( m_psiN_FCal,SubtractedET);
    m_SubtractedET_eta->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_eta"),SubtractedET ); 
    m_SubtractedET_2Dphi->Fill( Acos,SubtractedET);
    m_2d_SubtractedET_2Dphi->Fill( Acos,SubtractedET);
    m_SubtractedETNoVN_2Dphi->Fill(Acos,SubtractedETNoVN);
    m_2d_SubtractedETNoVN_2Dphi->Fill(Acos,SubtractedETNoVN);
  }

  ATH_MSG_INFO(" FCal ET, SubtractedET, JetUnsubtractedScaleMomentum_pt, JetSubtractedScaleMomentum_pt, eta  : "<< m_FCalET<<" "<<SubtractedET<<" "<<j.getAttribute<float>("JetEtaJESScaleMomentum_pt")<<" "<<j.getAttribute<float>("JetSubtractedScaleMomentum_pt")*toGeV<<" "<<m_psiN_FCal<<" "<<Acos <<" "<<m_vN_fcal);
  //ATH_MSG_INFO(" Teste : "<<p4.Eta()<<" "<<j.getAttribute<float>("JetSubtractedScaleMomentum_eta"));
  if (m_FCalET > 3){
    m_MaxOverMean_CentralColl->Fill( j.getAttribute<float>("MaxOverMean") );
    m_MaxConstituentET_CentralColl->Fill( j.getAttribute<float>("MaxConstituentET") );
    m_JetUnsubtractedScaleMomentum_pt_CentralColl->Fill( j.getAttribute<float>("JetUnsubtractedScaleMomentum_pt")*toGeV );
    m_JetUnsubtractedScaleMomentum_eta_CentralColl->Fill( j.getAttribute<float>("JetUnsubtractedScaleMomentum_eta") );
    m_JetUnsubtractedScaleMomentum_phi_CentralColl->Fill( j.getAttribute<float>("JetUnsubtractedScaleMomentum_phi") );
    m_JetUnsubtractedScaleMomentum_m_CentralColl->Fill( j.getAttribute<float>("JetUnsubtractedScaleMomentum_m")*toGeV );
    m_JetSubtractedScaleMomentum_pt_CentralColl->Fill( j.getAttribute<float>("JetSubtractedScaleMomentum_pt")*toGeV );
    m_JetSubtractedScaleMomentum_eta_CentralColl->Fill( j.getAttribute<float>("JetSubtractedScaleMomentum_eta") );
    m_JetSubtractedScaleMomentum_phi_CentralColl->Fill( j.getAttribute<float>("JetSubtractedScaleMomentum_phi") );
    m_JetSubtractedScaleMomentum_m_CentralColl->Fill( j.getAttribute<float>("JetSubtractedScaleMomentum_m")*toGeV );
    m_JetSubtractedScaleNoVnMomentum_pt_CentralColl->Fill( j.getAttribute<float>("JetSubtractedScaleNoVnMomentum_pt")*toGeV );
    m_JetSubtractedScaleNoVnMomentum_eta_CentralColl->Fill( j.getAttribute<float>("JetSubtractedScaleNoVnMomentum_eta") );
    m_JetSubtractedScaleNoVnMomentum_phi_CentralColl->Fill( j.getAttribute<float>("JetSubtractedScaleNoVnMomentum_phi") );
    m_JetSubtractedScaleNoVnMomentum_m_CentralColl->Fill( j.getAttribute<float>("JetSubtractedScaleNoVnMomentum_m")*toGeV );
  } 

  if (m_FCalET > minCut && m_FCalET < maxCut){
    m_SubtractedET_RP_CentralColl->Fill( m_psiN_FCal,SubtractedET);
    m_SubtractedET_eta_CentralColl->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_eta"),SubtractedET );
    m_SubtractedET_2Dphi_CentralColl->Fill( Acos,SubtractedET);
    m_2d_SubtractedET_2Dphi_CentralColl->Fill( Acos,SubtractedET);
    m_SubtractedETNoVN_2Dphi_CentralColl->Fill(Acos,SubtractedETNoVN);
    m_2d_SubtractedETNoVN_2Dphi_CentralColl->Fill(Acos,SubtractedETNoVN);
  }
  return 0;
}

 
