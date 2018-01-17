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

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
#include "MuonDQAUtils/MuonDQAHistMap.h" 
 
#include "TgcRawDataMonitoring/TgcLv1RawDataValAlg.h"
#include "AthenaMonitoring/AthenaMonManager.h"

//for express menu
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"

#include <TH1.h>
#include <TH2.h>
#include <TList.h>
#include <TBox.h>
#include <TMath.h>
#include <inttypes.h> 
#include <TProfile.h>

using namespace std;

///////////////////////////////////////////////////////////////////////////
// RoI Eta&Phi extractor function
void
TgcLv1RawDataValAlg::roi2etaphi(const Muon::TgcCoinData& cd, int& etaroi, int& phiroi){
  int roi=cd.roi();//[0:147]([0:63])
  //int localeta=roi/4;

  if(cd.isForward()){//forward
    int basephi=cd.phi();//basephi=[1:24]
    if(basephi>23) basephi-=24;//[0:23] 0 = sector1, phi0

    int localphi=roi%4;

    //etaroi=roi/4;
    etaroi=roi/4+37;//[37:52]
    phiroi=(localphi+basephi*4)*2;
  }else{//endcap
    int basephi=cd.phi()+1;//basephi=[2:49], 2 = sector1, phi2 (phi=0degree)
    if(basephi>47) basephi-=48;//[0:47] 0 = sector1, phi0

    int sector=phi2sector(cd.phi(),1);

    phiroi=basephi*4 + ( sector%2==0 ? roi%4 : 3-(roi%4)   );
    etaroi=roi/4;//[0:36]
  }
  //etaroi=52-etaroi;
  return;
}

///////////////////////////////////////////////////////////////////////////
// Get Sector[0:11] from Phi[0:23]/[0:47]
int
TgcLv1RawDataValAlg::phi2sector(int phi48, int fe){
  //phi=[0:47], [0:23]
  //ef=0:forward, 1:endcap
  //sector=[0:11]
  // phi is offset by 1 in the forward, 2 in the endcap
  // to match it up with 

  int sector=-1;
  if(fe==0){//forward
    sector = (phi48+1)/2;//[0:12]
  }else{//endcap
    sector = (phi48+2)/4;//[0:12]
  }
  if(sector>11)sector-=12;//[0:11]

  return sector;
}

///////////////////////////////////////////////////////////////////////////
// Get Layer Number from Station and GasGap
int
TgcLv1RawDataValAlg::stationGasGap2layer(int station, int GasGap){
  int layer=0;
  if(station==41||station==42){
    layer += GasGap;
  }else if(station==43||station==44){
    layer = 3+GasGap;
  }else if(station==45||station==46){
    layer = 5+GasGap;
  }
  return layer;
}

///////////////////////////////////////////////////////////////////////////
// Set Titles and Labels for BCID TH1&TH2 histograms
void
TgcLv1RawDataValAlg::setTH1TitleLabelBCID(TH1* h){
  h->GetXaxis()->SetTitle("BC");
  h->GetYaxis()->SetTitle("Counts");
  h->SetMinimum(0.);

  h->GetXaxis()->SetBinLabel(1,"Previous"); 
  h->GetXaxis()->SetBinLabel(2,"Current"); 
  h->GetXaxis()->SetBinLabel(3,"Next"); 

}
void
TgcLv1RawDataValAlg::setTH2TitleLabelBCID(TH2* h){
  h->GetXaxis()->SetTitle("SL Timing");
  h->GetYaxis()->SetTitle("Lpt Timing");
  h->SetMinimum(0.);
      
  h->GetXaxis()->SetBinLabel(1,"Previous"); 
  h->GetXaxis()->SetBinLabel(2,"Current"); 
  h->GetXaxis()->SetBinLabel(3,"Next"); 

  h->GetYaxis()->SetBinLabel(1,"Previous"); 
  h->GetYaxis()->SetBinLabel(2,"Current"); 
  h->GetYaxis()->SetBinLabel(3,"Next"); 

}

///////////////////////////////////////////////////////////////////////////
// Calculate deltaR between two eta-phi positions
float
TgcLv1RawDataValAlg::deltaR(float eta1, float phi1, float eta2, float phi2){
  float deta = eta1 - eta2;
  float dphi = acos(cos(phi1-phi2));

  return sqrt( deta*deta + dphi*dphi );
}

///////////////////////////////////////////////////////////////////////////
// Get RoI from tracklet wire&strip id and subMatrix, FE and phi48 positions
int
TgcLv1RawDataValAlg::trackletIdSub2RoI(int tidw, int subw, int tids, int subs, bool isForward, int phi){
  //Endcap Wire     : trackletID  1-19 (tew)
  //Endcap Strip    : trackletID 32-41 (tes)
  //Forward-0 Wire  : trackletID  0- 7 (tfw0)
  //Forward-1 Wire  : trackletID 16-23 (tfw1)
  //Forward-0 Strip : trackletID 32,33 (tfs0)
  //Forward-1 Strip : trackletID 48,49 (tfs1)

  if( isForward==false ){//Endcap
    int tew = tidw;
    int tes = tids;
    int roi = (2*(tew-1 ) + subw)*4 + 2*((tes-32)%2) + subs - 4;
    return roi;
  }else if(phi%2==0){//Forward odd stationPhi (phi=stationPhi-1)
    int tfw1 = tidw;
    int tfs1 = tids;
    int roi = (2*(tfw1-16)+ subw)*4 + 2*(tfs1%2)     + subs;
    return roi;
  }else{             //Forward even stationPhi (phi=stationPhi-1)
    int tfw0 = tidw;
    int tfs0 = tids;
    int roi = (2*tfw0     + subw)*4 + 2*(tfs0%2)     + subs;
    return roi;
  }

  return -1;
}

///////////////////////////////////////////////////////////////////////////
// Put blank gray box over histogram area
void
TgcLv1RawDataValAlg::putBox(TH2* h, float x1, float y1, float x2, float y2){
  TBox* box = new TBox(x1, y1, x2, y2);
  //  box->SetFillColor(kGray+1);
  //  box->SetLineColor(kGray+1);
  //  box->SetFillStyle(3013);
  //  box->SetFillStyle(3004);

  box->SetFillColor(kGray);
  box->SetLineColor(kGray);

  //  box->SetLineStyle(3);

  (h->GetListOfFunctions())->Add(box);    
  //  delete box; // will cause crash or not save box // is deleted along with h
}
void
TgcLv1RawDataValAlg::BlankPhi24(TH2 *h, int binx){
  float x1=h->GetXaxis()->GetBinLowEdge(binx);
  float x2=h->GetXaxis()->GetBinUpEdge(binx);
  for(int i=0;i<24;i++){
    int biny = (i+1)*2;
    float y1=h->GetYaxis()->GetBinLowEdge(biny);
    float y2=h->GetYaxis()->GetBinUpEdge(biny);
    putBox(h, x1, y1, x2, y2);
  }
}
void
TgcLv1RawDataValAlg::BlankStationMap(TH2 *h){
  BlankPhi24(h, 5);
  BlankPhi24(h, 10);
  BlankPhi24(h, 15);
  BlankPhi24(h, 21);
  BlankPhi24(h, 27);
  BlankPhi24(h, 33);
  BlankPhi24(h, 39);
  BlankPhi24(h, 40);
  BlankPhi24(h, 41);
  BlankPhi24(h, 42);
  BlankPhi24(h, 43);
  
  int x1=h->GetXaxis()->GetBinLowEdge(40);
  int x2=h->GetXaxis()->GetBinUpEdge(40);
  int x3=h->GetXaxis()->GetBinLowEdge(42);
  int x4=h->GetXaxis()->GetBinUpEdge(42);
  int y1=h->GetYaxis()->GetBinLowEdge(19);
  int y2=h->GetYaxis()->GetBinUpEdge(19);
  putBox(h, x1, y1, x2, y2);
  putBox(h, x3, y1, x4, y2);
  y1=h->GetYaxis()->GetBinLowEdge(35);
  y2=h->GetYaxis()->GetBinUpEdge(35);
  putBox(h, x1, y1, x2, y2);
  putBox(h, x3, y1, x4, y2);
  y1=h->GetYaxis()->GetBinLowEdge(43);
  y2=h->GetYaxis()->GetBinUpEdge(43);
  putBox(h, x1, y1, x2, y2);
  putBox(h, x3, y1, x4, y2);
}



