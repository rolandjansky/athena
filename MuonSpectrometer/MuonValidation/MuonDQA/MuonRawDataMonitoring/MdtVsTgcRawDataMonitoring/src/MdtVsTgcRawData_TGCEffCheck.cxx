/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////////////
// Package : MdtVsTgcRawDataValAlg
// Author:   M.King(Kobe)
// Feb.  2011
//
// DESCRIPTION:
// Subject: TGC Efficiency     -->TGC Efficiency plots including EIFI by comparing with MDT Segments
///////////////////////////////////////////////////////////////////////////////////////////// 

#include "MdtVsTgcRawDataMonitoring/MdtVsTgcRawDataValAlg.h"

#include "TrkEventPrimitives/TrkEventPrimitivesDict.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "muonEvent/MuonContainer.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
//use new MDT segment container 
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/MuonSegment.h"

#include <TH2.h>
#include <TList.h>
#include <inttypes.h>

#include <sstream>
#include <algorithm>
#include <fstream>

// Function to generate TGC efficiency histograms
// Selects valid segments and attempts to match them into a track, then looks for TGC hits in the various layers
//New to calculate the efficiency
void
MdtVsTgcRawDataValAlg::tgceffcalc(const xAOD::MuonSegmentContainer *newmdtsegment,
                                  const Muon::TgcPrepDataContainer *tgc_prepcontainer){
  ATH_MSG_DEBUG("inside tgcEIFIeffcalc" );
  //////////////////////////////////////////////////////
  // Declare vector arrays to hold segment pointers

  // Holds Segments sorted into MDT Stations on each side
  std::vector<const Muon::MuonSegment*> sortedSegments[2][4];           //[AC][MDTStation]
  
  // Holds Segments which have been disqualified, any segments in this array are ignored when looping through sortedSegments
  std::vector<const Muon::MuonSegment*> DQdisqualifiedSegments[2][4];   //[AC][MDTStation] // Segments which have been disqualified by DQ
  std::vector<const Muon::MuonSegment*> MATCHdisqualifiedSegments[2][4];//[AC][MDTStation] // Segments which have been disqualified by DQ or already been included in a track
  
  // Holds Segments which have been matched into a track
  std::vector<SegmTrack> matchedSegments[2];             //[AC]
  
  
  //////////////////////////////////////////////////////
  // Sort and filter Segments
  
  // Sort Segments from segmcollection into correct bin in sortedSegments array
  SortMDTSegments(newmdtsegment, sortedSegments);
  // Disqualify Segments with bad DQ
  DQCheckMDTSegments(sortedSegments, DQdisqualifiedSegments);
  for(int i=0;i<2;i++){
    for(int jMDT=0;jMDT<4;jMDT++){
      MATCHdisqualifiedSegments[i][jMDT] = DQdisqualifiedSegments[i][jMDT];
    }
  }
  
  
  //////////////////////////////////////////////////////
  // Segment Track Method
  // Match up Segments into tracks
  MatchMDTSegments(sortedSegments, MATCHdisqualifiedSegments, matchedSegments);
  // Use tracks to look for TGC hits
  CheckTGConTrack(matchedSegments, tgc_prepcontainer);

  
  //////////////////////////////////////////////////////
  // Midstation-only Method
  
  // Use segments to check Midstation again
  MidstationOnlyCheck(sortedSegments, DQdisqualifiedSegments, tgc_prepcontainer);
  
  return;
}// End of function

// Prepare array of TGC Readout Elements
void
MdtVsTgcRawDataValAlg::prepareTREarray(const MuonGM::MuonDetectorManager* MuonDetMgrDS){
  int TGCStationNames[8]={41, 42, 43, 44, 45, 46, 47, 48};
  
  // Make array of TGC Readout Element pointers
  for(int stationNameIndex=0; stationNameIndex<8; stationNameIndex++){// Station Name
    int stationName = TGCStationNames[stationNameIndex];
    for(int stationEta=-8; stationEta<=8; stationEta++){// Station Eta
      int tgcAC(stationEta<0);
      int absStationEta = std::abs(stationEta);
      for(int stationPhi=0; stationPhi<=48; stationPhi++){// Station Phi
        // Exclude non-existent "zero" sectors included in the array for ease of use
        if(stationEta==0){
          m_TREarray[stationNameIndex][0][absStationEta][stationPhi]=0;
          m_TREarray[stationNameIndex][1][absStationEta][stationPhi]=0;
          continue;
        }
        m_TREarray[stationNameIndex][tgcAC][absStationEta][stationPhi]=0;
        if(stationPhi==0)continue;
        
        // Exclude sectors known not to exist
        if(stationNameIndex==6){         // Inner Forward
          if(std::abs(stationEta)>1)continue;
          if(stationPhi>24)continue;
        }
        else if(stationNameIndex==7){    // Inner Endcap
          if(std::abs(stationEta)>1)continue;
          if(stationPhi>21)continue;
        }
        else if((stationNameIndex==0)||  // Midstation Forward
                (stationNameIndex==2)||
                (stationNameIndex==4)){
          if(std::abs(stationEta)>1)continue;
          if(stationPhi>24)continue;
        }
        else{                            // Midstation Endcap
          if(std::abs(stationEta)>5)continue;
          if((stationNameIndex==1)&&
             (std::abs(stationEta)>4))continue;
        } 
        
        // Get identifier of TRE at this set of indexes
        bool *isValid = new bool(true);
        Identifier tgc_testId = m_idHelperSvc->tgcIdHelper().elementID(stationName, stationEta, stationPhi, true, isValid);
        if(!*isValid){delete isValid; continue;}
        delete isValid;
        
        // Get TRE and put into to array
        m_TREarray[stationNameIndex][tgcAC][absStationEta][stationPhi] = MuonDetMgrDS->getTgcReadoutElement(tgc_testId);
        if(m_TREarray[stationNameIndex][tgcAC][absStationEta][stationPhi]==0){
          ATH_MSG_WARNING( "prepareTREarray: TgcReadoutElement==0 passed checks"  );
          continue;
        }
      }// Station Phi
    }// Station Eta
  }// Station Name
  return;
}// End of function

// Finalize histograms which need post processing
void
MdtVsTgcRawDataValAlg::tgceffcalcfinalize(){
  int   beff, bdenom, berror;
  float feff, fdenom;
  for(int i=0;i<2;i++){// AC
    // Station Coordinate Efficiency Histograms
    for(int k=0;k<2;k++){// WireStrip
      // Loop Numerator and Denominator, total up histograms
      for(int e=1;e<3;e++){
        TList histlist;
        histlist.Add(m_eff_stationmapbase[i][k][e]);
        histlist.Add(m_eff_stationmapmid[i][k][e]);
        m_eff_stationmap[i][k][e]->Merge(&histlist);
      }
      
      const int nhtypes = 3;
      // Make array of pointers to different efficiency histogram types
      TH2 *histarray[nhtypes][4] = {{  m_eff_stationmapbase[i][k][0],  m_eff_stationmapbase[i][k][1],  m_eff_stationmapbase[i][k][2],  m_eff_stationmapbase[i][k][3]},
                                    {   m_eff_stationmapmid[i][k][0],   m_eff_stationmapmid[i][k][1],   m_eff_stationmapmid[i][k][2],   m_eff_stationmapmid[i][k][3]},
                                    {      m_eff_stationmap[i][k][0],      m_eff_stationmap[i][k][1],      m_eff_stationmap[i][k][2],      m_eff_stationmap[i][k][3]}};
      for(int h=0;h<nhtypes;h++){
        // Calculate Efficiency
        histarray[h][0]->Divide(histarray[h][1], histarray[h][2]);
        
        // Calculate Error
        int nX=histarray[h][3]->GetNbinsX();
        int nY=histarray[h][3]->GetNbinsY();
        for(int x=1;x<=nX;x++){
          for(int y=1;y<=nY;y++){
            beff  =histarray[h][0]->GetBin(x,y);
            bdenom=histarray[h][2]->GetBin(x,y);
            berror=histarray[h][3]->GetBin(x,y);
            
            feff  =histarray[h][0]->GetBinContent(beff);
            fdenom=histarray[h][2]->GetBinContent(bdenom);
            
            float result = 0;
            if(fdenom>0){
              result=sqrt(feff*(1-feff)/fdenom);
            }
            histarray[h][3]->SetBinContent(berror,result);
          }// nY Bins
        }// nX Bins
      }
    }// WireStrip
  }// AC
  
  return;
}// End of function

// Get TGC layer number from stationName and gasgap values
int
MdtVsTgcRawDataValAlg::TGCgetlayer(int stationName, int g){
  if(g<1){
    ATH_MSG_WARNING( "TGCgetlayer passed invalid gasgap g=" << g  );
    return -1;
  }
  int l = g-1;
  if(stationName==41||stationName==42){
    if(g>3){
      ATH_MSG_WARNING( "TGCgetlayer passed invalid gasgap and stationName combination n=" << stationName << " g=" << g  );
      return -1;
    }
  }else if(stationName==43||stationName==44){
    if(g>2){
      ATH_MSG_WARNING( "TGCgetlayer passed invalid gasgap and stationName combination n=" << stationName << " g=" << g  );
      return -1;
    }
    l+=3;
  }else if(stationName==45||stationName==46){
    if(g>2){
      ATH_MSG_WARNING( "TGCgetlayer passed invalid gasgap and stationName combination n=" << stationName << " g=" << g  );
      return -1;
    }
    l+=5;
  }else if(stationName==47||stationName==48){
    if(g>2){
      ATH_MSG_WARNING( "TGCgetlayer passed invalid gasgap and stationName combination n=" << stationName << " g=" << g  );
      return -1;
    }
    l+=7;
  }else{
    ATH_MSG_WARNING( "TGCgetlayer passed invalid stationName n=" << stationName  );
    return -1;
  }
  return l;
}// End of function

// Get stationIndex from layer number
int
MdtVsTgcRawDataValAlg::TGClayer2stationindex(int l){
  if(l==0||l==1||l==2)return 0;
  else if(l==3||l==4)return 1;
  else if(l==5||l==6)return 2;
  else if(l==7||l==8)return 3;
  else{
    ATH_MSG_WARNING( "TGClayer2Station passed invalid layer number:" << l  );
    return -1;
  }
}// End of function
// Get stationIndex from stationName
int
MdtVsTgcRawDataValAlg::TGCstationname2stationindex(int stationName){
  if(stationName==41||stationName==42)return 0;
  else if(stationName==43||stationName==44)return 1;
  else if(stationName==45||stationName==46)return 2;
  else if(stationName==47||stationName==48)return 3;
  else{
    ATH_MSG_WARNING( "TGCstationname2stationindex passed invalid stationName n=" << stationName  );
    return -1;
  }
}// End of function

// Get Eta or Phi index for the StationMap histograms from TRE variables
int
MdtVsTgcRawDataValAlg::getStationMapIndex(int x, int l, int stationFE, int stationEta, int stationPhi){
  // Display error messages if invalid TRE variables are passed in
  if((stationFE!=0)&&(stationFE!=1)) ATH_MSG_WARNING( "getStationMapIndex passed invalid stationFE=" << stationFE  );
  if((l<0)||(l>8)) ATH_MSG_WARNING( "getStationMapIndex passed invalid layer index l=" << l  );
  if(stationEta<1) ATH_MSG_WARNING( "getStationMapIndex passed invalid stationEta=" << stationEta  );
  if(stationPhi<1) ATH_MSG_WARNING( "getStationMapIndex passed invalid stationPhi=" << stationPhi  );
  int index=0;
  switch(x){
   case 1:// Getting Eta Index //use old eta bin
    if(l==0||l==1||l==2){// T1
      if(stationEta>4) ATH_MSG_WARNING( "getStationMapIndex(" << x << ") passed invalid l=" << l << " stationEta=" << stationEta  );
      if(stationFE==0)index=32+l;
      else{ 
      	index=4-stationEta;
        index=index*7+l;
			}
    }
    else if(l==3||l==4){// T2
      if(stationEta>5) ATH_MSG_WARNING( "getStationMapIndex(" << x << ") passed invalid l=" << l << " stationEta=" << stationEta  );
      if(stationFE==0)index=32+l;
      else {
        index=5-stationEta;
        index=index*7+l;
        if(stationEta==1)index=25+l;
      }
    }
    else if(l==5||l==6){// T3
      if(stationEta>5) ATH_MSG_WARNING( "getStationMapIndex(" << x << ") passed invalid l=" << l << " stationEta=" << stationEta  );
      if(stationFE==0)index=32+l;
      else{
        index=5-stationEta;
        index=index*7+l;
        if(stationEta==1)index=25+l;
      }
    }
    else if(l==7||l==8){// T4
      if(stationEta>1) ATH_MSG_WARNING( "getStationMapIndex(" << x << ") passed invalid l=" << l << " stationEta=" << stationEta  );
      if(stationFE==0){
        if(l==7){index=41;}
        else if(l==8){index=42;}
      }else{
        if(l==7){index=39;}
        else if(l==8){index=40;}
      }
    }
    break;
  case 2:// Getting Phi Index
    if(stationFE==0){// Forward
      if((l==7)||(l==8)){// FI
        if(stationPhi>24) ATH_MSG_WARNING( "getStationMapIndex(" << x << ") passed invalid l=" << l << " FE=" << stationFE << " stationPhi=" << stationPhi  );
        index=(stationPhi-1)*2;
      }
      else{// Forward Midstation
        if(stationPhi>24) ATH_MSG_WARNING( "getStationMapIndex(" << x << ") passed invalid l=" << l << " FE=" << stationFE << " stationPhi=" << stationPhi  );
        index=(stationPhi-1)*2;
      }
    }
    else{// Endcap
      if((l==7)||(l==8)){// EI
        if(stationPhi>21) ATH_MSG_WARNING( "getStationMapIndex(" << x << ") passed invalid l=" << l << " FE=" << stationFE << " stationPhi=" << stationPhi  );
        index=(stationPhi-1);
        if(index>7)index++;
        if(index>15)index++;
        if(index>19)index++;
        index*=2;
      }
      else{// Endcap Midstation
        if(stationPhi>48) ATH_MSG_WARNING( "getStationMapIndex(" << x << ") passed invalid l=" << l << " FE=" << stationFE << " stationPhi=" << stationPhi  );
        index=stationPhi-1;
      }
    }
    // Adjust to set A01phi0(stationPhi=47 for Midstation)at 0
    index+=2;
    if(index>47)index-=48;
    break;
  default:
    ATH_MSG_WARNING( "getStationMapIndex(" << x << ") is invalid"  );
    break;
  }
  return index;
}// End of function
