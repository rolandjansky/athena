/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package: TgcRawDataValAlg
// Authors:  N. Benekos(Illinois), M. Ishino(ICEPP), H.Kurashige (Kobe), A.Ishikawa (Kobe)  akimasa.ishikawa@cern.ch, M.King(Kobe) kingmgl@stu.kobe-u.ac.jp
// Original: Apr. 2008
// Modified: June 2011
//
// DESCRIPTION:
// Subject: TGCLV1-->Offline Muon Data Quality/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

// GeoModel
#include "MuonReadoutGeometry/TgcReadoutParams.h"

// Cabling Service
//#include "TGCcablingInterface/ITGCcablingServerSvc.h"

#include "Identifier/Identifier.h"

// MuonRDO
#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoIdHash.h"
#include "MuonRDO/TgcRdoContainer.h"

#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
//#include "MuonDQAUtils/TGCDQAUtils.h"
 
#include "TgcRawDataMonitoring/TgcRawDataValAlg.h"
#include "AthenaMonitoring/AthenaMonManager.h"

#include <TError.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TF1.h>
#include <inttypes.h> 

#include <sstream>
#include <math.h>

using namespace std;

StatusCode 
TgcRawDataValAlg::bookHistogramsSummary(){
  m_log << MSG::INFO << "bookHistogramsSummary" << endreq;    
  StatusCode sc=StatusCode::SUCCESS;
  
  ///////////////////////////////////////////////////////////////////////////
  // Make MonGroups for histogram booking paths
  MonGroup tgcprd_summary( this, generic_path_tgcmonitoring + "/Global/Summary", run, ATTRIB_UNMANAGED ); 
  MonGroup tgcprd_summary_a( this, generic_path_tgcmonitoring + "/TGCEA/Summary", run, ATTRIB_UNMANAGED ); 
  MonGroup tgcprd_summary_c( this, generic_path_tgcmonitoring + "/TGCEC/Summary", run, ATTRIB_UNMANAGED ); 
  MonGroup* tgcprd_summary_ac[2] = { &tgcprd_summary_a,&tgcprd_summary_c };
  
  std::stringstream ss;
  std::string side[2] = {"A","C"};
  string wireoccupergasgap  = "Wire_Occupancy_Per_GasGap_";
  string stripoccupergasgap = "Strip_Occupancy_Per_GasGap_";
  
  for(int i=0;i<2;i++){// side
    //Summary
    ss.str(""); ss << "Summary_Of_Log10_"<<wireoccupergasgap<<side[i];
    tgcsummaryoflog10wireoccupancypergasgap[i]=new TH1F(ss.str().c_str(), (ss.str() + ";log10(Occupancy); Entries").c_str(),80,-8.,0.);
    sc=tgcprd_summary_ac[i]->regHist(tgcsummaryoflog10wireoccupancypergasgap[i]) ;  
    if(sc.isFailure()) { 
      m_log << MSG::FATAL << ss.str() << " Failed to register histogram " << endreq;       
      return sc;
    }
    
    ss.str(""); ss << "Summary_Of_Log10_"<<stripoccupergasgap<<side[i];
    tgcsummaryoflog10stripoccupancypergasgap[i]=new TH1F(ss.str().c_str(), (ss.str() + ";log10(Occupancy); Entries" ).c_str(),80,-8.,0.);
    sc=tgcprd_summary_ac[i]->regHist(tgcsummaryoflog10stripoccupancypergasgap[i]) ;  
    if(sc.isFailure()) { 
      m_log << MSG::FATAL << ss.str() << " Failed to register histogram " << endreq;       
      return sc;
    }
  }// side
  
  string wireoccuperchamtype  = "Wire_Occupancy_Per_Chamber_Type_Station";
  string stripoccuperchamtype = "Strip_Occupancy_Per_Chamber_Type_Station";

  int ntype=0;
  string chamtype[4][6] = {{ "_F_T1", "_E4_T3", "_E3_T6", "_E2_T7", "_E1_T8",  "EMPTY"},
                           { "_F_T2", "_E5_T4", "_E4_T6", "_E3_T7", "_E2_T8", "_E1_T9"},
                           { "_F_T2", "_E5_T5", "_E4_T6", "_E3_T7", "_E2_T8", "_E1_T9"},
                           {"_F_T10", "_E_T11",  "EMPTY",  "EMPTY",  "EMPTY",  "EMPTY"}};
  for(int sta=0;sta<4;sta++){// station
    for(int eta=0;eta<6;eta++){// eta
      if(sta==0 && eta==5){
        tgcsummaryoflog10wireoccupancyperchambertype[ntype] = 0;
        tgcsummaryoflog10stripoccupancyperchambertype[ntype] = 0;
        continue;
        
      }
      if(sta==3 && eta >1){
        tgcsummaryoflog10wireoccupancyperchambertype[ntype] = 0;
        tgcsummaryoflog10stripoccupancyperchambertype[ntype] = 0;
        continue;
      }
      
      if(chamtype[sta][eta].compare("EMPTY") == 0) continue;
      // Summary
      ss.str(""); ss << "Summary_Of_Log10_" << wireoccuperchamtype << sta+1 << chamtype[sta][eta];
      tgcsummaryoflog10wireoccupancyperchambertype[ntype]=new TH1F(ss.str().c_str(), ( ss.str() + ";log10(Occupancy); Entries").c_str(),80,-8.,0.);
      sc=tgcprd_summary.regHist(tgcsummaryoflog10wireoccupancyperchambertype[ntype]) ;  
      if(sc.isFailure()) { 
        m_log << MSG::FATAL << "tgcsummaryoflog10wireoccupancyperchambertype["<<ntype<<"] Failed to register histogram " << endreq;       
        return sc;
      }
      // Summary
      ss.str(""); ss << "Summary_Of_Log10_" << stripoccuperchamtype << sta+1 << chamtype[sta][eta];
      tgcsummaryoflog10stripoccupancyperchambertype[ntype]=new TH1F(ss.str().c_str(), ( ss.str() + ";log10(Occupancy); Entries").c_str(),80,-8.,0.);
      sc=tgcprd_summary.regHist(tgcsummaryoflog10stripoccupancyperchambertype[ntype]) ;  
      if(sc.isFailure()) { 
        m_log << MSG::FATAL << "tgcsummaryoflog10stripoccupancyperchambertype["<<ntype<<"] Failed to register histogram " << endreq;
        return sc;
      }
      ntype++;
    }// eta
  }// station
  
  return sc;
}



StatusCode 
TgcRawDataValAlg::bookHistogramsLowStat(){
  m_log << MSG::INFO << "bookHistogramsLowStat" << endreq;
  StatusCode sc=StatusCode::SUCCESS;

  MonGroup tgcprd_lowstat_a( this, generic_path_tgcmonitoring + "/TGCEA", lowStat, ATTRIB_UNMANAGED ); 
  MonGroup tgcprd_lowstat_c( this, generic_path_tgcmonitoring + "/TGCEC", lowStat, ATTRIB_UNMANAGED ); 
  MonGroup* tgcprd_lowstat_ac[2]={ &tgcprd_lowstat_a, &tgcprd_lowstat_c};

  std::stringstream ss;
  std::string side[2] = {"A","C"};
  std::string schamberT1[6]={"E1", "E2", "E3", "E4", "F"};
  std::string schamberT3[6]={"E1", "E2", "E3", "E4", "E5", "F"};
  std::string schamberEF[2]={"FI", "EI"};

  for(int ac=0;ac<2;ac++){
    //Wire Strip Coincidence per GasGap
    ss.str(""); ss<< "Wire_Strip_Coincidence_Per_GasGap_In_10LBs_" << side[ac];
    tgcwirestripcoinlowstat[ac] = new TH2F( ss.str().c_str(), ss.str() .c_str(),43, 0, 43, 48, 1, 49 );
    sc=tgcprd_lowstat_ac[ac]->regHist(tgcwirestripcoinlowstat[ac]) ;  
    if(sc.isFailure()) { 
      m_log << MSG::FATAL << ss.str() << "Failed to register histogram " << endreq;       
      return sc;
    }

    for(int sec=1;sec<=12;sec++){
      for(int phi=0;phi<=3;phi+=4){
        ss.str(""); ss << side[ac];
        if(sec<10)ss << "0";
        ss << sec << "phi" << phi;
        int k=(sec-1)*4+phi+1;
        tgcwirestripcoinlowstat[ac] ->GetYaxis()->SetBinLabel( k, ss.str().c_str() ); 
      }
    }
    int x=1;
    //layer1-3
    for(int l=0 ; l<3 ; l++ ){
      for( int c=0 ; c<5 ; c++ ){
        ss.str(""); ss << "L" << l+1 << "_" << schamberT1[c]; 
        tgcwirestripcoinlowstat[ac] ->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
      }
    }
    //layer4-7
    for(int l=3 ; l<7 ; l++ ){
      for( int c=0 ; c<6 ; c++ ){
        ss.str(""); ss << "L" << l+1 << "_" << schamberT3[c]; 
        tgcwirestripcoinlowstat[ac] ->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
      }
    }
    //layer8-9
    for(int l=7 ; l<9 ; l++ ){
      for( int c=0 ; c<2 ; c++ ){
        ss.str(""); ss << "L" << l+1 << "_" << schamberEF[c]; 
        tgcwirestripcoinlowstat[ac] ->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
      }
    }
    tgcwirestripcoinlowstat[ac]  ->GetXaxis()->LabelsOption("v");
  }//ac

  return sc;
}
