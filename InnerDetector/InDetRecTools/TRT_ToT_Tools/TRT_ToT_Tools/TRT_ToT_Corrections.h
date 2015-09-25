/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DEDXCORRECTIONTOOL_H
#define DEDXCORRECTIONTOOL_H

namespace Dedxcorrection {
   
  double paraL_dEdx_p1;
  double paraL_dEdx_p2;
  double paraL_dEdx_p3;
  double paraL_dEdx_p4;
  double paraL_dEdx_p5;

  double para_dEdx_p1;
  double para_dEdx_p2;
  double para_dEdx_p3;
  double para_dEdx_p4;
  double para_dEdx_p5;

      
  // resolution depends on the number of hits (and is different for e)
  double resolution[4];
  double resolution_e[4];

  // corrections for pile-up (as a function of NVtx linear behavior observed)
  // was in principle also done separately for different detector regions
  // should be checked in more details when high pileup data available

  double norm_offset_data;  // offset in normalization between data and MC
  double norm_slope_tot;    // nvtx dependence for ToT
  double norm_slope_totl;   // nvtx dependence for ToT/L
  double norm_offset_tot;  // nvtx dependence for ToT
  double norm_offset_totl; // nvtx dependence for ToT/L
  int norm_nzero;          // for which average NVtx the fit parameters were determined

  double para_long_corrRZ[3240];
  double para_short_corrRZ[216];
  double para_end_corrRZ[336];
  double para_long_corrRZ_MC[3240];
  double para_short_corrRZ_MC[216];
  double para_end_corrRZ_MC[336]; 
  double para_long_corrRZL_MC[630];
  double para_short_corrRZL_MC[144];
  double para_end_corrRZL_MC[252];
  double para_long_corrRZL_DATA[630];
  double para_short_corrRZL_DATA[63];
  double para_end_corrRZL_DATA[252]; 

}


#endif  /* DEDXCORRECTIONTOOL_H */
