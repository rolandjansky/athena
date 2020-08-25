/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// File:  Generators/FlowAfterburnber/CheckFlow_New.h
// Description:
//    This is a simple algorithm to histogram particle properties
//    for diagnosing of flow generation
//
//************************** THIS PROGRAM ANALYZES FILES WITH FIXED b_imp*********************
//
// AuthorList:
// Andrzej Olszewski: Initial Code February 2006
// Andrzej Olszewski: Converted to ROOT histograms July 2007
// Soumya Mohapatra : Re-written to check the new Flow implementations (JUNE 2011)

#include "FlowAfterburner/CheckFlow_New.h"
#include "GeneratorObjects/McEventCollection.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataSvc.h"

#include "GaudiKernel/ITHistSvc.h"

#include "TH1D.h"
#include "TProfile.h"

#include "TruthHelper/GenAccessIO.h"
#include "TruthHelper/IsGenStable.h"
#include "TruthHelper/GenAll.h"

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"

#include "GeneratorObjects/HijingEventParams.h"

using namespace TruthHelper;


/// @todo Migrate to a GenBase class or at least AthAlgorithm
CheckFlow_New::CheckFlow_New(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
  //Declare the algorithm's properties
  declareProperty("McEventKey",     m_key="FLOW_EVENT");
  declareProperty("HistogramFlag", m_produceHistogram = true );
  declareProperty("ImpactCutMin", m_bcut_min = 0 );
  declareProperty("ImpactCutMax", m_bcut_max = 99 );
  declareProperty("PtCutMin", m_ptcut_min = 0 );
  declareProperty("PtCutMax", m_ptcut_max = 999999 );
  declareProperty("RapidityCutMin", m_rapcut_min = 0 );
  declareProperty("RapidityCutMax", m_rapcut_max = 5.5 );

  m_profile_resolution=0;

  m_sgSvc = 0;
  m_tesIO = 0;
  m_thistSvc = 0;

  for ( int i = 0; i< 6; i++  ){
    m_hist_Psi_n_true[i] = 0;
    m_hist_Psi_n_reco[i] = 0;
    m_hist_Psi_n_ebe[i] = 0;
    m_hist_Psi_n_ebe_pt[i] = 0;
    m_hist_vn_ebe[i] = 0;
  }

  for ( int i = 0; i< 36; i++  ){
    m_hist_psi_corr_true[i] = 0;
    m_hist_psi_corr_reco[i] = 0;
  }


}





StatusCode CheckFlow_New::initialize(){
  StatusCode result = StatusCode::SUCCESS;
  
  msg(MSG::INFO) << ">>> CheckFlow_New from Initialize" << endmsg;

  float pt_binvals[n_ptbin+1]={0.0,0.25,0.5,1.0,1.5,2.0,2.5,3.0,3.5,4.0,6.0,8.0,12.0,16.0,20.0,30.0,40.0};
  float eta_bin_max = 4.0;


  StatusCode sc = service("StoreGateSvc", m_sgSvc);
  if (sc.isFailure()) {
     msg(MSG::ERROR) << "Could not find StoreGateSvc" << endmsg;
    return sc;
  }


  ITHistSvc *rootHistSvc;
  if (!service("THistSvc", rootHistSvc, true).isSuccess()) {
     msg(MSG::ERROR) << "Unable to locate THistSvc" << endmsg;
    return StatusCode::FAILURE;
  }

  std::string StreamAndPath="/FlowOutPut/";
  std::string histPath = StreamAndPath;
  char name[100],name1[100];
  for (int ihar=0;ihar<6;ihar++){

// /*
    sprintf(name,"hist_Psi_%d_true",ihar+1);
    sprintf(name1,"Truth Psi_{%d} distribution;%dPsi_{%d} Truth;events",ihar+1,ihar+1,ihar+1);
    m_hist_Psi_n_true  [ihar]=new TH1D (name,name1,1000,-M_PI,M_PI);
    if ( rootHistSvc->regHist(histPath+m_hist_Psi_n_true[ihar]->GetName(),m_hist_Psi_n_true[ihar]).isFailure() ){
       msg(MSG::WARNING) << "Can't book "<< histPath+m_hist_Psi_n_true[ihar]->GetName() << endmsg;
    }
    m_hist_Psi_n_true[ihar]->GetXaxis()->CenterTitle();
    m_hist_Psi_n_true[ihar]->GetYaxis()->CenterTitle();

    sprintf(name,"hist_Psi_%d_reco",ihar+1);
    sprintf(name1,"Reconstructed Psi_{%d} distribution;%dPsi_{%d} Reco;events",ihar+1,ihar+1,ihar+1);
    m_hist_Psi_n_reco  [ihar]=new TH1D (name,name1,1000,-M_PI,M_PI);
    if ( rootHistSvc->regHist(histPath+m_hist_Psi_n_reco[ihar]->GetName(),m_hist_Psi_n_reco[ihar]).isFailure() ){
       msg(MSG::WARNING) << "Can't book "<< histPath+m_hist_Psi_n_reco[ihar]->GetName() << endmsg;
    }
    m_hist_Psi_n_reco[ihar]->GetXaxis()->CenterTitle();
    m_hist_Psi_n_reco[ihar]->GetYaxis()->CenterTitle();


    for (int ihar2=0;ihar2<6;ihar2++)
    {
      int ihar_i=ihar*6+ihar2;

      sprintf(name,"hist_Psi_corr_true_%d_%d",ihar+1,ihar2+1);
      sprintf(name1,"true Psi_{%d} -Psi_{%d};%dPsi_{%d} -%dPsi_{%d} ;events",ihar+1,ihar2+1,ihar+1,ihar+1,ihar2+1,ihar2+1);
      m_hist_psi_corr_true  [ihar_i]=new TH1D (name,name1,1000,-2*M_PI,2*M_PI);
      if ( rootHistSvc->regHist(histPath+m_hist_psi_corr_true[ihar_i]->GetName(),m_hist_psi_corr_true[ihar_i]).isFailure() ){
         msg(MSG::WARNING) << "Can't book "<< histPath+m_hist_psi_corr_true[ihar_i]->GetName() << endmsg;
      }
      m_hist_psi_corr_true[ihar_i]->GetXaxis()->CenterTitle();
      m_hist_psi_corr_true[ihar_i]->GetYaxis()->CenterTitle();


      sprintf(name,"hist_Psi_corr_reco_%d_%d",ihar+1,ihar2+1);
      sprintf(name1,"reco Psi_{%d} -Psi_{%d};%dPsi_{%d} -%dPsi_{%d} ;events",ihar+1,ihar2+1,ihar+1,ihar+1,ihar2+1,ihar2+1);
      m_hist_psi_corr_reco  [ihar_i]=new TH1D (name,name1,1000,-2*M_PI,2*M_PI);
      if ( rootHistSvc->regHist(histPath+m_hist_psi_corr_reco[ihar_i]->GetName(),m_hist_psi_corr_reco[ihar_i]).isFailure() ){
         msg(MSG::WARNING) << "Can't book "<< histPath+m_hist_psi_corr_reco[ihar_i]->GetName() << endmsg;
      }
      m_hist_psi_corr_reco[ihar_i]->GetXaxis()->CenterTitle();
      m_hist_psi_corr_reco[ihar_i]->GetYaxis()->CenterTitle();
    }



    //integrated vn event by event
    sprintf(name,"hist_v%d_ebe",ihar+1);
    sprintf(name1,"v%d;v%d;events",ihar+1,ihar+1);
    m_hist_vn_ebe   [ihar]=new TH1D (name,name1,1000,-0.5,0.5);
    if ( rootHistSvc->regHist(histPath+m_hist_vn_ebe[ihar]->GetName(),m_hist_vn_ebe[ihar]).isFailure() ){
       msg(MSG::WARNING) << "Can't book "<< histPath+m_hist_vn_ebe[ihar]->GetName() << endmsg;
    }
    m_hist_vn_ebe[ihar]->GetXaxis()->CenterTitle();
    m_hist_vn_ebe[ihar]->GetYaxis()->CenterTitle();

    sprintf(name ,"hist_Psi%d_ebe",ihar+1);
    sprintf(name1,"%d#Delta#Psi;%d(#Psi_{reco}-#Psi_{Truth});events",ihar+1,ihar+1);
    m_hist_Psi_n_ebe [ihar]=new TH1D (name,name1,1000,-M_PI,M_PI);
    if ( rootHistSvc->regHist(histPath+m_hist_Psi_n_ebe[ihar]->GetName(),m_hist_Psi_n_ebe[ihar]).isFailure() ){
      msg(MSG::WARNING) << "Can't book "<< histPath+m_hist_Psi_n_ebe[ihar]->GetName() << endmsg;
    }
    m_hist_Psi_n_ebe[ihar]->GetXaxis()->CenterTitle();
    m_hist_Psi_n_ebe[ihar]->GetYaxis()->CenterTitle();

    sprintf(name ,"hist_Psi%d_ebe_pt",ihar+1);
    sprintf(name1,"%d#Delta#Psi (pT weighted);%d(#Psi_{reco}-#Psi_{Truth});events",ihar+1,ihar+1);
    m_hist_Psi_n_ebe_pt [ihar]=new TH1D (name,name1,1000,-M_PI,M_PI);
    if ( rootHistSvc->regHist(histPath+m_hist_Psi_n_ebe_pt[ihar]->GetName(),m_hist_Psi_n_ebe_pt[ihar]).isFailure() ){
      msg(MSG::WARNING) << "Can't book "<< histPath+m_hist_Psi_n_ebe_pt[ihar]->GetName() << endmsg;
    }
    m_hist_Psi_n_ebe_pt[ihar]->GetXaxis()->CenterTitle();
    m_hist_Psi_n_ebe_pt[ihar]->GetYaxis()->CenterTitle();






    for(int ieta=0;ieta<n_etabin;ieta++){
      sprintf(name ,"profile_pt_dep_%d_eta%d" ,ihar+1,ieta);
      sprintf(name1,"v%d vs pT (eta%d);pT;v%d",ihar+1,ieta,ihar+1);
      m_profile_pt_dep [ihar][ieta]=new TProfile (name,name1,n_ptbin,pt_binvals);
      if ( rootHistSvc->regHist(histPath+m_profile_pt_dep[ihar][ieta]->GetName(),m_profile_pt_dep[ihar][ieta]).isFailure() ){
        msg(MSG::WARNING) << "Can't book "<< histPath+m_profile_pt_dep[ihar][ieta]->GetName() << endmsg;
      }
      m_profile_pt_dep [ihar][ieta]->GetXaxis()->CenterTitle();
      m_profile_pt_dep [ihar][ieta]->GetYaxis()->CenterTitle();
    }

    for(int ipt=0;ipt<n_ptbin;ipt++){
      sprintf(name ,"profile_eta_dep_%d_pt%d",ihar+1,ipt);
      sprintf(name1,"v%d vs #eta; (ipt%d)#eta;v%d",ihar+1,ipt,ihar+1);
      m_profile_eta_dep [ihar][ipt]=new TProfile (name,name1,2*n_etabin, -eta_bin_max,eta_bin_max);
      if ( rootHistSvc->regHist(histPath+m_profile_eta_dep[ihar][ipt]->GetName(),m_profile_eta_dep[ihar][ipt]).isFailure() ){
        msg(MSG::WARNING) << "Can't book "<< histPath+m_profile_eta_dep[ihar][ipt]->GetName() << endmsg;
      }
      m_profile_eta_dep [ihar][ipt]->GetXaxis()->CenterTitle();
      m_profile_eta_dep [ihar][ipt]->GetYaxis()->CenterTitle();
    }


    for(int ieta=0;ieta<n_etabin;ieta++){
      sprintf(name ,"profile_pt_dep_reco_%d_eta%d",ihar+1,ieta);
      sprintf(name1,"v%d vs pT (eta%d);pT;v%d",ihar+1,ieta,ihar+1);
      m_profile_pt_dep_reco [ihar][ieta]=new TProfile (name,name1,n_ptbin,pt_binvals);
      if ( rootHistSvc->regHist(histPath+m_profile_pt_dep_reco[ihar][ieta]->GetName(),m_profile_pt_dep_reco[ihar][ieta]).isFailure() ){
        msg(MSG::WARNING) << "Can't book "<< histPath+m_profile_pt_dep_reco[ihar][ieta]->GetName() << endmsg;
      }
      m_profile_pt_dep_reco [ihar][ieta]->GetXaxis()->CenterTitle();
      m_profile_pt_dep_reco [ihar][ieta]->GetYaxis()->CenterTitle();
    }


    for(int ipt=0;ipt<n_ptbin;ipt++){
      sprintf(name ,"profile_eta_dep_reco_%d_pt%d",ihar+1,ipt);
      sprintf(name1,"v%d vs #eta (pt%d);#eta;v%d",ihar+1,ipt,ihar+1);
      m_profile_eta_dep_reco [ihar][ipt]=new TProfile (name,name1,2*n_etabin, -eta_bin_max,eta_bin_max);
      if ( rootHistSvc->regHist(histPath+m_profile_eta_dep_reco[ihar][ipt]->GetName(),m_profile_eta_dep_reco[ihar][ipt]).isFailure() ){
        msg(MSG::WARNING) << "Can't book "<< histPath+m_profile_eta_dep_reco[ihar][ipt]->GetName() << endmsg;
      }
      m_profile_eta_dep_reco [ihar][ipt]->GetXaxis()->CenterTitle();
      m_profile_eta_dep_reco [ihar][ipt]->GetYaxis()->CenterTitle();
    }


  }
  m_profile_resolution=new TProfile("profile_resolution","vn resolution;n;resolution",6, 0.5,6.5);
  if(rootHistSvc->regHist(histPath+m_profile_resolution->GetName(),m_profile_resolution).isFailure() ){
    msg(MSG::WARNING) << "Can't book "<< histPath+m_profile_resolution->GetName() << endmsg;
  }

  msg(MSG::DEBUG) << "Histograms have been booked " << endmsg;
  m_tesIO = new GenAccessIO();
  return result;
}



StatusCode CheckFlow_New::execute() {
  msg(MSG::INFO) << ">>> CheckFlow_New from execute" << endmsg;

  float pt_binvals[n_ptbin+1]={0.0,0.25,0.5,1.0,1.5,2.0,2.5,3.0,3.5,4.0,6.0,8.0,12.0,16.0,20.0,30.0,40.0};
  float eta_bin_max = 4.0;

  const HijingEventParams *hijing_pars;


  if ( m_sgSvc->retrieve(hijing_pars, "Hijing_event_params").isFailure() ) {
    msg(MSG::ERROR) << "Could not retrieve Hijing_event_params"<< endmsg;
    return StatusCode::FAILURE;
  }

  float b = hijing_pars->get_b();
  float Psi_n[6],Psi_n_reco[6];
  float Psi_n_reco_pos[6],Psi_n_reco_neg[6];
  for(int ihar=0;ihar<6;ihar++){Psi_n[ihar]=hijing_pars->get_psi(ihar+1);}
  msg(MSG::INFO)<<"SOUMYA  "<<hijing_pars->get_psi(1)<<"   "<<hijing_pars->get_psi(2)<<"  "<<hijing_pars->get_psi(3)
                                  <<hijing_pars->get_psi(4)<<"   "<<hijing_pars->get_psi(5)<<"  "<<hijing_pars->get_psi(6)<<"    "<<b  << endmsg;


  // Check cut on impact parameter b
  if(b<m_bcut_min || b>m_bcut_max)  return StatusCode::SUCCESS;


  double ngenerated_pos = 0,ngenerated_pt_pos=0;
  double cos_n_pos[6],sin_n_pos[6],cos_n_pt_pos[6],sin_n_pt_pos[6];
  double ngenerated_neg = 0,ngenerated_pt_neg=0;
  double cos_n_neg[6],sin_n_neg[6],cos_n_pt_neg[6],sin_n_pt_neg[6];
  for(int ihar=0;ihar<6;ihar++){cos_n_pos[ihar]=0;sin_n_pos[ihar]=0;   cos_n_pt_pos[ihar]=0;sin_n_pt_pos[ihar]=0;
                                cos_n_neg[ihar]=0;sin_n_neg[ihar]=0;   cos_n_pt_neg[ihar]=0;sin_n_pt_neg[ihar]=0;}

  // Iterate over all MC particles
  GenAll ifs;
  std::vector<HepMC::ConstGenParticlePtr> particles;
  StatusCode stat = m_tesIO->getMC(particles, &ifs, m_key);
  if (stat.isFailure()) {
    msg(MSG::ERROR) << "Could not find " << m_key << endmsg;
    return stat;
  }
  for (auto pitr: particles) {
    int    pid    = pitr->pdg_id();
    int    p_stat = pitr->status();
    double pt     = pitr->momentum().perp();
    double rapid  = pitr->momentum().pseudoRapidity();
    double phi    = pitr->momentum().phi();
    msg(MSG::DEBUG)
	   << " PID = " << pid << " Status = " << p_stat
	   << " Eta = " << rapid << "  Phi = " << phi<< endmsg;

    if( (std::abs(rapid) >= m_rapcut_min) && (std::abs(rapid) <= m_rapcut_max) &&
	(std::abs(pt) >= m_ptcut_min) && (std::abs(pt) <= m_ptcut_max) ) {

      for(int ihar=0;ihar<6;ihar++){
        float temp=(ihar+1)*(phi-Psi_n[ihar]);

        int ieta= (int)(std::abs(rapid)*n_etabin/eta_bin_max);
        if(ieta>=0 && ieta<n_etabin) m_profile_pt_dep [ihar][ieta]->Fill(pt/1000,cos(temp));


        float temp_pt=pt/1000;
        for(int ipt=0;ipt<n_ptbin;ipt++){
          if(temp_pt<pt_binvals[ipt+1]){
            m_profile_eta_dep[ihar][ipt]->Fill(rapid  ,cos(temp));
            break;
          }
        }

        if( rapid >3.2 && rapid< 4.9){
          cos_n_pos[ihar]+=cos(  (ihar+1)*phi);
          sin_n_pos[ihar]+=sin(  (ihar+1)*phi);
          ngenerated_pos++;

          cos_n_pt_pos[ihar]+=pt*cos(  (ihar+1)*phi);
          sin_n_pt_pos[ihar]+=pt*sin(  (ihar+1)*phi);
          ngenerated_pt_pos +=pt;
        }
        if( rapid <-3.2 && rapid >-4.9){
          cos_n_neg[ihar]+=cos(  (ihar+1)*phi);
          sin_n_neg[ihar]+=sin(  (ihar+1)*phi);
          ngenerated_neg++;

          cos_n_pt_neg[ihar]+=pt*cos(  (ihar+1)*phi);
          sin_n_pt_neg[ihar]+=pt*sin(  (ihar+1)*phi);
          ngenerated_pt_neg +=pt;
        }

      }
    }
  }


// Calculate the event by event vn and also the reconstructed Psi_n angles
// Also make correlation histos between Psi_n_truth and Psi_n_reco
  float cos_n[6],sin_n[6],cos_n_pt[6],sin_n_pt[6];
  for(int ihar=0;ihar<6;ihar++){
    cos_n[ihar] = ( cos_n_pos[ihar]+ cos_n_neg[ihar] )  /  (ngenerated_pos+ngenerated_neg);
    sin_n[ihar] = ( sin_n_pos[ihar]+ sin_n_neg[ihar] )  /  (ngenerated_pos+ngenerated_neg);

    float psi_reco=atan2(sin_n[ihar],cos_n[ihar])/(ihar+1);
    m_hist_Psi_n_ebe[ihar]->Fill( (ihar+1)*(psi_reco-Psi_n[ihar])  );
    m_hist_vn_ebe   [ihar]->Fill(sqrt(cos_n[ihar]*cos_n[ihar] +sin_n[ihar]*sin_n[ihar] ));

    Psi_n_reco_pos[ihar]=atan2(sin_n_pos[ihar],cos_n_pos[ihar])/ (ihar+1);
    Psi_n_reco_neg[ihar]=atan2(sin_n_neg[ihar],cos_n_neg[ihar])/ (ihar+1);
    Psi_n_reco    [ihar]=psi_reco;


    cos_n_pt[ihar] = ( cos_n_pt_pos[ihar]+ cos_n_pt_neg[ihar] )  /  (ngenerated_pt_pos+ngenerated_pt_neg);
    sin_n_pt[ihar] = ( sin_n_pt_pos[ihar]+ sin_n_pt_neg[ihar] )  /  (ngenerated_pt_pos+ngenerated_pt_neg);

    psi_reco=atan2(sin_n_pt[ihar],cos_n_pt[ihar])/(ihar+1);
    m_hist_Psi_n_ebe_pt[ihar]->Fill( (ihar+1)*(psi_reco-Psi_n[ihar])  );
  }


// Make the plots for the correlation between Psi_n truth (for different n)  (same for Psi_n reco)
  for(int ihar=0;ihar<6;ihar++){
    m_hist_Psi_n_true[ihar]->Fill((ihar+1)*Psi_n[ihar]);
    m_hist_Psi_n_reco[ihar]->Fill((ihar+1)*Psi_n_reco[ihar]);

    float psi1,psi2;
    for(int ihar2=0;ihar2<6;ihar2++){
      psi1=(ihar+1)*Psi_n[ihar];psi2=(ihar2+1)*Psi_n[ihar2];
      m_hist_psi_corr_true[ihar*6+ihar2]->Fill(  atan2(  sin(psi1-psi2),cos(psi1-psi2) )  );

      psi1=(ihar+1)*Psi_n_reco[ihar];psi2=(ihar2+1)*Psi_n_reco[ihar2];
      m_hist_psi_corr_reco[ihar*6+ihar2]->Fill( atan2(  sin(psi1-psi2),cos(psi1-psi2) )  );
    }
  }





// calculate the pt and eta dependence using the Psi_reco angles also fill the resolution TProfile
  for(int ihar=0;ihar<6;ihar++){
    m_profile_resolution->Fill( ihar+1,  cos(  (ihar+1) * (Psi_n_reco_pos[ihar] - Psi_n_reco_neg[ihar]) ) );
  }
  for (auto pitr: particles) {
    double pt     = pitr->momentum().perp();
    double rapid  = pitr->momentum().pseudoRapidity();
    double phi    = pitr->momentum().phi();
    if( (std::abs(rapid) >= m_rapcut_min) && (std::abs(rapid) <= m_rapcut_max) &&
	(std::abs(pt) >= m_ptcut_min) && (std::abs(pt) <= m_ptcut_max) ) {

      for(int ihar=0;ihar<6;ihar++){
        float       temp=(ihar+1)*(phi-Psi_n_reco_pos[ihar]);
        if(rapid>0) temp=(ihar+1)*(phi-Psi_n_reco_neg[ihar]);


        int ieta= (int)(std::abs(rapid)*n_etabin/eta_bin_max);
        if(ieta>=0 && ieta<n_etabin) m_profile_pt_dep_reco [ihar][ieta]->Fill(pt/1000,std::cos(temp));

        float temp_pt=pt/1000;
        for(int ipt=0;ipt<n_ptbin;ipt++){
          if(temp_pt<pt_binvals[ipt+1]){
            m_profile_eta_dep_reco[ihar][ipt]->Fill(rapid  ,cos(temp));
            break;
          }
        }
      }
    }
  }





  return StatusCode::SUCCESS;
}

StatusCode CheckFlow_New::finalize() {
  msg(MSG::INFO) << ">>> CheckFlow_New from finalize" << endmsg;

  return StatusCode::SUCCESS;
}

