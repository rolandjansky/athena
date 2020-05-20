/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package: TgcLv1RawDataValAlg
// Authors:  A.Ishikawa(Kobe)  akimasa.ishikawa@cern.ch, M.King(Kobe) kingmgl@stu.kobe-u.ac.jp
// Original: Apr. 2008
// Modified: June 2011
//
// DESCRIPTION:
// Subject: TGCLV1-->Offline Muon Data Quality/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/DataHandle.h"

#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "MuonRDO/TgcRdoIdHash.h"

// GeoModel
#include "MuonReadoutGeometry/TgcReadoutParams.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
#include "MuonDQAUtils/MuonDQAHistMap.h" 
 
#include "Identifier/Identifier.h"

#include "TgcRawDataMonitoring/TgcLv1RawDataValAlg.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

//for express menu
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"

#include <TH1F.h>
#include <TH2F.h>
#include <inttypes.h> 

#include <sstream>

///////////////////////////////////////////////////////////////////////////
// bookEfficiencyHisto
///////////////////////////////////////////////////////////////////////////
StatusCode
TgcLv1RawDataValAlg::bookHistogramsEfficiency(){
  StatusCode sc = StatusCode::SUCCESS; 

  ///////////////////////////////////////////////////////////////////////////
  // Make MonGroups for histogram booking paths
  std::string generic_path_tgclv1 = "Muon/MuonRawDataMonitoring/TGCLV1";

  MonGroup tgclv1_a( this, generic_path_tgclv1+"/TGCEA", run, ATTRIB_UNMANAGED  );
  MonGroup tgclv1_c( this, generic_path_tgclv1+"/TGCEC", run, ATTRIB_UNMANAGED  );

  MonGroup tgclv1_turnon( this, generic_path_tgclv1+"/Global", run, ATTRIB_UNMANAGED  );
  MonGroup tgclv1_turnon_a( this, generic_path_tgclv1+"/TGCEA/Eff", run, ATTRIB_UNMANAGED  );
  MonGroup tgclv1_turnon_c( this, generic_path_tgclv1+"/TGCEC/Eff", run, ATTRIB_UNMANAGED  );
  MonGroup* tgclv1_turnon_ac[2] = { &tgclv1_turnon_a, &tgclv1_turnon_c};

  MonGroup tgclv1_turnon_numdenom_a( this, generic_path_tgclv1+"/TGCEA/Eff/NumDenom", run, ATTRIB_UNMANAGED );
  MonGroup tgclv1_turnon_numdenom_c( this, generic_path_tgclv1+"/TGCEC/Eff/NumDenom", run, ATTRIB_UNMANAGED );
  MonGroup* tgclv1_turnon_numdenom_ac[2] = { &tgclv1_turnon_numdenom_a, &tgclv1_turnon_numdenom_c};

  MonGroup tgclv1_turnon_ES( this, generic_path_tgclv1+"/Global/ES", run, ATTRIB_UNMANAGED  );
  MonGroup tgclv1_turnon_a_ES( this, generic_path_tgclv1+"/TGCEA/ES/Eff", run, ATTRIB_UNMANAGED  );
  MonGroup tgclv1_turnon_c_ES( this, generic_path_tgclv1+"/TGCEC/ES/Eff", run, ATTRIB_UNMANAGED  );
  MonGroup* tgclv1_turnon_ac_ES[2] = { &tgclv1_turnon_a_ES, &tgclv1_turnon_c_ES};

  MonGroup tgclv1_turnon_a_ES_GM( this, generic_path_tgclv1+"/TGCEA/ES/Eff/GM", run, ATTRIB_UNMANAGED, "", "weightedEff"  );
  MonGroup tgclv1_turnon_c_ES_GM( this, generic_path_tgclv1+"/TGCEC/ES/Eff/GM", run, ATTRIB_UNMANAGED, "", "weightedEff"  );
  MonGroup* tgclv1_turnon_ac_ES_GM[2] = { &tgclv1_turnon_a_ES_GM, &tgclv1_turnon_c_ES_GM};

  MonGroup tgclv1_turnon_numdenom_a_ES( this, generic_path_tgclv1+"/TGCEA/ES/Eff/NumDenom", run, ATTRIB_UNMANAGED );
  MonGroup tgclv1_turnon_numdenom_c_ES( this, generic_path_tgclv1+"/TGCEC/ES/Eff/NumDenom", run, ATTRIB_UNMANAGED );
  MonGroup* tgclv1_turnon_numdenom_ac_ES[2] = { &tgclv1_turnon_numdenom_a_ES, &tgclv1_turnon_numdenom_c_ES};

  std::stringstream ss;
  std::string side[2] = {"A","C"};
  std::string muid[1] = {"Muid"};
  std::string spna[3] = {"_Posi","_Nega",""};
  std::string spcn[3] = {"_Previous", "", "_Next"};

  // Generate pt_bins array for specifying pT bin sizes in turn-on curves
  // 0.25 for <  8GeV : 32bins
  // 0.33 for < 17GeV : 27bins
  // 0.50 for < 25GeV : 16bins
  // 1.00 for >=25GeV : 26bins
  const int pt_nbins = 32 + 27 + 16 + 26;
  float pt_bins[pt_nbins+1];
  pt_bins[0]=0.;
  float boundary=0.;
  for( int bin = 1; bin <= 32 + 27 + 16 + 26 ; bin ++) {
    if( bin <= 32 )               boundary += 0.25;
    else if( bin <= 32+27 )       boundary += 1./3.;
    else if( bin <= 32+27+16 )    boundary += 0.5;
    else if( bin <= 32+27+16+26 ) boundary += 1.0;
    pt_bins[bin] = boundary;
  }

  ///////////////////////////////////////////////////////////////////////////
  // pT Level 1-6 Turn-On Curve histograms and fit graphs, also deltaR
  for(int ac=0;ac<2;ac++){
    for(int pt=0;pt<6;pt++){
      for(int m=0;m<m_nMuonAlgorithms;m++){// Typically Muid (and Staco)
        //DeltaR
        ss.str(""); ss << "DeltaR_BTW_Track_And_Trigger_PT"<<pt+1 << "_" << muid[m] << "_" << side[ac] ;
        ATH_MSG_DEBUG( ss.str()  );

        m_tgclv1deltar[ac][pt][m] = new TH1F(ss.str().c_str(), (ss.str() + ";DeltaR;Entries").c_str() , 200, -1., 1.); 
        ATH_CHECK(  tgclv1_turnon_numdenom_ac[ac]->regHist(m_tgclv1deltar[ac][pt][m]) );

        for(int pna=0;pna<3;pna++){// Posi/Nega/All
          /////////////////////////////////////
          // Turn on Curve
          // Efficiency
          ss.str(""); ss << "Trigger_Turn_On_Curve" << spna[pna] << "_PT"<< pt+1 << "_" << muid[m] << "_" << side[ac] ;
          ATH_MSG_DEBUG( ss.str()  );
          m_tgclv1turnon[ac][pt][pna][m] = new TH1F(ss.str().c_str(), (ss.str() + ";p_{T} GeV/c;Efficiency").c_str(), pt_nbins, pt_bins); 
          m_tgclv1turnon[ac][pt][pna][m]->Sumw2();
          ATH_CHECK( tgclv1_turnon_ac[ac]->regHist(m_tgclv1turnon[ac][pt][pna][m]) );
          // Efficiency Fit Graph
          m_tgclv1turnontg[ac][pt][pna][m] = new TGraphAsymmErrors();
          m_tgclv1turnontg[ac][pt][pna][m]->SetName( (ss.str() + "_Fit").c_str() );
          m_tgclv1turnontg[ac][pt][pna][m]->SetTitle( (ss.str() + ";p_{T} GeV/c;Efficiency").c_str() );
          m_tgclv1turnontg[ac][pt][pna][m]->SetMarkerStyle(22);
          ATH_CHECK( tgclv1_turnon_ac[ac]->regGraph(m_tgclv1turnontg[ac][pt][pna][m]) );

          // Numerator
          ss.str(""); ss << "Trigger_Turn_On_Curve" << spna[pna] << "_PT"<< pt+1 << "_" << muid[m] << "_" << side[ac] << "_Numerator";
          ATH_MSG_DEBUG( ss.str()  );
          m_tgclv1turnonnum[ac][pt][pna][m] = new TH1F(ss.str().c_str(), (ss.str() + ";p_{T} GeV/c;Entries").c_str(), pt_nbins, pt_bins); 
          m_tgclv1turnonnum[ac][pt][pna][m]->Sumw2();
          ATH_CHECK( tgclv1_turnon_numdenom_ac[ac]->regHist(m_tgclv1turnonnum[ac][pt][pna][m]) );

          // Denominator
          ss.str(""); ss << "Trigger_Turn_On_Curve" << spna[pna] << "_PT"<< pt+1 << "_" << muid[m] << "_" << side[ac] << "_Denominator";
          ATH_MSG_DEBUG( ss.str()  );
          m_tgclv1turnondenom[ac][pt][pna][m] = new TH1F(ss.str().c_str(), (ss.str() + ";p_{T} GeV/c;Entries").c_str(), pt_nbins, pt_bins); 
          m_tgclv1turnondenom[ac][pt][pna][m]->Sumw2();
          ATH_CHECK( tgclv1_turnon_numdenom_ac[ac]->regHist(m_tgclv1turnondenom[ac][pt][pna][m]) );

          /////////////////////////////////////
          // L1 Trigger Efficiency Maps
          for( int pcn = 0 ; pcn < 3 ; pcn++ ){// Prev/Curr/Next
            // Efficiency
            ss.str(""); ss << "Trigger_Efficiency" << spna[pna] << spcn[pcn] << "_PT"<< pt+1 << "_" << muid[m] << "_" << side[ac] ;
            ATH_MSG_DEBUG( ss.str()  );
          
            m_tgclv1effetavsphi[ac][pt][pna][m][pcn] = new TH2F(ss.str().c_str(), (ss.str() + ";eta;phi").c_str(), 28, 1.0, 2.4, 48, -CLHEP::pi/12, 23*CLHEP::pi/12 ); 
            m_tgclv1effetavsphi[ac][pt][pna][m][pcn]->Sumw2();
            ATH_CHECK( tgclv1_turnon_ac[ac]->regHist(m_tgclv1effetavsphi[ac][pt][pna][m][pcn]) );
            m_tgclv1effetavsphi[ac][pt][pna][m][pcn]->SetMaximum(1.0);
          
            // Numerator
            ss.str(""); ss << "Trigger_Efficiency" << spna[pna] << spcn[pcn] << "_PT"<< pt+1 << "_" << muid[m] << "_" << side[ac] << "_Numerator";
            ATH_MSG_DEBUG( ss.str()  );
          
            m_tgclv1effetavsphinum[ac][pt][pna][m][pcn] = new TH2F(ss.str().c_str(), (ss.str() + ";eta;phi").c_str(), 28, 1.0, 2.4, 48, -CLHEP::pi/12, 23*CLHEP::pi/12 ); 
            m_tgclv1effetavsphinum[ac][pt][pna][m][pcn]->Sumw2();
            ATH_CHECK( tgclv1_turnon_numdenom_ac[ac]->regHist(m_tgclv1effetavsphinum[ac][pt][pna][m][pcn]) );
          }//pcn

          // Denominator
          ss.str(""); ss << "Trigger_Efficiency" << spna[pna] << "_PT"<< pt+1 << "_" << muid[m] << "_" << side[ac] << "_Denominator";
          ATH_MSG_DEBUG( ss.str()  );

          m_tgclv1effetavsphidenom[ac][pt][pna][m] = new TH2F(ss.str().c_str(), (ss.str() + ";eta;phi").c_str(), 28, 1.0, 2.4, 48, -CLHEP::pi/12, 23*CLHEP::pi/12 ); 
          m_tgclv1effetavsphidenom[ac][pt][pna][m]->Sumw2();
          ATH_CHECK( tgclv1_turnon_numdenom_ac[ac]->regHist(m_tgclv1effetavsphidenom[ac][pt][pna][m]) );

        }// pna

      }// muid
    }// pT
  }// ac

  ///////////////////////////////////////////////////////////////////////////
  // Express Stream Only
  for(int ac=0;ac<2;ac++){
    /////////////////////////////////////
    // Trigger Turn-On Curve histograms and Fit Graph
    for(int pt=0;pt<6;pt++){
      // Efficiency
      ss.str(""); ss << "ES_Trigger_Turn_On_Curve" << "_PT"<< pt+1 << "_" << side[ac] ;
      ATH_MSG_DEBUG( ss.str()  );
      m_tgclv1turnon_ES[ac][pt] = new TH1F(ss.str().c_str(), (ss.str() + ";p_{T} GeV/c;Efficiency").c_str(), pt_nbins, pt_bins); 
      m_tgclv1turnon_ES[ac][pt]->Sumw2();
      ATH_CHECK( tgclv1_turnon_ac_ES[ac]->regHist(m_tgclv1turnon_ES[ac][pt]) );
      // Efficiency Fit Graph
      m_tgclv1turnontg_ES[ac][pt] = new TGraphAsymmErrors();
      m_tgclv1turnontg_ES[ac][pt]->SetName( (ss.str() + "_Fit").c_str() );
      m_tgclv1turnontg_ES[ac][pt]->SetTitle( (ss.str() + ";p_{T} GeV/c;Efficiency").c_str() );
      m_tgclv1turnontg_ES[ac][pt]->SetMarkerStyle(22);
      ATH_CHECK( tgclv1_turnon_ac_ES[ac]->regGraph(m_tgclv1turnontg_ES[ac][pt]) );

      // Numerator
      ss.str(""); ss << "ES_Trigger_Turn_On_Curve" << "_PT"<< pt+1 << "_" << side[ac] << "_Numerator";
      ATH_MSG_DEBUG( ss.str()  );
      m_tgclv1turnonnum_ES[ac][pt] = new TH1F(ss.str().c_str(), (ss.str() + ";p_{T} GeV/c;Entries").c_str(), pt_nbins, pt_bins); 
      m_tgclv1turnonnum_ES[ac][pt]->Sumw2();
      ATH_CHECK( tgclv1_turnon_numdenom_ac_ES[ac]->regHist(m_tgclv1turnonnum_ES[ac][pt]) );
      
      // Denominator
      ss.str(""); ss << "ES_Trigger_Turn_On_Curve" << "_PT"<< pt+1 << "_" << side[ac] << "_Denominator";
      ATH_MSG_DEBUG( ss.str()  );
      m_tgclv1turnondenom_ES[ac][pt] = new TH1F(ss.str().c_str(), (ss.str() + ";p_{T} GeV/c;Entries").c_str(), pt_nbins, pt_bins); 
      m_tgclv1turnondenom_ES[ac][pt]->Sumw2();
      ATH_CHECK( tgclv1_turnon_numdenom_ac_ES[ac]->regHist(m_tgclv1turnondenom_ES[ac][pt]) );
      
    }//pT

    /////////////////////////////////////
    // Plateau efficiency for each pT threshold
    // Efficiency
    ss.str(""); ss << "ES_Trigger_Efficiency" << "_" << side[ac] ;
    m_tgclv1_plateau_eff_counting_ES[ac] = new TH1F(ss.str().c_str(), (ss.str() + ";PT;efficiency").c_str(), 6, 0, 6 ); 
    m_tgclv1_plateau_eff_counting_ES[ac] ->Sumw2();
    m_tgclv1_plateau_eff_counting_ES[ac] ->SetMinimum(0.7);
    m_tgclv1_plateau_eff_counting_ES[ac] ->SetMaximum(1.05);
    ATH_CHECK( tgclv1_turnon_ac_ES[ac]->regHist( m_tgclv1_plateau_eff_counting_ES[ac] ) );
    
    // Numerator
    ss.str(""); ss << "ES_Trigger_Efficiency_Numerator" << "_" << side[ac] ;
    m_tgclv1_plateau_eff_counting_num_ES[ac] = new TH1F(ss.str().c_str(), (ss.str() + ";PT;efficiency").c_str(), 6, 0, 6 ); 
    m_tgclv1_plateau_eff_counting_num_ES[ac] ->Sumw2();
    ATH_CHECK( tgclv1_turnon_numdenom_ac_ES[ac]->regHist( m_tgclv1_plateau_eff_counting_num_ES[ac] ) );
     
    // Denominator
    ss.str(""); ss << "ES_Trigger_Efficiency_Denominator" << "_" << side[ac] ;
    m_tgclv1_plateau_eff_counting_denom_ES[ac] = new TH1F(ss.str().c_str(), (ss.str() + ";PT;efficiency").c_str(), 6, 0, 6 ); 
    m_tgclv1_plateau_eff_counting_denom_ES[ac] ->Sumw2();
    ATH_CHECK( tgclv1_turnon_numdenom_ac_ES[ac]->regHist( m_tgclv1_plateau_eff_counting_denom_ES[ac] ) );
    
    // Efficiency, General Monitoring
    ss.str(""); ss << "ES_GM_Trigger_Efficiency" << "_" << side[ac] ;
    m_tgclv1_plateau_eff_counting_ES_GM[ac] = new TH1F(ss.str().c_str(), (ss.str() + ";PT;efficiency").c_str(), 6, 0, 6 ); 
    m_tgclv1_plateau_eff_counting_ES_GM[ac] ->Sumw2();
    m_tgclv1_plateau_eff_counting_ES_GM[ac] ->SetMinimum(0.7);
    m_tgclv1_plateau_eff_counting_ES_GM[ac] ->SetMaximum(1.05);
    ATH_CHECK( tgclv1_turnon_ac_ES_GM[ac]->regHist( m_tgclv1_plateau_eff_counting_ES_GM[ac] ) );

    // Set Bin Labels
    for( int bin=0;bin<6;bin++){
      ss.str(""); ss << "PT" << bin+1 ;
      m_tgclv1_plateau_eff_counting_ES[ac] -> GetXaxis() -> SetBinLabel(bin+1, ss.str().c_str() );
      m_tgclv1_plateau_eff_counting_ES_GM[ac] -> GetXaxis() -> SetBinLabel(bin+1, ss.str().c_str() );
      m_tgclv1_plateau_eff_counting_num_ES[ac] -> GetXaxis() -> SetBinLabel(bin+1, ss.str().c_str() );
      m_tgclv1_plateau_eff_counting_denom_ES[ac] -> GetXaxis() -> SetBinLabel(bin+1, ss.str().c_str() );
    }//bin

  }//ac

  return sc;
}// EOF bookEfficiencyHisto



///////////////////////////////////////////////////////////////////////////
// fillNumberOfTrigger
///////////////////////////////////////////////////////////////////////////
// Fills Trigger Turn-on Curves and Efficiency Maps/Profiles
void
TgcLv1RawDataValAlg::fillEfficiency(int ms,// 0:Muid 1:Staco
                                    std::vector<float>* mu_pt, std::vector<float>* mu_eta,
                                    std::vector<float>* mu_phi,std::vector<float>* mu_q){
  ATH_MSG_DEBUG( "inside fillEfficiency"  );
  int osize = mu_pt->size();
  float deltarcutpt1 = 0.5;
  float deltarcut = 0.25;

  // Skip Event if any Offline Muons were found close to each other
  for(int o1=0;o1<osize;o1++){
    for(int o2=o1+1;o2<osize;o2++){
      float eta1 = mu_eta->at(o1);
      float phi1 = mu_phi->at(o1);
      float eta2 = mu_eta->at(o2);
      float phi2 = mu_phi->at(o2);
      float trackdr = deltaR(eta1, phi1, eta2, phi2);

      if(trackdr < 0.8 ) return;
    }
  }
  // pT thresholds for etavsphi maps
  float thresetavsphi[6] = { 4., 6., 10., 10., 15., 20.};
  // pT thresholds for counting(?) histograms
  float threscount[6] = { 10., 10., 12., 12., 18., 23.};

  // Loop Offline Muon Vectors
  for(int o=0;o<osize;o++){
    // Get Offline Muon VariabCLHEPles
    float opt  = mu_pt->at(o)/CLHEP::GeV; 
    float oeta = mu_eta->at(o);
    float ophi = mu_phi->at(o);
    float oq   = mu_q->at(o);
    // Get Charge index
    int pn=0;
    if(oq<0)pn=1;
    // Get Side
    int ac = (oeta<0);
    // Set pTs above 50 to 50.1
    if(std::abs(opt)>50.) opt = 50.1*oq;

    //ophi = -pi to pi
    //ophi_mod = -pi/12 to 23*pi/12
    float ophi_mod = (ophi < -CLHEP::pi/12 ? ophi+2*CLHEP::pi : ophi );

    // This Block Fills Denominators
    // Loop pT Thresholds
    for(int pt=0;pt<6;pt++){
      m_tgclv1turnondenom[ac][pt][pn][ms]->Fill(opt);
      if(opt> thresetavsphi[pt]){
        m_tgclv1effetavsphidenom[ac][pt][pn][ms]->Fill(std::abs(oeta),ophi_mod);
      }
      if( m_found_express_stream && m_found_nonmuon_express_chain ){
        m_tgclv1turnondenom_ES[ac][pt]->Fill(opt);
        if( opt > threscount[pt] )
          m_tgclv1_plateau_eff_counting_denom_ES[ac]->Fill(pt);
      }
    }
    
    // Initialize variables for storing best SL trigger's information
    int slpt=-1;
    int sltidw=-1;
    int sltids=-1;
    bool slisAside=false;
    bool slisForward=false;
    int slphi48=-1;
    
    // This Block fills Numerators
    // Loop over Timing values
    for(int pcn=0;pcn<3;pcn++){
      // Initialise pT Threshold flags
      bool flag[6];
      for( int pt =0; pt<6;pt++)flag[pt]=false;

      float deltarmin=1.;
      int   tptmin=-1;
      float charge=0;
      
      // Loop over SL Trigger Vectors
      int tsize = m_sl_pt[pcn].size();
      for(int isl=0;isl<tsize;isl++){
        // Get Variables
        int   tpt       = m_sl_pt[pcn].at(isl);
        float teta      = m_sl_eta[pcn].at(isl);
        float tphi      = m_sl_phi[pcn].at(isl);
        int   tidw      = m_sl_tidw[pcn].at(isl);
        int   tids      = m_sl_tids[pcn].at(isl);
        bool  isAside   = m_sl_isAside[pcn].at(isl);
        bool  isForward = m_sl_isForward[pcn].at(isl);
        int   phi48     = m_sl_phi48[pcn].at(isl);

        // Check that SL Trigger is on same side as Offline Muon
        if(oeta*teta<0.) continue;
        // Check that SL Trigger is close to Offline Muon
        float deltar = deltaR(oeta, ophi, teta, tphi);
        if(deltar>1.) continue;

        // Select SL Trigger with lowest dR
        if(deltar<deltarmin){
          deltarmin=deltar;
          tptmin=tpt;
          charge=oq;
          // Select SL Trigger with highest pT
          if(slpt<tpt){
            slpt=tpt;
            sltidw=tidw;
            sltids=tids;
            slisAside=isAside;
            slisForward=isForward;
            slphi48=phi48;
          }
        }

        // Set pT Trigger Flags based on dR
        //PT1
        if(deltar<deltarcutpt1) flag[0] = true;
        //PT2-PT6
        if(deltar<deltarcut){
          for(int pt=1;pt<tpt;pt++){
            flag[pt]=true;
          }
        }

      }// SL triggers

      // Fill deltaR histogram and turn-on curve numerators for current timing
      if(pcn==1){
        // If dR < 1 SL Trigger found, fill minimum dR found
        if(tptmin!=-1){
          m_tgclv1deltar[ac][tptmin-1][ms]->Fill(deltarmin*charge);
        }
        // Loop over pT Thresholds, fill Numerators if Thresholds met 
        for(int pt=0;pt<6;pt++){
          if(flag[pt]==true){
            m_tgclv1turnonnum[ac][pt][pn][ms]->Fill(opt);
            if(m_found_express_stream && m_found_nonmuon_express_chain){
              m_tgclv1turnonnum_ES[ac][pt]->Fill(opt);
              if(opt>threscount[pt])
                m_tgclv1_plateau_eff_counting_num_ES[ac]->Fill(pt);
            }
          }
        }
      }
      // Fill efficiency map numerators
      for(int pt=0;pt<6;pt++){
        if(flag[pt]==true){
          if(opt> thresetavsphi[pt]){
            m_tgclv1effetavsphinum[ac][pt][pn][ms][pcn]->Fill(std::abs(oeta),ophi_mod);
          }
        }
      }
    }// pcn
    
    
    bool lptflag=false;
    if( slpt > 0 ) {
      for(int pcn=2;pcn>-1;pcn--){
        
        for(int lptw=0;lptw<(int)m_lpt_delta[0][pcn].size();lptw++){
          int tidlptw       = m_lpt_tid[0][pcn].at(lptw);
          int isAsidelptw   = m_lpt_isAside[0][pcn].at(lptw);
          int isForwardlptw = m_lpt_isForward[0][pcn].at(lptw);
          int phi48lptw     = m_lpt_phi48[0][pcn].at(lptw);
          
          if( tidlptw != sltidw ||
              isAsidelptw != slisAside ||
              isForwardlptw != slisForward ||
              phi48lptw != slphi48 ) continue ;

          
          for(int lpts=0;lpts<(int)m_lpt_delta[1][pcn].size();lpts++){
            int tidlpts       = m_lpt_tid[1][pcn].at(lpts);
            int isAsidelpts   = m_lpt_isAside[1][pcn].at(lpts);
            int isForwardlpts = m_lpt_isForward[1][pcn].at(lpts);
            int phi48lpts     = m_lpt_phi48[1][pcn].at(lpts);
            
            if( tidlpts != sltids ||
                isAsidelpts != slisAside ||
                isForwardlpts != slisForward ||
                phi48lpts != slphi48 ) continue ;
            
            ATH_MSG_DEBUG( "fill triggertiming " <<ac <<" " << ms << " " << pcn  );
            m_tgclv1lpttimingtrack[ac][ms]->Fill(pcn);
            if( slpt > 1 )
              m_tgclv1lpttimingptcuttrack[ac][ms]->Fill(pcn);

            lptflag=true;
            break;
          }//lpt strip
          if(lptflag==true)break;
        }//lpt wire
        if(lptflag==true)break;
      }//pcn
    }//if slpt

  }//offline muid

  ATH_MSG_DEBUG( "finish fillEfficiency"  );
}// EOF fillEfficiency
