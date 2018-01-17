/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//HIJetUEMonitoring.cxx

#include "JetMonitoring/HIJetUEMonitoring.h"
#include "JetMonitoring/ToolHandleHistoHelper.h"
#include "xAODEventInfo/EventInfo.h"

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

  m_FCALET = bookHisto( hbuilder.build<TH1F>("FCalET") ); 

 //inclusive
  m_SubtractedET_Centrality = bookHisto( hbuilder.build<TProfile>("SubtractedET_Centrality") ); 
  m_2dSubtractedET_Centrality = bookHisto( hbuilder.build<TH2F>("2dSubtractedET_Centrality") ); 
  m_2dSubtractedET_Expected_Centrality = bookHisto( hbuilder.build<TH2F>("2dSubtractedET_Expected_Centrality") ); 
  m_2dSubtractedET_Expected_eta = bookHisto( hbuilder.build<TH2F>("2dSubtractedET_Expected_eta") ); 
  m_SubtractedET_Expected_eta = bookHisto( hbuilder.build<TProfile>("SubtractedET_Expected_eta") ); 
  m_2dSubtractedET_2Dphi = bookHisto( hbuilder.build<TH2F>("2dSubtractedET_2Dphi") );
  m_SubtractedET_eta = bookHisto( hbuilder.build<TProfile>("SubtractedET_eta") ); 
  m_SubtractedET_pt = bookHisto( hbuilder.build<TProfile>("SubtractedET_pt") ); 
  m_SubtractedET_2Dphi = bookHisto( hbuilder.build<TProfile>("SubtractedET_2Dphi") ); 

  //0-10%
  m_JetUnsubtractedScaleMomentum_pt_0_10 = bookHisto( hbuilder.build<TH1F>("JetUnsubtractedScaleMomentum_pt_0_10") );
  m_JetUnsubtractedScaleMomentum_eta_0_10 = bookHisto( hbuilder.build<TH1F>("JetUnsubtractedScaleMomentum_eta_0_10") );
  m_JetUnsubtractedScaleMomentum_phi_0_10 = bookHisto( hbuilder.build<TH1F>("JetUnsubtractedScaleMomentum_phi_0_10") ); 
  m_JetUnsubtractedScaleMomentum_m_0_10 = bookHisto( hbuilder.build<TH1F>("JetUnsubtractedScaleMomentum_m_0_10") ); 
  m_JetSubtractedScaleMomentum_pt_0_10 = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleMomentum_pt_0_10") );
  m_JetSubtractedScaleMomentum_eta_0_10 = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleMomentum_eta_0_10") );
  m_JetSubtractedScaleMomentum_phi_0_10 = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleMomentum_phi_0_10") ); 
  m_JetSubtractedScaleMomentum_m_0_10 = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleMomentum_m_0_10") ); 

  m_2dSubtractedET_Expected_eta_0_10 = bookHisto( hbuilder.build<TH2F>("2dSubtractedET_Expected_eta_0_10") ); 
  m_SubtractedET_Expected_eta_0_10 = bookHisto( hbuilder.build<TProfile>("SubtractedET_Expected_eta_0_10") ); 

  m_2dSubtractedET_2Dphi_0_10 = bookHisto( hbuilder.build<TH2F>("2dSubtractedET_2Dphi_0_10") );
  m_SubtractedET_eta_0_10 = bookHisto( hbuilder.build<TProfile>("SubtractedET_eta_0_10") ); 
  m_SubtractedET_pt_0_10 = bookHisto( hbuilder.build<TProfile>("SubtractedET_pt_0_10") ); 

  m_SubtractedET_2Dphi_0_10 = bookHisto( hbuilder.build<TProfile>("SubtractedET_2Dphi_0_10") ); 

  // // //10-20%
  m_JetUnsubtractedScaleMomentum_pt_10_20 = bookHisto( hbuilder.build<TH1F>("JetUnsubtractedScaleMomentum_pt_10_20") );
  m_JetUnsubtractedScaleMomentum_eta_10_20 = bookHisto( hbuilder.build<TH1F>("JetUnsubtractedScaleMomentum_eta_10_20") );
  m_JetUnsubtractedScaleMomentum_phi_10_20 = bookHisto( hbuilder.build<TH1F>("JetUnsubtractedScaleMomentum_phi_10_20") ); 
  m_JetUnsubtractedScaleMomentum_m_10_20 = bookHisto( hbuilder.build<TH1F>("JetUnsubtractedScaleMomentum_m_10_20") ); 
  m_JetSubtractedScaleMomentum_pt_10_20 = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleMomentum_pt_10_20") );
  m_JetSubtractedScaleMomentum_eta_10_20 = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleMomentum_eta_10_20") );
  m_JetSubtractedScaleMomentum_phi_10_20 = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleMomentum_phi_10_20") ); 
  m_JetSubtractedScaleMomentum_m_10_20 = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleMomentum_m_10_20") ); 

  m_2dSubtractedET_Expected_eta_10_20 = bookHisto( hbuilder.build<TH2F>("2dSubtractedET_Expected_eta_10_20") ); 
  m_SubtractedET_Expected_eta_10_20 = bookHisto( hbuilder.build<TProfile>("SubtractedET_Expected_eta_10_20") ); 

  m_2dSubtractedET_2Dphi_10_20 = bookHisto( hbuilder.build<TH2F>("2dSubtractedET_2Dphi_10_20") );

  m_SubtractedET_eta_10_20 = bookHisto( hbuilder.build<TProfile>("SubtractedET_eta_10_20") ); 
  m_SubtractedET_pt_10_20 = bookHisto( hbuilder.build<TProfile>("SubtractedET_pt_10_20") ); 

  m_SubtractedET_2Dphi_10_20 = bookHisto( hbuilder.build<TProfile>("SubtractedET_2Dphi_10_20") ); 


  // //20-40%
  m_JetUnsubtractedScaleMomentum_pt_20_40 = bookHisto( hbuilder.build<TH1F>("JetUnsubtractedScaleMomentum_pt_20_40") );
  m_JetUnsubtractedScaleMomentum_eta_20_40 = bookHisto( hbuilder.build<TH1F>("JetUnsubtractedScaleMomentum_eta_20_40") );
  m_JetUnsubtractedScaleMomentum_phi_20_40 = bookHisto( hbuilder.build<TH1F>("JetUnsubtractedScaleMomentum_phi_20_40") ); 
  m_JetUnsubtractedScaleMomentum_m_20_40 = bookHisto( hbuilder.build<TH1F>("JetUnsubtractedScaleMomentum_m_20_40") ); 
  m_JetSubtractedScaleMomentum_pt_20_40 = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleMomentum_pt_20_40") );
  m_JetSubtractedScaleMomentum_eta_20_40 = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleMomentum_eta_20_40") );
  m_JetSubtractedScaleMomentum_phi_20_40 = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleMomentum_phi_20_40") ); 
  m_JetSubtractedScaleMomentum_m_20_40 = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleMomentum_m_20_40") ); 

  m_2dSubtractedET_Expected_eta_20_40 = bookHisto( hbuilder.build<TH2F>("2dSubtractedET_Expected_eta_20_40") ); 
  m_SubtractedET_Expected_eta_20_40 = bookHisto( hbuilder.build<TProfile>("SubtractedET_Expected_eta_20_40") ); 

  m_2dSubtractedET_2Dphi_20_40 = bookHisto( hbuilder.build<TH2F>("2dSubtractedET_2Dphi_20_40") );

  m_SubtractedET_eta_20_40 = bookHisto( hbuilder.build<TProfile>("SubtractedET_eta_20_40") ); 
  m_SubtractedET_pt_20_40 = bookHisto( hbuilder.build<TProfile>("SubtractedET_pt_20_40") ); 

  m_SubtractedET_2Dphi_20_40 = bookHisto( hbuilder.build<TProfile>("SubtractedET_2Dphi_20_40") ); 

  // //60-100%
  m_JetUnsubtractedScaleMomentum_pt_60_100 = bookHisto( hbuilder.build<TH1F>("JetUnsubtractedScaleMomentum_pt_60_100") );
  m_JetUnsubtractedScaleMomentum_eta_60_100 = bookHisto( hbuilder.build<TH1F>("JetUnsubtractedScaleMomentum_eta_60_100") );
  m_JetUnsubtractedScaleMomentum_phi_60_100 = bookHisto( hbuilder.build<TH1F>("JetUnsubtractedScaleMomentum_phi_60_100") ); 
  m_JetUnsubtractedScaleMomentum_m_60_100 = bookHisto( hbuilder.build<TH1F>("JetUnsubtractedScaleMomentum_m_60_100") ); 
  m_JetSubtractedScaleMomentum_pt_60_100 = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleMomentum_pt_60_100") );
  m_JetSubtractedScaleMomentum_eta_60_100 = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleMomentum_eta_60_100") );
  m_JetSubtractedScaleMomentum_phi_60_100 = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleMomentum_phi_60_100") ); 
  m_JetSubtractedScaleMomentum_m_60_100 = bookHisto( hbuilder.build<TH1F>("JetSubtractedScaleMomentum_m_60_100") ); 

  m_2dSubtractedET_Expected_eta_60_100 = bookHisto( hbuilder.build<TH2F>("2dSubtractedET_Expected_eta_60_100") ); 
  m_SubtractedET_Expected_eta_60_100 = bookHisto( hbuilder.build<TProfile>("SubtractedET_Expected_eta_60_100") ); 

  m_2dSubtractedET_2Dphi_60_100 = bookHisto( hbuilder.build<TH2F>("2dSubtractedET_2Dphi_60_100") );

  m_SubtractedET_eta_60_100 = bookHisto( hbuilder.build<TProfile>("SubtractedET_eta_60_100") ); 
  m_SubtractedET_pt_60_100 = bookHisto( hbuilder.build<TProfile>("SubtractedET_pt_60_100") ); 

  m_SubtractedET_2Dphi_60_100 = bookHisto( hbuilder.build<TProfile>("SubtractedET_2Dphi_60_100") ); 

  return 0;
}

int HIJetUEMonitoring::fillHistosFromJet(const xAOD::Jet &j){

  const xAOD::EventInfo* evtInfo;
  CHECK(evtStore()->retrieve( evtInfo ), 1);

//LAr event veto: skip events rejected by LAr
  if(evtInfo->errorState(xAOD::EventInfo::LAr)==xAOD::EventInfo::Error){
    ATH_MSG_DEBUG("SKIP for LAR error");
    return 1;
  }

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

  m_FCALET->Fill(m_FCalET);

  // m_jetScale is a property of the base tool
  //const xAOD::JetFourMom_t p4 = j.jetP4( (xAOD::JetScale) m_jetScale);

  float SubtractedET =  j.getAttribute<float>("JetUnsubtractedScaleMomentum_pt")*toGeV -  j.getAttribute<float>("JetSubtractedScaleMomentum_pt")*toGeV;

  float Acos = std::acos(std::cos(2*(j.getAttribute<float>("JetEtaJESScaleMomentum_phi") - m_psiN_FCal)));
  // float diff = fabs(j.getAttribute<float>("JetSubtractedScaleMomentum_phi") - m_psiN_FCal);
  // while (diff > TMath::Pi()/2. ) diff = TMath::Pi() - diff;

  if (fabs(j.getAttribute<float>("JetEtaJESScaleMomentum_eta"))<2.1){
    if (m_FCalET > 2.7){//0-10%
      m_SubtractedET_pt_0_10->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_pt")*toGeV,SubtractedET ); 
    }
    if (m_FCalET < 2.7 && m_FCalET > 1.75 ){//10-20%
      m_SubtractedET_pt_10_20->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_pt")*toGeV,SubtractedET ); 
    }
    if (m_FCalET < 1.75 && m_FCalET > 0.65 ){//20-40%
      m_SubtractedET_pt_20_40->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_pt")*toGeV,SubtractedET ); 
    }
    if (m_FCalET < 0.20 ){//60-100%
      m_SubtractedET_pt_60_100->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_pt")*toGeV,SubtractedET ); 
    }
  }

  if (j.getAttribute<float>("JetEtaJESScaleMomentum_pt")*toGeV > m_ptcut){
    m_SubtractedET_Centrality->Fill( m_FCalET, SubtractedET);
    m_2dSubtractedET_Centrality->Fill( m_FCalET, SubtractedET);
    m_2dSubtractedET_Expected_Centrality->Fill( m_FCalET, (SubtractedET/m_FCalET)*0.025);

    m_2dSubtractedET_Expected_eta->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_eta") , (SubtractedET/m_FCalET)*0.025);
    m_SubtractedET_Expected_eta->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_eta") , (SubtractedET/m_FCalET)*0.025);
    m_2dSubtractedET_2Dphi->Fill( Acos,SubtractedET);
    m_SubtractedET_eta->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_eta"),SubtractedET ); 
    m_SubtractedET_pt->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_pt")*toGeV,SubtractedET ); 
    m_SubtractedET_2Dphi->Fill( Acos,SubtractedET);

    if (m_FCalET > 2.7){//0-10%
      m_JetUnsubtractedScaleMomentum_pt_0_10->Fill( j.getAttribute<float>("JetUnsubtractedScaleMomentum_pt")*toGeV );
      m_JetUnsubtractedScaleMomentum_eta_0_10->Fill( j.getAttribute<float>("JetUnsubtractedScaleMomentum_eta") );
      m_JetUnsubtractedScaleMomentum_phi_0_10->Fill( j.getAttribute<float>("JetUnsubtractedScaleMomentum_phi") );
      m_JetUnsubtractedScaleMomentum_m_0_10->Fill( j.getAttribute<float>("JetUnsubtractedScaleMomentum_m")*toGeV );
      m_JetSubtractedScaleMomentum_pt_0_10->Fill( j.getAttribute<float>("JetSubtractedScaleMomentum_pt")*toGeV );
      m_JetSubtractedScaleMomentum_eta_0_10->Fill( j.getAttribute<float>("JetSubtractedScaleMomentum_eta") );
      m_JetSubtractedScaleMomentum_phi_0_10->Fill( j.getAttribute<float>("JetSubtractedScaleMomentum_phi") );
      m_JetSubtractedScaleMomentum_m_0_10->Fill( j.getAttribute<float>("JetSubtractedScaleMomentum_m")*toGeV );
      m_2dSubtractedET_Expected_eta_0_10->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_eta") , (SubtractedET/m_FCalET)*0.025);
      m_SubtractedET_Expected_eta_0_10->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_eta") , (SubtractedET/m_FCalET)*0.025);

      m_2dSubtractedET_2Dphi_0_10->Fill( Acos,SubtractedET);
      m_SubtractedET_eta_0_10->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_eta"),SubtractedET ); 
      m_SubtractedET_2Dphi_0_10->Fill( Acos,SubtractedET);
    }
    if (m_FCalET < 2.7 && m_FCalET > 1.75 ){//10-20%
      m_JetUnsubtractedScaleMomentum_pt_10_20->Fill( j.getAttribute<float>("JetUnsubtractedScaleMomentum_pt")*toGeV );
      m_JetUnsubtractedScaleMomentum_eta_10_20->Fill( j.getAttribute<float>("JetUnsubtractedScaleMomentum_eta") );
      m_JetUnsubtractedScaleMomentum_phi_10_20->Fill( j.getAttribute<float>("JetUnsubtractedScaleMomentum_phi") );
      m_JetUnsubtractedScaleMomentum_m_10_20->Fill( j.getAttribute<float>("JetUnsubtractedScaleMomentum_m")*toGeV );
      m_JetSubtractedScaleMomentum_pt_10_20->Fill( j.getAttribute<float>("JetSubtractedScaleMomentum_pt")*toGeV );
      m_JetSubtractedScaleMomentum_eta_10_20->Fill( j.getAttribute<float>("JetSubtractedScaleMomentum_eta") );
      m_JetSubtractedScaleMomentum_phi_10_20->Fill( j.getAttribute<float>("JetSubtractedScaleMomentum_phi") );
      m_JetSubtractedScaleMomentum_m_10_20->Fill( j.getAttribute<float>("JetSubtractedScaleMomentum_m")*toGeV );

      m_2dSubtractedET_Expected_eta_10_20->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_eta") , (SubtractedET/m_FCalET)*0.025);
      m_SubtractedET_Expected_eta_10_20->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_eta") , (SubtractedET/m_FCalET)*0.025);

      m_2dSubtractedET_2Dphi_10_20->Fill( Acos,SubtractedET);

      m_SubtractedET_eta_10_20->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_eta"),SubtractedET ); 
      m_SubtractedET_2Dphi_10_20->Fill( Acos,SubtractedET);
    }

    if (m_FCalET < 1.75 && m_FCalET > 0.65 ){//20-40%
      m_JetUnsubtractedScaleMomentum_pt_20_40->Fill( j.getAttribute<float>("JetUnsubtractedScaleMomentum_pt")*toGeV );
      m_JetUnsubtractedScaleMomentum_eta_20_40->Fill( j.getAttribute<float>("JetUnsubtractedScaleMomentum_eta") );
      m_JetUnsubtractedScaleMomentum_phi_20_40->Fill( j.getAttribute<float>("JetUnsubtractedScaleMomentum_phi") );
      m_JetUnsubtractedScaleMomentum_m_20_40->Fill( j.getAttribute<float>("JetUnsubtractedScaleMomentum_m")*toGeV );
      m_JetSubtractedScaleMomentum_pt_20_40->Fill( j.getAttribute<float>("JetSubtractedScaleMomentum_pt")*toGeV );
      m_JetSubtractedScaleMomentum_eta_20_40->Fill( j.getAttribute<float>("JetSubtractedScaleMomentum_eta") );
      m_JetSubtractedScaleMomentum_phi_20_40->Fill( j.getAttribute<float>("JetSubtractedScaleMomentum_phi") );
      m_JetSubtractedScaleMomentum_m_20_40->Fill( j.getAttribute<float>("JetSubtractedScaleMomentum_m")*toGeV );

      m_2dSubtractedET_Expected_eta_20_40->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_eta") , (SubtractedET/m_FCalET)*0.025);
      m_SubtractedET_Expected_eta_20_40->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_eta") , (SubtractedET/m_FCalET)*0.025);

      m_2dSubtractedET_2Dphi_20_40->Fill( Acos,SubtractedET);

      m_SubtractedET_eta_20_40->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_eta"),SubtractedET ); 
      m_SubtractedET_2Dphi_20_40->Fill( Acos,SubtractedET);
    }
    if (m_FCalET < 0.20 ){//60-100%
      m_JetUnsubtractedScaleMomentum_pt_60_100->Fill( j.getAttribute<float>("JetUnsubtractedScaleMomentum_pt")*toGeV );
      m_JetUnsubtractedScaleMomentum_eta_60_100->Fill( j.getAttribute<float>("JetUnsubtractedScaleMomentum_eta") );
      m_JetUnsubtractedScaleMomentum_phi_60_100->Fill( j.getAttribute<float>("JetUnsubtractedScaleMomentum_phi") );
      m_JetUnsubtractedScaleMomentum_m_60_100->Fill( j.getAttribute<float>("JetUnsubtractedScaleMomentum_m")*toGeV );
      m_JetSubtractedScaleMomentum_pt_60_100->Fill( j.getAttribute<float>("JetSubtractedScaleMomentum_pt")*toGeV );
      m_JetSubtractedScaleMomentum_eta_60_100->Fill( j.getAttribute<float>("JetSubtractedScaleMomentum_eta") );
      m_JetSubtractedScaleMomentum_phi_60_100->Fill( j.getAttribute<float>("JetSubtractedScaleMomentum_phi") );
      m_JetSubtractedScaleMomentum_m_60_100->Fill( j.getAttribute<float>("JetSubtractedScaleMomentum_m")*toGeV );

      m_2dSubtractedET_Expected_eta_60_100->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_eta") , (SubtractedET/m_FCalET)*0.025);
      m_SubtractedET_Expected_eta_60_100->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_eta") , (SubtractedET/m_FCalET)*0.025);

      m_2dSubtractedET_2Dphi_60_100->Fill( Acos,SubtractedET);

      m_SubtractedET_eta_60_100->Fill(j.getAttribute<float>("JetEtaJESScaleMomentum_eta"),SubtractedET ); 
      m_SubtractedET_2Dphi_60_100->Fill( Acos,SubtractedET);
    }

  }

  //  ATH_MSG_INFO(" FCal ET: "<< m_FCalET<<" SubtractedE:, "<<SubtractedET<<" m_psiN_FCal: "<<m_psiN_FCal<<" Acos: "<<Acos <<" m_vN_fcal: "<<m_vN_fcal<<" ptcut: "<<m_ptcut);
  //ATH_MSG_INFO(" Teste : "<<p4.Eta()<<" "<<j.getAttribute<float>("JetSubtractedScaleMomentum_eta"));
  return 0;
}

 
