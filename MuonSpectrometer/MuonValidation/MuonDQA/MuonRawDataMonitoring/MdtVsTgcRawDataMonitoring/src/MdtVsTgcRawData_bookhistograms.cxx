/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include <TH1F.h>
#include <TH2F.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>

#include <sstream>
#include <algorithm>
#include <fstream>

StatusCode
MdtVsTgcRawDataValAlg::bookmaphists(MonGroup &mdtvstgclv1_expert_a,
                                    MonGroup &mdtvstgclv1_expert_c){
  StatusCode sc = StatusCode::SUCCESS; 
  
  std::stringstream sst;
  std::string AC[2]=          {"A","C"};
  int   nbins3D_1[4]=         { 0, 0, 0, 0};
  float fGlobalCoords3Dlo[4]= { 0, 0, 0, 0};
  float fGlobalCoords3Dup[4]= { 0, 0, 0, 0};
  // **************************************************************
  // ** maphists
  // **************************************************************
  ////////////////////////////////////////////////////////////////
  // Number of~ hists
  nbins3D_1[0]=              480; nbins3D_1[1]=              120; nbins3D_1[2]=              128; nbins3D_1[3]=              100;
  fGlobalCoords3Dlo[0]=        0; fGlobalCoords3Dlo[1]=        0; fGlobalCoords3Dlo[2]=        0; fGlobalCoords3Dlo[3]=        0;
  fGlobalCoords3Dup[0]=    12000; fGlobalCoords3Dup[1]=        3; fGlobalCoords3Dup[2]=   2*M_PI; fGlobalCoords3Dup[3]= 0.5*M_PI;
  for(int i=0;i<2;i++){// AC
    // nEvents passing cut criteria
    sst<<"EffCheck_CutsPassed_"<<AC[i];
    m_log<<MSG::DEBUG<<"EffCheck_CutsPassed "<<i<<" "<<sst.str().c_str()<<std::endl;
    mvt_cutspassed[i] =new TH1I(sst.str().c_str(), sst.str().c_str(), 14, 1, 15);
    if(i==0){
      sc=mdtvstgclv1_expert_a.regHist(mvt_cutspassed[i]);
    }else{
      sc=mdtvstgclv1_expert_c.regHist(mvt_cutspassed[i]);
    }
    if(sc.isFailure()){
      m_log << MSG::FATAL << "mvt_cutspassed["<<i<<"] Failed to register histogram " << endreq;
      return sc;
    }
    mvt_cutspassed[i]->GetYaxis()->SetTitle("nEvents");
    mvt_cutspassed[i]->GetXaxis()->SetBinLabel(1, "Events");
    mvt_cutspassed[i]->GetXaxis()->SetBinLabel(2, "matchedSegm=0");
    mvt_cutspassed[i]->GetXaxis()->SetBinLabel(3, "matchedSegm>1");
    mvt_cutspassed[i]->GetXaxis()->SetBinLabel(4, "matchedSegm=1");
    mvt_cutspassed[i]->GetXaxis()->SetBinLabel(5, "+HasEIFI");
    mvt_cutspassed[i]->GetXaxis()->SetBinLabel(6, "EIFIMatch");
    mvt_cutspassed[i]->GetXaxis()->SetBinLabel(7, "+HasT1");
    mvt_cutspassed[i]->GetXaxis()->SetBinLabel(8, "T1Match");
    mvt_cutspassed[i]->GetXaxis()->SetBinLabel(9,"+HasT2");
    mvt_cutspassed[i]->GetXaxis()->SetBinLabel(10,"T2Match");
    mvt_cutspassed[i]->GetXaxis()->SetBinLabel(11,"+HasT3");
    mvt_cutspassed[i]->GetXaxis()->SetBinLabel(12,"T3Match");
    mvt_cutspassed[i]->GetXaxis()->SetBinLabel(13,"AllStationsMatch");
    mvt_cutspassed[i]->GetXaxis()->SetBinLabel(14,"AllLayersMatch");
    sst.str("");
  }// AC
  
  
  ////////////////////////////////////////////////////////////////
  // Distributions
  nbins3D_1[0]=              600; nbins3D_1[1]=              120; nbins3D_1[2]=              128; nbins3D_1[3]=              128;
  fGlobalCoords3Dlo[0]=        0; fGlobalCoords3Dlo[1]=        0; fGlobalCoords3Dlo[2]=        0; fGlobalCoords3Dlo[3]=        0;
  fGlobalCoords3Dup[0]=    12000; fGlobalCoords3Dup[1]=        3; fGlobalCoords3Dup[2]=   2*M_PI; fGlobalCoords3Dup[3]= 0.5*M_PI;
  for(int i=0;i<2;i++){// AC
    for(int jMDT=0;jMDT<4;jMDT++){// MDT Station
      // MDT Segm Eta vs Phi
      sst<<"MDT_SegmMap_MDT"<<jMDT+1;
      sst<<"_"<<AC[i];
      m_log<<MSG::DEBUG<<"MDT_SegmMap "<<i<<" "<<jMDT<<" "<<sst.str().c_str()<<std::endl;
      mdt_segmmap[i][jMDT] =new TH2F(sst.str().c_str(),sst.str().c_str(), nbins3D_1[2], fGlobalCoords3Dlo[2], fGlobalCoords3Dup[2], nbins3D_1[1], fGlobalCoords3Dlo[1], fGlobalCoords3Dup[1]);
      if(i==0){
        sc=mdtvstgclv1_expert_a.regHist(mdt_segmmap[i][jMDT]);
      }else{
        sc=mdtvstgclv1_expert_c.regHist(mdt_segmmap[i][jMDT]);
      }
      if(sc.isFailure()){
        m_log << MSG::FATAL << "mdt_segmmap["<<i<<"]["<<jMDT<<"] Failed to register histogram " << endreq;
        return sc;
      }
      mdt_segmmap[i][jMDT]->GetXaxis()->SetTitle("MDT Phi");
      mdt_segmmap[i][jMDT]->GetYaxis()->SetTitle("MDT Eta");
      sst.str("");
    }// MDT Station
  }// AC
  return sc;
}

StatusCode
MdtVsTgcRawDataValAlg::bookeffhists(MonGroup &mdtvstgclv1_expert_a,
                                    MonGroup &mdtvstgclv1_expert_c){
  StatusCode sc = StatusCode::SUCCESS; 
  
  std::stringstream sst;
  std::string AC[2]=          {"A","C"};
  std::string RhoEtaPhiThe[4]={"Rho","Eta","Phi","Theta"};
  std::string RhoEtaPhiZ[4]=  {"Rho","Eta","Phi","Z"};
  std::string FE[2]=          {"F","E"};// Forward, Endcap
  std::string EffNumDenom[4]= {"","num","denom","error"};
  std::string WireStrip[2]=   {"Wire","Strip"};
  int   nbins3D_1[4]=         { 0, 0, 0, 0};
  float fGlobalCoords3Dlo[4]= { 0, 0, 0, 0};
  float fGlobalCoords3Dup[4]= { 0, 0, 0, 0};
  // **************************************************************
  // ** EffCheck
  // **************************************************************
  ////////////////////////////////////////////////////////////////
  // Efficiencies
  nbins3D_1[0]=              100; nbins3D_1[1]=                8; nbins3D_1[2]=               48; nbins3D_1[3]=              100;
  fGlobalCoords3Dlo[0]=    -1000; fGlobalCoords3Dlo[1]=       -1; fGlobalCoords3Dlo[2]=    -M_PI; fGlobalCoords3Dlo[3]=        0;
  fGlobalCoords3Dup[0]=     1000; fGlobalCoords3Dup[1]=        1; fGlobalCoords3Dup[2]=     M_PI; fGlobalCoords3Dup[3]= 0.5*M_PI;
  for(int i=0;i<2;i++){// AC
    for(int k=0;k<2;k++){// WireStrip
      for(int e=0;e<4;e++){// EffNumDenom
        // Station Efficiency Map Segm Track
        sst<<WireStrip[k];
        sst<<"_EfficiencyAgainstMDT";
        sst<<"_MapSegmTrack"<<EffNumDenom[e];
        sst<<"_"<<AC[i];
        m_log<<MSG::DEBUG<<"Eff_StationMapBase "<<i<<" "<<k<<" "<<e<<sst.str().c_str()<<std::endl;
        eff_stationmapbase[i][k][e]=new TH2F( sst.str().c_str(), sst.str().c_str(),43, 0, 43, 48, 0, 48 );
        if(i==0){
          sc=mdtvstgclv1_expert_a.regHist(eff_stationmapbase[i][k][e]);
        }else{
          sc=mdtvstgclv1_expert_c.regHist(eff_stationmapbase[i][k][e]);
        }
        if(sc.isFailure()){
          m_log << MSG::FATAL << "eff_stationmapbase["<<i<<"]["<<k<<"]["<<e<<"] Failed to register histogram " << endreq;
          return sc;
        }
        labelStationMap(eff_stationmapbase[i][k][e], i,k);
        sst.str("");
        
        // Station Efficiency Map Midstation Segments
        sst<<WireStrip[k];
        sst<<"_EfficiencyAgainstMDT";
        sst<<"_MapMidOnly"<<EffNumDenom[e];
        sst<<"_"<<AC[i];
        m_log<<MSG::DEBUG<<"Eff_StationMapMid "<<i<<" "<<k<<" "<<e<<sst.str().c_str()<<std::endl;
        eff_stationmapmid[i][k][e]=new TH2F( sst.str().c_str(), sst.str().c_str(),43, 0, 43, 48, 0, 48 );
        if(i==0){
          sc=mdtvstgclv1_expert_a.regHist(eff_stationmapmid[i][k][e]);
        }else{
          sc=mdtvstgclv1_expert_c.regHist(eff_stationmapmid[i][k][e]);
        }
        if(sc.isFailure()){
          m_log << MSG::FATAL << "eff_stationmapmid["<<i<<"]["<<k<<"]["<<e<<"] Failed to register histogram " << endreq;
          return sc;
        }
        labelStationMap(eff_stationmapmid[i][k][e], i,k);
        sst.str("");
        
        // Station Efficiency Map Total
        sst<<WireStrip[k];
        sst<<"_EfficiencyAgainstMDT";
        sst<<"_Map"<<EffNumDenom[e];
        sst<<"_"<<AC[i];
        m_log<<MSG::DEBUG<<"Eff_StationMap "<<i<<" "<<k<<" "<<e<<sst.str().c_str()<<std::endl;
        eff_stationmap[i][k][e]=new TH2F( sst.str().c_str(), sst.str().c_str(),43, 0, 43, 48, 0, 48 );
        if(i==0){
          sc=mdtvstgclv1_expert_a.regHist(eff_stationmap[i][k][e]);
        }else{
          sc=mdtvstgclv1_expert_c.regHist(eff_stationmap[i][k][e]);
        }
        if(sc.isFailure()){
          m_log << MSG::FATAL << "eff_stationmap["<<i<<"]["<<k<<"]["<<e<<"] Failed to register histogram " << endreq;
          return sc;
        }
        labelStationMap(eff_stationmap[i][k][e], i,k);
        sst.str("");
      }// EffNumDenom
    }// WireStrip
  }// AC
  
  ////////////////////////////////////////////////////////////////
  // Sagittas
  
  
  nbins3D_1[0]=             8000; nbins3D_1[1]=              200; nbins3D_1[2]=             4096; nbins3D_1[3]=            40000;
  fGlobalCoords3Dlo[0]=    -8000; fGlobalCoords3Dlo[1]=       -1; fGlobalCoords3Dlo[2]=    -M_PI; fGlobalCoords3Dlo[3]=  -200000;
  fGlobalCoords3Dup[0]=     8000; fGlobalCoords3Dup[1]=        1; fGlobalCoords3Dup[2]=     M_PI; fGlobalCoords3Dup[3]=   200000;
  for(int i=0;i<2;i++){// AC
    for(int jTGC=0;jTGC<4;jTGC++){// TGC Station
      for(int f=0;f<2;f++){// FE
        for(int k=0;k<2;k++){// WireStrip
          for(int x=0;x<4;x++){// RhoEtaPhi
            if(x==0||x==2){ // Only interested in Rho and Phi
              // TGC EIFI vs MDT extrapolated position, Sagitta
              sst<<"TGC_SegmTrack_"<<RhoEtaPhiZ[x];
              sst<<"Sagitta_T"<<jTGC+1;
              sst<<FE[f];
              sst<<WireStrip[k];
              sst<<"_"<<AC[i];
              m_log<<MSG::DEBUG<<"TGC_Sagitta "<<i<<" "<<jTGC<<" "<<f<<" "<<k<<" "<<x<<" "<<sst.str().c_str()<<std::endl;
              mvt_extrprdsag[i][jTGC][f][k][x] =new TH1F(sst.str().c_str(), sst.str().c_str(), nbins3D_1[x],fGlobalCoords3Dlo[x],fGlobalCoords3Dup[x]);
              if(i==0){
                sc=mdtvstgclv1_expert_a.regHist(mvt_extrprdsag[i][jTGC][f][k][x]);
              }else{
                sc=mdtvstgclv1_expert_c.regHist(mvt_extrprdsag[i][jTGC][f][k][x]);
              }
              if(sc.isFailure()){
                m_log << MSG::FATAL << "mvt_extrprdsag["<<i<<"]["<<jTGC<<"]["<<f<<"]["<<k<<"]["<<x<<"] Failed to register histogram " << endreq;
                return sc;
              }
              mvt_extrprdsag[i][jTGC][f][k][x]->GetXaxis()->SetTitle(RhoEtaPhiZ[x].c_str());
              sst.str("");
            
              // TGC EIFI vs MDT extrapolated position, Sagitta
              sst<<"TGC_MidSegm_"<<RhoEtaPhiZ[x];
              sst<<"Sagitta_T"<<jTGC+1;
              sst<<FE[f];
              sst<<WireStrip[k];
              sst<<"_"<<AC[i];
              m_log<<MSG::DEBUG<<"TGC_Sagitta "<<i<<" "<<jTGC<<" "<<f<<" "<<k<<" "<<x<<" "<<sst.str().c_str()<<std::endl;
              mvt_extrprdsag2[i][jTGC][f][k][x] =new TH1F(sst.str().c_str(), sst.str().c_str(), nbins3D_1[x],fGlobalCoords3Dlo[x],fGlobalCoords3Dup[x]);
              if(i==0){
                sc=mdtvstgclv1_expert_a.regHist(mvt_extrprdsag2[i][jTGC][f][k][x]);
              }else{
                sc=mdtvstgclv1_expert_c.regHist(mvt_extrprdsag2[i][jTGC][f][k][x]);
              }
              if(sc.isFailure()){
                m_log << MSG::FATAL << "mvt_extrprdsag2["<<i<<"]["<<jTGC<<"]["<<f<<"]["<<k<<"]["<<x<<"] Failed to register histogram " << endreq;
                return sc;
              }
              mvt_extrprdsag2[i][jTGC][f][k][x]->GetXaxis()->SetTitle(RhoEtaPhiZ[x].c_str());
              sst.str("");
            }
          }// RhoEtaPhi
        }// WireStrip
      }// FE
    }// TGC Station
  }// AC
  
  
  for(int k=0;k<2;k++){// WireStrip
    if(k==0){
      nbins3D_1[0]=            10000; nbins3D_1[1]=             1200; nbins3D_1[2]=             8192; nbins3D_1[3]=            40000;
      fGlobalCoords3Dlo[0]=    -1000; fGlobalCoords3Dlo[1]=       -3; fGlobalCoords3Dlo[2]=-1*M_PI/8; fGlobalCoords3Dlo[3]=  -200000;
      fGlobalCoords3Dup[0]=     1000; fGlobalCoords3Dup[1]=        3; fGlobalCoords3Dup[2]=   M_PI/8; fGlobalCoords3Dup[3]=   200000;
    }
    else{
      nbins3D_1[0]=            10000; nbins3D_1[1]=             1200; nbins3D_1[2]=             8192; nbins3D_1[3]=            40000;
      fGlobalCoords3Dlo[0]=    -5000; fGlobalCoords3Dlo[1]=       -3; fGlobalCoords3Dlo[2]=-1*M_PI/8; fGlobalCoords3Dlo[3]=  -200000;
      fGlobalCoords3Dup[0]=     5000; fGlobalCoords3Dup[1]=        3; fGlobalCoords3Dup[2]=   M_PI/8; fGlobalCoords3Dup[3]=   200000;
    }
    for(int i=0;i<2;i++){// AC
      for(int x=0;x<4;x++){// RhoEtaPhiZ
        if(x==0||x==2){ // Only interested in Rho and Phi
          // TGC PRD comparison for Mid Only Check
          sst<<"TGC_MidStationPRD_"<<RhoEtaPhiZ[x];
          sst<<"Sagitta_"<<WireStrip[k];
          sst<<"_"<<AC[i];
          m_log<<MSG::DEBUG<<"TGC_PRDonly_Sagitta "<<i<<" "<<" "<<k<<" "<<x<<" "<<sst.str().c_str()<<std::endl;
          tgc_prdcompsag[i][k][x] =new TH1F(sst.str().c_str(), sst.str().c_str(), nbins3D_1[x],fGlobalCoords3Dlo[x],fGlobalCoords3Dup[x]);
          if(i==0){
            sc=mdtvstgclv1_expert_a.regHist(tgc_prdcompsag[i][k][x]);
          }else{
            sc=mdtvstgclv1_expert_c.regHist(tgc_prdcompsag[i][k][x]);
          }
          if(sc.isFailure()){
            m_log << MSG::FATAL << "tgc_prdcompsag["<<i<<"]["<<k<<"]["<<x<<"] Failed to register histogram " << endreq;
            return sc;
          }
          tgc_prdcompsag[i][k][x]->GetXaxis()->SetTitle(RhoEtaPhiZ[x].c_str());
          sst.str("");
        }
      }// WireStrip
    }// RhoEtaPhiThe
  }// AC
  
  
  ////////////////////////////////////////////////////////////////
  // Segment Positions
  
  // Initialize to zero
  for(int i=0;i<2;i++)// AC
    for(int jMDT1=0;jMDT1<4;jMDT1++)// MDT Station1
      for(int jMDT2=0;jMDT2<4;jMDT2++)// MDT Station2
        for(int x=0;x<4;x++){
          mdt_segmmatchsag[i][jMDT1][jMDT2][x]=0;
        }
  
  nbins3D_1[0]=             2000; nbins3D_1[1]=              200; nbins3D_1[2]=             4096; nbins3D_1[3]=             1024;
  fGlobalCoords3Dlo[0]=    -1000; fGlobalCoords3Dlo[1]=       -1; fGlobalCoords3Dlo[2]=  -M_PI/8; fGlobalCoords3Dlo[3]=        0;
  fGlobalCoords3Dup[0]=     1000; fGlobalCoords3Dup[1]=        1; fGlobalCoords3Dup[2]=   M_PI/8; fGlobalCoords3Dup[3]= 0.5*M_PI;
  for(int i=0;i<2;i++){// AC
    for(int jMDT1=0;jMDT1<4;jMDT1++){// MDT Station1
      for(int jMDT2=0;jMDT2<4;jMDT2++){// MDT Station2
        //if(jMDT1==jMDT2)continue;
        for(int x=0;x<4;x++){// RhoEtaPhiThe
          if(x==0||x==2||x==3){// Not interested in Eta
            // Segm Matching Sag
            sst<<"MDT_Matching_"<<RhoEtaPhiThe[x];
            sst<<"Sagitta_MDT"<<jMDT1+1;
            sst<<"vsMDT"<<jMDT2+1;
            sst<<"_"<<AC[i];
            m_log<<MSG::DEBUG<<"MDT_MatchingSagitta "<<i<<" "<<jMDT1<<" "<<jMDT2<<" "<<x<<" "<<sst.str().c_str()<<std::endl;
            mdt_segmmatchsag[i][jMDT1][jMDT2][x] =new TH1F(sst.str().c_str(),sst.str().c_str(),  nbins3D_1[x], fGlobalCoords3Dlo[x], fGlobalCoords3Dup[x]);
            if(i==0){
              sc=mdtvstgclv1_expert_a.regHist(mdt_segmmatchsag[i][jMDT1][jMDT2][x]);
            }else{
              sc=mdtvstgclv1_expert_c.regHist(mdt_segmmatchsag[i][jMDT1][jMDT2][x]);
            }
            if(sc.isFailure()){
              m_log << MSG::FATAL << "mdt_segmmatchsag["<<i<<"]["<<jMDT1<<"]["<<jMDT2<<"]["<<x<<"] Failed to register histogram " << endreq;
              return sc;
            }
            mdt_segmmatchsag[i][jMDT1][jMDT2][x]->GetXaxis()->SetTitle(RhoEtaPhiThe[x].c_str());
            sst.str("");
          }
        }
      }
    }
  }
  
  
  ////////////////////////////////////////////////////////////////
  // Segment Directions
  
  for(int i=0;i<2;i++)// AC
    for(int jMDT1=0;jMDT1<4;jMDT1++){// MDT Station2
      for(int x=0;x<4;x++){// RhoEtaPhiThe
        mdt_segmposdirsag[i][jMDT1][x]=0;
      }
      for(int jMDT2=0;jMDT2<4;jMDT2++){
        for(int x=0;x<4;x++){
          mdt_trackdirdirsag[i][jMDT1][jMDT2][x]=0;
          for(int v=0;v<2;v++){
            mdt_trackchecksag[i][jMDT1][jMDT2][x][v]=0;
          }
        }
      }
    }
  
  nbins3D_1[0]=             2000; nbins3D_1[1]=              200; nbins3D_1[2]=             8192; nbins3D_1[3]=             2048;
  fGlobalCoords3Dlo[0]=    -1000; fGlobalCoords3Dlo[1]=       -1; fGlobalCoords3Dlo[2]=  -2*M_PI; fGlobalCoords3Dlo[3]=-0.5*M_PI;
  fGlobalCoords3Dup[0]=     1000; fGlobalCoords3Dup[1]=        1; fGlobalCoords3Dup[2]=   2*M_PI; fGlobalCoords3Dup[3]= 0.5*M_PI;
  for(int i=0;i<2;i++){// AC
    for(int jMDT1=0;jMDT1<4;jMDT1++){// MDT Station2
      for(int x=0;x<4;x++){// RhoEtaPhiThe
        if(x==2||x==3){ // Only interested in Phi & Theta
          // Segm Pos-Dir Sag
          sst<<"MDT_PosDir_"<<RhoEtaPhiThe[x];
          sst<<"Sagitta_MDT"<<jMDT1+1;
          sst<<"_"<<AC[i];
          m_log<<MSG::DEBUG<<"MDT_PosDirSagitta "<<i<<" "<<jMDT1<<" "<<x<<" "<<sst.str().c_str()<<std::endl;
          mdt_segmposdirsag[i][jMDT1][x] =new TH1F(sst.str().c_str(),sst.str().c_str(), nbins3D_1[x], fGlobalCoords3Dlo[x], fGlobalCoords3Dup[x]);
          if(i==0){
            sc=mdtvstgclv1_expert_a.regHist(mdt_segmposdirsag[i][jMDT1][x]);
          }else{
            sc=mdtvstgclv1_expert_c.regHist(mdt_segmposdirsag[i][jMDT1][x]);
          }
          if(sc.isFailure()){
            m_log << MSG::FATAL << "mdt_segmposdirsag["<<i<<"]["<<jMDT1<<"]["<<x<<"] Failed to register histogram " << endreq;
            return sc;
          }
          mdt_segmposdirsag[i][jMDT1][x]->GetXaxis()->SetTitle(RhoEtaPhiThe[x].c_str());
          sst.str("");
        }
      }// RhoEtaPhiThe

      for(int jMDT2=0;jMDT2<4;jMDT2++){// MDT Station2
        //if(jMDT1==jMDT2)continue;// Cut same station comparison
        for(int x=0;x<4;x++){// RhoEtaPhiThe
          if(x==2||x==3){ // Only interested in Phi & Theta
            // Track Dir-Dir Sag
            sst<<"MDT_DirDir_"<<RhoEtaPhiThe[x];
            sst<<"Sagitta_MDT"<<jMDT1+1;
            sst<<"vsMDT"<<jMDT2+1;
            sst<<"_"<<AC[i];
            m_log<<MSG::DEBUG<<"MDT_DirDirSagitta "<<i<<" "<<jMDT1<<" "<<jMDT2<<" "<<x<<" "<<sst.str().c_str()<<std::endl;
            mdt_trackdirdirsag[i][jMDT1][jMDT2][x] =new TH1F(sst.str().c_str(),sst.str().c_str(),  nbins3D_1[x], fGlobalCoords3Dlo[x], fGlobalCoords3Dup[x]);
            if(i==0){
              sc=mdtvstgclv1_expert_a.regHist(mdt_trackdirdirsag[i][jMDT1][jMDT2][x]);
            }else{
              sc=mdtvstgclv1_expert_c.regHist(mdt_trackdirdirsag[i][jMDT1][jMDT2][x]);
            }
            if(sc.isFailure()){
              m_log << MSG::FATAL << "mdt_trackdirdirsag["<<i<<"]["<<jMDT1<<"]["<<jMDT2<<"]["<<x<<"] Failed to register histogram " << endreq;
              return sc;
            }
            mdt_trackdirdirsag[i][jMDT1][jMDT2][x]->GetXaxis()->SetTitle(RhoEtaPhiThe[x].c_str());
            sst.str("");
              
            for(int v=0;v<2;v++){
              if(jMDT1>=jMDT2)continue;// Regard vector comparison between two stations as 
              // Track Direction Check
              sst<<"MDT_TrackCheck_"<<RhoEtaPhiThe[x];
              sst<<"Sagitta_MDT"<<jMDT1+1;
              sst<<"vs"<<jMDT2+1;
              sst<<"_MDT";
              if(v==0)sst<<jMDT1+1;
              else    sst<<jMDT2+1;
              sst<<"_"<<AC[i];
              m_log<<MSG::DEBUG<<"MDT_TrackCheckSagitta "<<i<<" "<<jMDT1<<" "<<jMDT2<<" "<<x<<" "<<sst.str().c_str()<<std::endl;
              mdt_trackchecksag[i][jMDT1][jMDT2][x][v] =new TH1F(sst.str().c_str(),sst.str().c_str(),  nbins3D_1[x], fGlobalCoords3Dlo[x], fGlobalCoords3Dup[x]);
              if(i==0){
                sc=mdtvstgclv1_expert_a.regHist(mdt_trackchecksag[i][jMDT1][jMDT2][x][v]);
              }else{
                sc=mdtvstgclv1_expert_c.regHist(mdt_trackchecksag[i][jMDT1][jMDT2][x][v]);
              }
              if(sc.isFailure()){
                m_log << MSG::FATAL << "mdt_segmmatchsag["<<i<<"]["<<jMDT1<<"]["<<jMDT2<<"]["<<x<<"]["<<v<<"] Failed to register histogram " << endreq;
                return sc;
              }
              mdt_trackchecksag[i][jMDT1][jMDT2][x][v]->GetXaxis()->SetTitle(RhoEtaPhiThe[x].c_str());
              sst.str("");
            }
          }
        }// RhoEtaPhiThe
      }// MDT Station2
    }// MDT Station1
  }// AC
  
  return sc;
}

// Apply labels to StationMap histogram's axis
void
MdtVsTgcRawDataValAlg::labelStationMap(TH2 *h2, int ac, int ws){
  // Blank non-existent chambers
  BlankStationMap(h2,ws);
  
  std::stringstream sst;
  std::string AC[2]={"A","C"};
  int nChambers[9]={5,5,5,6,6,6,6,2,2};
  std::string chamber1[6]={"E1", "E2", "E3", "E4", "F"};
  std::string chamber3[6]={"E1", "E2", "E3", "E4", "E5", "F"};
  std::string chamberE[2]={"EI", "FI"};
  
  bool m_rebin=true;
  // Name StationEtas/Layers
  int ibin=1;
  if(m_rebin){//use new bin
    for(int l=0 ; l<9 ; l++ ){// Layer
      for( int c=0 ; c<nChambers[l] ; c++ ){// Chamber
        sst << "L" << l+1 << "_";
        if(nChambers[l]==5)sst<<chamber1[c];
        else if(nChambers[l]==6)sst<<chamber3[c];
        else if(nChambers[l]==2)sst<<chamberE[c];
          if(l+1>0 && l+1<=3 && c+1<5)ibin=7*c+(l+1);
          else if(l+1>3 && l+1<=7 && c+1<5)ibin=7*c+(l+1);
          else if(l+1>3 && l+1<=7 && c+1==5)ibin=25+(l+1);
          else if(l+1==1 && c+1==5)ibin=33;
          else if(l+1==2 && c+1==5)ibin=34;
          else if(l+1==3 && c+1==5)ibin=35;
          else if(l+1==4 && c+1==6)ibin=36;
          else if(l+1==5 && c+1==6)ibin=37;
          else if(l+1==6 && c+1==6)ibin=38;
          else if(l+1==7 && c+1==6)ibin=39;
          else if(l+1==8 && c+1==1)ibin=40;
          else if(l+1==8 && c+1==2)ibin=42;
          else if(l+1==9 && c+1==1)ibin=41;
          else if(l+1==9 && c+1==2)ibin=43;
        h2->GetXaxis()->SetBinLabel(ibin, sst.str().c_str()); 
        h2->GetXaxis()->SetBit(TAxis::kLabelsVert);
        sst.str(""); 
      }// Chamber 
    }// Layer
  }else{//use old bin   logically dead code, comment out
//    for(int l=0 ; l<9 ; l++ ){// Layer
//      for( int c=0 ; c<nChambers[l] ; c++ ){// Chamber
//        sst << "L" << l+1 << "_";
//        if(nChambers[l]==5)sst<<chamber1[c];
//        else if(nChambers[l]==6)sst<<chamber3[c];
//        else if(nChambers[l]==2)sst<<chamberE[c];
//        h2->GetXaxis()->SetBinLabel(ibin, sst.str().c_str()); 
//        sst.str(""); ibin++;
//      }// Chamber
//    }// Layer
//    h2->GetXaxis()->LabelsOption("v");
  }
  // Name Phi Sectors
  for(int isec=1;isec<=12;isec++){// Sector
    for(int iphi=0;iphi<=3;iphi+=4){// Phi number
      if(ac==0||ac==1)sst << AC[ac];
      if(isec<10)sst << "0";
      sst << isec << "phi" << iphi;
      ibin=(isec-1)*4+iphi+1;
      h2->GetYaxis()->SetBinLabel(ibin, sst.str().c_str()); 
      sst.str("");
    }// Phi number
  }// Sector
}// End of function
