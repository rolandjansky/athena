/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/AlgFactory.h"
#include "StoreGate/DataHandle.h"

#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "MuonRDO/TgcRdoIdHash.h"

// GeoModel
#include "MuonReadoutGeometry/MuonDetectorManager.h"
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
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TMath.h>
#include <inttypes.h> 
#include <TProfile.h>

#include <sstream>

using namespace std;


///////////////////////////////////////////////////////////////////////////
// bookSummaryHisto
///////////////////////////////////////////////////////////////////////////
StatusCode
TgcLv1RawDataValAlg::bookHistogramsSummary(){
  
  ///////////////////////////////////////////////////////////////////////////
  // Make MonGroups for histogram booking paths
  std::string generic_path_tgclv1 = "Muon/MuonRawDataMonitoring/TGCLV1";
  MonGroup tgclv1_summary( this, generic_path_tgclv1+"/Global/Summary",  run, ATTRIB_UNMANAGED );
  MonGroup tgclv1_summary_a( this, generic_path_tgclv1+"/TGCEA/Summary", run, ATTRIB_UNMANAGED );
  MonGroup tgclv1_summary_c( this, generic_path_tgclv1+"/TGCEC/Summary", run, ATTRIB_UNMANAGED );

  std::stringstream ss;
  std::string side[2] ={"A","C"};
  std::string type[6] ={"_F_T2", "_E5_T5", "_E4_T6", "_E3_T7", "_E2_T8","_E1_T9" };

  for(int ac=0;ac<2;ac++){// side
    // SL Timing summary
    ss.str(""); ss << "Summary_Of_SL_Timing_" << side[ac];
    m_tgclv1summaryofsltiming[ac] = new TH1F(ss.str().c_str(), ss.str().c_str(), 151, 0.7, 1.002);
    if(ac==0) ATH_CHECK( tgclv1_summary_a.regHist(m_tgclv1summaryofsltiming[ac]) );
    else      ATH_CHECK( tgclv1_summary_c.regHist(m_tgclv1summaryofsltiming[ac]) );
    m_tgclv1summaryofsltiming[ac]->GetXaxis()->SetTitle("Current BC fraction");
    m_tgclv1summaryofsltiming[ac]->GetYaxis()->SetTitle("Entries");

    // LpT Timing summary
    ss.str(""); ss << "Summary_Of_Low_Pt_Timing_" << side[ac];
    m_tgclv1summaryoflpttiming[ac] = new TH1F(ss.str().c_str(), ss.str().c_str(), 151, 0.7, 1.002);
    if(ac==0) ATH_CHECK( tgclv1_summary_a.regHist(m_tgclv1summaryoflpttiming[ac]) );
    else      ATH_CHECK( tgclv1_summary_c.regHist(m_tgclv1summaryoflpttiming[ac]) );
    m_tgclv1summaryoflpttiming[ac]->GetXaxis()->SetTitle("Current BC fraction");
    m_tgclv1summaryoflpttiming[ac]->GetYaxis()->SetTitle("Entries");
  }// side
  
  
  //Summary
  for(int itype=0;itype<6;itype++){// chambertype
    // SL Timing summary per chamber type
    ss.str(""); ss << "Summary_Of_SL_Timing_Per_Chamber_Type" << type[itype];
    m_tgclv1summaryofsltimingperchambertype[itype]=new TH1F(ss.str().c_str(), ss.str().c_str(), 151, 0.7, 1.002);
        
    ATH_CHECK( tgclv1_summary.regHist(m_tgclv1summaryofsltimingperchambertype[itype]) );
    m_tgclv1summaryofsltimingperchambertype[itype]->GetXaxis()->SetTitle("Current BC fraction"); 
    m_tgclv1summaryofsltimingperchambertype[itype]->GetYaxis()->SetTitle("Entries"); 
    
    // LpT Timing summary per chamber type
    ss.str(""); ss << "Summary_Of_Low_Pt_Timing_Per_Chamber_Type" << type[itype];
    m_tgclv1summaryoflpttimingperchambertype[itype]=new TH1F(ss.str().c_str(), ss.str().c_str(), 151, 0.7, 1.002);
      
    ATH_CHECK( tgclv1_summary.regHist(m_tgclv1summaryoflpttimingperchambertype[itype]) );
    m_tgclv1summaryoflpttimingperchambertype[itype]->GetXaxis()->SetTitle("Current BC fraction"); 
    m_tgclv1summaryoflpttimingperchambertype[itype]->GetYaxis()->SetTitle("Entries");
  }// chambertype

  return StatusCode::SUCCESS;
}// EOF


///////////////////////////////////////////////////////////////////////////
// bookLowStatHisto
///////////////////////////////////////////////////////////////////////////
StatusCode
TgcLv1RawDataValAlg::bookHistogramsLowStat(){
  
  ///////////////////////////////////////////////////////////////////////////
  // Make MonGroups for histogram booking paths
  std::string generic_path_tgclv1 = "Muon/MuonRawDataMonitoring/TGCLV1";

  MonGroup tgclv1_lowstat_a( this, generic_path_tgclv1+"/TGCEA", lowStat, ATTRIB_UNMANAGED );
  MonGroup tgclv1_lowstat_c( this, generic_path_tgclv1+"/TGCEC", lowStat, ATTRIB_UNMANAGED );
  MonGroup* tgclv1_lowstat_ac[2] = { &tgclv1_lowstat_a, &tgclv1_lowstat_c};

  int k=0;
  std::stringstream ss;
  std::string side[2]       ={"A","C"};
  std::string schamberT3[6] ={"E1", "E2", "E3", "E4", "E5", "F"};
  std::string sltiming      = "SL_Timing";
  std::string morethanpt1   = "_More_Than_PT1";

  for(int ac=0;ac<2;ac++){// side
    // SL trigger chamber map in 10LBs
    ss.str(""); ss << "SL_Chamber_In_10LBs_" << side[ac];
    m_tgclv1slchamberlowstat[ac] =new TH2F(ss.str().c_str(),ss.str().c_str(), 6, 0, 6, 48, 1, 49); 
    ATH_CHECK( tgclv1_lowstat_ac[ac]->regHist(m_tgclv1slchamberlowstat[ac]) );
    
    // SL trigger chamber map in 10LBs for pT threshold > pT1
    ss.str(""); ss << sltiming << morethanpt1 << "_In_10LBs_" << side[ac];
    m_tgclv1sltimingptcutlowstat[ac] = new TH1F(ss.str().c_str(),ss.str().c_str(), 3, 0, 3);
    setTH1TitleLabelBCID(m_tgclv1sltimingptcutlowstat[ac]);
    ATH_CHECK( tgclv1_lowstat_ac[ac]->regHist(m_tgclv1sltimingptcutlowstat[ac]) );
    
    // Set Bin Labels
    for(int icham=0;icham<6;icham++){
      m_tgclv1slchamberlowstat[ac]->GetXaxis()->SetBinLabel(icham+1, schamberT3[icham].c_str()); 
    }// chambertype
    k=1;
    for(int isect=1;isect<=12;isect++){// sector
      int nphi=4, phistep=1;
      for(int iphi=0;iphi<nphi;iphi+=phistep){// phi
        ss.str(""); ss << side[ac];
        if(isect<10)ss << "0";
        ss << isect << "phi" << iphi;
        m_tgclv1slchamberlowstat[ac]->GetYaxis()->SetBinLabel(k, ss.str().c_str());
        k+=phistep;
      }// phi
    }// sector
  }// side
  
  return StatusCode::SUCCESS;
}// EOF
