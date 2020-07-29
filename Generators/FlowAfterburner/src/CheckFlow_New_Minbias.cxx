/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// File:  Generators/FlowAfterburnber/CheckFlow_New_Minbias.h
// Description:
//    This is a simple algorithm to histogram particle properties
//    for diagnosing of flow generation
//
//************************** THIS PROGRAM ANALYZES FILES WITH VARYING b_imp(Min Bias)*********************
//
// AuthorList:
// Andrzej Olszewski: Initial Code February 2006
// Andrzej Olszewski: Converted to ROOT histograms July 2007
// Soumya Mohapatra : Re-written to check the new Flow implementations (JUNE 2011)

#include "FlowAfterburner/CheckFlow_New_Minbias.h"
#include "GeneratorObjects/McEventCollection.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataSvc.h"

#include "GaudiKernel/ITHistSvc.h"

#include <TH1D.h>
#include <TProfile.h>

#include "TruthHelper/IsGenStable.h"
#include "TruthHelper/GenAll.h"

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"

#include "GeneratorObjects/HijingEventParams.h"




using namespace TruthHelper;

//typedef std::vector<const HepMC::GenParticlePtr>  MCParticleCollection ;

CheckFlow_New_Minbias::CheckFlow_New_Minbias(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
  //Declare the algorithm's properties
  declareProperty("McEventKey"    , m_key              ="FLOW_EVENT");
  declareProperty("HistogramFlag" , m_produceHistogram = true       );
  declareProperty("ImpactCutMin"  , m_bcut_min         = 0          );
  declareProperty("ImpactCutMax"  , m_bcut_max         = 99         );
  declareProperty("PtCutMin"      , m_ptcut_min        = 0          );
  declareProperty("PtCutMax"      , m_ptcut_max        = 999999     );
  declareProperty("RapidityCutMin", m_rapcut_min       = 0          );
  declareProperty("RapidityCutMax", m_rapcut_max       = 5.5        );

  m_thistSvc = 0;
  m_tesIO    = 0;
}


StatusCode CheckFlow_New_Minbias::initialize(){
  StatusCode result = StatusCode::SUCCESS;

  ATH_MSG_INFO(">>> CheckFlow_New_Minbias from Initialize");


  ServiceHandle<ITHistSvc> rootHistSvc("THistSvc",name()); 
  CHECK( rootHistSvc.retrieve() );
  std::string histPath = "/FlowOutPut/";
  std::vector<TH1*> hist_vec;
  char name[100],name1[100];


  for (int ihar=0;ihar<6;ihar++){
    for(int ib_imp=0;ib_imp<n_b_bins;ib_imp++){

     const float pt_binvals[]={0.0,0.25,0.5,1.0,1.5,2.0,2.5,3.0,3.5,4.0,6.0,8.0,12.0,16.0,20.0,30.0,40.0};
     //const float b_bin_vals[]={0.0,3.4955,4.9315,6.0375,6.9695,7.7895,8.5335,9.2135,9.8515,10.4485,11.0175,
     //                          11.554,12.070,12.560,13.033,13.492,13.944,14.409,14.929,15.6425};
     const float eta_bin_max = 4.0; 
//----------------------------------------The reconstructed and truth Psi and the correlations between them-----------------------------
      sprintf(name,"hist_Psi_%d_true_b%d",ihar+1,ib_imp);
      sprintf(name1,"Truth Psi_{%d} distribution;%dPsi_{%d} Truth;events",ihar+1,ihar+1,ihar+1);
      m_hist_Psi_n_true  [ihar][ib_imp]=new TH1D (name,name1,1000,-M_PI,M_PI);
      hist_vec.push_back(m_hist_Psi_n_true  [ihar][ib_imp]);

      sprintf(name,"hist_Psi_%d_reco_b%d",ihar+1,ib_imp);
      sprintf(name1,"Reconstructed Psi_{%d} distribution;%dPsi_{%d} Reco;events",ihar+1,ihar+1,ihar+1);
      m_hist_Psi_n_reco  [ihar][ib_imp]=new TH1D (name,name1,1000,-M_PI,M_PI);
      hist_vec.push_back(m_hist_Psi_n_reco  [ihar][ib_imp]);


      for (int ihar2=0;ihar2<6;ihar2++){
        int ihar_i=ihar*6+ihar2;

        sprintf(name,"hist_Psi_corr_true_b%d_%d_%d",ib_imp,ihar+1,ihar2+1);
        sprintf(name1,"true Psi_{%d} -Psi_{%d};%dPsi_{%d} -%dPsi_{%d} ;events",ihar+1,ihar2+1,ihar+1,ihar+1,ihar2+1,ihar2+1);
        m_hist_psi_corr_true  [ihar_i][ib_imp]=new TH1D (name,name1,1000,-2*M_PI,2*M_PI);
        hist_vec.push_back(m_hist_psi_corr_true  [ihar_i][ib_imp]);

        sprintf(name,"hist_Psi_corr_reco_%d_%d_%d",ib_imp,ihar+1,ihar2+1);
        sprintf(name1,"reco Psi_{%d} -Psi_{%d};%dPsi_{%d} -%dPsi_{%d} ;events",ihar+1,ihar2+1,ihar+1,ihar+1,ihar2+1,ihar2+1);
        m_hist_psi_corr_reco  [ihar_i][ib_imp]=new TH1D (name,name1,1000,-2*M_PI,2*M_PI);
        hist_vec.push_back(m_hist_psi_corr_reco  [ihar_i][ib_imp]);
      }
//------------------------------------------------------------------------------------------



//-----------------------------------------------------------------------------------------
      //integrated vn event by event
      sprintf(name,"hist_v%d_b%d_ebe",ihar+1,ib_imp);
      sprintf(name1,"v%d;v%d;events",ihar+1,ihar+1);
      m_hist_vn_ebe   [ihar][ib_imp]=new TH1D (name,name1,1000,-0.5,0.5);
      hist_vec.push_back(m_hist_vn_ebe   [ihar][ib_imp]);

      sprintf(name,"hist_v%d_b%d_ebe_ID1",ihar+1,ib_imp);
      sprintf(name1,"v%d;v%d;events",ihar+1,ihar+1);
      m_hist_vn_ebe_ID1   [ihar][ib_imp]=new TH1D (name,name1,800,0.0,0.4);
      sprintf(name,"hist_v%d_b%d_ebe_ID2",ihar+1,ib_imp);
      m_hist_vn_ebe_ID2   [ihar][ib_imp]=new TH1D (name,name1,800,0.0,0.4);
      hist_vec.push_back(m_hist_vn_ebe_ID1  [ihar][ib_imp]);
      hist_vec.push_back(m_hist_vn_ebe_ID2  [ihar][ib_imp]);

      sprintf(name ,"hist_Psi%d_b%d_ebe",ihar+1,ib_imp);
      sprintf(name1,"%d#Delta#Psi;%d(#Psi_{reco}-#Psi_{Truth});events",ihar+1,ihar+1);
      m_hist_Psi_n_ebe [ihar][ib_imp]=new TH1D (name,name1,1000,-M_PI,M_PI);
      hist_vec.push_back(m_hist_Psi_n_ebe [ihar][ib_imp]);

      sprintf(name ,"hist_Psi%d_b%d_ebe_pt",ihar+1,ib_imp);
      sprintf(name1,"%d#Delta#Psi (pT weighted);%d(#Psi_{reco}-#Psi_{Truth});events",ihar+1,ihar+1);
      m_hist_Psi_n_ebe_pt [ihar][ib_imp]=new TH1D (name,name1,1000,-M_PI,M_PI);
      hist_vec.push_back(m_hist_Psi_n_ebe_pt [ihar][ib_imp]);
//-----------------------------------------------------------------------------------------





//-----------------------------------------------------------------------------------------
      for(int ieta=0;ieta<n_etabin;ieta++){
        sprintf(name ,"profile_pt_dep_%d_eta%d_b%d" ,ihar+1,ieta,ib_imp);
        sprintf(name1,"v%d vs pT (eta%d);pT;v%d",ihar+1,ieta,ihar+1);
        m_profile_pt_dep [ihar][ib_imp][ieta]=new TProfile (name,name1,n_ptbin,pt_binvals);
        hist_vec.push_back(m_profile_pt_dep [ihar][ib_imp][ieta]);
      }

      for(int ipt=0;ipt<n_ptbin;ipt++){
        sprintf(name ,"profile_eta_dep_%d_pt%d_b%d",ihar+1,ipt,ib_imp);
        sprintf(name1,"v%d vs #eta; (ipt%d)#eta;v%d",ihar+1,ipt,ihar+1);
        m_profile_eta_dep [ihar][ib_imp][ipt]=new TProfile (name,name1,2*n_etabin, -eta_bin_max,eta_bin_max);
        hist_vec.push_back(m_profile_eta_dep [ihar][ib_imp][ipt]);
      }


      for(int ieta=0;ieta<n_etabin;ieta++){
        sprintf(name ,"profile_pt_dep_reco_%d_eta%d_b%d",ihar+1,ieta,ib_imp);
        sprintf(name1,"v%d vs pT (eta%d);pT;v%d",ihar+1,ieta,ihar+1);
        m_profile_pt_dep_reco [ihar][ib_imp][ieta]=new TProfile (name,name1,n_ptbin,pt_binvals);
        hist_vec.push_back(m_profile_pt_dep_reco [ihar][ib_imp][ieta]);
      }


      for(int ipt=0;ipt<n_ptbin;ipt++){
        sprintf(name ,"profile_eta_dep_reco_%d_pt%d_b%d",ihar+1,ipt,ib_imp);
        sprintf(name1,"v%d vs #eta (pt%d);#eta;v%d",ihar+1,ipt,ihar+1);
        m_profile_eta_dep_reco [ihar][ib_imp][ipt]=new TProfile (name,name1,2*n_etabin, -eta_bin_max,eta_bin_max);
        hist_vec.push_back(m_profile_eta_dep_reco [ihar][ib_imp][ipt]);
      }
    }



    for(int ipt=0;ipt<n_ptbin;ipt++){
      for(int ieta=0;ieta<n_etabin;ieta++){
        sprintf(name ,"profile_b_dep_%d_pt%d_eta%d",ihar+1,ipt,ieta);
        sprintf(name1,"v%d vs cent ;cent;v%d",ihar+1,ihar+1);
        m_profile_b_dep [ihar][ipt][ieta]=new TProfile (name,name1,n_b_bins,-0.5,n_b_bins-0.5);
        hist_vec.push_back(m_profile_b_dep [ihar][ipt][ieta]);

        sprintf(name ,"profile_b_dep_reco_%d_pt%d_eta%d",ihar+1,ipt,ieta);
        sprintf(name1,"v%d vs cent ;cent;v%d",ihar+1,ihar+1);
        m_profile_b_dep_reco [ihar][ipt][ieta]=new TProfile (name,name1,n_b_bins,-0.5,n_b_bins-0.5);
        hist_vec.push_back(m_profile_b_dep_reco [ihar][ipt][ieta]);
      }
    }

    sprintf(name,"profile_resolution_%d",ihar+1);
    m_profile_resolution[ihar]=new TProfile(name,"vn resolution;n;resolution",n_b_bins,-0.5,n_b_bins-0.5);
    hist_vec.push_back(m_profile_resolution[ihar]);
  }
//-----------------------------------------------------------------------------------------

  for(auto& hist: hist_vec){
    CHECK(rootHistSvc->regHist(histPath+hist->GetName(),hist));
    hist->GetXaxis()->CenterTitle();
    hist->GetYaxis()->CenterTitle();
  }


  ATH_MSG_DEBUG("Histograms have been booked ");
  m_tesIO = new GenAccessIO();
  return result;
}



StatusCode CheckFlow_New_Minbias::execute() {

  ATH_MSG_INFO(">>> CheckFlow_New_Minbias from execute");
  const float pt_binvals[]={0.0,0.25,0.5,1.0,1.5,2.0,2.5,3.0,3.5,4.0,6.0,8.0,12.0,16.0,20.0,30.0,40.0};
  const float b_bin_vals[]={0.0,3.4955,4.9315,6.0375,6.9695,7.7895,8.5335,9.2135,9.8515,10.4485,11.0175,11.554,12.070,12.560,13.033,13.492,13.944,14.409,14.929,15.6425};
  const float eta_bin_max = 4.0; 
  const HijingEventParams *hijing_pars;


  CHECK(evtStore()->retrieve(hijing_pars, "Hijing_event_params"));
  float b = hijing_pars->get_b();
  float Psi_n[6],Psi_n_reco[6];
  for(int ihar=0;ihar<6;ihar++){Psi_n[ihar]=hijing_pars->get_psi(ihar+1);}
  ATH_MSG_INFO("SOUMYA  "<<hijing_pars->get_psi(1)<<"   "<<hijing_pars->get_psi(2)<<"   "\
                         <<hijing_pars->get_psi(3)<<"   "<<hijing_pars->get_psi(4)<<"   "\
                         <<hijing_pars->get_psi(5)<<"   "<<hijing_pars->get_psi(6)<<"   "\
                         <<b);


 
  int ib_imp=-1;
  for(int ib=0;ib<n_b_bins;ib++){if(b<b_bin_vals[ib+1]) {ib_imp=ib;break;}} 
  if(ib_imp<0) return StatusCode::SUCCESS;
  if(ib_imp==0) {std::cout<<"AAAAAAAAAAAAAAAAAAAA  "<<b<<std::endl;}

  // Check cut on impact parameter b
  if(b<m_bcut_min || b>m_bcut_max)  return StatusCode::SUCCESS;


  double ngenerated_pos = 0,ngenerated_pt_pos=0;
  double ngenerated_neg = 0,ngenerated_pt_neg=0;
  double cos_n_pos[6],sin_n_pos[6],cos_n_pt_pos[6],sin_n_pt_pos[6];
  double cos_n_neg[6],sin_n_neg[6],cos_n_pt_neg[6],sin_n_pt_neg[6];
  double cos_ID1[6],sin_ID1[6],tot_ID1=0.0;
  double cos_ID2[6],sin_ID2[6],tot_ID2=0.0;
  for(int ihar=0;ihar<6;ihar++){
    cos_n_pos   [ihar]=0;
    sin_n_pos   [ihar]=0;
    cos_n_pt_pos[ihar]=0;
    sin_n_pt_pos[ihar]=0;
    cos_n_neg   [ihar]=0;
    sin_n_neg   [ihar]=0;
    cos_n_pt_neg[ihar]=0;
    sin_n_pt_neg[ihar]=0;
    cos_ID1     [ihar]=0;
    sin_ID1     [ihar]=0;
    cos_ID2     [ihar]=0;
    sin_ID2     [ihar]=0;
  }


  // Iterate over MC particles  We are using the IsGenStable predicate from IsGenStable ifs;
  GenAll ifs;
  std::vector<HepMC::ConstGenParticlePtr> particles;
  CHECK(m_tesIO->getMC(particles, &ifs, m_key));

  for (auto pitr: particles) {
    int    pid    = pitr->pdg_id();
    int    p_stat = pitr->status();
    double pt     = pitr->momentum().perp();
    double rapid  = pitr->momentum().pseudoRapidity();
    double phi    = pitr->momentum().phi();
    ATH_MSG_DEBUG(" PID = " << pid   << " Status = " << p_stat \
	       << " Eta = " << rapid << " Phi = "    << phi);
    
    if( (std::abs(rapid) >= m_rapcut_min) && 
        (std::abs(rapid) <= m_rapcut_max) &&
	(std::abs(pt)    >= m_ptcut_min ) &&
        (std::abs(pt)    <= m_ptcut_max)) {
      
      for(int ihar=0;ihar<6;ihar++){
        float temp=(ihar+1)*(phi-Psi_n[ihar]);

        int ieta= (int)(std::abs(rapid)*n_etabin/eta_bin_max);
        if(ieta>=0 && ieta<n_etabin) m_profile_pt_dep [ihar][ib_imp][ieta]->Fill(pt/1000,cos(temp));
        

        float temp_pt=pt/1000;
        for(int ipt=0;ipt<n_ptbin;ipt++){
          if(temp_pt<pt_binvals[ipt+1]){
            m_profile_eta_dep[ihar][ib_imp][ipt]->Fill(rapid  ,cos(temp));
            if(ieta>=0 && ieta<n_etabin) m_profile_b_dep [ihar][ipt][ieta]->Fill(ib_imp,cos(temp));
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

    //EbE vn for ID acceptance end pt>0.5GeV
    if(std::abs(pt)>=500){
      tot_ID1++;
      for(int ihar=0;ihar<6;ihar++){
        cos_ID1[ihar]+=cos((ihar+1)*phi);
        sin_ID1[ihar]+=sin((ihar+1)*phi);
      } 
    }
    if(std::abs(pt)>=0){
      tot_ID2++;
      for(int ihar=0;ihar<6;ihar++){
        cos_ID2[ihar]+=cos((ihar+1)*phi);
        sin_ID2[ihar]+=sin((ihar+1)*phi);
      } 
    }
  }


//EbE vn for ID acceptance end pt>0.5GeV
  for(int ihar=0;ihar<6;ihar++){
    if(tot_ID1>0.01){
      double temp1= sqrt(cos_ID1[ihar]*cos_ID1[ihar] + sin_ID1[ihar]*sin_ID1[ihar])/tot_ID1;
      m_hist_vn_ebe_ID1[ihar][ib_imp]->Fill(temp1);
    }
    if(tot_ID2>0.01){
      double temp2= sqrt(cos_ID2[ihar]*cos_ID2[ihar] + sin_ID2[ihar]*sin_ID2[ihar])/tot_ID2;
      m_hist_vn_ebe_ID2[ihar][ib_imp]->Fill(temp2);
    }
  } 


// Calculate the event by event vn and also the reconstructed Psi_n angles 
// Also make correlation histos between Psi_n_truth and Psi_n_reco
  float cos_n[6],sin_n[6],cos_n_pt[6],sin_n_pt[6];
  float  Psi_n_reco_pos[6],Psi_n_reco_neg[6]; 
  for(int ihar=0;ihar<6;ihar++){
    cos_n[ihar] = ( cos_n_pos[ihar]+ cos_n_neg[ihar] )  /  (ngenerated_pos+ngenerated_neg);
    sin_n[ihar] = ( sin_n_pos[ihar]+ sin_n_neg[ihar] )  /  (ngenerated_pos+ngenerated_neg);

    float psi_reco=atan2(sin_n[ihar],cos_n[ihar])/(ihar+1);
    m_hist_Psi_n_ebe[ihar][ib_imp]->Fill( (ihar+1)*(psi_reco-Psi_n[ihar])  );
    m_hist_vn_ebe   [ihar][ib_imp]->Fill(sqrt(cos_n[ihar]*cos_n[ihar] +sin_n[ihar]*sin_n[ihar] ));

    Psi_n_reco_pos[ihar]=atan2(sin_n_pos[ihar],cos_n_pos[ihar])/ (ihar+1);
    Psi_n_reco_neg[ihar]=atan2(sin_n_neg[ihar],cos_n_neg[ihar])/ (ihar+1);
    Psi_n_reco    [ihar]=psi_reco;


    cos_n_pt[ihar] = ( cos_n_pt_pos[ihar]+ cos_n_pt_neg[ihar] )  /  (ngenerated_pt_pos+ngenerated_pt_neg);
    sin_n_pt[ihar] = ( sin_n_pt_pos[ihar]+ sin_n_pt_neg[ihar] )  /  (ngenerated_pt_pos+ngenerated_pt_neg);

    psi_reco=atan2(sin_n_pt[ihar],cos_n_pt[ihar])/(ihar+1);
    m_hist_Psi_n_ebe_pt[ihar][ib_imp]->Fill( (ihar+1)*(psi_reco-Psi_n[ihar])  );
  }


// Make the plots for the correlation between Psi_n truth (for different n)  (same for Psi_n reco)
  for(int ihar=0;ihar<6;ihar++){
    m_hist_Psi_n_true[ihar][ib_imp]->Fill((ihar+1)*Psi_n[ihar]);
    m_hist_Psi_n_reco[ihar][ib_imp]->Fill((ihar+1)*Psi_n_reco[ihar]);
    
    float psi1,psi2;
    for(int ihar2=0;ihar2<6;ihar2++){
      psi1=(ihar+1)*Psi_n[ihar];psi2=(ihar2+1)*Psi_n[ihar2]; 
      m_hist_psi_corr_true[ihar*6+ihar2][ib_imp]->Fill(  atan2(  sin(psi1-psi2),cos(psi1-psi2) )  );

      psi1=(ihar+1)*Psi_n_reco[ihar];psi2=(ihar2+1)*Psi_n_reco[ihar2]; 
      m_hist_psi_corr_reco[ihar*6+ihar2][ib_imp]->Fill( atan2(  sin(psi1-psi2),cos(psi1-psi2) )  );
    }
  }





// calculate the pt and eta dependence using the Psi_reco angles also fill the resolution TProfile
  for(int ihar=0;ihar<6;ihar++){
    m_profile_resolution[ihar]->Fill( ib_imp,  cos(  (ihar+1) * (Psi_n_reco_pos[ihar] - Psi_n_reco_neg[ihar]) ) );
    if(ib_imp==0) {std::cout<<"i11111111111111111111  "<<b<<std::endl;}
  }
  for (auto  pitr: particles) {
    double pt     = pitr->momentum().perp();
    double rapid  = pitr->momentum().pseudoRapidity();
    double phi    = pitr->momentum().phi();
    if( (std::abs(rapid) >= m_rapcut_min) && (std::abs(rapid) <= m_rapcut_max) &&
	(std::abs(pt) >= m_ptcut_min) && (std::abs(pt) <= m_ptcut_max) ) {
      
      for(int ihar=0;ihar<6;ihar++){
        float       temp=(ihar+1)*(phi-Psi_n_reco_pos[ihar]);
        if(rapid>0) temp=(ihar+1)*(phi-Psi_n_reco_neg[ihar]);


        int ieta= (int)(std::abs(rapid)*n_etabin/eta_bin_max);
        if(ieta>=0 && ieta<n_etabin) m_profile_pt_dep_reco [ihar][ib_imp][ieta]->Fill(pt/1000,cos(temp));

        float temp_pt=pt/1000;
        for(int ipt=0;ipt<n_ptbin;ipt++){
          if(temp_pt<pt_binvals[ipt+1]){
            m_profile_eta_dep_reco[ihar][ib_imp][ipt]->Fill(rapid  ,cos(temp));
            if(ieta>=0 && ieta<n_etabin) m_profile_b_dep_reco [ihar][ipt][ieta]->Fill(ib_imp,cos(temp));
            break;
          }
        } 
      } 
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode CheckFlow_New_Minbias::finalize() {

  ATH_MSG_INFO(">>> CheckFlow_New from finalize");

  return StatusCode::SUCCESS;
}

