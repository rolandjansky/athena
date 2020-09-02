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

// GeoModel
#include "MuonReadoutGeometry/TgcReadoutParams.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
#include "MuonDQAUtils/MuonDQAHistMap.h" 
 
#include "Identifier/Identifier.h"

#include "TgcRawDataMonitoring/TgcLv1RawDataValAlg.h"
#include "AthenaMonitoring/AthenaMonManager.h"

//for express menu
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"

#include <TH1F.h>
#include <TH2F.h>
#include <inttypes.h> 

#include <sstream>

///////////////////////////////////////////////////////////////////////////
// bookTimingHisto
///////////////////////////////////////////////////////////////////////////
StatusCode
TgcLv1RawDataValAlg::bookHistogramsTiming(){
  
  ///////////////////////////////////////////////////////////////////////////
  // Make MonGroups for histogram booking paths
  std::string generic_path_tgclv1 = "Muon/MuonRawDataMonitoring/TGCLV1";
  MonGroup tgclv1_shift( this, generic_path_tgclv1+"/Global", run, ATTRIB_UNMANAGED );
  MonGroup tgclv1_shift_a( this, generic_path_tgclv1+"/TGCEA", run, ATTRIB_UNMANAGED );
  MonGroup tgclv1_shift_c( this, generic_path_tgclv1+"/TGCEC", run, ATTRIB_UNMANAGED );
  MonGroup* tgclv1_shift_ac[2] = { &tgclv1_shift_a, &tgclv1_shift_c};

  MonGroup tgclv1_expert( this, generic_path_tgclv1+"/Global", run, ATTRIB_UNMANAGED );
  MonGroup tgclv1_expert_a( this, generic_path_tgclv1+"/TGCEA", run, ATTRIB_UNMANAGED );
  MonGroup tgclv1_expert_c( this, generic_path_tgclv1+"/TGCEC", run, ATTRIB_UNMANAGED );
  MonGroup* tgclv1_expert_ac[2] = { &tgclv1_expert_a, &tgclv1_expert_c};

  MonGroup tgclv1_timing( this, generic_path_tgclv1+"/Global/Timing", run, ATTRIB_UNMANAGED );
  MonGroup tgclv1_timing_a( this, generic_path_tgclv1+"/TGCEA/Timing", run, ATTRIB_UNMANAGED );
  MonGroup tgclv1_timing_c( this, generic_path_tgclv1+"/TGCEC/Timing", run, ATTRIB_UNMANAGED );
  MonGroup* tgclv1_timing_ac[2] = { &tgclv1_timing_a, &tgclv1_timing_c};

  MonGroup tgclv1_timing_a_ES( this, generic_path_tgclv1+"/TGCEA/ES/Timing", run, ATTRIB_UNMANAGED );
  MonGroup tgclv1_timing_c_ES( this, generic_path_tgclv1+"/TGCEC/ES/Timing", run, ATTRIB_UNMANAGED );
  MonGroup* tgclv1_timing_ac_ES[2] = { &tgclv1_timing_a_ES, &tgclv1_timing_c_ES};

  MonGroup tgclv1_timing_a_ES_GM( this, generic_path_tgclv1+"/TGCEA/ES/GM", run, ATTRIB_UNMANAGED, "", "weightedEff" );
  MonGroup tgclv1_timing_c_ES_GM( this, generic_path_tgclv1+"/TGCEC/ES/GM", run, ATTRIB_UNMANAGED, "", "weightedEff");
  MonGroup* tgclv1_timing_ac_ES_GM[2] = { &tgclv1_timing_a_ES_GM, &tgclv1_timing_c_ES_GM};

  int k=0;
  std::stringstream ss;
  std::string side[2]          ={"A","C"};
  std::string triggertype[4]   ={"_TGC_Triggered", "_RPC_Low_Triggered", "_RPC_High_Triggered", "_L1Calo_Triggered"};
  std::string smuid[1]         ={"_Muid"};
  std::string schamberT3[6]    ={"E1", "E2", "E3", "E4", "E5", "F"};
  std::string sbc[3]           ={"_{Prev}", "_{Curr}", "_{Next}"};
  std::string lpt              = "Low_Pt_";
  std::string lpttiming        = "Low_Pt_Timing";
  std::string sl               = "SL_";
  std::string sltiming         = "SL_Timing";
  std::string slvslpttiming    = "SL_Vs_Low_Pt_Timing";
  std::string Sector           = "_Sector";
  std::string Layer            = "_Layer";
  std::string PT               = "_PT";
  std::string morethanpt1      = "_More_Than_PT1";
  std::string lessthanpt2      = "_Less_Than_PT2";


  ///////////////////////////////////////////////////////////////////////////
  // SL Timing histograms
  for(int ac=0;ac<2;ac++){// side
    /////////////////////////////////////
    // SL Timing hists per side
    // SL Timing hist
    ss.str(""); ss << sltiming << "_" << side[ac];
    m_tgclv1sltiming[ac] = new TH1F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3);
    setTH1TitleLabelBCID(m_tgclv1sltiming[ac]);
    ATH_CHECK( tgclv1_shift_ac[ac]->regHist(m_tgclv1sltiming[ac]) );
    
    // SL Timing hist for pT threshold > pT1
    ss.str(""); ss << sltiming << morethanpt1 << "_" << side[ac];
    m_tgclv1sltimingptcut[ac] = new TH1F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3);
    setTH1TitleLabelBCID(m_tgclv1sltimingptcut[ac]);
    ATH_CHECK( tgclv1_shift_ac[ac]->regHist(m_tgclv1sltimingptcut[ac]) );
    
    // SL Timing hist for each pT threshold
    for(int ipt=0;ipt<6;ipt++){
      ss.str(""); ss << sltiming << PT << ipt+1 << "_" << side[ac];
      m_tgclv1sltimingpt[ac][ipt] = new TH1F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3);
      setTH1TitleLabelBCID(m_tgclv1sltimingpt[ac][ipt]);
      ATH_CHECK( tgclv1_timing_ac[ac]->regHist(m_tgclv1sltimingpt[ac][ipt]) );
    }
    
    /////////////////////////////////////
    // SL Timing maps per side
    // Timing maps show prev,curr,next stationEta (pcn3xeta6) vs stationPhi (phi48)
    // SL Timing map
    ss.str(""); ss << sltiming << "_Map_" << side[ac];
    m_tgclv1sltimingmap[ac] = new TH2F(ss.str().c_str(),ss.str().c_str(), 18, 0, 18, 48, 1 , 49);
    ATH_CHECK( tgclv1_expert_ac[ac]->regHist(m_tgclv1sltimingmap[ac]) );
    m_tgclv1sltimingmap[ac]->SetMinimum(0.0);
    
    // SL Timing Current Fraction map
    ss.str(""); ss << sl << "Timing_Fraction_Map_" << side[ac];
    m_tgclv1slcurrentfractionmap[ac] = new TH2F(ss.str().c_str(),ss.str().c_str(), 18, 0, 18, 48, 1 , 49);
    ATH_CHECK( tgclv1_expert_ac[ac]->regHist(m_tgclv1slcurrentfractionmap[ac]) );
    
    // SL Timing map for pT threshold > pT1
    ss.str(""); ss << sltiming << "_Map" << morethanpt1 << "_" << side[ac];
    m_tgclv1sltimingmapptcut[ac] = new TH2F(ss.str().c_str(),ss.str().c_str(), 18, 0, 18, 48, 1 , 49);
    ATH_CHECK( tgclv1_expert_ac[ac]->regHist(m_tgclv1sltimingmapptcut[ac]) );
    m_tgclv1sltimingmapptcut[ac]->SetMinimum(0.0);
    
    // SL Timing Current Fraction map for pT threshold > pT1
    ss.str(""); ss << sl << "Timing_Fraction_Map" << morethanpt1 << side[ac];
    m_tgclv1slcurrentfractionmapptcut[ac] = new TH2F(ss.str().c_str(),ss.str().c_str(), 18, 0, 18, 48, 1 , 49);
    ATH_CHECK( tgclv1_expert_ac[ac]->regHist(m_tgclv1slcurrentfractionmapptcut[ac]) );
    
    // Set Bin Labels for SL Timing maps
    for(int pcn=0;pcn<3;pcn++){
      for(int eta=0;eta<6;eta++){
        m_tgclv1sltimingmap[ac]         ->GetXaxis()->SetBinLabel(pcn*6 + eta+1, (schamberT3[eta] + sbc[pcn]).c_str()); 
        m_tgclv1slcurrentfractionmap[ac]->GetXaxis()->SetBinLabel(pcn*6 + eta+1, (schamberT3[eta] + sbc[pcn]).c_str()); 
        m_tgclv1sltimingmapptcut[ac]         ->GetXaxis()->SetBinLabel(pcn*6 + eta+1, (schamberT3[eta] + sbc[pcn]).c_str()); 
        m_tgclv1slcurrentfractionmapptcut[ac]->GetXaxis()->SetBinLabel(pcn*6 + eta+1, (schamberT3[eta] + sbc[pcn]).c_str());
      }// eta
    }// pcn
    k=1;
    for(int sec=1;sec<=12;sec++){
      for(int phi=0;phi<4;phi+=4){
        ss.str(""); ss << side[ac];
        if(sec<10)ss << "0";
        ss << sec << "Ephi" << phi;
        m_tgclv1sltimingmap[ac]         ->GetYaxis()->SetBinLabel(k, ss.str().c_str());
        m_tgclv1slcurrentfractionmap[ac]->GetYaxis()->SetBinLabel(k, ss.str().c_str());
        m_tgclv1sltimingmapptcut[ac]         ->GetYaxis()->SetBinLabel(k, ss.str().c_str());
        m_tgclv1slcurrentfractionmapptcut[ac]->GetYaxis()->SetBinLabel(k, ss.str().c_str());
      }// phi
    }// sector
    
    /////////////////////////////////////
    // SL Timing per sector
    for(int isect=0;isect<12;isect++){// sector
      // SL Timing hist
      ss.str(""); ss << sltiming << "_" << side[ac];
      if(isect+1<10)ss << "0";
      ss << isect+1;
      m_tgclv1sltimingsector[ac][isect] = new TH1F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3);
      ATH_CHECK( tgclv1_timing_ac[ac]->regHist(m_tgclv1sltimingsector[ac][isect]) );
      setTH1TitleLabelBCID(m_tgclv1sltimingsector[ac][isect]);
      
      // SL Timing hist for pT threshold > pT1
      ss.str(""); ss << sltiming << morethanpt1 << "_" << side[ac];
      if(isect+1<10)ss << "0";
      ss << isect+1;
      m_tgclv1sltimingptcutsector[ac][isect] = new TH1F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3);
      ATH_CHECK( tgclv1_timing_ac[ac]->regHist(m_tgclv1sltimingptcutsector[ac][isect]) );
      setTH1TitleLabelBCID(m_tgclv1sltimingptcutsector[ac][isect]);
    }// sector

    /////////////////////////////////////
    // SL Timing per trigger type
    for(int itrig=0;itrig<4;itrig++){// trigger type
      // SL Timing hist
      ss.str(""); ss << sltiming << triggertype[itrig] << "_" << side[ac];
      m_tgclv1sltimingtrg[ac][itrig] = new TH1F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3);
      setTH1TitleLabelBCID(m_tgclv1sltimingtrg[ac][itrig]);
      ATH_CHECK( tgclv1_expert_ac[ac]->regHist(m_tgclv1sltimingtrg[ac][itrig]) );

      // SL Timing hist for pT threshold > pT1
      ss.str(""); ss << sltiming << morethanpt1 << triggertype[itrig] << "_" << side[ac];
      m_tgclv1sltimingptcuttrg[ac][itrig] = new TH1F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3);
      setTH1TitleLabelBCID(m_tgclv1sltimingptcuttrg[ac][itrig]);
      ATH_CHECK( tgclv1_expert_ac[ac]->regHist(m_tgclv1sltimingptcuttrg[ac][itrig]) );

      // SL Timing hist for each pT threshold
      for(int ipt=0;ipt<6;ipt++){// pT
        ss.str(""); ss << sltiming << PT << ipt+1 << triggertype[itrig] << "_" << side[ac];
        m_tgclv1sltimingpttrg[ac][ipt][itrig] = new TH1F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3);
        setTH1TitleLabelBCID(m_tgclv1sltimingpttrg[ac][ipt][itrig]);
        ATH_CHECK( tgclv1_timing_ac[ac]->regHist(m_tgclv1sltimingpttrg[ac][ipt][itrig]) );
      }// pT
    }// trigger type
    
    /////////////////////////////////////
    // SL Timing per sector-trigger
    for(int isect=0;isect<12;isect++){// sector
      for(int itrig=0;itrig<4;itrig++){// trigger type
        // SL Timing hist
        ss.str(""); ss << sltiming << triggertype[itrig] << "_" << side[ac];
        if(isect+1<10)ss << "0";
        ss << isect+1;
        m_tgclv1sltimingsectortrg[ac][isect][itrig] = new TH1F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3);
        ATH_CHECK( tgclv1_timing_ac[ac]->regHist(m_tgclv1sltimingsectortrg[ac][isect][itrig]) );
        setTH1TitleLabelBCID(m_tgclv1sltimingsectortrg[ac][isect][itrig]);
        
        // SL Timing hist for pT threshold > pT1
        ss.str(""); ss << sltiming << morethanpt1 << triggertype[itrig] << "_" << side[ac];
        if(isect+1<10)ss << "0";
        ss << isect+1;
        ATH_MSG_DEBUG( "histos for SL sector timing for pt>2"  );
        m_tgclv1sltimingptcutsectortrg[ac][isect][itrig] = new TH1F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3);
        ATH_CHECK( tgclv1_timing_ac[ac]->regHist(m_tgclv1sltimingptcutsectortrg[ac][isect][itrig]) );
        setTH1TitleLabelBCID(m_tgclv1sltimingptcutsectortrg[ac][isect][itrig]);
      }// trigger type
    }// sector
    
    /////////////////////////////////////
    // SL Timing of associated offline muons
    for(int imuid=0;imuid<m_nMuonAlgorithms;imuid++){// muonalg
      // SL Timing hist
      ss.str(""); ss << sltiming << smuid[imuid] << "_" << side[ac];
      m_tgclv1sltimingtrack[ac][imuid] = new TH1F(ss.str().c_str(), ss.str().c_str(), 3, 0, 3 );
      setTH1TitleLabelBCID(m_tgclv1sltimingtrack[ac][imuid]);
      ATH_CHECK( tgclv1_shift_ac[ac]->regHist(m_tgclv1sltimingtrack[ac][imuid] ) );
      
      // SL Timing hist for pT threshold > pT1
      ss.str(""); ss << sltiming << morethanpt1 << smuid[imuid] << "_" << side[ac];
      m_tgclv1sltimingptcuttrack[ac][imuid] = new TH1F(ss.str().c_str(), ss.str().c_str(), 3, 0, 3 );
      setTH1TitleLabelBCID(m_tgclv1sltimingptcuttrack[ac][imuid]);
      ATH_CHECK( tgclv1_shift_ac[ac]->regHist(m_tgclv1sltimingptcuttrack[ac][imuid] ) );
    }// muonalg

  }// side
  
  
  ///////////////////////////////////////////////////////////////////////////
  // LpT Timing histograms
  for(int ac=0;ac<2;ac++){// side
    /////////////////////////////////////
    // LpT Timing hists per side
    // LpT Timing hist
    ss.str(""); ss << lpttiming << "_" << side[ac];
    m_tgclv1lpttiming[ac] = new TH1F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3);
    ATH_CHECK(  tgclv1_shift_ac[ac]->regHist(m_tgclv1lpttiming[ac]) );
    setTH1TitleLabelBCID(m_tgclv1lpttiming[ac]);

    // LpT Timing hist for pT threshold > pT1
    ss.str(""); ss << lpttiming << morethanpt1 << "_" << side[ac];
    m_tgclv1lpttimingptcut[ac] = new TH1F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3);
    ATH_CHECK( tgclv1_expert_ac[ac]->regHist(m_tgclv1lpttimingptcut[ac]) );
    setTH1TitleLabelBCID(m_tgclv1lpttimingptcut[ac]);

    // LpT Timing hist for each pT threshold
    for(int ipt=0;ipt<6;ipt++){
      ss.str(""); ss << lpttiming << PT << ipt+1 << "_" << side[ac];
      m_tgclv1lpttimingpt[ac][ipt] = new TH1F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3);
      ATH_CHECK( tgclv1_timing_ac[ac]->regHist(m_tgclv1lpttimingpt[ac][ipt]) );
      m_tgclv1lpttimingpt[ac][ipt]->SetMinimum(0.0);
      setTH1TitleLabelBCID(m_tgclv1lpttimingpt[ac][ipt]);
    }

    /////////////////////////////////////
    // LpT Timing maps per side
    // Timing maps show prev,curr,next stationEta (pcn3xeta6) vs stationPhi (phi48)
    // LpT Timing map
    ss.str(""); ss << lpttiming << "_Map_" << side[ac];
    m_tgclv1lpttimingmap[ac] = new TH2F(ss.str().c_str(),ss.str().c_str(), 18, 0, 18, 48, 1 , 49);
    ATH_CHECK( tgclv1_expert_ac[ac]->regHist(m_tgclv1lpttimingmap[ac]) );
    m_tgclv1lpttimingmap[ac]->SetMinimum(0.0);
    
    // LpT Timing Current Fraction map
    ss.str(""); ss << lpt << "Timing_Fraction_Map_" << side[ac];
    m_tgclv1lptcurrentfractionmap[ac] = new TH2F(ss.str().c_str(),ss.str().c_str(), 18, 0, 18, 48, 1 , 49);
    ATH_CHECK( tgclv1_expert_ac[ac]->regHist(m_tgclv1lptcurrentfractionmap[ac]) );
    
    // LpT Timing map for pT threshold > pT1
    ss.str(""); ss << lpttiming << "_Map" << morethanpt1 << side[ac];
    m_tgclv1lpttimingmapptcut[ac] = new TH2F(ss.str().c_str(),ss.str().c_str(), 18, 0, 18, 48, 1 , 49);
    ATH_CHECK( tgclv1_expert_ac[ac]->regHist(m_tgclv1lpttimingmapptcut[ac]) );
    m_tgclv1lpttimingmapptcut[ac]->SetMinimum(0.0);
    
    // LpT Timing Current Fraction map for pT threshold > pT1
    ss.str(""); ss << lpt << "Timing_Fraction_Map" << morethanpt1 << side[ac];
    m_tgclv1lptcurrentfractionmapptcut[ac] = new TH2F(ss.str().c_str(),ss.str().c_str(), 18, 0, 18, 48, 1 , 49);
    ATH_CHECK( tgclv1_expert_ac[ac]->regHist(m_tgclv1lptcurrentfractionmapptcut[ac]) );

    // Set Bin Labels for LpT Timing maps
    for(int pcn=0;pcn<3;pcn++){// 
      for(int eta=0;eta<6;eta++){
        m_tgclv1lpttimingmap[ac]         ->GetXaxis()->SetBinLabel(pcn*6 + eta+1, (schamberT3[eta] + sbc[pcn]).c_str()); 
        m_tgclv1lptcurrentfractionmap[ac]->GetXaxis()->SetBinLabel(pcn*6 + eta+1, (schamberT3[eta] + sbc[pcn]).c_str()); 
        m_tgclv1lpttimingmapptcut[ac]         ->GetXaxis()->SetBinLabel(pcn*6 + eta+1, (schamberT3[eta] + sbc[pcn]).c_str()); 
        m_tgclv1lptcurrentfractionmapptcut[ac]->GetXaxis()->SetBinLabel(pcn*6 + eta+1, (schamberT3[eta] + sbc[pcn]).c_str()); 
      }// chambertype
    }// pcn
    k=1;
    for(int sec=1;sec<=12;sec++){
      for(int phi=0;phi<4;phi+=4){
        ss.str(""); ss << side[ac];
        if(sec<10)ss << "0";
        ss << sec << "Ephi" << phi;
        m_tgclv1lpttimingmap[ac]         ->GetYaxis()->SetBinLabel(k, ss.str().c_str());
        m_tgclv1lptcurrentfractionmap[ac]->GetYaxis()->SetBinLabel(k, ss.str().c_str());
        m_tgclv1lpttimingmapptcut[ac]    ->GetYaxis()->SetBinLabel(k, ss.str().c_str());
        m_tgclv1lptcurrentfractionmapptcut[ac]->GetYaxis()->SetBinLabel(k, ss.str().c_str());
        k+=4;
      }// phi
    }// sector

    /////////////////////////////////////
    // LpT Timing per sector 
    for(int k=0;k<12;k++){// sector
      // LpT Timing hist
      ss.str(""); ss << lpttiming << "_" << side[ac];
      if(k+1<10)ss << "0";
      ss << k+1;
      m_tgclv1lpttimingsector[ac][k] = new TH1F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3);
      ATH_CHECK( tgclv1_timing_ac[ac]->regHist(m_tgclv1lpttimingsector[ac][k]) );
      setTH1TitleLabelBCID(m_tgclv1lpttimingsector[ac][k]);

      // LpT Timing hist for pT threshold > pT1
      ATH_MSG_DEBUG( "histos for LPT sector timing for pt>2"  );
      ss.str(""); ss << lpttiming << morethanpt1 << "_" << side[ac];
      if(k+1<10)ss << "0";
      ss << k+1;
      m_tgclv1lpttimingptcutsector[ac][k] = new TH1F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3);
      ATH_CHECK( tgclv1_timing_ac[ac]->regHist(m_tgclv1lpttimingptcutsector[ac][k]) );
      setTH1TitleLabelBCID(m_tgclv1lpttimingptcutsector[ac][k]);
    }// sector
    
    /////////////////////////////////////
    // LpT Timing per trigger type
    for(int itrig=0;itrig<4;itrig++){// trigger type
      // LpT Timing hist
      ss.str(""); ss << lpttiming << triggertype[itrig] << "_" << side[ac];
      m_tgclv1lpttimingtrg[ac][itrig] = new TH1F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3);
      setTH1TitleLabelBCID(m_tgclv1lpttimingtrg[ac][itrig]);
      ATH_CHECK( tgclv1_expert_ac[ac]->regHist(m_tgclv1lpttimingtrg[ac][itrig]) );
      m_tgclv1lpttimingtrg[ac][itrig]->SetMinimum(0.0);
      
      // LpT Timing hist for pT threshold > pT1
      ss.str(""); ss << lpttiming << morethanpt1 << triggertype[itrig] << "_" << side[ac];
      m_tgclv1lpttimingptcuttrg[ac][itrig] = new TH1F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3);
      setTH1TitleLabelBCID(m_tgclv1lpttimingptcuttrg[ac][itrig]);
      ATH_CHECK( tgclv1_expert_ac[ac]->regHist(m_tgclv1lpttimingptcuttrg[ac][itrig]) );
      m_tgclv1lpttimingptcuttrg[ac][itrig]->SetMinimum(0.0);
      
      // LpT Timing hist for each pT threshold
      for(int ipt=0;ipt<6;ipt++){
        ss.str(""); ss << lpttiming << PT << ipt+1 << triggertype[itrig] << "_" << side[ac];
        m_tgclv1lpttimingpttrg[ac][ipt][itrig] = new TH1F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3);
        setTH1TitleLabelBCID(m_tgclv1lpttimingpttrg[ac][ipt][itrig]);
        ATH_CHECK( tgclv1_timing_ac[ac]->regHist(m_tgclv1lpttimingpttrg[ac][ipt][itrig]) );
        m_tgclv1lpttimingpttrg[ac][ipt][itrig]->SetMinimum(0.0);
      }// pT
    }// trigger type

    /////////////////////////////////////
    // LpT Timing per sector-trigger
    for(int isect=0;isect<12;isect++){// sector
      for(int itrig=0;itrig<4;itrig++){// trigger type
        // LpT Timing hist
        ss.str(""); ss << lpttiming << triggertype[itrig] << "_" << side[ac];
        if(isect+1<10)ss << "0";
        ss << isect+1;
        m_tgclv1lpttimingsectortrg[ac][isect][itrig] = new TH1F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3);
        ATH_CHECK( tgclv1_timing_ac[ac]->regHist(m_tgclv1lpttimingsectortrg[ac][isect][itrig]) );
        setTH1TitleLabelBCID(m_tgclv1lpttimingsectortrg[ac][isect][itrig]);
        
        // LpT Timing hist for pT threshold > pT1
        ss.str(""); ss << lpttiming << morethanpt1 << triggertype[itrig] << "_" << side[ac];
        if(isect+1<10)ss << "0";
        ss << isect+1;
        m_tgclv1lpttimingptcutsectortrg[ac][isect][itrig] = new TH1F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3);
        ATH_CHECK( tgclv1_timing_ac[ac]->regHist(m_tgclv1lpttimingptcutsectortrg[ac][isect][itrig]) );
        setTH1TitleLabelBCID(m_tgclv1lpttimingptcutsectortrg[ac][isect][itrig]);
      }// trigger type
    }// sector

    /////////////////////////////////////
    // LpT Timing of associated offline muons
    for(int imuid=0;imuid<m_nMuonAlgorithms;imuid++){// muonalg
      // LpT Timing hist
      ss.str(""); ss << lpttiming << smuid[imuid] << "_" << side[ac];
      m_tgclv1lpttimingtrack[ac][imuid] = new TH1F(ss.str().c_str(), ss.str().c_str(), 3, 0, 3 );
      setTH1TitleLabelBCID(m_tgclv1lpttimingtrack[ac][imuid]);
      ATH_CHECK( tgclv1_shift_ac[ac]->regHist(m_tgclv1lpttimingtrack[ac][imuid] ) );
      
      // LpT Timing hist for pT threshold > pT1
      ss.str(""); ss << lpttiming << morethanpt1 << smuid[imuid] << "_" << side[ac];
      m_tgclv1lpttimingptcuttrack[ac][imuid] = new TH1F(ss.str().c_str(), ss.str().c_str(), 3, 0, 3 );
      setTH1TitleLabelBCID(m_tgclv1lpttimingptcuttrack[ac][imuid]);
      ATH_CHECK( tgclv1_shift_ac[ac]->regHist(m_tgclv1lpttimingptcuttrack[ac][imuid] ) );
    }// muonalg

  }// side

  ///////////////////////////////////////////////////////////////////////////
  // SL vs LpT Timings
  for(int ac=0;ac<2;ac++){// side
    /////////////////////////////////////
    // SL Timing hists per side
    // SL vs LpT Timing hist
    ss.str(""); ss << slvslpttiming << "_" << side[ac];
    m_tgclv1slvslpttiming[ac] = new TH2F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3, 3, 0, 3);
    ATH_CHECK( tgclv1_shift_ac[ac]->regHist(m_tgclv1slvslpttiming[ac]) );
    setTH2TitleLabelBCID(m_tgclv1slvslpttiming[ac]);
    
    // SL vs LpT Timing hists for pT threshold > pT1
    ss.str(""); ss << slvslpttiming << morethanpt1 << "_" << side[ac];
    m_tgclv1slvslpttimingptcut[ac] = new TH2F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3, 3, 0, 3);
    ATH_CHECK( tgclv1_shift_ac[ac]->regHist(m_tgclv1slvslpttimingptcut[ac]) );
    setTH2TitleLabelBCID(m_tgclv1slvslpttimingptcut[ac]);

    /////////////////////////////////////
    // SL Timing per sector
    for(int isect=0;isect<12;isect++){// sector
      // SL vs LpT Timing matrix
      ss.str(""); ss << slvslpttiming << "_" << side[ac];
      if(isect+1<10)ss << "0";
      ss << isect+1;
      m_tgclv1slvslpttimingsector[ac][isect] = new TH2F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3, 3, 0, 3);
      ATH_CHECK( tgclv1_timing_ac[ac]->regHist(m_tgclv1slvslpttimingsector[ac][isect]) );
      setTH1TitleLabelBCID(m_tgclv1slvslpttimingsector[ac][isect]);
      
      // SL vs LpT Timing matrix for pT threshold > pT1
      ss.str(""); ss << slvslpttiming << morethanpt1 << "_" << side[ac];
      if(isect+1<10)ss << "0";
      ss << isect+1;
      m_tgclv1slvslpttimingptcutsector[ac][isect] = new TH2F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3, 3, 0, 3);
      ATH_CHECK( tgclv1_timing_ac[ac]->regHist(m_tgclv1slvslpttimingptcutsector[ac][isect]) );
      setTH1TitleLabelBCID(m_tgclv1slvslpttimingptcutsector[ac][isect]);
    }// sector

    /////////////////////////////////////
    // SL Timing per sector-trigger
    for(int isect=0;isect<12;isect++){// sector
      for( int itrig = 0 ; itrig < 4; itrig ++){// trigger type
        // SL vs Lpt Timing matrix
        ss.str(""); ss << slvslpttiming << triggertype[itrig] << "_" << side[ac];
        if(isect+1<10)ss << "0";
        ss << isect+1;
        m_tgclv1slvslpttimingsectortrg[ac][isect][itrig] = new TH2F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3, 3, 0, 3);
        ATH_CHECK( tgclv1_timing_ac[ac]->regHist(m_tgclv1slvslpttimingsectortrg[ac][isect][itrig]) );
        setTH1TitleLabelBCID(m_tgclv1slvslpttimingsectortrg[ac][isect][itrig]);

        //SL vs Lpt Timing matrix for pT threshold > pT1
        ss.str(""); ss << slvslpttiming << morethanpt1 << triggertype[itrig] << "_" << side[ac];
        if(isect+1<10)ss << "0";
        ss << isect+1;
        m_tgclv1slvslpttimingptcutsectortrg[ac][isect][itrig] = new TH2F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3, 3, 0, 3);
        ATH_CHECK( tgclv1_timing_ac[ac]->regHist(m_tgclv1slvslpttimingptcutsectortrg[ac][isect][itrig]) );
        setTH1TitleLabelBCID(m_tgclv1slvslpttimingptcutsectortrg[ac][isect][itrig]);
      }//trigger type
    }//sector
        
  }// ac
  
  
  ///////////////////////////////////////////////////////////////////////////
  // Express Stream histograms
  // Timing, Associated with Track
  for(int ac=0;ac<2;ac++){// side
    // SL Sector profile of Current Timing fraction
    ss.str(""); ss << "ES_" << sltiming << "_" << side[ac];
    m_tgclv1_SL_trigger_timing_ES[ac] = new TH1F(ss.str().c_str(), ( ss.str() + ";;Current Fraction").c_str(), 12, 0, 12);
    ATH_CHECK( tgclv1_timing_ac_ES[ac]->regHist( m_tgclv1_SL_trigger_timing_ES[ac] ) );

    // SL Sector profile of Current Timing fraction GM
    ss.str(""); ss << "ES_GM_" << sltiming << "_" << side[ac];
    m_tgclv1_SL_trigger_timing_ES_GM[ac] = new TH1F(ss.str().c_str(), ( ss.str() + ";;Current Fraction").c_str(), 12, 0, 12);
    ATH_CHECK( tgclv1_timing_ac_ES_GM[ac]->regHist( m_tgclv1_SL_trigger_timing_ES_GM[ac] ) );

    // SL Sector profile of Current Timing fraction Numerator
    ss.str(""); ss << "ES_" << sltiming << "_" << side[ac] << "_Numerator";
    m_tgclv1_SL_trigger_timing_num_ES[ac] = new TH1F(ss.str().c_str(), ( ss.str() + ";;Entry").c_str(), 12, 0, 12);
    ATH_CHECK( tgclv1_timing_ac_ES[ac]->regHist( m_tgclv1_SL_trigger_timing_num_ES[ac] ) );

    // SL Sector profile of Current Timing fraction Denominator
    ss.str(""); ss << "ES_" << sltiming << "_" << side[ac] << "_Denominator";
    m_tgclv1_SL_trigger_timing_denom_ES[ac] = new TH1F(ss.str().c_str(), ( ss.str() + ";;Entry").c_str(), 12, 0, 12);
    ATH_CHECK( tgclv1_timing_ac_ES[ac]->regHist( m_tgclv1_SL_trigger_timing_denom_ES[ac] ) );

    // LpT Sector profile of Current Timing fraction
    ss.str(""); ss << "ES_" << lpttiming << "_" << side[ac];
    m_tgclv1_LPT_trigger_timing_ES[ac] = new TH1F(ss.str().c_str(), ( ss.str() + ";;Current Fraction").c_str(), 12, 0, 12);
    ATH_CHECK( tgclv1_timing_ac_ES[ac]->regHist( m_tgclv1_LPT_trigger_timing_ES[ac] ) );

    // LpT Sector profile of Current Timing fraction GM
    ss.str(""); ss << "ES_GM_" << lpttiming << "_" << side[ac];
    m_tgclv1_LPT_trigger_timing_ES_GM[ac] = new TH1F(ss.str().c_str(), ( ss.str() + ";;Current Fraction").c_str(), 12, 0, 12);
    ATH_CHECK( tgclv1_timing_ac_ES_GM[ac]->regHist( m_tgclv1_LPT_trigger_timing_ES_GM[ac] ) );

    // LpT Sector profile of Current Timing fraction Numerator
    ss.str(""); ss << "ES_" << lpttiming << "_" << side[ac] << "_Numerator";
    m_tgclv1_LPT_trigger_timing_num_ES[ac] = new TH1F(ss.str().c_str(), ( ss.str() + ";;Entry").c_str(), 12, 0, 12);
    ATH_CHECK( tgclv1_timing_ac_ES[ac]->regHist( m_tgclv1_LPT_trigger_timing_num_ES[ac] ) );

    // LpT Sector profile of Current Timing fraction Denominator
    ss.str(""); ss << "ES_" << lpttiming << "_" << side[ac] << "_Denominator";
    m_tgclv1_LPT_trigger_timing_denom_ES[ac] = new TH1F(ss.str().c_str(), ( ss.str() + ";;Entry").c_str(), 12, 0, 12);
    ATH_CHECK( tgclv1_timing_ac_ES[ac]->regHist( m_tgclv1_LPT_trigger_timing_denom_ES[ac] ) );
    
    // Set Bin Labels for Sector profiles
    for( int isect=0 ; isect<12 ; isect++ ){// sector
      ss.str(""); ss << side[ac] ;
      if( isect<9 ) ss<< "0";
      ss << isect + 1 ;
      m_tgclv1_SL_trigger_timing_ES[ac]       ->GetXaxis()->SetBinLabel(isect+1, ss.str().c_str());
      m_tgclv1_SL_trigger_timing_num_ES[ac]   ->GetXaxis()->SetBinLabel(isect+1, ss.str().c_str());
      m_tgclv1_SL_trigger_timing_denom_ES[ac] ->GetXaxis()->SetBinLabel(isect+1, ss.str().c_str());
      m_tgclv1_SL_trigger_timing_ES_GM[ac]    ->GetXaxis()->SetBinLabel(isect+1, ss.str().c_str());

      m_tgclv1_LPT_trigger_timing_ES[ac]      ->GetXaxis()->SetBinLabel(isect+1, ss.str().c_str());
      m_tgclv1_LPT_trigger_timing_num_ES[ac]  ->GetXaxis()->SetBinLabel(isect+1, ss.str().c_str());
      m_tgclv1_LPT_trigger_timing_denom_ES[ac]->GetXaxis()->SetBinLabel(isect+1, ss.str().c_str());
      m_tgclv1_LPT_trigger_timing_ES_GM[ac]   ->GetXaxis()->SetBinLabel(isect+1, ss.str().c_str());
    }// sector

  }// side
  // Express Stream end
  
  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////
// fillTriggerTiming
///////////////////////////////////////////////////////////////////////////
void
TgcLv1RawDataValAlg::fillTriggerTiming(int ptcut){
  //fillTriggerTiming0(ptcut);//multiple triggers from TGC is accepted
  fillTriggerTiming1(ptcut);//only single trigger from TGC is accepted
}

///////////////////////////////////////////////////////////////////////////
// fillTriggerTiming0
///////////////////////////////////////////////////////////////////////////
// Accepts multiple TGC triggers
void
TgcLv1RawDataValAlg::fillTriggerTiming0(int ptcut){
  for(int ac=0;ac<2;ac++){// side
    for(int eta=0;eta<6;eta++){// stationEta
      // Get Forward/Endcap Index
      int ef=0;
      if(eta!=0)ef=1;

      for(int phi48=0;phi48<48;phi48++){// stationPhi
        // Get sector index and phi map index
        int sect=phi2sector(phi48, ef);//[0:11]
        int phi = phi48+2;// [2:49] shifted to align with sector acis
        if(phi>47) phi-=48;// [0:47]
        
        
        ///////////////////////////////////////////////////////////////////////////
        // Fill SL trigger histograms
        // Initialize SL Bunch Crossing ID variables
        int SLBC   = -1;
        int SLBCpt = -1;

        // Select earliest SL trigger in this chamber
        if      (m_maxSLtrigger[ac][eta][phi48][PREV]>0) SLBC = PREV;
        else if (m_maxSLtrigger[ac][eta][phi48][CURR]>0) SLBC = CURR;
        else if (m_maxSLtrigger[ac][eta][phi48][NEXT]>0) SLBC = NEXT;

        // Check that SL trigger exists
        if(SLBC>-1){        
          // Select earliest SL trigger over the ptcut in this chamber
          if      (m_maxSLtrigger[ac][eta][phi48][PREV]>ptcut) SLBCpt = PREV;
          else if (m_maxSLtrigger[ac][eta][phi48][CURR]>ptcut) SLBCpt = CURR;
          else if (m_maxSLtrigger[ac][eta][phi48][NEXT]>ptcut) SLBCpt = NEXT;
          
          // Fill no-ptcut timing histograms
          m_tgclv1sltiming[ac]            ->Fill(SLBC);
          m_tgclv1sltimingsector[ac][sect]->Fill(SLBC);
          
          // Fill ptcut timing histograms
          if(SLBCpt>ptcut){
            m_tgclv1sltimingptcut[ac]            ->Fill(SLBCpt);
            m_tgclv1sltimingptcutsector[ac][sect]->Fill(SLBCpt);

            // not monitor these profiles at GM
            if( m_environment != AthenaMonManager::online )
              m_tgclv1sltimingptcutlowstat[ac]->Fill(SLBCpt);
          }
        }
        
        
        ///////////////////////////////////////////////////////////////////////////
        // Fill LpT trigger histograms
        // Initialize LpT Bunch Crossing ID variables
        int LptBC = -1;

        // Select earliest LpT trigger for which both a wire and a strip trigger exist
        if     (m_hasLpTtrigger[0][ac][eta][phi48][PREV]==true &&
                m_hasLpTtrigger[1][ac][eta][phi48][PREV]==true) LptBC = PREV;
        else if(m_hasLpTtrigger[0][ac][eta][phi48][CURR]==true &&
                m_hasLpTtrigger[1][ac][eta][phi48][CURR]==true) LptBC = CURR; 
        else if(m_hasLpTtrigger[0][ac][eta][phi48][NEXT]==true &&
                m_hasLpTtrigger[1][ac][eta][phi48][NEXT]==true) LptBC = NEXT;

        // Get eta&BCID index for filling maps
        int etaBCLpt = eta + LptBC*6;

        // Check that LpT trigger exists
        if(LptBC>-1){
          // Fill timing histograms
          m_tgclv1lpttiming[ac]            ->Fill(LptBC);
          m_tgclv1lpttimingsector[ac][sect]->Fill(LptBC);
           
          // Fill timing maps
          m_tgclv1lpttimingmap[ac]->Fill(etaBCLpt, phi+1);
          
          // Fill ptcut timing histograms
          if(SLBCpt>ptcut){
            m_tgclv1lpttimingptcut[ac]            ->Fill(LptBC);
            m_tgclv1lpttimingptcutsector[ac][sect]->Fill(LptBC);
            m_tgclv1lpttimingmapptcut[ac]         ->Fill(LptBC, phi+1);
          }
        }
        
        
        ///////////////////////////////////////////////////////////////////////////
        // Fill SL trigger vs LpT trigger histograms
        if((LptBC>-1)&&(SLBC>-1)){
          m_tgclv1slvslpttiming[ac]            ->Fill(SLBC, LptBC);
          m_tgclv1slvslpttimingsector[ac][sect]->Fill(SLBC, LptBC);

          if(SLBCpt>ptcut){
            m_tgclv1slvslpttimingptcut[ac]            ->Fill(SLBCpt, LptBC);
            m_tgclv1slvslpttimingptcutsector[ac][sect]->Fill(SLBCpt, LptBC);
          }
        }

      }// stationPhi
    }// stationEta
  }// ac
}// EOF

///////////////////////////////////////////////////////////////////////////
// fillTriggerTiming1
///////////////////////////////////////////////////////////////////////////
// Accepts earliest SL triggers only
void
TgcLv1RawDataValAlg::fillTriggerTiming1(int ptcut){//only the earlest triggers in TGC are accepted
  // Check that a trigger was detected
  if(m_earliestTrigger<0)return;
  
  // Initialize SL Bunch Crossing ID and pT variables
  int SLBC = -1;
  int SLpt = -1;

  for(int ac=0;ac<2;ac++){// side
    for(int eta=0;eta<6;eta++){// stationEta
      // Get Forward/Endcap Index
      int ef=0;
      if(eta!=0)ef=1;

      for(int phi48=0;phi48<48;phi48++){// stationPhi
        // Only run if SL trigger exists at earliest detected timing
        if(m_maxSLtrigger[ac][eta][phi48][m_earliestTrigger]>0){
          // Get sector index and phi map index
          int sect=phi2sector(phi48, ef);//[0:11]
          int phi = phi48+2;// [2:49] shifted to align with sector acis
          if(phi>47) phi-=48;// [0:47]
          
          
          ///////////////////////////////////////////////////////////////////////////
          // Fill SL trigger histograms
          // Get SL Bunch Crossing ID and variables
          SLpt = m_maxSLtrigger[ac][eta][phi48][m_earliestTrigger];
          SLBC = m_earliestTrigger;

          // Get eta&BCID and phi bin indexes for filling maps
          int etaBCSL = (5 - eta) + SLBC*6;

          // Fill timing histograms
          m_tgclv1sltiming[ac]            ->Fill(SLBC);
          m_tgclv1sltimingpt[ac][SLpt-1]  ->Fill(SLBC);
          m_tgclv1sltimingsector[ac][sect]->Fill(SLBC);
          
          // Fill timing maps
          m_tgclv1sltimingmap[ac]->Fill(etaBCSL, phi+1);

          // Fill timing histograms for different trigger types
          for(int trg=0;trg<4;trg++){
            if(m_L1TriggerType[trg]>0 && SLpt>0){
              m_tgclv1sltimingtrg[ac][trg]            ->Fill(SLBC); 
              m_tgclv1sltimingpttrg[ac][SLpt-1][trg]  ->Fill(SLBC); 
              m_tgclv1sltimingsectortrg[ac][sect][trg]->Fill(SLBC); 
            }
          }

          // Fill ptcut timing histograms
          if(SLpt>ptcut){
            m_tgclv1sltimingptcut[ac]            ->Fill(SLBC);
            m_tgclv1sltimingptcutsector[ac][sect]->Fill(SLBC);
            m_tgclv1sltimingmapptcut[ac]         ->Fill(etaBCSL, phi+1);
            for(int trg=0;trg<4;trg++){
              if(m_L1TriggerType[trg]>0){
                m_tgclv1sltimingptcuttrg[ac][trg]            ->Fill(SLBC); 
                m_tgclv1sltimingptcutsectortrg[ac][sect][trg]->Fill(SLBC); 
              }
            }

            // not monitor these profiles at GM
            if( m_environment != AthenaMonManager::online )
              m_tgclv1sltimingptcutlowstat[ac]     ->Fill(SLBC);
          }


          ///////////////////////////////////////////////////////////////////////////
          // Fill LpT trigger histograms
          // Initialize LpT Bunch Crossing ID variables
          int LptBC   = -1;
          
          // Select latest LpT trigger for which both a wire and a strip trigger exist
          if     (m_hasLpTtrigger[0][ac][eta][phi48][NEXT]==true &&
                  m_hasLpTtrigger[1][ac][eta][phi48][NEXT]==true) LptBC = NEXT;
          else if(m_hasLpTtrigger[0][ac][eta][phi48][CURR]==true &&
                  m_hasLpTtrigger[1][ac][eta][phi48][CURR]==true) LptBC = CURR;
          else if(m_hasLpTtrigger[0][ac][eta][phi48][PREV]==true &&
                  m_hasLpTtrigger[1][ac][eta][phi48][PREV]==true) LptBC = PREV;

          // Check that LpT trigger exists
          if(LptBC>-1){
            // Get eta&BCIDbin index for filling maps
            int etaBCLpt = eta + LptBC * 6;
            
            // Fill timing histograms
            m_tgclv1lpttiming[ac]            ->Fill(LptBC);
            m_tgclv1lpttimingpt[ac][SLpt-1]    ->Fill(LptBC);
            m_tgclv1lpttimingsector[ac][sect]->Fill(LptBC);
            
            // Fill timing maps
            m_tgclv1lpttimingmap[ac]->Fill(etaBCLpt, phi+1);
            
            // Fill timing histograms for different trigger types
            for(int trg=0;trg<4;trg++){
              if(m_L1TriggerType[trg]>0){
                m_tgclv1lpttimingtrg[ac][trg]            ->Fill(LptBC); 
                m_tgclv1lpttimingpttrg[ac][SLpt-1][trg]    ->Fill(LptBC); 
                m_tgclv1lpttimingsectortrg[ac][sect][trg]->Fill(LptBC); 
              }
            }
            
            // Fill ptcut timing histograms
            if(SLpt>ptcut){
              m_tgclv1lpttimingptcut[ac]            ->Fill(LptBC);
              m_tgclv1lpttimingptcutsector[ac][sect]->Fill(LptBC);
              m_tgclv1lpttimingmapptcut[ac]         ->Fill(etaBCLpt, phi+1);
              for(int trg=0;trg<4;trg++){
                if(m_L1TriggerType[trg]>0){
                  m_tgclv1lpttimingptcuttrg[ac][trg]            ->Fill(LptBC); 
                  m_tgclv1lpttimingptcutsectortrg[ac][sect][trg]->Fill(LptBC); 
                }
              }
            }
          }
          
          
          ///////////////////////////////////////////////////////////////////////////
          // Fill SL trigger vs LpT trigger histograms
          if(LptBC>-1){
            m_tgclv1slvslpttiming[ac]            ->Fill(SLBC, LptBC);
            m_tgclv1slvslpttimingsector[ac][sect]->Fill(SLBC, LptBC);
            
            if(SLpt>ptcut){
              m_tgclv1slvslpttimingptcut[ac]            ->Fill(SLBC, LptBC);
              m_tgclv1slvslpttimingptcutsector[ac][sect]->Fill(SLBC, LptBC);
            }
          }
          
        }// SL trigger for earliest timing
      }// stationPhi
    }// stationEta
  }// ac
}// EOF


///////////////////////////////////////////////////////////////////////////
// fillTriggerTimingAssociatedWithTrack
///////////////////////////////////////////////////////////////////////////
void
TgcLv1RawDataValAlg::fillTriggerTimingAssociatedWithTrack( int ms,// 0:Muid 1:Staco
                                                           std::vector<float>* mu_pt, std::vector<float>* mu_eta,
                                                           std::vector<float>* mu_phi,std::vector<float>* mu_q ){
  // Get/Set vector size and cut variables
  int   osize        = mu_pt->size();

  // Skip Event if any Offline Muons were found close to each other
  for(int o1 = 0 ; o1 < osize ; o1++){
    for(int o2 = o1+1 ; o2 < osize ; o2++){
      float eta1 = mu_eta->at(o1);
      float phi1 = mu_phi->at(o1);
      float eta2 = mu_eta->at(o2);
      float phi2 = mu_phi->at(o2);
      float trackdr = deltaR(eta1, phi1, eta2, phi2);

      if(trackdr < 0.8 ) return;
    }
  }

  // Loop over Offline Muon vectors
  for(int o = 0 ; o < osize ; o++ ){
    // Get variables
    float opt  = mu_pt->at(o)/CLHEP::GeV; 
    float oeta = mu_eta->at(o);
    float ophi = mu_phi->at(o);
    float oq   = mu_q->at(o);
    if( std::abs(opt) > 50. ) opt = 50.1 * oq;
    
    // Get side index
    int ac = (oeta<0);
    
    // Initialise SL variables
    int slpt=-1;
    int sltidw=-1;
    int sltids=-1;
    bool slisAside=false;
    bool slisForward=false;
    int slphi48=-1;

    for(int pcn=0;pcn<3;pcn++){
      
      float deltarmin=1.;
      int   tptmin=-1;

      int tsize = m_sl_pt[pcn].size();
      for(int t=0;t<tsize;t++){
        int   tpt  = m_sl_pt[pcn].at(t);
        float teta = m_sl_eta[pcn].at(t);
        float tphi = m_sl_phi[pcn].at(t);
        int   tidw = m_sl_tidw[pcn].at(t);
        int   tids = m_sl_tids[pcn].at(t);
        bool  isAside   = m_sl_isAside[pcn].at(t);
        bool  isForward = m_sl_isForward[pcn].at(t);
        int   phi48 = m_sl_phi48[pcn].at(t);
        
        if(oeta*teta<0.) continue;
        
        float deltar = deltaR( oeta, ophi, teta, tphi );
        
        if(deltar>1.) continue;
        //select lowest dr trigger
        if(deltar<deltarmin){
          deltarmin=deltar;
          tptmin=tpt;
          
          if(slpt<tpt){
            slpt=tpt;
            sltidw=tidw;
            sltids=tids;
            slisAside=isAside;
            slisForward=isForward;
            slphi48=phi48;
          }
        }


      }//trigger
      //fill SL timing
      if(tptmin!=-1){
        ATH_MSG_DEBUG( "fill triggertiming " <<ac <<" " << ms << " " << pcn  );
        m_tgclv1sltimingtrack[ac][ms]->Fill(pcn);
        if( tptmin > 1 )
          m_tgclv1sltimingptcuttrack[ac][ms]->Fill(pcn);

        if( m_found_express_stream && m_found_nonmuon_express_chain ){

          int sect12=phi2sector(slphi48, !slisForward);//[0:11] 

          m_tgclv1_SL_trigger_timing_denom_ES[ac]->Fill( sect12 );
          if( pcn == 1 )
            m_tgclv1_SL_trigger_timing_num_ES[ac]->Fill( sect12 );

        }
      }
    }//pcn

    //LPT timing

    // This block fills tgclv1pttimingtrack(cuttrack) histogram(s)
    bool lptflag=false;
    if(slpt>=0) {
      for(int pcn=2;pcn>-1;pcn--){
        // Loop Wire LpT Trigger Vectors
        for(int lptw=0;lptw<(int) m_lpt_delta[0][pcn].size();lptw++){
          // Get Wire Low pT Variables
          int tidlptw       = m_lpt_tid[0][pcn].at(lptw);
          int isAsidelptw   = m_lpt_isAside[0][pcn].at(lptw);
          int isForwardlptw = m_lpt_isForward[0][pcn].at(lptw);
          int phi48lptw     = m_lpt_phi48[0][pcn].at(lptw);
          // Check LpTwire is the same tracklet as the SL
          if( tidlptw != sltidw ||
              isAsidelptw != slisAside ||
              isForwardlptw != slisForward ||
              phi48lptw != slphi48 ) continue ;
          
          // Loop Strip LpT Trigger Vectors
          for(int lpts=0;lpts<(int) m_lpt_delta[1][pcn].size();lpts++ ){
            // Get Strip Low pT Variables
            int tidlpts       = m_lpt_tid[1][pcn].at(lpts);
            int isAsidelpts   = m_lpt_isAside[1][pcn].at(lpts);
            int isForwardlpts = m_lpt_isForward[1][pcn].at(lpts);
            int phi48lpts     = m_lpt_phi48[1][pcn].at(lpts);
            // Check LpTstrip is the same tracklet as the SL
            if( tidlpts != sltids ||
                isAsidelpts != slisAside ||
                isForwardlpts != slisForward ||
                phi48lpts != slphi48 ) continue ;
            
            // Fill LpT Track Timing Histograms
            ATH_MSG_DEBUG( "fill triggertiming " <<ac <<" " << ms << " " << pcn  );
            m_tgclv1lpttimingtrack[ac][ms]->Fill(pcn);
            if( slpt > 1 )
              m_tgclv1lpttimingptcuttrack[ac][ms]->Fill(pcn);
            
            // Fill Express Stream, "Current Fraction" histograms
            if(m_found_express_stream&&m_found_nonmuon_express_chain){
              int sect12=phi2sector(slphi48, !slisForward);//[0:11] 
              m_tgclv1_LPT_trigger_timing_denom_ES[ac]->Fill( sect12 );
              if( pcn == 1 )
                m_tgclv1_LPT_trigger_timing_num_ES[ac]->Fill( sect12 );
            }

            lptflag=true;
            break;
          }// Strip LpT strip
          if(lptflag==true)break;
        }// Wire LpT
        if(lptflag==true)break;
      }// pcn
    }// SLpT
  }// Offline
}// EOF fillTriggerTimingAssociatedWithTrack
