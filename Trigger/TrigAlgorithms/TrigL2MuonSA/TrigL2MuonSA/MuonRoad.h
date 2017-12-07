/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MUONROAD_H
#define  TRIGL2MUONSA_MUONROAD_H

#include "Identifier/Identifier.h"

namespace TrigL2MuonSA {
  
  // --------------------------------------------------------------------------------
  // --------------------------------------------------------------------------------
  
  const int N_STATION = 11; //barrel  0: inner, 1: middle, 2: outer, endcap 3: inner, 4: middle, 5: outer, 6: ee, 7: csc 8: BEE 9: BME 10:BMG
  const int N_SECTOR = 2; // 0: normal, 1:overlap
  const int N_LAYER = 8; 
  
  class MuonRoad
  {
  public:
    MuonRoad() { Clear(); };
    ~MuonRoad() {};

  public:
    void Clear()
    {
      isEndcap   = true;
      side       = 0;
      LargeSmall = 0;	
      Special    = 0;
      phiMiddle  = 0;
      phiRoI     = 0;
      MDT_sector_trigger = 0;
      MDT_sector_overlap = 0;
      for(int i=0; i<N_STATION; i++) {
	scales[i] = 1;
	for(int j=0; j<N_LAYER; j++) {	  
	  rWidth[i][j] = 0; 
	}
	for(int j=0; j<N_SECTOR; j++) {	  
	  aw[i][j]   = 0;
	  bw[i][j]   = 0;
	  phi[i][j]  = 0;
	}
      }
      stationList.clear();
    };
    
    double MaxWidth(int i_station)
    {
      double max=0;
      for(int j=0; j<N_LAYER; j++) {	  
	if (rWidth[i_station][j] > max) max = rWidth[i_station][j]; 
      }
      return max;
    };

    void setScales(double inner, double middle, double outer)
    {
      scales[0] = inner;
      scales[1] = middle;
      scales[2] = outer;
    };
    
  public:
    bool   isEndcap;
    int    side;
    int    LargeSmall;
    int    Special;
    double phiMiddle;
    double phiRoI;
    double aw[N_STATION][N_SECTOR];
    double bw[N_STATION][N_SECTOR];
    double phi[N_STATION][N_SECTOR];
    double rWidth[N_STATION][N_LAYER];
    double scales[N_STATION];
    int    MDT_sector_trigger;
    int    MDT_sector_overlap;

    std::vector<Identifier> stationList;

  };
  // --------------------------------------------------------------------------------
  // --------------------------------------------------------------------------------
}

#endif
