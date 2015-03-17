/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __VNHYPOHELPER_H__
#define __VNHYPOHELPER_H__

namespace VnHypoHelper{

   enum{
      N_CENT_MAX=70,//0-70% centrality in 1% bins
      N_QBINS   =17,//Percentile bins for event-shape 
        TOP_97_5= 0,//top 97.5%
        TOP_95_0= 1,
        TOP_92_5= 2, 
        TOP_90_0= 3, 
        TOP_80_0= 4, 
        TOP_70_0= 5, 
        TOP_60_0= 6, 
        TOP_50_0= 7, 
        TOP_40_0= 8, 
        TOP_30_0= 9, 
        TOP_20_0=10, 
        TOP_10_0=11, 
        TOP_07_5=12, 
        TOP_05_0=13, 
        TOP_02_5=14, 
        TOP_01_0=15,//top 1%
        TOP_00_0=16,//There should be no events here 

      DATA_2010=2010,
      DATA_2011=2011,
   };


   //FCal ET Thresholds in TeV for 1% centrality bins from 0-98% centrality, (2010 data)
   extern double centcuts[];

   //Takes energy in TeV returns centrality over [0-70)%
   int GetCentBin(double m_Tot_Et, int run_year=DATA_2011);

   extern double FCal_Combined_Thresholds_q2[N_CENT_MAX][N_QBINS];
   extern double FCal_Combined_Thresholds_q3[N_CENT_MAX][N_QBINS];
}

#endif

