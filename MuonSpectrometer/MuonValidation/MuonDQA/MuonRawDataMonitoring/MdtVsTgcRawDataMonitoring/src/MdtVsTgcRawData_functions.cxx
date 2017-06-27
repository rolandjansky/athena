/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package : MdtVsTgcRawDataValAlg
// Authors:  A. Ishikawa(Kobe)
// Jun. 2008
//
// DESCRIPTION:
// Subject: correlation btw MDT hits vs TGC RoI -->Offline Muon Data Quality
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

#include "Identifier/Identifier.h"

//mdt stuff
#include "MuonCalibIdentifier/MuonFixedId.h"
 
#include "MdtVsTgcRawDataMonitoring/MdtVsTgcRawDataValAlg.h"

#include <TList.h>
#include <TBox.h>
#include <TMath.h>
#include <inttypes.h> 

using namespace std;


void
MdtVsTgcRawDataValAlg::roi2etaphi(const Muon::TgcCoinData& cd, int& eta, int& phi){
  
  int roiphi=cd.phi();//phi in 1/48(24) sector for endcap(forward)
  int roi=cd.roi();//0-147(0-63)

  //int localeta=roi/4;

  if(cd.isForward()){//forward

    int localphi=roi%4;

    //eta=roi/4;
    eta=roi/4+36;
    phi=(localphi+roiphi*4)*2;

  }else{//endcap

    int sector=phi2sector(roiphi,1);

    phi=roiphi*4 + ( sector%2==0 ? roi%4 : 3-(roi%4)   );
    eta=roi/4;
  }

  //eta=52-eta;
  return;

}

int
MdtVsTgcRawDataValAlg::phi2sector(int phi, int ef){
  int sector=-1;
  if(ef==0){//forward
    sector=phi/2+1;//(phi,sector)= (1,1), (2,2), (3,2), (4,3)
  }else{//endcap
    sector=(phi+1)/4+1;//(phi,sector)= (1,1), (2,1), (3,2), (4,2)
  }
  if(sector>12)sector=1;

  return sector;
}

int
MdtVsTgcRawDataValAlg::roiphi2mdtSector(int roiphi, int ef){
  int mdtSector=-1;
  if(ef==0){//forward phi1-24
    mdtSector=roiphi/2+1;//(phi,sector)= (1,1), (2,2), (3,2), (4,3)
  }else{//endcap
    mdtSector=(roiphi+1)/4+1;//(phi,sector)= (1,1), (2,1), (3,2), (4,2)
  }
  if(mdtSector>8)mdtSector=1;

  return mdtSector;
}

int
MdtVsTgcRawDataValAlg::numberOfSL(const Muon::TgcCoinDataContainer* tgctrgcontainer){

  int nSL=0;

  //loop over TGC RoI container
  Muon::TgcCoinDataContainer::const_iterator it_end=tgctrgcontainer->end();
  for( Muon::TgcCoinDataContainer::const_iterator it=tgctrgcontainer->begin();
       it!=it_end;
       ++it){
  

    ATH_MSG_DEBUG( "size of tgc collection is " << (*it) -> size()  );

    //loop over TGC RoI collection
    Muon::TgcCoinDataCollection::const_iterator itc_end=(*it)->end();
    for( Muon::TgcCoinDataCollection::const_iterator itc=(*it)->begin();
         itc!= itc_end;
         ++itc){

      const Muon::TgcCoinData* tcd=*itc;

      if( tcd->type() != Muon::TgcCoinData::TYPE_SL )continue;
      ATH_MSG_DEBUG("pt"<<tcd->pt() );
      nSL++;
    }
  }

  return nSL;
}

int
MdtVsTgcRawDataValAlg::stationGasGap2layer(int station, int GasGap){
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
// Put blank gray box over histogram area
void
MdtVsTgcRawDataValAlg::putBox(TH2* h, float x1, float y1, float x2, float y2){
  TBox* box = new TBox(x1, y1, x2, y2);
//  box->SetFillColor(kGray+1);
//  box->SetLineColor(kGray+1);
//  box->SetFillStyle(3013);
//  box->SetFillStyle(3004);

  box->SetFillColor(kGray);
  box->SetLineColor(kGray);

//  box->SetLineStyle(3);

  (h->GetListOfFunctions())->Add(box);    
}
void
MdtVsTgcRawDataValAlg::BlankPhi24(TH2 *h, int binx){
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
MdtVsTgcRawDataValAlg::BlankStationMap(TH2 *h, int ws){
  bool rebin=true;
  if(rebin){//use new bin
      for(int i=33; i<44;i++)BlankPhi24(h,i);
      int x1=h->GetXaxis()->GetBinLowEdge(40);
      int x2=h->GetXaxis()->GetBinUpEdge(40);
      int x3=h->GetXaxis()->GetBinLowEdge(41);
      int x4=h->GetXaxis()->GetBinUpEdge(41);
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
      
      if(ws==1){//Strip
        x1=h->GetXaxis()->GetBinLowEdge(2);
        y1=h->GetYaxis()->GetBinLowEdge(1);				
        x2=h->GetXaxis()->GetBinUpEdge(2); 				
        y2=h->GetYaxis()->GetBinUpEdge(48); 				
        putBox(h, x1, y1, x2, y2);          				
        x1=h->GetXaxis()->GetBinLowEdge(9);			
        x2=h->GetXaxis()->GetBinUpEdge(9); 			
        putBox(h, x1, y1, x2, y2);          			
        x1=h->GetXaxis()->GetBinLowEdge(16);
        x2=h->GetXaxis()->GetBinUpEdge(16);
        putBox(h, x1, y1, x2, y2);
        x1=h->GetXaxis()->GetBinLowEdge(23);
        x2=h->GetXaxis()->GetBinUpEdge(23); 			
        putBox(h, x1, y1, x2, y2);          			
        x1=h->GetXaxis()->GetBinLowEdge(34);
        x2=h->GetXaxis()->GetBinUpEdge(34);
        putBox(h, x1, y1, x2, y2);
      }
  }else{//use old bin, logically dead code, comment out
//    BlankPhi24(h, 5);
//    BlankPhi24(h, 10);
//    BlankPhi24(h, 15);
//    BlankPhi24(h, 21);
//    BlankPhi24(h, 27);
//    BlankPhi24(h, 33);
//    BlankPhi24(h, 39);
//    BlankPhi24(h, 40);
//    BlankPhi24(h, 41);
//    BlankPhi24(h, 42);
//    BlankPhi24(h, 43);
//    int x1=h->GetXaxis()->GetBinLowEdge(40);
//    int x2=h->GetXaxis()->GetBinUpEdge(40);
//    int x3=h->GetXaxis()->GetBinLowEdge(42);
//    int x4=h->GetXaxis()->GetBinUpEdge(42);
//    int y1=h->GetYaxis()->GetBinLowEdge(19);
//    int y2=h->GetYaxis()->GetBinUpEdge(19);
//    putBox(h, x1, y1, x2, y2);
//    putBox(h, x3, y1, x4, y2);
//    y1=h->GetYaxis()->GetBinLowEdge(35);
//    y2=h->GetYaxis()->GetBinUpEdge(35);
//    putBox(h, x1, y1, x2, y2);
//    putBox(h, x3, y1, x4, y2);
//    y1=h->GetYaxis()->GetBinLowEdge(43);
//    y2=h->GetYaxis()->GetBinUpEdge(43);
//    putBox(h, x1, y1, x2, y2);
//    putBox(h, x3, y1, x4, y2);
  }
}
