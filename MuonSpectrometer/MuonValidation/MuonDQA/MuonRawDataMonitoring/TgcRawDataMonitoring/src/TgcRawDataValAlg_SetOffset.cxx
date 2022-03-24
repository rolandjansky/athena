/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

// GeoModel
#include "MuonReadoutGeometry/TgcReadoutParams.h"

#include "Identifier/Identifier.h"

// MuonRDO
#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoIdHash.h"
#include "MuonRDO/TgcRdoContainer.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
 
#include "TgcRawDataValAlg.h"
#include "AthenaMonitoring/AthenaMonManager.h"

#include <inttypes.h> 

#include <sstream>
#include <cmath>

// Set Offset for numbering
void
TgcRawDataValAlg::setOffset(){
  //offset for Wire Nubmering
  // m_offset[TGC][Eta]
  // TGC: TGC1 TGC2 TGC3 EIFI
  // Eta: 0:forward, [1:5]:endcap

  //TGC1
  m_offset[0][0] = 0;
  m_offset[0][1] =                  getNumberOfWires(41,1);
  m_offset[0][2] = m_offset[0][1] + getNumberOfWires(42,1);
  m_offset[0][3] = m_offset[0][2] + getNumberOfWires(42,2);
  m_offset[0][4] = m_offset[0][3] + getNumberOfWires(42,3);
  m_nWireTGCSingleLayerInPhi48Division[0] = m_offset[0][4] + getNumberOfWires(42,4);//total number of wires in TGC1 layer

  //TGC2
  m_offset[1][0] = 0;
  m_offset[1][1] =                  getNumberOfWires(43,1);
  m_offset[1][2] = m_offset[1][1] + getNumberOfWires(44,1);
  m_offset[1][3] = m_offset[1][2] + getNumberOfWires(44,2);
  m_offset[1][4] = m_offset[1][3] + getNumberOfWires(44,3);
  m_offset[1][5] = m_offset[1][4] + getNumberOfWires(44,4);
  m_nWireTGCSingleLayerInPhi48Division[1] = m_offset[1][5] + getNumberOfWires(44,5);//total number of wires in TGC2 layer

  //TGC3
  m_offset[2][0] = 0;
  m_offset[2][1] =                  getNumberOfWires(45,1);
  m_offset[2][2] = m_offset[2][1] + getNumberOfWires(46,1);
  m_offset[2][3] = m_offset[2][2] + getNumberOfWires(46,2);
  m_offset[2][4] = m_offset[2][3] + getNumberOfWires(46,3);
  m_offset[2][5] = m_offset[2][4] + getNumberOfWires(46,4);
  m_nWireTGCSingleLayerInPhi48Division[2] = m_offset[2][5] + getNumberOfWires(46,5);//total number of wires in TGC2 layer

  //EIFI 
  // m_offsetEIFI[sector][sectorPhi]
  // sector    [0:3]
  // sectorPhi [0:5]FI [6:11]EI
  m_offsetEIFI[0][0]  = 0;
  m_offsetEIFI[0][1]  =                       getNumberOfWires(47, 0, 24);
  m_offsetEIFI[0][2]  = m_offsetEIFI[0][1]  + getNumberOfWires(47, 0, 1);
  m_offsetEIFI[0][3]  = m_offsetEIFI[0][2]  + getNumberOfWires(47, 0, 2);
  m_offsetEIFI[0][4]  = m_offsetEIFI[0][3]  + getNumberOfWires(47, 0, 3);
  m_offsetEIFI[0][5]  = m_offsetEIFI[0][4]  + getNumberOfWires(47, 0, 4);
  m_offsetEIFI[0][6]  = m_offsetEIFI[0][5]  + getNumberOfWires(47, 0, 5);
  m_offsetEIFI[0][7]  = m_offsetEIFI[0][6]  + getNumberOfWires(48, 0, 21);
  m_offsetEIFI[0][8]  = m_offsetEIFI[0][7]  + getNumberOfWires(48, 0, 1);
  m_offsetEIFI[0][9]  = m_offsetEIFI[0][8]  + getNumberOfWires(48, 0, 2);
  m_offsetEIFI[0][10] = m_offsetEIFI[0][9]  + getNumberOfWires(48, 0, 3);
  m_offsetEIFI[0][11] = m_offsetEIFI[0][10] + getNumberOfWires(48, 0, 4);
  m_nWireTGCEIFISingleLayerInSect4Division[0] = m_offsetEIFI[0][11] + getNumberOfWires(48, 0, 5);

  // m_offsetEIFI[sector][sectorPhi]
  m_offsetEIFI[1][0]  = 0;
  m_offsetEIFI[1][1]  =                       getNumberOfWires(47, 0, 6);
  m_offsetEIFI[1][2]  = m_offsetEIFI[1][1]  + getNumberOfWires(47, 0, 7);
  m_offsetEIFI[1][3]  = m_offsetEIFI[1][2]  + getNumberOfWires(47, 0, 8);
  m_offsetEIFI[1][4]  = m_offsetEIFI[1][3]  + getNumberOfWires(47, 0, 9);
  m_offsetEIFI[1][5]  = m_offsetEIFI[1][4]  + getNumberOfWires(47, 0, 10);
  m_offsetEIFI[1][6]  = m_offsetEIFI[1][5]  + getNumberOfWires(47, 0, 11);
  m_offsetEIFI[1][7]  = m_offsetEIFI[1][6]  + getNumberOfWires(48, 0, 6);
  m_offsetEIFI[1][8]  = m_offsetEIFI[1][7]  + getNumberOfWires(48, 0, 7);
  m_offsetEIFI[1][9]  = m_offsetEIFI[1][8]  + getNumberOfWires(48, 0, 8);
  m_offsetEIFI[1][10] = m_offsetEIFI[1][9]  + getNumberOfWires(48, 0, 9);
  m_nWireTGCEIFISingleLayerInSect4Division[1] = m_offsetEIFI[1][10] + getNumberOfWires(48, 0, 10);

  // m_offsetEIFI[sector][sectorPhi]
  m_offsetEIFI[2][0]  = 0;
  m_offsetEIFI[2][1]  =                       getNumberOfWires(47, 0, 12);
  m_offsetEIFI[2][2]  = m_offsetEIFI[2][1]  + getNumberOfWires(47, 0, 13);
  m_offsetEIFI[2][3]  = m_offsetEIFI[2][2]  + getNumberOfWires(47, 0, 14);
  m_offsetEIFI[2][4]  = m_offsetEIFI[2][3]  + getNumberOfWires(47, 0, 15);
  m_offsetEIFI[2][5]  = m_offsetEIFI[2][4]  + getNumberOfWires(47, 0, 16);
  m_offsetEIFI[2][6]  = m_offsetEIFI[2][5]  + getNumberOfWires(47, 0, 17);
  m_offsetEIFI[2][7]  = m_offsetEIFI[2][6]  + getNumberOfWires(48, 0, 11);
  m_offsetEIFI[2][8]  = m_offsetEIFI[2][7]  + getNumberOfWires(48, 0, 12);
  m_offsetEIFI[2][9]  = m_offsetEIFI[2][8]  + getNumberOfWires(48, 0, 13);
  m_offsetEIFI[2][10] = m_offsetEIFI[2][9]  + getNumberOfWires(48, 0, 14);
  m_nWireTGCEIFISingleLayerInSect4Division[2] = m_offsetEIFI[2][10] + getNumberOfWires(48, 0, 15);

  // m_offsetEIFI[sector][sectorPhi]
  m_offsetEIFI[3][0]  = 0;
  m_offsetEIFI[3][1]  =                       getNumberOfWires(47, 0, 18);
  m_offsetEIFI[3][2]  = m_offsetEIFI[3][1]  + getNumberOfWires(47, 0, 19);
  m_offsetEIFI[3][3]  = m_offsetEIFI[3][2]  + getNumberOfWires(47, 0, 20);
  m_offsetEIFI[3][4]  = m_offsetEIFI[3][3]  + getNumberOfWires(47, 0, 21);
  m_offsetEIFI[3][5]  = m_offsetEIFI[3][4]  + getNumberOfWires(47, 0, 22);
  m_offsetEIFI[3][6]  = m_offsetEIFI[3][5]  + getNumberOfWires(47, 0, 23);
  m_offsetEIFI[3][7]  = m_offsetEIFI[3][6]  + getNumberOfWires(48, 0, 16);
  m_offsetEIFI[3][8]  = m_offsetEIFI[3][7]  + getNumberOfWires(48, 0, 17);
  m_offsetEIFI[3][9]  = m_offsetEIFI[3][8]  + getNumberOfWires(48, 0, 18);
  m_offsetEIFI[3][10] = m_offsetEIFI[3][9]  + getNumberOfWires(48, 0, 19);
  m_nWireTGCEIFISingleLayerInSect4Division[3] = m_offsetEIFI[3][10] + getNumberOfWires(48, 0, 20);
  
  // Clear Delta Channel Offsets
  for(int ws=0;ws<2;ws++){
    for(int ac=0;ac<2;ac++){
      for(int eta=0;eta<6;eta++){
        for(int l=0;l<9;l++){
          if(l<3&&eta>4){//triplet has eta[0:4] only
            // non-existent chamber
            m_SLBoffset[ws][ac][eta][l] = -99;
          }
          else if(ws==1&&l==1){
            //layer2 has no strips
            m_SLBoffset[ws][ac][eta][l] = -99;
          }else{
            // valid chamber
            m_SLBoffset[ws][ac][eta][l] = 0;
          }
        }
      }
    }
  }
  
  // Set Delta Channel Wire Offsets
  int theoffset = 3;
  for(int ac=0;ac<2;ac ++){
    // m_SLBoffset[ws][side][eta][layer] (note layer indexes are zero based)
    
    // layer 1;
    m_SLBoffset[0][ac][2][0] = 1;
    
    // layer 4&5;
    m_SLBoffset[0][ac][0][3] = m_SLBoffset[0][ac][0][4] = 0;
    m_SLBoffset[0][ac][1][3] = m_SLBoffset[0][ac][1][4] = 0;
    m_SLBoffset[0][ac][2][3] = m_SLBoffset[0][ac][2][4] = 109;
    m_SLBoffset[0][ac][3][3] = m_SLBoffset[0][ac][3][4] = 211;
    m_SLBoffset[0][ac][4][3] = m_SLBoffset[0][ac][4][4] = 241;
    m_SLBoffset[0][ac][5][3] = m_SLBoffset[0][ac][5][4] = 271;
    
    // layer 6&7
    m_SLBoffset[0][ac][0][5] = m_SLBoffset[0][ac][0][6] = 0;
    m_SLBoffset[0][ac][1][5] = m_SLBoffset[0][ac][1][6] = 1   + theoffset;
    m_SLBoffset[0][ac][2][5] = m_SLBoffset[0][ac][2][6] = 95  + theoffset;
    m_SLBoffset[0][ac][3][5] = m_SLBoffset[0][ac][3][6] = 200 + theoffset;
    m_SLBoffset[0][ac][4][5] = m_SLBoffset[0][ac][4][6] = 231 + theoffset;
    m_SLBoffset[0][ac][5][5] = m_SLBoffset[0][ac][5][6] = 260 + theoffset;
  }
  // Set Delta Channel Strip Offsets
  for(int eta=0;eta<6;eta++){
    if(eta!=5)
      m_SLBoffset[1][1][eta][2] = 1;
    m_SLBoffset[1][1][eta][4] = 1;
    m_SLBoffset[1][1][eta][6] = 1;
  }
  
  //strip
  m_nStripTGCSingleLayerInPhi48Division[0]=160;
  m_nStripTGCSingleLayerInPhi48Division[1]=192;
  m_nStripTGCSingleLayerInPhi48Division[2]=192;

  m_nStripTGCEIFISingleLayerInSect4Division[0]=32*12;
  m_nStripTGCEIFISingleLayerInSect4Division[1]=32*11;
  m_nStripTGCEIFISingleLayerInSect4Division[2]=32*11;
  m_nStripTGCEIFISingleLayerInSect4Division[3]=32*11;
}

void 
TgcRawDataValAlg::setChannelDifferenceRange(){
  // m_dchmin[layerA][layerB][ws][ac]
  // Difference calculation is channel(layerA)-channel(layerB), 
  // dchmin is minimum allowable difference, dchmax is maximum
  // m_dchmin[x][y] === -m_dchmax[y][x]
  
  // clear array
  for(int ac=0;ac<2;ac++)
    for(int ws=0;ws<2;ws++)
      for(int layerA=0;layerA<9;layerA++)
        for(int layerB=0;layerB<9;layerB++){
          m_dchmin[layerA][layerB][ws][ac] =  99;
          m_dchmax[layerA][layerB][ws][ac] = -99;
        }
  
  ///////////////////////////////////////////////////////////////////////////
  // Triplet

  // Wires, A and C sides identical
  int dmin_wt[3][3] = {{  99,   -1,   -1 },
                       {   0,   99,   -1 },
                       {   0,    0,   99 }};
  
  int dmax_wt[3][3] = {{ -99,    0,    0 },
                       {   1,  -99,    0 },
                       {   1,    1,  -99 }};
  
  // Strips, A and C sides identical, no strips in layer 2
  int dmin_st[3][3] = {{  99,   99,   -1 },
                       {  99,   99,   99 },
                       {   0,   99,   99 }};
  
  int dmax_st[3][3] = {{ -99,   99,    0 },
                       {  99,  -99,  -99 },
                       {   1,   99,  -99 }};
  
  for(int ac=0;ac<2;ac++){
    for(int i1=0;i1<3;i1++){int layerA=i1;//[0:2]
      for(int i2=0;i2<3;i2++){int layerB=i2;//[0:2]
        // Wires
        m_dchmin[layerA][layerB][WIRE][ac] = dmin_wt[i1][i2];
        m_dchmax[layerA][layerB][WIRE][ac] = dmax_wt[i1][i2];
        // Strips
        m_dchmin[layerA][layerB][STRP][ac] = dmin_st[i1][i2];
        m_dchmax[layerA][layerB][STRP][ac] = dmax_st[i1][i2];
      }
    }
  }
  
  ///////////////////////////////////////////////////////////////////////////
  // Doublet
  
  // Wires, A and C sides identical
  int dmin_wd[4][4] = {{  99,   -1,   -4,   -4 },
                       {   0,   99,   -4,   -4 },
                       {  -4,   -4,   99,   -1 },
                       {  -4,   -4,    0,   99 }};
  
  int dmax_wd[4][4] = {{ -99,    0,    4,    4 },
                       {   1,  -99,    4,    4 },
                       {   4,    4,  -99,    0 },
                       {   4,    4,    1,  -99 }};
                       
  // Strips, A and C sides identical
  int dmin_sd[4][4] = {{  99,   -1,   -2,   -2 },
                       {   0,   99,   -1,   -2 },
                       {  -2,   -2,   99,   -1 },
                       {  -1,   -2,    0,   99 }};
  
  int dmax_sd[4][4] = {{ -99,    0,    2,    1 },
                       {   1,  -99,    2,    2 },
                       {   2,    1,  -99,    0 },
                       {   2,    2,    1,  -99 }};
                       
  for(int ac=0;ac<2;ac++){
    for(int i1=0;i1<4;i1++){int layerA=i1+3;//[3:6]
      for(int i2=0;i2<4;i2++){int layerB=i2+3;//[3:6]
        // Wires
        m_dchmin[layerA][layerB][WIRE][ac] = dmin_wd[i1][i2];
        m_dchmax[layerA][layerB][WIRE][ac] = dmax_wd[i1][i2];
        // Strips
        m_dchmin[layerA][layerB][STRP][ac] = dmin_sd[i1][i2];
        m_dchmax[layerA][layerB][STRP][ac] = dmax_sd[i1][i2];
        
      }
    }
  }
  
  
  ///////////////////////////////////////////////////////////////////////////
  // Inner TGC (not actually used)
  
  // Wires, A and C sides identical
  int dmin_wi[2][2] = {{ 99, -1},
                       {  0, 99}};
  
  int dmax_wi[2][2] = {{ 99,  0},
                       {  1, 99}};
  
  // Strips, A and C sides not identical
  int dmin_sai[2][2] = {{ 99, -1},
                        {  0, 99}};
  
  int dmax_sai[2][2] = {{ 99,  0},
                        {  1, 99}};
  
  int dmin_sci[2][2] = {{ 99,  0},
                        { -1, 99}};
  
  int dmax_sci[2][2] = {{ 99,  1},
                        {  0, 99}};
  
  for(int ac=0;ac<2;ac++){
    for(int i1=0;i1<2;i1++){int layerA=i1+7;//[7:8]
      for(int i2=0;i2<2;i2++){int layerB=i2+7;//[7:8]
        // Wires
        m_dchmin[layerA][layerB][WIRE][ac] = dmin_wi[i1][i2];
        m_dchmax[layerA][layerB][WIRE][ac] = dmax_wi[i1][i2];
        // Strips
        if(ac==0){
          m_dchmin[layerA][layerB][STRP][ac] = dmin_sai[i1][i2];
          m_dchmax[layerA][layerB][STRP][ac] = dmax_sai[i1][i2];
        }else{
          m_dchmin[layerA][layerB][STRP][ac] = dmin_sci[i1][i2];
          m_dchmax[layerA][layerB][STRP][ac] = dmax_sci[i1][i2];
        }
      }
    }
  }
  
}
