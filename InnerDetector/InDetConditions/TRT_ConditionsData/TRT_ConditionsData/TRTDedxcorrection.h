/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTDEDXCORRECTION_H
#define TRTDEDXCORRECTION_H


struct TRTDedxcorrection {
   
  double paraL_dEdx_p1[3];
  double paraL_dEdx_p2[3];
  double paraL_dEdx_p3[3];
  double paraL_dEdx_p4[3];
  double paraL_dEdx_p5[3];

  double para_dEdx_p1[3];
  double para_dEdx_p2[3];
  double para_dEdx_p3[3];
  double para_dEdx_p4[3];
  double para_dEdx_p5[3];

      
  // resolution depends on the number of hits (and is different for e)
  double resolution[3][4];
  double resolution_e[3][4];

  // corrections for pile-up (as a function of NVtx linear behavior observed)
  // was in principle also done separately for different detector regions
  // should be checked in more details when high pileup data available

  double norm_offset_data[3];  // offset in normalization between data and MC
  double norm_slope_tot[3];    // nvtx dependence for ToT
  double norm_slope_totl[3];   // nvtx dependence for ToT/L
  double norm_offset_tot[3];   // nvtx dependence for ToT
  double norm_offset_totl[3];  // nvtx dependence for ToT/L
  int norm_nzero[3];           // for which average NVtx the fit parameters were determined

  double para_long_corrRZ[3][3240];
  double para_short_corrRZ[3][216];
  double para_end_corrRZ[3][336];
  
  double para_long_corrRZ_MC[3][3240];
  double para_short_corrRZ_MC[3][216];
  double para_end_corrRZ_MC[3][336]; 
  double para_long_corrRZL_MC[3][630];
  double para_short_corrRZL_MC[3][144];
  double para_end_corrRZL_MC[3][252];
  
  double para_long_corrRZL_DATA[3][630];
  double para_short_corrRZL_DATA[3][63];
  double para_end_corrRZL_DATA[3][252]; 

  float para_end_mimicToXe_MC[3][560];
  float para_end_mimicToXe_DATA[3][560];

  float para_short_mimicToXe_MC[3][180];
  float para_short_mimicToXe_DATA[3][180];
  float para_long_mimicToXe_MC[3][1800];
  float para_long_mimicToXe_DATA[3][1800];
 
  //==============================================================  
  

};
CLASS_DEF(TRTDedxcorrection,105466510,1)
CONDCONT_DEF(TRTDedxcorrection,114226988);

#endif  /* TRTDEDXCORRECTION_H */
