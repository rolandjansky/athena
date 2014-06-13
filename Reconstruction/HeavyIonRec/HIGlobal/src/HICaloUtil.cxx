/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HICaloUtil.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "HIGlobal/HICaloUtil.h"
#include <fstream>
#include <iostream>

//================ Constructor =================================================

HICaloUtil::HICaloUtil()
{
  m_map = 0;
  m_mapN = 0;
  m_mapP = 0;
  for(int i=0; i<HICaloUtil::Unknown; i++)  m_BinSize[i] = 0;
}

//================ Destructor =================================================

HICaloUtil::~HICaloUtil(){}

void HICaloUtil::InitBinSizeEtaRange(const bool CaloTag[HICaloUtil::Unknown])
{
  MappingDetectors();
  BinSize();
  EtaRange(CaloTag); 
  return;
}

std::string HICaloUtil::CaloSampleImage(int inum)
{
  std::string CaloSam[HICaloUtil::End] = {
    "PreSamplerB",  "EMB1", "EMB2", "EMB3",   // LAr barrel
    "PreSamplerE", "EME1", "EME2", "EME3",    // LAr EM endcap 
    "HEC0", "HEC1", "HEC2", "HEC3",           // Hadronic end cap cal.
    "TileBar0", "TileBar1", "TileBar2",       // Tile gap (ITC & scint) 
    "TileGap1", "TileGap2", "TileGap3",       // Tile gap (ITC & scint) 
    "TileExt0", "TileExt1", "TileExt2",       // Tile extended barrel   
    "FCAL0", "FCAL1", "FCAL2"                 // Forward EM endcap       
  };

    return CaloSam[inum];
}
std::string HICaloUtil::CaloDetImage(int inum)
{
    std::string CaloDet[HICaloUtil::Unknown] = {
      "PreSamplerBN",  "EMBN1", "EMBN2", "EMBN3",  // LAr barrel N (negative eta) side
      "PreSamplerEN", "EMEN1", "EMEN2", "EMEN3",   // LAr EM endcap N side
      "HECN0", "HECN1", "HECN2", "HECN3",          // Hadronic end cap cal. N side
      "TileBarN0", "TileBarN1", "TileBarN2",       // Tile gap (ITC & scint) N side
      "TileGapN1", "TileGapN2", "TileGapN3",       // Tile gap (ITC & scint) N side
      "TileExtN0", "TileExtN1", "TileExtN2",       // Tile extended barrel   N side
      "FCALN0", "FCALN1", "FCALN2",                // Forward EM endcap      N side 
      "PreSamplerBP",  "EMBP1", "EMBP2", "EMBP3",  // LAr barrel P
      "PreSamplerEP", "EMEP1", "EMEP2", "EMEP3",   // LAr EM endcap P side
      "HECP0", "HECP1", "HECP2", "HECP3",          // Hadronic end cap cal. P side
      "TileBarP0", "TileBarP1", "TileBarP2",       // Tile gap (ITC & scint) P side
      "TileGapP1", "TileGapP2", "TileGapP3",       // Tile gap (ITC & scint) P side
      "TileExtP0", "TileExtP1", "TileExtP2",       // Tile extended barrel   P side
      "FCALP0", "FCALP1", "FCALP2"                 // Forward EM endcap      P side
    };
    
    return CaloDet[inum];
}

void HICaloUtil::SetToZero(std::vector<float> *x){
  for(int i=0;i<HICaloUtil::Unknown;i++){
    for(int j=0;j<(int) m_EtaRange[i].size()-1;j++){
      x[i].push_back(0.0);
    }
  }
  return ;					
}
void HICaloUtil::MappingDetectors(){ 
  m_map=new int[HICaloUtil::Unknown];
  m_map[HICaloUtil::PreSamplerBN] =HICaloUtil::PreSamplerB  ; 
  m_map[HICaloUtil::EMBN1]        =HICaloUtil::EMB1         ; 
  m_map[HICaloUtil::EMBN2]        =HICaloUtil::EMB2         ; 
  m_map[HICaloUtil::EMBN3]        =HICaloUtil::EMB3         ; 
  m_map[HICaloUtil::TileBarN0]    =HICaloUtil::TileBar0     ; 
  m_map[HICaloUtil::TileBarN1]    =HICaloUtil::TileBar1     ; 
  m_map[HICaloUtil::TileBarN2]    =HICaloUtil::TileBar2     ;
  
  m_map[HICaloUtil::PreSamplerBP] =HICaloUtil::PreSamplerB  ; 
  m_map[HICaloUtil::EMBP1]        =HICaloUtil::EMB1         ; 
  m_map[HICaloUtil::EMBP2]        =HICaloUtil::EMB2         ; 
  m_map[HICaloUtil::EMBP3]        =HICaloUtil::EMB3         ; 
  m_map[HICaloUtil::TileBarP0]    =HICaloUtil::TileBar0     ; 
  m_map[HICaloUtil::TileBarP1]    =HICaloUtil::TileBar1     ; 
  m_map[HICaloUtil::TileBarP2]    =HICaloUtil::TileBar2     ;
  
  m_map[HICaloUtil::PreSamplerEN]=HICaloUtil::PreSamplerE ;
  m_map[HICaloUtil::EMEN1]       =HICaloUtil::EME1        ;
  m_map[HICaloUtil::EMEN2]       =HICaloUtil::EME2        ;
  m_map[HICaloUtil::EMEN3]       =HICaloUtil::EME3        ;
  m_map[HICaloUtil::HECN0]       =HICaloUtil::HEC0        ;
  m_map[HICaloUtil::HECN1]       =HICaloUtil::HEC1        ;
  m_map[HICaloUtil::HECN2]       =HICaloUtil::HEC2        ;
  m_map[HICaloUtil::HECN3]       =HICaloUtil::HEC3        ;
  m_map[HICaloUtil::TileGapN1]   =HICaloUtil::TileGap1    ;
  m_map[HICaloUtil::TileGapN2]   =HICaloUtil::TileGap2    ;
  m_map[HICaloUtil::TileGapN3]   =HICaloUtil::TileGap3    ;
  m_map[HICaloUtil::TileExtN0]   =HICaloUtil::TileExt0    ; 
  m_map[HICaloUtil::TileExtN1]   =HICaloUtil::TileExt1    ;
  m_map[HICaloUtil::TileExtN2]   =HICaloUtil::TileExt2    ;
  m_map[HICaloUtil::FCALN0]      =HICaloUtil::FCAL0       ; 
  m_map[HICaloUtil::FCALN1]      =HICaloUtil::FCAL1       ; 
  m_map[HICaloUtil::FCALN2]      =HICaloUtil::FCAL2       ; 
  
  m_map[HICaloUtil::PreSamplerEP]=HICaloUtil::PreSamplerE ;
  m_map[HICaloUtil::EMEP1]       =HICaloUtil::EME1        ;
  m_map[HICaloUtil::EMEP2]       =HICaloUtil::EME2        ;
  m_map[HICaloUtil::EMEP3]       =HICaloUtil::EME3        ;
  m_map[HICaloUtil::HECP0]       =HICaloUtil::HEC0        ;
  m_map[HICaloUtil::HECP1]       =HICaloUtil::HEC1        ;
  m_map[HICaloUtil::HECP2]       =HICaloUtil::HEC2        ;
  m_map[HICaloUtil::HECP3]       =HICaloUtil::HEC3        ;
  m_map[HICaloUtil::TileGapP1]   =HICaloUtil::TileGap1    ;
  m_map[HICaloUtil::TileGapP2]   =HICaloUtil::TileGap2    ;
  m_map[HICaloUtil::TileGapP3]   =HICaloUtil::TileGap3    ;
  m_map[HICaloUtil::TileExtP0]   =HICaloUtil::TileExt0    ; 
  m_map[HICaloUtil::TileExtP1]   =HICaloUtil::TileExt1    ;
  m_map[HICaloUtil::TileExtP2]   =HICaloUtil::TileExt2    ;
  m_map[HICaloUtil::FCALP0]      =HICaloUtil::FCAL0       ; 
  m_map[HICaloUtil::FCALP1]      =HICaloUtil::FCAL1       ; 
  m_map[HICaloUtil::FCALP2]      =HICaloUtil::FCAL2       ; 
  
  m_mapN = new int[HICaloUtil::End] ; 
  
  m_mapN[HICaloUtil::PreSamplerB]=  HICaloUtil::PreSamplerBN ; 
  m_mapN[HICaloUtil::EMB1]       =  HICaloUtil::EMBN1        ; 
  m_mapN[HICaloUtil::EMB2]       =  HICaloUtil::EMBN2        ; 
  m_mapN[HICaloUtil::EMB3]       =  HICaloUtil::EMBN3        ; 
  m_mapN[HICaloUtil::TileBar0]   =  HICaloUtil::TileBarN0    ; 
  m_mapN[HICaloUtil::TileBar1]   =  HICaloUtil::TileBarN1    ; 
  m_mapN[HICaloUtil::TileBar2]   =  HICaloUtil::TileBarN2    ;
  
  m_mapN[HICaloUtil::PreSamplerE]=  HICaloUtil::PreSamplerEN ;
  m_mapN[HICaloUtil::EME1]       =  HICaloUtil::EMEN1        ;
  m_mapN[HICaloUtil::EME2]       =  HICaloUtil::EMEN2        ;
  m_mapN[HICaloUtil::EME3]       =  HICaloUtil::EMEN3        ;
  m_mapN[HICaloUtil::HEC0]       =  HICaloUtil::HECN0        ;
  m_mapN[HICaloUtil::HEC1]       =  HICaloUtil::HECN1        ;
  m_mapN[HICaloUtil::HEC2]       =  HICaloUtil::HECN2        ;
  m_mapN[HICaloUtil::HEC3]       =  HICaloUtil::HECN3        ;
  m_mapN[HICaloUtil::TileGap1]   =  HICaloUtil::TileGapN1    ;
  m_mapN[HICaloUtil::TileGap2]   =  HICaloUtil::TileGapN2    ;
  m_mapN[HICaloUtil::TileGap3]   =  HICaloUtil::TileGapN3    ;
  m_mapN[HICaloUtil::TileExt0]   =  HICaloUtil::TileExtN0    ; 
  m_mapN[HICaloUtil::TileExt1]   =  HICaloUtil::TileExtN1    ;
  m_mapN[HICaloUtil::TileExt2]   =  HICaloUtil::TileExtN2    ;
  m_mapN[HICaloUtil::FCAL0]      =  HICaloUtil::FCALN0       ; 
  m_mapN[HICaloUtil::FCAL1]      =  HICaloUtil::FCALN1       ; 
  m_mapN[HICaloUtil::FCAL2]      =  HICaloUtil::FCALN2       ; 

  m_mapP = new int[HICaloUtil::End] ; 

  m_mapP[HICaloUtil::PreSamplerB]=  HICaloUtil::PreSamplerBP ; 
  m_mapP[HICaloUtil::EMB1]       =  HICaloUtil::EMBP1        ; 
  m_mapP[HICaloUtil::EMB2]       =  HICaloUtil::EMBP2        ; 
  m_mapP[HICaloUtil::EMB3]       =  HICaloUtil::EMBP3        ; 
  m_mapP[HICaloUtil::TileBar0]   =  HICaloUtil::TileBarP0    ; 
  m_mapP[HICaloUtil::TileBar1]   =  HICaloUtil::TileBarP1    ; 
  m_mapP[HICaloUtil::TileBar2]   =  HICaloUtil::TileBarP2    ;
       			    
  m_mapP[HICaloUtil::PreSamplerE]=  HICaloUtil::PreSamplerEP ;
  m_mapP[HICaloUtil::EME1]       =  HICaloUtil::EMEP1        ;
  m_mapP[HICaloUtil::EME2]       =  HICaloUtil::EMEP2        ;
  m_mapP[HICaloUtil::EME3]       =  HICaloUtil::EMEP3        ;
  m_mapP[HICaloUtil::HEC0]       =  HICaloUtil::HECP0        ;
  m_mapP[HICaloUtil::HEC1]       =  HICaloUtil::HECP1        ;
  m_mapP[HICaloUtil::HEC2]       =  HICaloUtil::HECP2        ;
  m_mapP[HICaloUtil::HEC3]       =  HICaloUtil::HECP3        ;
  m_mapP[HICaloUtil::TileGap1]   =  HICaloUtil::TileGapP1    ;
  m_mapP[HICaloUtil::TileGap2]   =  HICaloUtil::TileGapP2    ;
  m_mapP[HICaloUtil::TileGap3]   =  HICaloUtil::TileGapP3    ;
  m_mapP[HICaloUtil::TileExt0]   =  HICaloUtil::TileExtP0    ; 
  m_mapP[HICaloUtil::TileExt1]   =  HICaloUtil::TileExtP1    ;
  m_mapP[HICaloUtil::TileExt2]   =  HICaloUtil::TileExtP2    ;
  m_mapP[HICaloUtil::FCAL0]      =  HICaloUtil::FCALP0       ; 
  m_mapP[HICaloUtil::FCAL1]      =  HICaloUtil::FCALP1       ; 
  m_mapP[HICaloUtil::FCAL2]      =  HICaloUtil::FCALP2       ; 

  return;

} 

void HICaloUtil::BinSize(){
  
  float BinSizeIni[HICaloUtil::End]={0.0};
  
  BinSizeIni[HICaloUtil::PreSamplerB] =0.1; 
  BinSizeIni[HICaloUtil::EMB1]        =0.1; 
  BinSizeIni[HICaloUtil::EMB2]        =0.1; 
  BinSizeIni[HICaloUtil::EMB3]        =0.1; 
  BinSizeIni[HICaloUtil::PreSamplerE] =0.1;
  BinSizeIni[HICaloUtil::EME1]        =0.1;
  BinSizeIni[HICaloUtil::EME2]        =0.1;
  BinSizeIni[HICaloUtil::EME3]        =0.1;
  BinSizeIni[HICaloUtil::HEC0]        =0.2;
  BinSizeIni[HICaloUtil::HEC1]        =0.2;
  BinSizeIni[HICaloUtil::HEC2]        =0.2;
  BinSizeIni[HICaloUtil::HEC3]        =0.2;
  BinSizeIni[HICaloUtil::TileBar0]    =0.1; 
  BinSizeIni[HICaloUtil::TileBar1]    =0.1; 
  BinSizeIni[HICaloUtil::TileBar2]    =0.2;
  BinSizeIni[HICaloUtil::TileGap1]    =0.1;
  BinSizeIni[HICaloUtil::TileGap2]    =0.1;
  BinSizeIni[HICaloUtil::TileGap3]    =0.2;
  BinSizeIni[HICaloUtil::TileExt0]    =0.1; 
  BinSizeIni[HICaloUtil::TileExt1]    =0.1;
  BinSizeIni[HICaloUtil::TileExt2]    =0.2;
  BinSizeIni[HICaloUtil::FCAL0]       =0.2; 
  BinSizeIni[HICaloUtil::FCAL1]       =0.2; 
  BinSizeIni[HICaloUtil::FCAL2]       =0.2; 
  
  for(int i=0;i<HICaloUtil::Unknown;i++){
    m_BinSize[i] = BinSizeIni[m_map[i]];
  }
  
  return;
}
void  HICaloUtil::EtaRange(const bool CaloTag[HICaloUtil::Unknown])
{
  // the lower eta limit on positive eta side
  float LowEtaRangeIni[HICaloUtil::End];
  float HighEtaRangeIni[HICaloUtil::End];
  LowEtaRangeIni[HICaloUtil::PreSamplerB] =0.00;    
  LowEtaRangeIni[HICaloUtil::EMB1]        =0.00;    
  LowEtaRangeIni[HICaloUtil::EMB2]        =0.00;    
  LowEtaRangeIni[HICaloUtil::EMB3]        =0.00;    
  LowEtaRangeIni[HICaloUtil::PreSamplerE] =1.50;  
  LowEtaRangeIni[HICaloUtil::EME1]        =1.3999; 
  LowEtaRangeIni[HICaloUtil::EME2]        =1.3999; 
  LowEtaRangeIni[HICaloUtil::EME3]        =1.50; 
  LowEtaRangeIni[HICaloUtil::HEC0]        =1.50; 
  LowEtaRangeIni[HICaloUtil::HEC1]        =1.50; 
  LowEtaRangeIni[HICaloUtil::HEC2]        =1.50; 
  LowEtaRangeIni[HICaloUtil::HEC3]        =1.70;  
  LowEtaRangeIni[HICaloUtil::TileBar0]    =0.00;  
  LowEtaRangeIni[HICaloUtil::TileBar1]    =0.00;  
  LowEtaRangeIni[HICaloUtil::TileBar2]  =-0.5* m_BinSize[HICaloUtil::TileBarP2]; // bin at eta = 0 goes into TileBarP2  
  LowEtaRangeIni[HICaloUtil::TileGap1]    =0.90;
  LowEtaRangeIni[HICaloUtil::TileGap2]    =0.80;
  LowEtaRangeIni[HICaloUtil::TileGap3]    =1.00; 
  LowEtaRangeIni[HICaloUtil::TileExt0]    =1.10; 
  LowEtaRangeIni[HICaloUtil::TileExt1]    =1.00; 
  LowEtaRangeIni[HICaloUtil::TileExt2]    =0.90; 
  LowEtaRangeIni[HICaloUtil::FCAL0]       =3.00;  
  LowEtaRangeIni[HICaloUtil::FCAL1]       =3.10;  
  LowEtaRangeIni[HICaloUtil::FCAL2]       =3.2;  
  
  // the upper eta limit on positive eta side 
  // later on fine tuned to the bin size				       
  HighEtaRangeIni[HICaloUtil::PreSamplerB] =1.55; 
  HighEtaRangeIni[HICaloUtil::EMB1]        =1.45; 
  HighEtaRangeIni[HICaloUtil::EMB2]        =1.45; 
  HighEtaRangeIni[HICaloUtil::EMB3]        =1.35; 
  HighEtaRangeIni[HICaloUtil::PreSamplerE] =1.75;
  HighEtaRangeIni[HICaloUtil::EME1]        =2.45;
  HighEtaRangeIni[HICaloUtil::EME2]        =3.15;
  HighEtaRangeIni[HICaloUtil::EME3]        =3.15;
  HighEtaRangeIni[HICaloUtil::HEC0]        =3.15;
  HighEtaRangeIni[HICaloUtil::HEC1]        =3.05;
  HighEtaRangeIni[HICaloUtil::HEC2]        =3.05;
  HighEtaRangeIni[HICaloUtil::HEC3]        =3.25;
  HighEtaRangeIni[HICaloUtil::TileBar0]    =0.95; 
  HighEtaRangeIni[HICaloUtil::TileBar1]    =0.85; 
  HighEtaRangeIni[HICaloUtil::TileBar2]    =0.7;
  HighEtaRangeIni[HICaloUtil::TileGap1]    =0.98;
  HighEtaRangeIni[HICaloUtil::TileGap2]    =0.88;
  HighEtaRangeIni[HICaloUtil::TileGap3]    =1.55;
  HighEtaRangeIni[HICaloUtil::TileExt0]    =1.57; 
  HighEtaRangeIni[HICaloUtil::TileExt1]    =1.47;
  HighEtaRangeIni[HICaloUtil::TileExt2]    =1.23;
  HighEtaRangeIni[HICaloUtil::FCAL0]       =4.78; 
  HighEtaRangeIni[HICaloUtil::FCAL1]       =4.8; 
  HighEtaRangeIni[HICaloUtil::FCAL2]       =4.7; 
  
  // fine tune HighEtaRange the the bin size
  for(int i=0;i< HICaloUtil::End ;i++){
    int j = (int)((HighEtaRangeIni[i]-LowEtaRangeIni[i])/m_BinSize[m_mapN[i]]);
    HighEtaRangeIni[i]=(j+1)*m_BinSize[m_mapN[i]]+LowEtaRangeIni[i];
  } 

  // eta limits and bin size for each detector at both eta sides  
  float LowEtaRange[HICaloUtil::Unknown];
  float HighEtaRange[HICaloUtil::Unknown];
  
  // high limit
  for(int i=0;i<HICaloUtil::Unknown;i++){
    if(isN(i)){
      if(i==HICaloUtil::TileBarN2)
	HighEtaRange[i] = LowEtaRangeIni[m_map[i]];// bin at eta = 0 gos to TileBarP2
      else
	HighEtaRange[i] =  -LowEtaRangeIni[m_map[i]]; 
    }
    else{
      HighEtaRange[i] =   HighEtaRangeIni[m_map[i]];
    }
  }
  
  // low limit
  for(int i=0;i<HICaloUtil::Unknown;i++){
    if(isP(i)){
      LowEtaRange[i] =  LowEtaRangeIni[m_map[i]];
    }
    else{
      LowEtaRange[i] =   -HighEtaRangeIni[m_map[i]];
    }
  }
  
  // put each bin eta limits into vectors, for each detector layer 
  for(int i=0;i<HICaloUtil::Unknown;i++){
    if(CaloTag[i] && m_BinSize[i]!=0.){
      int Nbins = (int)((0.001+HighEtaRange[i]-LowEtaRange[i])/m_BinSize[i]);
      for(int j=0;j<Nbins+1;j++){
	m_EtaRange[i].push_back(j*m_BinSize[i]+LowEtaRange[i]);
      }
    }
  }
  
  return;					
}


bool HICaloUtil::isN(int i){ 
  bool b = false;
  if(  
     i==HICaloUtil::PreSamplerBN||  
     i==HICaloUtil::EMBN1||         
     i==HICaloUtil::EMBN2||         
     i==HICaloUtil::EMBN3||         
     i==HICaloUtil::TileBarN0||     
     i==HICaloUtil::TileBarN1||     
     i==HICaloUtil::TileBarN2 ||   
     i==HICaloUtil::PreSamplerEN||
     i==HICaloUtil::EMEN1||       
     i==HICaloUtil::EMEN2||       
     i==HICaloUtil::EMEN3||       
     i==HICaloUtil::HECN0||       
     i==HICaloUtil::HECN1||       
     i==HICaloUtil::HECN2||       
     i==HICaloUtil::HECN3||       
     i==HICaloUtil::TileGapN1||   
     i==HICaloUtil::TileGapN2||   
     i==HICaloUtil::TileGapN3||   
     i==HICaloUtil::TileExtN0||   
     i==HICaloUtil::TileExtN1||   
     i==HICaloUtil::TileExtN2||   
     i==HICaloUtil::FCALN0||      
     i==HICaloUtil::FCALN1||      
     i==HICaloUtil::FCALN2      
     ) b = true;
  return b;
}

bool HICaloUtil::isP(int i){ 
  bool b = false;
  if(
     i==HICaloUtil::PreSamplerBP||  
     i==HICaloUtil::EMBP1||         
     i==HICaloUtil::EMBP2||         
     i==HICaloUtil::EMBP3||         
     i==HICaloUtil::TileBarP0||     
     i==HICaloUtil::TileBarP1||     
     i==HICaloUtil::TileBarP2||   
     i==HICaloUtil::PreSamplerEP||
     i==HICaloUtil::EMEP1||       
     i==HICaloUtil::EMEP2||       
     i==HICaloUtil::EMEP3||       
     i==HICaloUtil::HECP0||       
     i==HICaloUtil::HECP1||       
     i==HICaloUtil::HECP2||       
     i==HICaloUtil::HECP3||       
     i==HICaloUtil::TileGapP1||   
     i==HICaloUtil::TileGapP2||   
     i==HICaloUtil::TileGapP3||   
     i==HICaloUtil::TileExtP0||   
     i==HICaloUtil::TileExtP1||   
     i==HICaloUtil::TileExtP2||   
     i==HICaloUtil::FCALP0||      
     i==HICaloUtil::FCALP1||      
     i==HICaloUtil::FCALP2
     ) b= true;

  return b;
      
}
bool HICaloUtil::isB(int i){
  bool b = false;
  if(   
     i==HICaloUtil::PreSamplerBN||  
     i==HICaloUtil::EMBN1||         
     i==HICaloUtil::EMBN2||         
     i==HICaloUtil::EMBN3||         
     i==HICaloUtil::TileBarN0||     
     i==HICaloUtil::TileBarN1||     
     i==HICaloUtil::TileBarN2 ||   
     i==HICaloUtil::PreSamplerBP||  
     i==HICaloUtil::EMBP1||         
     i==HICaloUtil::EMBP2||         
     i==HICaloUtil::EMBP3||         
     i==HICaloUtil::TileBarP0||     
     i==HICaloUtil::TileBarP1||     
     i==HICaloUtil::TileBarP2    
     ) b = true;
  return b; 
}

float HICaloUtil::C_Correction(float eta){
 float C_CorrectionFactor[50] ; 
   C_CorrectionFactor[  0] = 5.105993 ; //  -5<=eta<-4.8
   C_CorrectionFactor[  1] = 1.092147 ; //-4.8<=eta<-4.6
   C_CorrectionFactor[  2] = 0.9596405; //-4.6<=eta<-4.4
   C_CorrectionFactor[  3] = 1.025018 ; //..............
   C_CorrectionFactor[  4] = 1.057949 ; 
   C_CorrectionFactor[  5] = 1.366468 ;
   C_CorrectionFactor[  6] = 1.047082 ; 
   C_CorrectionFactor[  7] = 1.112748 ;
   C_CorrectionFactor[  8] = 1.664264 ;
   C_CorrectionFactor[  9] = 0.8557471;
   C_CorrectionFactor[ 10] = 1.037353 ;
   C_CorrectionFactor[ 11] = 1.110458 ;
   C_CorrectionFactor[ 12] = 1.271888 ;
   C_CorrectionFactor[ 13] = 1.125005 ;
   C_CorrectionFactor[ 14] = 1.254932 ;
   C_CorrectionFactor[ 15] = 1.339557 ;
   C_CorrectionFactor[ 16] = 1.456826 ;
   C_CorrectionFactor[ 17] = 1.370008 ;
   C_CorrectionFactor[ 18] = 1.340226 ;
   C_CorrectionFactor[ 19] = 1.437588 ;
   C_CorrectionFactor[ 20] = 1.705499 ;
   C_CorrectionFactor[ 21] = 1.613205 ;
   C_CorrectionFactor[ 22] = 1.533662 ;
   C_CorrectionFactor[ 23] = 1.573142 ;
   C_CorrectionFactor[ 24] = 1.64838  ;
   C_CorrectionFactor[ 25] = 1.655232 ;
   C_CorrectionFactor[ 26] = 1.566483 ;
   C_CorrectionFactor[ 27] = 1.525779 ;
   C_CorrectionFactor[ 28] = 1.600238 ;
   C_CorrectionFactor[ 29] = 1.716159 ;
   C_CorrectionFactor[ 30] = 1.44978  ;
   C_CorrectionFactor[ 31] = 1.323489 ;
   C_CorrectionFactor[ 32] = 1.370875 ;
   C_CorrectionFactor[ 33] = 1.448504 ;
   C_CorrectionFactor[ 34] = 1.346931 ;
   C_CorrectionFactor[ 35] = 1.255092 ;
   C_CorrectionFactor[ 36] = 1.12499  ;
   C_CorrectionFactor[ 37] = 1.268884 ;
   C_CorrectionFactor[ 38] = 1.100656 ;
   C_CorrectionFactor[ 39] = 1.029857 ;
   C_CorrectionFactor[ 40] = 0.8475514;
   C_CorrectionFactor[ 41] = 1.656335 ;
   C_CorrectionFactor[ 42] = 1.109191 ;
   C_CorrectionFactor[ 43] = 1.042222 ;
   C_CorrectionFactor[ 44] = 1.366476 ;
   C_CorrectionFactor[ 45] = 1.061376 ;
   C_CorrectionFactor[ 46] = 1.02181  ;
   C_CorrectionFactor[ 47] = 0.9606821;
   C_CorrectionFactor[ 48] = 1.0896   ;
   C_CorrectionFactor[ 49] = 5.064901 ;
 
  int etabin = static_cast<int>((eta+5.001)/0.2);
  return C_CorrectionFactor[etabin];
}
//===============================================================================
float HICaloUtil::C_Correction_pp(float eta) { 
	float C_CorrectionFactor_pp[50]; 
   C_CorrectionFactor_pp[  0] = 4.779174 ; //  -5<=eta<-4.8
   C_CorrectionFactor_pp[  1] = 1.017974 ; //-4.8<=eta<-4.6
   C_CorrectionFactor_pp[  2] = 0.9157794; //-4.6<=eta<-4.4
   C_CorrectionFactor_pp[  3] = 0.9901101; //..............
   C_CorrectionFactor_pp[  4] = 1.027643 ;
   C_CorrectionFactor_pp[  5] = 1.341478 ;
   C_CorrectionFactor_pp[  6] = 1.030229 ;
   C_CorrectionFactor_pp[  7] = 1.10951  ;
   C_CorrectionFactor_pp[  8] = 1.652281 ;
   C_CorrectionFactor_pp[  9] = 0.8546951;
   C_CorrectionFactor_pp[ 10] = 1.055475 ;
   C_CorrectionFactor_pp[ 11] = 1.132409 ;
   C_CorrectionFactor_pp[ 12] = 1.299548 ;
   C_CorrectionFactor_pp[ 13] = 1.161663 ;
   C_CorrectionFactor_pp[ 14] = 1.288473 ;
   C_CorrectionFactor_pp[ 15] = 1.379271 ;
   C_CorrectionFactor_pp[ 16] = 1.495312 ;
   C_CorrectionFactor_pp[ 17] = 1.436444 ;
   C_CorrectionFactor_pp[ 18] = 1.395891 ;
   C_CorrectionFactor_pp[ 19] = 1.501055 ;
   C_CorrectionFactor_pp[ 20] = 1.776364 ;
   C_CorrectionFactor_pp[ 21] = 1.662133 ;
   C_CorrectionFactor_pp[ 22] = 1.58846  ;
   C_CorrectionFactor_pp[ 23] = 1.622877 ;
   C_CorrectionFactor_pp[ 24] = 1.691348 ;
   C_CorrectionFactor_pp[ 25] = 1.69046  ;
   C_CorrectionFactor_pp[ 26] = 1.617873 ;
   C_CorrectionFactor_pp[ 27] = 1.582633 ;
   C_CorrectionFactor_pp[ 28] = 1.657571 ;
   C_CorrectionFactor_pp[ 29] = 1.776572 ;
   C_CorrectionFactor_pp[ 30] = 1.500854 ;
   C_CorrectionFactor_pp[ 31] = 1.380251 ;
   C_CorrectionFactor_pp[ 32] = 1.41778  ;
   C_CorrectionFactor_pp[ 33] = 1.491657 ;
   C_CorrectionFactor_pp[ 34] = 1.373287 ;
   C_CorrectionFactor_pp[ 35] = 1.287834 ;
   C_CorrectionFactor_pp[ 36] = 1.157364 ;
   C_CorrectionFactor_pp[ 37] = 1.294291 ;
   C_CorrectionFactor_pp[ 38] = 1.12368  ;
   C_CorrectionFactor_pp[ 39] = 1.046033 ;
   C_CorrectionFactor_pp[ 40] = 0.849536 ;
   C_CorrectionFactor_pp[ 41] = 1.649579 ;
   C_CorrectionFactor_pp[ 42] = 1.107595 ;
   C_CorrectionFactor_pp[ 43] = 1.026711 ;
   C_CorrectionFactor_pp[ 44] = 1.33634  ;
   C_CorrectionFactor_pp[ 45] = 1.025469 ;
   C_CorrectionFactor_pp[ 46] = 0.9892962;
   C_CorrectionFactor_pp[ 47] = 0.91502  ;
   C_CorrectionFactor_pp[ 48] = 1.014912 ;
   C_CorrectionFactor_pp[ 49] = 4.789541 ;
	int etabin = static_cast<int>((eta+5.001)/0.2);
  return C_CorrectionFactor_pp[etabin]; 
}
