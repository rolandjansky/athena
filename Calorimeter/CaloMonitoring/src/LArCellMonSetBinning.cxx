/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCellMonTool.h"
using namespace CaloMonitoring;

void LArCellMonTool::setLArCellBinning() {

 //EMB Pre-sampler 
  const int embpaNEtaReg = 1;
  double embpaEtaBreakPts[embpaNEtaReg+1]={0.,1.52};
  int embpaEtaBins[embpaNEtaReg]={61};
  m_binning[EMBPA].doEtaBinning(embpaNEtaReg,embpaEtaBreakPts,embpaEtaBins);
  m_binning[EMBPA].doPhiBinning(64);
  m_binning[EMBPC]= m_binning[EMBPA].etaMirror();

  // EMB Layer 1 
  const int emb1aNEtaReg = 1;
  double emb1aEtaBreakPts[emb1aNEtaReg+1]={0.,1.4};
  int emb1aEtaBins[emb1aNEtaReg]={448};
  m_binning[EMB1A].doEtaBinning(emb1aNEtaReg,emb1aEtaBreakPts,emb1aEtaBins);
  m_binning[EMB1A].doPhiBinning(64);
  m_binning[EMB1C]=m_binning[EMB1A].etaMirror();
  
  // EMB Layer 2
  const int emb2aNEtaReg = 1;
  double emb2aEtaBreakPts[emb2aNEtaReg+1]={0.,1.475};
  int emb2aEtaBins[emb2aNEtaReg]={59};
  m_binning[EMB2A].doEtaBinning(emb2aNEtaReg,emb2aEtaBreakPts,emb2aEtaBins);
  m_binning[EMB2A].doPhiBinning(256);
  m_binning[EMB2C]=m_binning[EMB2A].etaMirror();
  
  // EMB Layer 3
  const int emb3aNEtaReg = 2;
  double emb3aEtaBreakPts[emb3aNEtaReg+1]={0.00,1.35,1.475};
  int emb3aEtaBins[emb3aNEtaReg]={27,1};
  m_binning[EMB3A].doEtaBinning(emb3aNEtaReg,emb3aEtaBreakPts,emb3aEtaBins);
  m_binning[EMB3A].doPhiBinning(256);
  m_binning[EMB3C]= m_binning[EMB3A].etaMirror();

  // HEC Layer 0 
  const int hec0aNEtaReg = 2;
  double hec0aEtaBreakPts[hec0aNEtaReg+1]={1.5,2.5,3.3};
  int hec0aEtaBins[hec0aNEtaReg]={10,4};
  m_binning[HEC0A].doEtaBinning(hec0aNEtaReg,hec0aEtaBreakPts,hec0aEtaBins);
  m_binning[HEC0A].doPhiBinning(64);
  m_binning[HEC0C]=m_binning[HEC0A].etaMirror();
  
  // HEC Layer 1 
  const int hec1aNEtaReg = 2;
  double hec1aEtaBreakPts[hec1aNEtaReg+1]={1.5,2.5,3.1};
  int hec1aEtaBins[hec1aNEtaReg]={10,3};
  m_binning[HEC1A].doEtaBinning(hec1aNEtaReg,hec1aEtaBreakPts,hec1aEtaBins);
  m_binning[HEC1A].doPhiBinning(64);
  m_binning[HEC1C]=m_binning[HEC1A].etaMirror();
  
  // HEC Layer 2 
  const int hec2aNEtaReg = 2;
  double hec2aEtaBreakPts[hec2aNEtaReg+1]={1.6,2.5,3.1};
  int hec2aEtaBins[hec2aNEtaReg]={9,3};
  m_binning[HEC2A].doEtaBinning(hec2aNEtaReg,hec2aEtaBreakPts,hec2aEtaBins);
  m_binning[HEC2A].doPhiBinning(64);
  m_binning[HEC2C]=m_binning[HEC2A].etaMirror();

  // HEC Layer 3 
  const int hec3aNEtaReg = 2;
  double hec3aEtaBreakPts[hec3aNEtaReg+1]={1.7,2.5,3.3};
  int hec3aEtaBins[hec3aNEtaReg]={8,4};
  m_binning[HEC3A].doEtaBinning(hec3aNEtaReg,hec3aEtaBreakPts,hec3aEtaBins);
  m_binning[HEC3A].doPhiBinning(64);
  m_binning[HEC3C]=m_binning[HEC3A].etaMirror();
  
  // EMEC Pre-sampler (Layer 0) 
   const int emecpaNEtaReg = 1;
  double emecpaEtaBreakPts[emecpaNEtaReg+1]={1.5,1.8};
  int emecpaEtaBins[emecpaNEtaReg]={12};
  m_binning[EMECPA].doEtaBinning(emecpaNEtaReg,emecpaEtaBreakPts,emecpaEtaBins);
  m_binning[EMECPA].doPhiBinning(64);
  m_binning[EMECPC]=m_binning[EMECPA].etaMirror();
 
  // EMEC Layer 1 
  const int emec1aNEtaReg = 7;
  double emec1aEtaBreakPts[emec1aNEtaReg+1]={1.375,1.425,1.5,1.8,2.0,2.4,2.5,3.2};
  int emec1aEtaBins[emec1aNEtaReg]={1,3,96,48,64,4,7};
  m_binning[EMEC1A].doEtaBinning(emec1aNEtaReg,emec1aEtaBreakPts,emec1aEtaBins);
  m_binning[EMEC1A].doPhiBinning(64);
  m_binning[EMEC1C]=m_binning[EMEC1A].etaMirror();

  // EMEC Layer 2 
  const int emec2aNEtaReg = 3;
  double emec2aEtaBreakPts[emec2aNEtaReg+1]={1.375,1.425,2.5,3.2};
  int emec2aEtaBins[emec2aNEtaReg]={1,43,7};
  m_binning[EMEC2A].doEtaBinning(emec2aNEtaReg,emec2aEtaBreakPts,emec2aEtaBins);
  m_binning[EMEC2A].doPhiBinning(256);
  m_binning[EMEC2C]=m_binning[EMEC2A].etaMirror();

  // EMEC Layer 3 
  const int emec3aNEtaReg = 1;
  double emec3aEtaBreakPts[emec3aNEtaReg+1]={1.5,2.5};
  int emec3aEtaBins[emec3aNEtaReg]={20};
  m_binning[EMEC3A].doEtaBinning(emec3aNEtaReg,emec3aEtaBreakPts,emec3aEtaBins);
  m_binning[EMEC3A].doPhiBinning(256);
  m_binning[EMEC3C]=m_binning[EMEC3A].etaMirror();

  // FCAL Layer 1  (0th layer in the code) 
  const int fcal1aNEtaReg = 4;
  double fcal1aEtaBreakPts[fcal1aNEtaReg+1]={3.08,3.2,3.8,4.6,4.82};
  int fcal1aEtaBins[fcal1aNEtaReg]={3,6,4,2};
  m_binning[FCAL1A].doEtaBinning(fcal1aNEtaReg,fcal1aEtaBreakPts,fcal1aEtaBins);
  
  const int fcal1aNPhiReg = 95;
  double fcal1aPhiBreakPts[fcal1aNPhiReg+1]=
    {-M_PI,(-250.0/256)*M_PI,(-248.0/256)*M_PI,(-243.0/256)*M_PI,(-240.0/256)*M_PI,(-234.0/256)*M_PI,
     (-230.0/256)*M_PI,(-225.0/256)*M_PI,(-219.0/256)*M_PI,(-195.0/256)*M_PI,(-190.0/256)*M_PI,(-186.0/256)*M_PI,
     (-183.0/256)*M_PI,(-178.0/256)*M_PI,(-175.0/256)*M_PI,(-173.0/256)*M_PI,(-167.0/256)*M_PI,(-161.0/256)*M_PI,
     (-159.0/256)*M_PI,(-156.0/256)*M_PI,(-151.0/256)*M_PI,(-148.0/256)*M_PI,(-146.0/256)*M_PI,(-141.0/256)*M_PI,
     (-139.0/256)*M_PI,(-135.0/256)*M_PI,(-132.0/256)*M_PI,(-120.0/256)*M_PI,(-117.0/256)*M_PI,(-114.0/256)*M_PI,
     (-110.0/256)*M_PI,(-105.0/256)*M_PI,(-97.0/256)*M_PI,(-94.0/256)*M_PI,(-86.0/256)*M_PI,(-78.0/256)*M_PI,
     (-63.0/256)*M_PI,(-60.0/256)*M_PI,(-56.0/256)*M_PI,(-46.0/256)*M_PI,(-44.0/256)*M_PI,(-41.0/256)*M_PI,
     (-32.0/256)*M_PI,(-28.0/256)*M_PI,(-22.0/256)*M_PI,(-10.0/256)*M_PI,(-7.0/256)*M_PI,(-3.0/256)*M_PI,
     (6.0/256)*M_PI,(8.0/256)*M_PI,(13.0/256)*M_PI,(16.0/256)*M_PI,(22.0/256)*M_PI,(26.0/256)*M_PI,(31.0/256)*M_PI,
     (37.0/256)*M_PI,(61.0/256)*M_PI,(66.0/256)*M_PI,(70.0/256)*M_PI,(73.0/256)*M_PI,(78.0/256)*M_PI,(81.0/256)*M_PI,
     (83.0/256)*M_PI,(89.0/256)*M_PI,(95.0/256)*M_PI,(97.0/256)*M_PI,(100.0/256)*M_PI,(105.0/256)*M_PI,
     (108.0/256)*M_PI,(110.0/256)*M_PI,(115.0/256)*M_PI,(117.0/256)*M_PI,(121.0/256)*M_PI,(124.0/256)*M_PI,
     (136.0/256)*M_PI,(139.0/256)*M_PI,(142.0/256)*M_PI,(146.0/256)*M_PI,(151.0/256)*M_PI,(159.0/256)*M_PI,
     (162.0/256)*M_PI,(170.0/256)*M_PI,(178.0/256)*M_PI,(193.0/256)*M_PI,(196.0/256)*M_PI,(200.0/256)*M_PI,
     (210.0/256)*M_PI,(212.0/256)*M_PI,(215.0/256)*M_PI,(224.0/256)*M_PI,(228.0/256)*M_PI,(234.0/256)*M_PI,
     (246.0/256)*M_PI,(249.0/256)*M_PI,(253.0/256)*M_PI,M_PI};
  int fcal1aPhiBins[fcal1aNPhiReg+1] =
    {2,1,1,1,1,1,1,2,6,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,3,1,1,2,1,2,1,2,4,3,1,1,2,1,1,3,1,2,2,1,2,3,
     1,1,1,1,1,1,2,6,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,3,1,1,2,1,2,1,2,4,3,1,1,2,1,1,3,1,2,2,1,2,1};
  m_binning[FCAL1A].doPhiBinning(fcal1aNPhiReg,fcal1aPhiBreakPts,fcal1aPhiBins);
  m_binning[FCAL1C]=m_binning[FCAL1A].etaMirror();
  
  // FCAL Layer 2  (1st layer in the code);
  const int fcal2aNEtaReg = 15;
  double fcal2aEtaBreakPts[fcal2aNEtaReg+1]=
    {3.192,3.1987,3.206,3.22,3.26,3.34,3.37,3.39,3.52,3.57,3.835,4.1,4.365,4.5,4.65,4.81};
  int fcal2aEtaBins[fcal2aNEtaReg]={1,1,1,2,2,1,1,3,1,4,2,1,1,1,1};
  m_binning[FCAL2A].doEtaBinning(fcal2aNEtaReg,fcal2aEtaBreakPts,fcal2aEtaBins);
  
  const int fcal2aNPhiReg = 32;
  double fcal2aPhiBreakPts[fcal2aNPhiReg+1]=
    {-M_PI,(-62.0/64)*M_PI,(-59.0/64)*M_PI,(-55.0/64)*M_PI,(-52.0/64)*M_PI,(-50.0/64)*M_PI,(-48.0/64)*M_PI,
     (-45.0/64)*M_PI,(-41.0/64)*M_PI,(-38.0/64)*M_PI,(-22.0/64)*M_PI,(-18.0/64)*M_PI,(-12.0/64)*M_PI,
     (-8.0/64)*M_PI,(-5.0/64)*M_PI,(-1.0/64)*M_PI,0,(2.0/64)*M_PI,(5.0/64)*M_PI,(9.0/64)*M_PI,(12.0/64)*M_PI,
     (14.0/64)*M_PI,(16.0/64)*M_PI,(19.0/64)*M_PI,(23.0/64)*M_PI,(26.0/64)*M_PI,(42.0/64)*M_PI,(46.0/64)*M_PI,
     (52.0/64)*M_PI,(56.0/64)*M_PI,(59.0/64)*M_PI,(63.0/64)*M_PI,M_PI};
  int fcal2aPhiBins[fcal2aNPhiReg+1] = 
    {1,1,1,1,1,1,1,1,1,8,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,8,1,3,1,1,1,1};
  m_binning[FCAL2A].doPhiBinning(fcal2aNPhiReg,fcal2aPhiBreakPts,fcal2aPhiBins);
  m_binning[FCAL2C]=m_binning[FCAL2A].etaMirror();

  // FCAL Layer 3  (2nd layer in the code) 
  const int fcal3aNEtaReg = 5;
  double fcal3aEtaBreakPts[fcal3aNEtaReg+1]={3.27,3.43,3.7,3.91,4.33,4.75};
  int fcal3aEtaBins[fcal3aNEtaReg]={2,2,1,1,2};
  m_binning[FCAL3A].doEtaBinning(fcal3aNEtaReg,fcal3aEtaBreakPts,fcal3aEtaBins);
  
  const int fcal3aNPhiReg = 22;
  double fcal3aPhiBreakPts[fcal3aNPhiReg+1]=
    {-M_PI,(-40.0/64)*M_PI,(-39.0/64)*M_PI,(-35.0/64)*M_PI,(-29.0/64)*M_PI,(-24.0/64)*M_PI,(-22.0/64)*M_PI,
     (-16.0/64)*M_PI,(-11.0/64)*M_PI,(-9.0/64)*M_PI,(-6.0/64)*M_PI,0,(24.0/64)*M_PI,(25.0/64)*M_PI,
     (29.0/64)*M_PI,(35.0/64)*M_PI,(40.0/64)*M_PI,(42.0/64)*M_PI,(48.0/64)*M_PI,(53.0/64)*M_PI,(55.0/64)*M_PI,
     (58.0/64)*M_PI,M_PI};
  int fcal3aPhiBins[fcal3aNPhiReg+1] = {10,1,1,4,2,1,2,2,1,1,2,10,1,1,4,2,1,2,2,1,1,2};
  m_binning[FCAL3A].doPhiBinning(fcal3aNPhiReg,fcal3aPhiBreakPts,fcal3aPhiBins);
  m_binning[FCAL3C]=m_binning[FCAL3A].etaMirror();
  return;
}
