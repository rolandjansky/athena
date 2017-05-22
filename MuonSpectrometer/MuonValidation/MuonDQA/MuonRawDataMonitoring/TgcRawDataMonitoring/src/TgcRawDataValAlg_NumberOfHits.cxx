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
TgcRawDataValAlg::bookHistogramsNumberOfHits(){
  //Following histograms are booked
  //
  // Event_Counter
  // Number_Of_TGC_Hits_Per_Event
  // Number_Of_TGC_Wire_Hits_Per_Event
  // Number_Of_TGC_Strip_Hits_Per_Event
  // Number_Of_TGC_Hits_Per_Event_{A,C}

  m_log << MSG::INFO << "bookHistogramsNumberOfHits" << endmsg;       

  StatusCode sc=StatusCode::SUCCESS;

  //MonGroup tgcprd_shift( this, generic_path_tgcmonitoring + "/Overview", shift, run ); 
  MonGroup tgcprd_shift( this, m_generic_path_tgcmonitoring + "/Global", run, ATTRIB_UNMANAGED ); 
  MonGroup tgcprd_shift_a( this, m_generic_path_tgcmonitoring + "/TGCEA", run, ATTRIB_UNMANAGED ); 
  MonGroup tgcprd_shift_c( this, m_generic_path_tgcmonitoring + "/TGCEC", run, ATTRIB_UNMANAGED ); 
  MonGroup tgcprd_expert( this, m_generic_path_tgcmonitoring + "/Global", run, ATTRIB_UNMANAGED ); 

  std::stringstream ss;
  std::string side[2] = {"A","C"};

  //event counter
  m_tgceventcounter=new TH1F("Event_Counter","Event_Counter; [counts]; Number of Hits/Events",1,0,1.);       
  sc=tgcprd_shift.regHist(m_tgceventcounter) ;  
  if(sc.isFailure()) { 
    m_log << MSG::FATAL << "m_tgceventcounter Failed to register histogram " << endmsg;       
    return sc;
  }

  //number of hits per event
  m_tgcevents=new TH1F("Number_Of_TGC_Hits_Per_Event", "Number_Of_TGC_Hits_Per_Event; [counts]; Number of Hits/Events",500,0,500.);       
  sc=tgcprd_shift.regHist(m_tgcevents) ;  
  if(sc.isFailure()) { 
    m_log << MSG::FATAL << "m_tgcevents Failed to register histogram " << endmsg;       
    return sc;
  }
  
  //number of wire hits per event
  std::string tgcnumberofwirehits_title = "Number_Of_TGC_Wire_Hits_Per_Event";
  m_tgcnumberofwirehits=new TH1F("Number_Of_TGC_Wire_Hits_Per_Event", "Number_Of_TGC_Wire_Hits_Per_Event; [counts]; Number of Wire Hits/Event",300,0,300.);       
  sc=tgcprd_expert.regHist(m_tgcnumberofwirehits) ;  
  if(sc.isFailure()) { 
    m_log << MSG::FATAL << "m_tgcnumberofwirehits Failed to register histogram " << endmsg;       
    return sc;
  }

  //number of strip hits per event
  std::string tgcnumberofstriphits_title = "Number_Of_TGC_Strip_Hits_Per_Event";
  m_tgcnumberofstriphits=new TH1F("Number_Of_TGC_Strip_Hits_Per_Event", "Number_Of_TGC_Strip_Hits_Per_Event; [counts]; Number of Strip Hits/Events",300,0,300.);       
  sc=tgcprd_expert.regHist(m_tgcnumberofstriphits) ;  
  if(sc.isFailure()) { 
    m_log << MSG::FATAL << "m_tgcnumberofstriphits Failed to register histogram " << endmsg;       
    return sc;
  }

  //number of hits per event for A and C side
  for(int ac=0;ac<2;ac++){
    ss.str(""); ss<<"Number_Of_TGC_Hits_Per_Event_"<<side[ac];
    m_tgcnumberofhits[ac]=new TH1F(ss.str().c_str(), (ss.str() + "; [counts]; Number of Hits/Events" ).c_str(),500,0.,500.);       
    if(ac==0) sc=tgcprd_shift_a.regHist(m_tgcnumberofhits[ac]) ;  
    else      sc=tgcprd_shift_c.regHist(m_tgcnumberofhits[ac]) ;  
    if(sc.isFailure()) { 
      m_log << MSG::FATAL << ss.str() << " Failed to register histogram " << endmsg;       
      return sc;
    }
  }

  return sc;
}

StatusCode
TgcRawDataValAlg::fillNumberOfHits(){
  // Get nHit values for filling histograms
  int nHitsTotal=0;
  int nHitsWS[2]  ={0,0};//[ws]
  int nHitsSide[2]={0,0};//[ac]
  for(int ac=0;ac<2;ac++)
    for(int ws=0;ws<2;ws++){
      nHitsTotal   +=m_nHits[ac][ws];
      nHitsWS[ws]  +=m_nHits[ac][ws];
      nHitsSide[ac]+=m_nHits[ac][ws];
    }
  if(nHitsTotal>=500)   nHitsTotal=1499;
  if(nHitsWS[0]>=300)   nHitsWS[0]=299;
  if(nHitsWS[1]>=300)   nHitsWS[1]=299;
  if(nHitsSide[0]>=500) nHitsSide[0]=499;
  if(nHitsSide[1]>=500) nHitsSide[1]=499;
  
  // Fill Event Counter
  m_tgceventcounter->Fill(0);
  
  // Fill nHits Histograms
  m_tgcevents->Fill(nHitsTotal);
  for(int ac=0;ac<2;ac++)
    m_tgcnumberofhits[ac]->Fill(nHitsSide[ac]);
  m_tgcnumberofwirehits->Fill(nHitsWS[0]);
  m_tgcnumberofstriphits->Fill(nHitsWS[1]);
  
  return StatusCode::SUCCESS;
}
