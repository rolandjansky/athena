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
// bookNumberOfTriggersAndProfileHisto
///////////////////////////////////////////////////////////////////////////
StatusCode
TgcLv1RawDataValAlg::bookHistogramsNumberOfTriggersAndProfile(){

  ///////////////////////////////////////////////////////////////////////////
  // Make MonGroups for histogram booking paths
  std::string generic_path_tgclv1 = "Muon/MuonRawDataMonitoring/TGCLV1";

  MonGroup tgclv1_shift( this, generic_path_tgclv1+"/Global",  run, ATTRIB_UNMANAGED );
  MonGroup tgclv1_shift_a( this, generic_path_tgclv1+"/TGCEA", run, ATTRIB_UNMANAGED );
  MonGroup tgclv1_shift_c( this, generic_path_tgclv1+"/TGCEC", run, ATTRIB_UNMANAGED );
  MonGroup* tgclv1_shift_ac[2] = { &tgclv1_shift_a, &tgclv1_shift_c};

  MonGroup tgclv1_expert( this, generic_path_tgclv1+"/Global", run, ATTRIB_UNMANAGED );
  MonGroup tgclv1_expert_a( this, generic_path_tgclv1+"/TGCEA", run, ATTRIB_UNMANAGED );
  MonGroup tgclv1_expert_c( this, generic_path_tgclv1+"/TGCEC", run, ATTRIB_UNMANAGED );
  MonGroup* tgclv1_expert_ac[2] = { &tgclv1_expert_a, &tgclv1_expert_c};

  MonGroup tgclv1_expert_ES( this, generic_path_tgclv1+"/Global/ES", run, ATTRIB_UNMANAGED );
  //MonGroup tgclv1_expert_a_ES( this, generic_path_tgclv1+"/TGCEA/ES", run, ATTRIB_UNMANAGED );
  //MonGroup tgclv1_expert_c_ES( this, generic_path_tgclv1+"/TGCEC/ES", run, ATTRIB_UNMANAGED );
  //MonGroup* tgclv1_expert_ac_ES[2] = { &tgclv1_expert_a_ES, &tgclv1_expert_c_ES};

  int k=0;
  std::stringstream ss;
  std::string side[2]       ={"A","C"};
  std::string s_pcn[4]      ={"_Previous","_Current","_Next","_Total"};
  std::string schamberT1[6] ={"E1", "E2", "E3", "E4", "F"};
  std::string schamberT3[6] ={"E1", "E2", "E3", "E4", "E5", "F"};

  ///////////////////////////////////////////////////////////////////////////
  // Trigger Rate histograms, per LumiBlock
  for(int ac=0;ac<2;ac++){// side
    // LpT wire triggers per sector, per lumi block
    ss.str(""); ss << "LB_VS_LPT_Wire_Trigger_Sector_" << side[ac] ;
    m_tgclv1lptwireinlbvssect[ac] = new TH2F( ss.str().c_str(), (ss.str() + "; LB;").c_str(), 1250, 1, 2501, 72, 1, 73 );
    ATH_CHECK( tgclv1_expert_ac[ac]->regHist( m_tgclv1lptwireinlbvssect[ac] ) );
    // Lpt strip triggers per sector, per lumi block
    ss.str(""); ss << "LB_VS_LPT_Strip_Trigger_Sector_" << side[ac] ;
    m_tgclv1lptstripinlbvssect[ac] = new TH2F( ss.str().c_str(), (ss.str() + "; LB;").c_str(), 1250, 1, 2501, 72, 1, 73 );
    ATH_CHECK( tgclv1_expert_ac[ac]->regHist( m_tgclv1lptstripinlbvssect[ac] ) );

    //EIFI Coin

    ss.str(""); ss << "EIFI_Sector_VS_Bit_" << side[ac] ;
    m_tgclv1hptstripinlbvssecteifisect[ac] = new TH2F( ss.str().c_str(), (ss.str() + "; Trigger_Sector;Bit").c_str(), 48, 1, 49, 8, 0, 8 );
    ATH_CHECK( tgclv1_expert_ac[ac]->regHist( m_tgclv1hptstripinlbvssecteifisect[ac] ) );
    ss.str(""); ss << "EIFI_Lumi_VS_Bit_" << side[ac] ;
    m_tgclv1hptstripinlbvssecteifilumi[ac] = new TH2F( ss.str().c_str(), (ss.str() + "; LB;Bit").c_str(), 1250, 1, 2501, 8, 0, 8 );
    ATH_CHECK( tgclv1_expert_ac[ac]->regHist( m_tgclv1hptstripinlbvssecteifilumi[ac] ) );

    // HpT wire triggers per sector, per lumi block
    ss.str(""); ss << "LB_VS_HPT_Wire_Trigger_Sector_" << side[ac] ;
    m_tgclv1hptwireinlbvssect[ac] = new TH2F( ss.str().c_str(), (ss.str() + "; LB;").c_str(), 1250, 1, 2501, 72, 1, 73 );
    ATH_CHECK( tgclv1_expert_ac[ac]->regHist( m_tgclv1hptwireinlbvssect[ac] ) );

    // Hpt strip triggers per sector, per lumi block
    ss.str(""); ss << "LB_VS_HPT_Strip_Trigger_Sector_" << side[ac] ;
    m_tgclv1hptstripinlbvssect[ac] = new TH2F( ss.str().c_str(), (ss.str() + "; LB;").c_str(), 1250, 1, 2501, 72, 1, 73 );
    ATH_CHECK( tgclv1_expert_ac[ac]->regHist( m_tgclv1hptstripinlbvssect[ac] ) );

    // SL triggers per sector, per lumi block
    ss.str(""); ss << "LB_VS_SL_Trigger_Sector_" << side[ac] ;
    m_tgclv1slinlbvssect[ac] = new TH2F( ss.str().c_str(), (ss.str() + "; LB;").c_str(), 1250, 1, 2501, 72, 1, 73 );
    ATH_CHECK( tgclv1_shift_ac[ac]->regHist( m_tgclv1slinlbvssect[ac] ) );

    int k=1;
    // Set bin labels for Y axis on per sector-lumiblock histograms
    for(int isect=1;isect<=12;isect++){// Endcap sectors
      int nphi=4, phistep=4;
      for(int iphi=0;iphi<nphi;iphi+=phistep){
	ss.str(""); ss << side[ac];
	if(isect<10)ss << "0";
	ss << isect << "Ephi" << iphi;
	m_tgclv1lptwireinlbvssect[ac] ->GetYaxis()->SetBinLabel(k,ss.str().c_str());
	m_tgclv1lptstripinlbvssect[ac]->GetYaxis()->SetBinLabel(k,ss.str().c_str());
	m_tgclv1hptwireinlbvssect[ac] ->GetYaxis()->SetBinLabel(k,ss.str().c_str());
	m_tgclv1hptstripinlbvssect[ac]->GetYaxis()->SetBinLabel(k,ss.str().c_str());
	m_tgclv1slinlbvssect[ac]      ->GetYaxis()->SetBinLabel(k,ss.str().c_str());

	k+=phistep;
      }// phi
    }// endcap sectors
    for(int isect=1;isect<=12;isect++){// Forward sectors
      int nphi=2, phistep=2;
      for(int iphi=0;iphi<nphi;iphi+=phistep){
	ss.str(""); ss << side[ac];
	if(isect<10)ss << "0";
	ss << isect << "Fphi" << iphi;
	m_tgclv1lptwireinlbvssect[ac] ->GetYaxis()->SetBinLabel(k,ss.str().c_str());
	m_tgclv1lptstripinlbvssect[ac]->GetYaxis()->SetBinLabel(k,ss.str().c_str());
	m_tgclv1hptwireinlbvssect[ac] ->GetYaxis()->SetBinLabel(k,ss.str().c_str());
	m_tgclv1hptstripinlbvssect[ac]->GetYaxis()->SetBinLabel(k,ss.str().c_str());
	m_tgclv1slinlbvssect[ac]      ->GetYaxis()->SetBinLabel(k,ss.str().c_str());

	k+=phistep;
      }// phi
    }// forward sectors

  }// ac

  ///////////////////////////////////////////////////////////////////////////
  // Number of Trigger histograms, not produced in General Monitoring
  if(m_environment!=AthenaMonManager::online){
    for(int pcnt=0;pcnt<4;pcnt++){
      // Number of low pt wire hits
      std::string tgclv1_nlpt_wire = "Number_Of_Low_Pt_Wire_Triggers" + s_pcn[pcnt];
      m_tgclv1numberoflptwire[pcnt] = new TH1F(tgclv1_nlpt_wire.c_str(),tgclv1_nlpt_wire.c_str(), 20, 0, 20);
      if(pcnt>2) ATH_CHECK( tgclv1_shift.regHist(m_tgclv1numberoflptwire[pcnt]) );
      else    ATH_CHECK( tgclv1_expert.regHist(m_tgclv1numberoflptwire[pcnt]) );
      m_tgclv1numberoflptwire[pcnt]->GetXaxis()->SetTitle("Low Pt Wire Triggers");
      m_tgclv1numberoflptwire[pcnt]->GetYaxis()->SetTitle("Counts");

      // Number of low pt strip hits
      std::string tgclv1_nlpt_strip = "Number_Of_Low_Pt_Strip_Triggers" + s_pcn[pcnt];
      m_tgclv1numberoflptstrip[pcnt] = new TH1F(tgclv1_nlpt_strip.c_str(),tgclv1_nlpt_strip.c_str(), 20, 0, 20);
      if(pcnt>2) ATH_CHECK( tgclv1_shift.regHist(m_tgclv1numberoflptstrip[pcnt]) );
      else    ATH_CHECK( tgclv1_expert.regHist(m_tgclv1numberoflptstrip[pcnt]) );
      m_tgclv1numberoflptstrip[pcnt]->GetXaxis()->SetTitle("Low Pt Strip Triggers");
      m_tgclv1numberoflptstrip[pcnt]->GetYaxis()->SetTitle("Counts");

      // Number of high pt hits
      std::string tgclv1_nhpt_wire = "Number_Of_High_Pt_Wire_Triggers" + s_pcn[pcnt];
      m_tgclv1numberofhptwire[pcnt] = new TH1F(tgclv1_nhpt_wire.c_str(),tgclv1_nhpt_wire.c_str(), 20, 0, 20);
      if(pcnt>2) ATH_CHECK( tgclv1_shift.regHist(m_tgclv1numberofhptwire[pcnt]) );
      else    ATH_CHECK( tgclv1_expert.regHist(m_tgclv1numberofhptwire[pcnt]) );
      m_tgclv1numberofhptwire[pcnt]->GetXaxis()->SetTitle("High Pt Wire Triggers");
      m_tgclv1numberofhptwire[pcnt]->GetYaxis()->SetTitle("Counts");

      // Number of high pt hits
      std::string tgclv1_nhpt_strip = "Number_Of_High_Pt_Strip_Triggers" + s_pcn[pcnt];
      m_tgclv1numberofhptstrip[pcnt] = new TH1F(tgclv1_nhpt_strip.c_str(),tgclv1_nhpt_strip.c_str(), 20, 0, 20);
      if(pcnt>2) ATH_CHECK( tgclv1_shift.regHist(m_tgclv1numberofhptstrip[pcnt]) );
      else    ATH_CHECK( tgclv1_expert.regHist(m_tgclv1numberofhptstrip[pcnt]) );
      m_tgclv1numberofhptstrip[pcnt]->GetXaxis()->SetTitle("High Pt Strip Triggers");
      m_tgclv1numberofhptstrip[pcnt]->GetYaxis()->SetTitle("Counts");

      // Number of sector logic hits
      std::string tgclv1_nsl = "Number_Of_SL_Triggers" + s_pcn[pcnt];
      m_tgclv1numberofsl[pcnt] = new TH1F(tgclv1_nsl.c_str(),tgclv1_nsl.c_str(), 20, 0, 20);
      if(pcnt>2) ATH_CHECK( tgclv1_shift.regHist(m_tgclv1numberofsl[pcnt]) );
      else    ATH_CHECK( tgclv1_expert.regHist(m_tgclv1numberofsl[pcnt]) );
      m_tgclv1numberofsl[pcnt]->GetXaxis()->SetTitle("Sector Logic Triggers");
      m_tgclv1numberofsl[pcnt]->GetYaxis()->SetTitle("Counts");
    }// pcnt
  }//no GM
  else
    for(int pcnt=0;pcnt<4;pcnt++){
      m_tgclv1numberoflptwire[pcnt] = 0;
      m_tgclv1numberoflptstrip[pcnt] = 0;
      m_tgclv1numberofhptwire[pcnt] = 0;
      m_tgclv1numberofhptstrip[pcnt] = 0;
      m_tgclv1numberofsl[pcnt] = 0;
    }

  ///////////////////////////////////////////////////////////////////////////
  // RoI Maps and pT Threshold histograms
  // RoI trigger map with both sides
  ss.str(""); ss << "RoI_Eta_Vs_Phi_All";
  ATH_MSG_DEBUG( ss.str().c_str()  );
  m_tgclv1roietavsphiAll =new TH2F(ss.str().c_str(), (ss.str() + ";C-side: Eta < 0, A-side: Eta > 0").c_str(), 55, -55, 55, 48, 0, 48); 
  ATH_CHECK( tgclv1_shift.regHist(m_tgclv1roietavsphiAll) );

  // RoI trigger maps for each pT threshold
  for(int ipt=0;ipt<6;ipt++){// pt
    ss.str(""); ss << "RoI_Eta_Vs_Phi_PT" << ipt+1 <<"_All";
    ATH_MSG_DEBUG( ss.str().c_str()  );
    m_tgclv1roietavsphiptAll[ipt] =new TH2F(ss.str().c_str(), (ss.str() + ";C-side: Eta < 0, A-side: Eta > 0").c_str(), 55, -55, 55, 48, 0, 48); 
    ATH_CHECK( tgclv1_shift.regHist(m_tgclv1roietavsphiptAll[ipt]) );
  }// pt

  int ka;
  // Set bin labels for RoI trigger maps
  for(int isect=1;isect<=12;isect++){// sector
    int nphi=4, phistep=4;
    for(int iphi=0;iphi<nphi;iphi+=phistep){// phi
      ss.str("");
      if(isect<10) ss << "0";
      ss << isect << "phi"<<iphi;

      ka = (isect-1)*4 + iphi*4 + 1;
      m_tgclv1roietavsphiAll->GetYaxis()->SetBinLabel(ka, ss.str().c_str());
      for(int ipt=0;ipt<6;ipt++)
	m_tgclv1roietavsphiptAll[ipt]->GetYaxis()->SetBinLabel(ka, ss.str().c_str());
    }// phi
  }// sector


  for(int ac=0;ac<2;ac++){// side
    // Triggers per pT threshold hist
    ss.str(""); ss << "Pt_Threshold_";
    ss << side[ac];
    m_tgclv1pt[ac] =new TH1F(ss.str().c_str(),ss.str().c_str(), 6, 1, 7);
    if(ac==0) ATH_CHECK( tgclv1_shift_a.regHist(m_tgclv1pt[ac]) );
    else      ATH_CHECK( tgclv1_shift_c.regHist(m_tgclv1pt[ac]) );
    m_tgclv1pt[ac]->GetXaxis()->SetTitle("Pt Threshold");
    m_tgclv1pt[ac]->GetYaxis()->SetTitle("Counts");
    //m_tgclv1pt[ac]->GetXaxis()->SetBinLabel(1,"4"); //pt1=4GeV
    //m_tgclv1pt[ac]->GetXaxis()->SetBinLabel(2,"6"); //pt2=6GeV
    //m_tgclv1pt[ac]->GetXaxis()->SetBinLabel(3,"10");//pt3=10GeV
    //m_tgclv1pt[ac]->GetXaxis()->SetBinLabel(4,"11");//pt4=15GeV, 11GeV for 2009
    //m_tgclv1pt[ac]->GetXaxis()->SetBinLabel(5,"15");//pt5=20GeV, 15GeV for 2009
    //m_tgclv1pt[ac]->GetXaxis()->SetBinLabel(6,"20");//pt6=40GeV, 20GeV for 2009

    // RoI trigger map
    ss.str(""); ss << "RoI_Eta_Vs_Phi_" << side[ac];
    ATH_MSG_DEBUG( ss.str().c_str()  );
    m_tgclv1roietavsphi[ac] =new TH2F(ss.str().c_str(), (ss.str() + ";Eta").c_str(), 53, 0, 53, 192, 0, 48); 
    ATH_CHECK( tgclv1_shift_ac[ac]->regHist(m_tgclv1roietavsphi[ac]) );

    // RoI trigger maps for each pT threshold
    for(int ipt=0;ipt<6;ipt++){// pt
      ss.str(""); ss << "RoI_Eta_Vs_Phi_PT" << ipt+1 <<"_" << side[ac];
      ATH_MSG_DEBUG( ss.str().c_str()  );
      m_tgclv1roietavsphipt[ac][ipt] =new TH2F(ss.str().c_str(), (ss.str() + ";Eta").c_str(), 53, 0, 53, 192, 0, 48); 
      ATH_CHECK( tgclv1_shift_ac[ac]->regHist(m_tgclv1roietavsphipt[ac][ipt]) );
    }// pt

    int k;
    // Set bin labels for RoI trigger maps
    for(int isect=1;isect<=12;isect++){// sector
      int nphi=4, phistep=4;
      for(int iphi=0;iphi<nphi;iphi+=phistep){// phi
	ss.str(""); ss << side[ac];
	if(isect<10) ss << "0";
	ss << isect << "phi"<<iphi;

	k = (isect-1)*16 + iphi*4 + 1;
	m_tgclv1roietavsphi[ac]->GetYaxis()->SetBinLabel(k, ss.str().c_str());
	for(int ipt=0;ipt<6;ipt++)
	  m_tgclv1roietavsphipt[ac][ipt]->GetYaxis()->SetBinLabel(k, ss.str().c_str());
      }// phi
    }// sector
  }// side

  ///////////////////////////////////////////////////////////////////////////
  // LpT, HpT, SL Profile histograms
  // SL trigger chamber map for both sides
  ss.str(""); ss << "SL_Chamber_All";
  m_tgclv1slchamberAll =new TH2F(ss.str().c_str(),ss.str().c_str(), 14, -7, 7, 48, 1, 49); 
  ATH_CHECK( tgclv1_shift.regHist(m_tgclv1slchamberAll) );
  for(int ieta=0;ieta<6;ieta++){// X bins
    m_tgclv1slchamberAll      ->GetXaxis()->SetBinLabel(ieta+9, ("A-" + schamberT3[ieta]).c_str()); 
    m_tgclv1slchamberAll      ->GetXaxis()->SetBinLabel(6-ieta, ("C-" + schamberT3[ieta]).c_str()); 
  }
  ka=1;
  for(int isect=1;isect<=12;isect++){// sector
    int nphi=4, phistep=4;
    for(int iphi=0;iphi<nphi;iphi+=phistep){// phi
      ss.str(""); 
      if(isect<10)ss << "0";
      ss << isect << "phi0";

      m_tgclv1slchamberAll      ->GetYaxis()->SetBinLabel( ka, ss.str().c_str() ); 
      ka+=phistep;
    }
  }


  for(int ac=0;ac<2;ac++){// side
    /////////////////////////////////////
    // nTriggers Phi Profile histograms
    // LpT wire trigger sector profile
    ss.str(""); ss << "Low_Pt_Wire_Sector_";
    ss << side[ac];
    m_tgclv1lptsectwire[ac] = new TH1F(ss.str().c_str(),ss.str().c_str(), 72, 1, 73);
    if(ac==0) ATH_CHECK( tgclv1_expert_a.regHist(m_tgclv1lptsectwire[ac]) );
    else      ATH_CHECK( tgclv1_expert_c.regHist(m_tgclv1lptsectwire[ac]) );
    //m_tgclv1lptsectwire->GetXaxis()->SetTitle("Side*12+Sector");
    //m_tgclv1lptsectwire[ac]->GetXaxis()->LabelsOption("v");
    m_tgclv1lptsectwire[ac]->GetYaxis()->SetTitle("Counts");

    // LpT strip trigger sector profile
    ss.str(""); ss << "Low_Pt_Strip_Sector_";
    ss << side[ac];
    m_tgclv1lptsectstrip[ac] = new TH1F(ss.str().c_str(),ss.str().c_str(), 72, 1, 73);
    if(ac==0) ATH_CHECK( tgclv1_expert_a.regHist(m_tgclv1lptsectstrip[ac]) );
    else      ATH_CHECK( tgclv1_expert_c.regHist(m_tgclv1lptsectstrip[ac]) );
    //m_tgclv1lptsectstrip->GetXaxis()->SetTitle("Side*12+Sector");
    //m_tgclv1lptsectstrip[ac]->GetXaxis()->LabelsOption("v");
    m_tgclv1lptsectstrip[ac]->GetYaxis()->SetTitle("Counts");

    // HpT wire trigger sector profile
    ss.str(""); ss << "High_Pt_Wire_Sector_";
    ss << side[ac];
    m_tgclv1hptsectwire[ac] = new TH1F(ss.str().c_str(),ss.str().c_str(), 72, 1, 73);
    if(ac==0) ATH_CHECK( tgclv1_expert_a.regHist(m_tgclv1hptsectwire[ac]) );
    else      ATH_CHECK( tgclv1_expert_c.regHist(m_tgclv1hptsectwire[ac]) );
    //m_tgclv1hptsectwire->GetXaxis()->SetTitle("Side*12+Sector");
    //m_tgclv1hptsectwire[ac]->GetXaxis()->LabelsOption("v");
    m_tgclv1hptsectwire[ac]->GetYaxis()->SetTitle("Counts");

    // HpT strip trigger sector profile
    ss.str(""); ss << "High_Pt_Strip_Sector_";
    ss << side[ac];
    m_tgclv1hptsectstrip[ac] = new TH1F(ss.str().c_str(),ss.str().c_str(), 72, 1, 73);
    if(ac==0) ATH_CHECK( tgclv1_expert_a.regHist(m_tgclv1hptsectstrip[ac]) );
    else      ATH_CHECK( tgclv1_expert_c.regHist(m_tgclv1hptsectstrip[ac]) );
    //m_tgclv1hptsectstrip[ac]->GetXaxis()->SetTitle("Sector");
    //m_tgclv1hptsectstrip[ac]->GetXaxis()->LabelsOption("v");
    m_tgclv1hptsectstrip[ac]->GetYaxis()->SetTitle("Counts");

    // SL trigger sector profile
    ss.str(""); ss << "SL_Sector_";
    ss << side[ac];
    m_tgclv1slsect[ac] = new TH1F(ss.str().c_str(),ss.str().c_str(), 72, 1, 73);
    if(ac==0)ATH_CHECK( tgclv1_shift_a.regHist(m_tgclv1slsect[ac]) );
    else     ATH_CHECK( tgclv1_shift_c.regHist(m_tgclv1slsect[ac]) );
    //m_tgclv1slsect->GetXaxis()->SetTitle("Side*12+Sector");
    //m_tgclv1slsect[ac]->GetXaxis()->LabelsOption("v");
    m_tgclv1slsect[ac]->GetYaxis()->SetTitle("Counts");

    k=1;
    // Set Bin Labels for sector profiles
    for(int isect=1;isect<=12;isect++){// Endcap sector
      int nphi=4, phistep=1;
      for(int iphi=0;iphi<nphi;iphi+=phistep){// phi
	ss.str(""); ss << side[ac];
	if(isect<10)ss << "0";
	ss << isect << "Ephi" << iphi;
	m_tgclv1lptsectwire[ac] ->GetXaxis()->SetBinLabel(k,ss.str().c_str());
	m_tgclv1lptsectstrip[ac]->GetXaxis()->SetBinLabel(k,ss.str().c_str());
	m_tgclv1hptsectwire[ac] ->GetXaxis()->SetBinLabel(k,ss.str().c_str());
	m_tgclv1hptsectstrip[ac]->GetXaxis()->SetBinLabel(k,ss.str().c_str());
	m_tgclv1slsect[ac]      ->GetXaxis()->SetBinLabel(k,ss.str().c_str());      
	k+=phistep;
      }// phi
    }// Endcap sector
    for(int isect=1;isect<=12;isect++){// Forward sector
      int nphi=2, phistep=2;
      for(int iphi=0;iphi<nphi;iphi+=phistep){// phi
	ss.str(""); ss << side[ac];
	if(isect<10)ss << "0";
	ss << isect << "Fphi" << iphi;
	m_tgclv1lptsectwire[ac] ->GetXaxis()->SetBinLabel(k,ss.str().c_str());
	m_tgclv1lptsectstrip[ac]->GetXaxis()->SetBinLabel(k,ss.str().c_str());
	m_tgclv1hptsectwire[ac] ->GetXaxis()->SetBinLabel(k,ss.str().c_str());
	m_tgclv1hptsectstrip[ac]->GetXaxis()->SetBinLabel(k,ss.str().c_str());
	m_tgclv1slsect[ac]      ->GetXaxis()->SetBinLabel(k,ss.str().c_str());      
	k+=phistep;
      }// phi
    }// Forward sector
    m_tgclv1lptsectwire[ac] ->GetXaxis()->LabelsOption("v");
    m_tgclv1lptsectstrip[ac]->GetXaxis()->LabelsOption("v");
    m_tgclv1hptsectwire[ac] ->GetXaxis()->LabelsOption("v");
    m_tgclv1hptsectstrip[ac]->GetXaxis()->LabelsOption("v");
    m_tgclv1slsect[ac]      ->GetXaxis()->LabelsOption("v");

    /////////////////////////////////////
    // inEta & outEta, Phi Map histograms
    // LpT wire trigger chamber map
    ss.str(""); ss << "Low_Pt_Wire_Chamber_" << side[ac];
    m_tgclv1lptwirechamber[ac] =new TH2F(ss.str().c_str(),ss.str().c_str(), 12, 0, 12, 48, 1, 49); 
    ATH_CHECK( tgclv1_expert_ac[ac]->regHist(m_tgclv1lptwirechamber[ac]) );

    // LpT strip trigger chamber map
    ss.str(""); ss << "Low_Pt_Strip_Chamber_" << side[ac];
    m_tgclv1lptstripchamber[ac] =new TH2F(ss.str().c_str(),ss.str().c_str(), 12, 0, 12, 48, 1, 49); 
    ATH_CHECK( tgclv1_expert_ac[ac]->regHist(m_tgclv1lptstripchamber[ac]) );

    // HpT wire trigger chamber map
    ss.str(""); ss << "High_Pt_Wire_Chamber_" << side[ac];
    m_tgclv1hptwirechamber[ac] =new TH2F(ss.str().c_str(),ss.str().c_str(), 11, 0, 11, 48, 1, 49); 
    ATH_CHECK( tgclv1_expert_ac[ac]->regHist(m_tgclv1hptwirechamber[ac]) );

    // HpT strip trigger chamber map
    ss.str(""); ss << "High_Pt_Strip_Chamber_" << side[ac];
    m_tgclv1hptstripchamber[ac] =new TH2F(ss.str().c_str(),ss.str().c_str(), 11, 0, 11, 48, 1, 49); 
    ATH_CHECK( tgclv1_expert_ac[ac]->regHist(m_tgclv1hptstripchamber[ac]) );

    // SL trigger chamber map
    ss.str(""); ss << "SL_Chamber_" << side[ac];
    m_tgclv1slchamber[ac] =new TH2F(ss.str().c_str(),ss.str().c_str(), 6, 0, 6, 48, 1, 49); 
    ATH_CHECK( tgclv1_shift_ac[ac]->regHist(m_tgclv1slchamber[ac]) );

    // Blank out chambers that don't exist
    BlankPhi24(m_tgclv1hptwirechamber[ac], 5);
    BlankPhi24(m_tgclv1hptstripchamber[ac], 5);
    BlankPhi24(m_tgclv1hptwirechamber[ac], 11);
    BlankPhi24(m_tgclv1hptstripchamber[ac], 11);
    BlankPhi24(m_tgclv1lptwirechamber[ac], 6);
    BlankPhi24(m_tgclv1lptstripchamber[ac], 6);
    BlankPhi24(m_tgclv1lptwirechamber[ac], 12);
    BlankPhi24(m_tgclv1lptstripchamber[ac], 12);
    // Set Bin Labels for chamber maps
    for(int ieta=0;ieta<6;ieta++){// X bins
      m_tgclv1lptwirechamber[ac] ->GetXaxis()->SetBinLabel(ieta+1, (schamberT3[ieta] + "_{In2}").c_str()); 
      m_tgclv1lptstripchamber[ac]->GetXaxis()->SetBinLabel(ieta+1, (schamberT3[ieta] + "_{In2}").c_str()); 
      if(ieta<5){
	m_tgclv1hptwirechamber[ac] ->GetXaxis()->SetBinLabel(ieta+1, (schamberT1[ieta] + "_{In1}").c_str()); 

	m_tgclv1hptstripchamber[ac]->GetXaxis()->SetBinLabel(ieta+1, (schamberT1[ieta] + "_{In1}").c_str()); 
      }
      m_tgclv1lptwirechamber[ac] ->GetXaxis()->SetBinLabel(ieta+7, (schamberT3[ieta] + "_{Out3}").c_str()); 
      m_tgclv1lptstripchamber[ac]->GetXaxis()->SetBinLabel(ieta+7, (schamberT3[ieta] + "_{Out3}").c_str()); 
      m_tgclv1hptwirechamber[ac] ->GetXaxis()->SetBinLabel(ieta+6, (schamberT3[ieta] + "_{Out3}").c_str()); 
      m_tgclv1hptstripchamber[ac]->GetXaxis()->SetBinLabel(ieta+6, (schamberT3[ieta] + "_{Out3}").c_str()); 

      m_tgclv1slchamber[ac]      ->GetXaxis()->SetBinLabel(ieta+1, schamberT3[ieta].c_str()); 
    }// ieta
    k=1;
    for(int isect=1;isect<=12;isect++){// sector
      int nphi=4, phistep=4;
      for(int iphi=0;iphi<nphi;iphi+=phistep){// phi
	ss.str(""); ss << side[ac];
	if(isect<10)ss << "0";
	ss << isect << "phi0";

	m_tgclv1lptwirechamber[ac] ->GetYaxis()->SetBinLabel( k, ss.str().c_str() ); 
	m_tgclv1lptstripchamber[ac]->GetYaxis()->SetBinLabel( k, ss.str().c_str() ); 
	m_tgclv1hptwirechamber[ac] ->GetYaxis()->SetBinLabel( k, ss.str().c_str() ); 
	m_tgclv1hptstripchamber[ac]->GetYaxis()->SetBinLabel( k, ss.str().c_str() ); 
	m_tgclv1slchamber[ac]      ->GetYaxis()->SetBinLabel( k, ss.str().c_str() ); 
	k+=phistep;
      }
    }
  }// side


  ///////////////////////////////////////////////////////////////////////////
  // Express Stream
  ss.str(""); ss << "ES_SL_vs_Muon_Chain_";
  m_tgclv1_SL_vs_muonchain_ES = new TH2F( ss.str().c_str(), ( ss.str() + ";SL;No Muon Chain" ).c_str(), 2, 0, 2, 2, 0, 2 );
  ATH_CHECK( tgclv1_expert_ES.regHist( m_tgclv1_SL_vs_muonchain_ES ) );
  m_tgclv1_SL_vs_muonchain_ES->GetXaxis()->SetBinLabel(1,"false");
  m_tgclv1_SL_vs_muonchain_ES->GetXaxis()->SetBinLabel(2,"true");
  m_tgclv1_SL_vs_muonchain_ES->GetYaxis()->SetBinLabel(1,"false");
  m_tgclv1_SL_vs_muonchain_ES->GetYaxis()->SetBinLabel(2,"true");


  return StatusCode::SUCCESS;
}// EOF


///////////////////////////////////////////////////////////////////////////
// bookTriggerRateHisto
///////////////////////////////////////////////////////////////////////////
StatusCode
TgcLv1RawDataValAlg::bookHistogramsTriggerRate(){

  ///////////////////////////////////////////////////////////////////////////
  // Make MonGroups for histogram booking paths
  std::string generic_path_tgclv1 = "Muon/MuonRawDataMonitoring/TGCLV1";
  MonGroup tgclv1_rate( this, generic_path_tgclv1+"/Global/Rate", run, ATTRIB_UNMANAGED );
  MonGroup tgclv1_rate_a( this, generic_path_tgclv1+"/TGCEA/Rate", run, ATTRIB_UNMANAGED );
  MonGroup tgclv1_rate_c( this, generic_path_tgclv1+"/TGCEC/Rate", run, ATTRIB_UNMANAGED );
  MonGroup* tgclv1_rate_ac[2] = { &tgclv1_rate_a, &tgclv1_rate_c};

  MonGroup tgclv1_ratio_a( this, generic_path_tgclv1+"/TGCEA/Rate/Ratio", run, ATTRIB_UNMANAGED   );
  MonGroup tgclv1_ratio_c( this, generic_path_tgclv1+"/TGCEC/Rate/Ratio", run, ATTRIB_UNMANAGED   );
  MonGroup* tgclv1_ratio_ac[2] = { &tgclv1_ratio_a, &tgclv1_ratio_c};

  std::stringstream ss;
  std::string side[2]={"A","C"};

  ///////////////////////////////////////////////////////////////////////////
  // Event Rate histograms
  // Events per 2LB
  ss.str(""); ss << "Number_Of_Events_In_2LB";
  m_tgclv1eventsinlb = new TH1F( ss.str().c_str(), ( ss.str() + ";LB;Events / 2LB" ).c_str(), 1250, 1,2501 );
  ATH_CHECK( tgclv1_rate.regHist( m_tgclv1eventsinlb ) );

  // Events per 10BCID
  ss.str(""); ss << "Number_Of_Events_In_10BCID";
  m_tgclv1eventsinbcid = new TH1F( ss.str().c_str(), ( ss.str() + ";BCID;Events / 10BCID" ).c_str(), 357, 0, 3570 );//BCID 0-3564
  ATH_CHECK( tgclv1_rate.regHist( m_tgclv1eventsinbcid ) );

  ///////////////////////////////////////////////////////////////////////////
  // SL Trigger Rate histograms
  //loop over side
  for(int ac=0;ac<2;ac++){
    // SL Trigger 2LB profile
    ss.str(""); ss << "Number_Of_SL_Triggers_In_2LB_" << side[ac];
    m_tgclv1slinlb[ac] = new TH1F( ss.str().c_str(), ( ss.str() + ";LB;SL / 2LB" ).c_str(), 1250, 1,2501 );
    ATH_CHECK( tgclv1_rate_ac[ac]->regHist( m_tgclv1slinlb[ac] ) );

    // SL Trigger 10BCID profile
    ss.str(""); ss << "Number_Of_SL_Triggers_In_10BCID_" << side[ac];
    m_tgclv1slinbcid[ac] = new TH1F( ss.str().c_str(), ( ss.str() + ";BCID;SL / 10BCID" ).c_str(), 357, 0, 3570 );//BCID 0-3564
    ATH_CHECK( tgclv1_rate_ac[ac]->regHist( m_tgclv1slinbcid[ac] ) );

    // SL Trigger per Event 2LB profile
    ss.str(""); ss << "Number_Of_SL_Triggers_Per_Event_Vs_2LB_" << side[ac];
    m_tgclv1slpereventlb[ac] = new TH1F( ss.str().c_str(), ( ss.str() + ";LB;SL / Events" ).c_str(), 1250, 1,2501 );
    ATH_CHECK( tgclv1_ratio_ac[ac]->regHist( m_tgclv1slpereventlb[ac] ) );

    // SL Trigger per Event 10BCID profile
    ss.str(""); ss << "Number_Of_SL_Triggers_Per_Event_Vs_10BCID_" << side[ac];
    m_tgclv1slpereventbcid[ac] = new TH1F( ss.str().c_str(), ( ss.str() + ";BCID;SL / Events" ).c_str(), 357, 0, 3570 );//BCID 0-3564
    ATH_CHECK( tgclv1_ratio_ac[ac]->regHist( m_tgclv1slpereventbcid[ac] ) );

    // SL Trigger LB profiles for each pT threshold
    for(int pt=0;pt<6;pt++){
      // SL Trigger 2LB profile for current pT threshold
      ss.str(""); ss << "Number_Of_PT" << pt + 1 << "_Triggers_In_2LB_" << side[ac];
      m_tgclv1slinlbpt[ac][pt] = new TH1F( ss.str().c_str(), ( ss.str() + ";LB;SL / 2LB" ).c_str(), 1250, 1,2501 );
      ATH_CHECK( tgclv1_rate_ac[ac]->regHist( m_tgclv1slinlbpt[ac][pt] ) );

      // SL Trigger 10BCID profile for current pT threshold
      ss.str(""); ss << "Number_Of_PT" << pt + 1 << "_Triggers_In_10BCID_" << side[ac];
      m_tgclv1slinbcidpt[ac][pt] = new TH1F( ss.str().c_str(), ( ss.str() + ";BCID;SL / 10BCID" ).c_str(), 357, 0, 3570 );//BCID 0-3564
      ATH_CHECK( tgclv1_rate_ac[ac]->regHist( m_tgclv1slinbcidpt[ac][pt] ) );

      // SL Trigger per Event 2LB profile for current pT threshold
      ss.str(""); ss << "Number_Of_PT" << pt + 1 << "_Triggers_Per_Event_Vs_2LB_" << side[ac];
      m_tgclv1slpereventlbpt[ac][pt] = new TH1F( ss.str().c_str(), ( ss.str() + ";LB;SL / Events" ).c_str(), 1250, 1,2501 );
      ATH_CHECK( tgclv1_ratio_ac[ac]->regHist( m_tgclv1slpereventlbpt[ac][pt] ) );

      // SL Trigger per Event 10BCID profile for current pT threshold
      ss.str(""); ss << "Number_Of_PT" << pt + 1 << "_Triggers_Per_Event_Vs_10BCID_" << side[ac];
      m_tgclv1slpereventbcidpt[ac][pt] = new TH1F( ss.str().c_str(), ( ss.str() + ";BCID;SL / Events" ).c_str(), 357, 0, 3570 );//BCID 0-3564
      ATH_CHECK( tgclv1_ratio_ac[ac]->regHist( m_tgclv1slpereventbcidpt[ac][pt] ) );
    }// pT
  }// side

  return StatusCode::SUCCESS;
}// EOF


///////////////////////////////////////////////////////////////////////////
// fillNumberOfTrigger
///////////////////////////////////////////////////////////////////////////
// Fills Trigger Profile histograms from vectors
void 
TgcLv1RawDataValAlg::fillNumberOfTrigger(){
  //std::string generic_path_tgclv1 = "Muon/MuonRawDataMonitoring/TGCLV1";
  //MonGroup tgclv1_shift( this, generic_path_tgclv1+"/Overview", shift, run );

  ///////////////////////////////////////////////////////////////////////////
  // Fill Event Rate histograms
  m_tgclv1eventsinlb  ->Fill(m_lumiblock);
  m_tgclv1eventsinbcid->Fill(m_BCID);

  ///////////////////////////////////////////////////////////////////////////
  // Fill Trigger Rate histograms
  for(int pcn=0;pcn<3;pcn++){
    ///////////////////////////////////////////////////////////////////////////
    // Fill LpT Trigger histograms
    for(int ws=0;ws<2;ws++){// ws
      for(int ilpt=0;ilpt<(int)m_lpt_delta[ws][pcn].size();ilpt++){// iLpT
	if(pcn!=CURR) continue;

	// Get variables from vectors
	int phi48  = m_lpt_phi48[ws][pcn].at(ilpt);
	int ef     = (m_lpt_isForward[ws][pcn].at(ilpt)==false);
	int ac     = (m_lpt_isAside[ws][pcn].at(ilpt)==false);
	int etain  = m_lpt_etain[ws][pcn].at(ilpt);
	int etaout = m_lpt_etaout[ws][pcn].at(ilpt);

	// Get shifted phi index
	int phi48mod=phi48;//[0:47]
	if(ef==0) phi48mod*=2;//[0:2:46]
	phi48mod+=2;//[2:49]
	if(phi48mod>47) phi48mod-=48;//[0:47], shifted to align with sector axis

	// Get phi index across whole side
	int phi48sect;
	if(ef==0){//->[0:23] Forward, shift index by +48 to seperate from Midstation
	  phi48sect=phi48+49;//[49:72]
	  if(phi48sect>71)phi48sect-=24;//[48:71], shifted to align with sector axis
	}else{//->[0:47]     Endcap
	  phi48sect=phi48+2;//[2:49]
	  if(phi48sect>47)phi48sect-=48;//[0:47], shifted to align with sector axis
	}

	if(pcn==CURR){
	  if(ws==1){
	    // Strip
	    // Fill phi profiles
	    m_tgclv1lptsectstrip[ac]      ->Fill(phi48sect+1);
	    m_tgclv1lptstripinlbvssect[ac]->Fill(m_lumiblock, phi48sect+1);
	    // Fill in&out comparison map
	    // 6 etain bins, followed by 6 etaout bins
	    m_tgclv1lptstripchamber[ac]->Fill((5-etain), phi48mod+1);
	    m_tgclv1lptstripchamber[ac]->Fill((5-etaout)+6, phi48mod+1);
	  }else{
	    // Wire
	    // Fill phi profiles
	    m_tgclv1lptsectwire[ac]      ->Fill(phi48sect+1);
	    m_tgclv1lptwireinlbvssect[ac]->Fill(m_lumiblock, phi48sect+1);
	    // Fill in&out comparison map
	    // 6 etain bins, followed by 6 etaout bins
	    m_tgclv1lptwirechamber[ac]->Fill((5-etain), phi48mod+1);
	    m_tgclv1lptwirechamber[ac]->Fill((5-etaout)+6, phi48mod+1);
	  }
	}// current
      }// iLpT
    }// ws

    ///////////////////////////////////////////////////////////////////////////
    // Fill HpT Trigger histograms
    for(int ws=0;ws<2;ws++){
      for(int ihpt=0;ihpt<(int)m_hpt_delta[ws][pcn].size();ihpt++){
	if(pcn!=CURR) continue;

	// Get variables from vectors
	int phi48  = m_hpt_phi48[ws][pcn].at(ihpt);
	int ef     = !(m_hpt_isForward[ws][pcn].at(ihpt));
	int ac     = (m_hpt_isAside[ws][pcn].at(ihpt)==false);
	int etain  = m_hpt_etain[ws][pcn].at(ihpt);
	int etaout = m_hpt_etaout[ws][pcn].at(ihpt);

	//EIFI bit information
	bool t_isInner = m_hpt_isInner[ws][pcn].at(ihpt);
	int  t_inner   = m_hpt_inner[ws][pcn].at(ihpt);
	std::bitset<8> m8bits (t_inner);

	int Bits_19th = t_inner;
	int b_EI0 = 0;
	int b_EI1 = 0;
	int b_EI2 = 0;
	int b_EI3 = 0;
	int b_FI0 = 0;
	int b_FI1 = 0;
	int b_FI2 = 0;
	int b_FI3 = 0;
	if (Bits_19th & 0x01) b_EI0 = 1;
	if (Bits_19th & 0x02) b_FI0 = 1;
	if (Bits_19th & 0x04) b_EI1 = 1;
	if (Bits_19th & 0x08) b_FI1 = 1;
	if (Bits_19th & 0x10) b_EI2 = 1;
	if (Bits_19th & 0x20) b_FI2 = 1;
	if (Bits_19th & 0x40) b_EI3 = 1;
	if (Bits_19th & 0x80) b_FI3 = 1;

	// Get shifted phi index
	int phi48mod=phi48;//[0:47]
	if(ef==0)phi48mod=phi48mod*2;//[0:2:46]
	phi48mod+=2;//[2:49]
	if(phi48mod>47) phi48mod-=48;//[0:47], shifted to align with sector axis

	// Get phi index across whole side
	int phi48sect;
	if(ef==0){ //->[0:23] Forward, shift index by +48 to seperate from Endcap
	  phi48sect=phi48+49;//[49:72]
	  if(phi48sect>71)phi48sect-=24;//[48:71], shifted to align with sector axis
	}else{//->[0:47] Endcap
	  phi48sect=phi48+2;//[2:49]
	  if(phi48sect>47)phi48sect-=48;//[0:47], shifted to align with sector axis
	}

	if(pcn==CURR){
	  if(t_isInner){
	    if(ws==0){
	      if (b_EI0 == 1)m_tgclv1hptstripinlbvssecteifilumi[ac]->Fill(m_lumiblock,0);
	      if (b_FI0 == 1)m_tgclv1hptstripinlbvssecteifilumi[ac]->Fill(m_lumiblock,1);
	      if (b_EI1 == 1)m_tgclv1hptstripinlbvssecteifilumi[ac]->Fill(m_lumiblock,2);
	      if (b_FI1 == 1)m_tgclv1hptstripinlbvssecteifilumi[ac]->Fill(m_lumiblock,3);
	      if (b_EI2 == 1)m_tgclv1hptstripinlbvssecteifilumi[ac]->Fill(m_lumiblock,4);
	      if (b_FI2 == 1)m_tgclv1hptstripinlbvssecteifilumi[ac]->Fill(m_lumiblock,5);
	      if (b_EI3 == 1)m_tgclv1hptstripinlbvssecteifilumi[ac]->Fill(m_lumiblock,6);
	      if (b_FI3 == 1)m_tgclv1hptstripinlbvssecteifilumi[ac]->Fill(m_lumiblock,7);

	      if (b_EI0 == 1)m_tgclv1hptstripinlbvssecteifisect[ac]->Fill(phi48+1,0);
	      if (b_FI0 == 1)m_tgclv1hptstripinlbvssecteifisect[ac]->Fill(phi48+1,1);
	      if (b_EI1 == 1)m_tgclv1hptstripinlbvssecteifisect[ac]->Fill(phi48+1,2);
	      if (b_FI1 == 1)m_tgclv1hptstripinlbvssecteifisect[ac]->Fill(phi48+1,3);
	      if (b_EI2 == 1)m_tgclv1hptstripinlbvssecteifisect[ac]->Fill(phi48+1,4);
	      if (b_FI2 == 1)m_tgclv1hptstripinlbvssecteifisect[ac]->Fill(phi48+1,5);
	      if (b_EI3 == 1)m_tgclv1hptstripinlbvssecteifisect[ac]->Fill(phi48+1,6);
	      if (b_FI3 == 1)m_tgclv1hptstripinlbvssecteifisect[ac]->Fill(phi48+1,7);
	    }
	  }else{ 
	    if(ws==1){
	      // Strip
	      // Fill phi profiles
	      m_tgclv1hptsectstrip[ac]      ->Fill(phi48sect+1);
	      m_tgclv1hptstripinlbvssect[ac]->Fill(m_lumiblock, phi48sect+1);
	      // Fill in&out comparison map
	      // 5 etain bins, followed by 6 etaout bins
	      m_tgclv1hptstripchamber[ac]->Fill((4-etain), phi48mod+1);
	      m_tgclv1hptstripchamber[ac]->Fill((5-etaout)+5, phi48mod+1);
	    }else{
	      // Wire
	      // Fill phi profiles
	      m_tgclv1hptsectwire[ac]      ->Fill(phi48sect+1);
	      m_tgclv1hptwireinlbvssect[ac]->Fill(m_lumiblock, phi48sect+1);
	      // Fill in&out comparison map
	      // 5 etain bins, followed by 6 etaout bins
	      m_tgclv1hptwirechamber[ac]->Fill((4-etain), phi48mod+1);
	      m_tgclv1hptwirechamber[ac]->Fill((5-etaout)+5, phi48mod+1);
	    }
	  }//EIFI coin 
	}// current
      }// iHpT
    }// ws

    ///////////////////////////////////////////////////////////////////////////
    // SL Triggers
    //m_nSL[pcn] = 0;
    for(int isl=0;isl<(int)m_sl_pt[pcn].size();isl++){
      if(pcn!=CURR) continue;
      // Get variables from vectors
      int pt     = m_sl_pt[pcn].at(isl);
      int phi48  = m_sl_phi48[pcn].at(isl);
      int ef     = (m_sl_isForward[pcn].at(isl)==false);
      int ac     = (m_sl_isAside[pcn].at(isl)==false);
      int etaout = m_sl_etaout[pcn].at(isl);
      int etaroi = m_sl_etaroi[pcn].at(isl);
      int phiroi = m_sl_phiroi[pcn].at(isl);

      // Get shifted phi index
      int phi48mod=phi48;//[0:47]
      if(ef==0)phi48mod=phi48mod*2;//[0:2:46]
      phi48mod+=2;//[2:49]
      if(phi48mod>47) phi48mod-=48;//[0:47], shifted to align with sector axis

      // Get phi index across whole side
      int phi48sect;
      if(ef==0){ //->[0:23] Forward, shift index by +48 to seperate from Midstation
	phi48sect=phi48+49;//[49:72]
	if(phi48sect>71)phi48sect-=24;//[48:71], shifted to align with sector axis
      }
      else{//->[0:47] Endcap
	phi48sect=phi48+2;//[2:49]
	if(phi48sect>47)phi48sect-=48;//[0:47], shifted to align with sector axis
      }

      if(pcn==CURR){
	// Fill pT threshold profile
	m_tgclv1pt[ac]->Fill(pt);

	// Fill RoI eta vs phi maps
	m_tgclv1roietavsphi[ac]        ->Fill(etaroi, (float)phiroi/4.);
	m_tgclv1roietavsphipt[ac][pt-1]->Fill(etaroi, (float)phiroi/4.);
	if(ac==0){  // Fill A-side
	  m_tgclv1roietavsphiAll        ->Fill(etaroi+1.1, (float)phiroi/4.);
	  m_tgclv1roietavsphiptAll[pt-1]->Fill(etaroi+1.1, (float)phiroi/4.);
	} else {  // Fill C-side
	  m_tgclv1roietavsphiAll        ->Fill(-(etaroi+1.1), (float)phiroi/4.);
	  m_tgclv1roietavsphiptAll[pt-1]->Fill(-(etaroi+1.1), (float)phiroi/4.);
	}  
	// Fill phi profiles
	m_tgclv1slsect[ac]      ->Fill(phi48sect+1);
	m_tgclv1slinlbvssect[ac]->Fill(m_lumiblock, phi48sect+1);

	// Fill in&out comparison map
	m_tgclv1slchamber[ac]->Fill((5-etaout), phi48mod+1);
	if(ac==0)m_tgclv1slchamberAll ->Fill((5-etaout)+1, phi48mod+1); // fill A-side
	else m_tgclv1slchamberAll ->Fill(-(5-etaout)-1.1, phi48mod+1); // fill C-side
	// not monitor these profiles at GM
	if( m_environment != AthenaMonManager::online )
	  m_tgclv1slchamberlowstat[ac]->Fill((5-etaout), phi48mod+1);

	// Fill LB profiles
	m_tgclv1slinlb[ac]        ->Fill(m_lumiblock);
	m_tgclv1slinlbpt[ac][pt-1]->Fill(m_lumiblock);
	// Fill BCID profiles
	m_tgclv1slinbcid[ac]        ->Fill(m_BCID);
	m_tgclv1slinbcidpt[ac][pt-1]->Fill(m_BCID);
      }// current
    }// isl

  }// pcn

  // Fill Trigger timing profiles
  for(int ipcnt=0;ipcnt<4;ipcnt++){
    if( m_environment != AthenaMonManager::online ){
      m_tgclv1numberoflptwire[ipcnt] ->Fill(m_nLptWire[ipcnt]);
      m_tgclv1numberoflptstrip[ipcnt]->Fill(m_nLptStrip[ipcnt]);
      m_tgclv1numberofhptwire[ipcnt] ->Fill(m_nHptWire[ipcnt]);
      m_tgclv1numberofhptstrip[ipcnt]->Fill(m_nHptStrip[ipcnt]);
      m_tgclv1numberofsl[ipcnt]      ->Fill(m_nSL[ipcnt]);
    }

    if( ipcnt==1 ){
      int hasSLcurrent = (m_nSL[1]>0) ;
      m_tgclv1_SL_vs_muonchain_ES->Fill(hasSLcurrent, m_found_nonmuon_express_chain);
    }
  }

  return;
}
