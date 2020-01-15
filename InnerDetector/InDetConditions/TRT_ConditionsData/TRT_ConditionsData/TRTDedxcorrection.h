/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTDEDXCORRECTION_H
#define TRTDEDXCORRECTION_H


struct TRTDedxcorrection {
   
  static constexpr int nParametersTrackBaseddEdx = 100;
  static constexpr int nParametersHitBaseddEdx = 204;

  double HitOccPar[nParametersHitBaseddEdx];

  // TrckOccupancy calibration constants are separated in three arrays as we use a polynomial
  // function 2nd order of the form f(x)=a+b*x+c*x^2
  
  // TrckOccupancy calibration constants, HT hits are excluded, Pt>0.4 GeV d0<0.4 mm deltaZ0sin(theta)<0.4 mm
  double TrackOccPar0_noHT[nParametersTrackBaseddEdx];
  double TrackOccPar1_noHT[nParametersTrackBaseddEdx];
  double TrackOccPar2_noHT[nParametersTrackBaseddEdx];

  // TrckOccupancy calibration constants, Pt>0.4 GeV d0<0.4 mm deltaZ0sin(theta)<0.4 mm
  double TrackOccPar0[nParametersTrackBaseddEdx];
  double TrackOccPar1[nParametersTrackBaseddEdx];
  double TrackOccPar2[nParametersTrackBaseddEdx];

  static constexpr int nGasTypes = 3;
  
  double paraL_dEdx_p1[nGasTypes];
  double paraL_dEdx_p2[nGasTypes];
  double paraL_dEdx_p3[nGasTypes];
  double paraL_dEdx_p4[nGasTypes];
  double paraL_dEdx_p5[nGasTypes];

  double para_dEdx_p1[nGasTypes];
  double para_dEdx_p2[nGasTypes];
  double para_dEdx_p3[nGasTypes];
  double para_dEdx_p4[nGasTypes];
  double para_dEdx_p5[nGasTypes];

      
  // resolution depends on the number of hits (and is different for e)
  static constexpr int nParametersResolution = 4; //Polynomial function 3rd used
  double resolution[nGasTypes][nParametersResolution];
  double resolution_e[nGasTypes][nParametersResolution];

  // corrections for pile-up (as a function of NVtx linear behavior observed)
  // was in principle also done separately for different detector regions
  // should be checked in more details when high pileup data available

  double norm_Offset_Data[nGasTypes];  // offset in normalization between data and MC
  double norm_Slope_Tot[nGasTypes];    // nvtx dependence for ToT
  double norm_Slope_Totl[nGasTypes];   // nvtx dependence for ToT/L
  double norm_Offset_Tot[nGasTypes];   // nvtx dependence for ToT
  double norm_Offset_Totl[nGasTypes];  // nvtx dependence for ToT/L
  int norm_Nzero[nGasTypes];           // for which average NVtx the fit parameters were determined

  static constexpr int nParametersLongStrawsRZ = 3240;
  static constexpr int nParametersShortStrawsRZ = 216;
  static constexpr int nParametersEndcapRZ = 336;
      
  double para_Long_CorrRZ[nGasTypes][nParametersLongStrawsRZ];
  double para_Short_CorrRZ[nGasTypes][nParametersShortStrawsRZ];
  double para_End_CorrRZ[nGasTypes][nParametersEndcapRZ];
  double para_Long_CorrRZ_MC[nGasTypes][nParametersLongStrawsRZ];
  double para_Short_CorrRZ_MC[nGasTypes][nParametersShortStrawsRZ];
  double para_End_CorrRZ_MC[nGasTypes][nParametersEndcapRZ];

  static constexpr int nParametersLongStrawsRZL = 630;
  static constexpr int nParametersShortStrawsRZL = 63;
  static constexpr int nParametersEndcapRZL = 252;
  
  double para_Long_CorrRZL_MC[nGasTypes][nParametersLongStrawsRZL];
  double para_Short_CorrRZL_MC[nGasTypes][nParametersShortStrawsRZL];
  double para_End_CorrRZL_MC[nGasTypes][nParametersEndcapRZL];
  double para_Long_CorrRZL_DATA[nGasTypes][nParametersLongStrawsRZL];
  double para_Short_CorrRZL_DATA[nGasTypes][nParametersShortStrawsRZL];
  double para_End_CorrRZL_DATA[nGasTypes][nParametersEndcapRZL]; 

  static constexpr int nParametersLongStrawsMimic = 1800;
  static constexpr int nParametersShortStrawsMimic = 180;
  static constexpr int nParametersEndcapMimic = 560;

  float para_Long_MimicToXe_MC[nGasTypes][nParametersLongStrawsMimic];
  float para_Long_MimicToXe_DATA[nGasTypes][nParametersLongStrawsMimic];
  float para_Short_MimicToXe_MC[nGasTypes][nParametersShortStrawsMimic];
  float para_Short_MimicToXe_DATA[nGasTypes][nParametersShortStrawsMimic];
  float para_End_MimicToXe_MC[nGasTypes][nParametersEndcapMimic];
  float para_End_MimicToXe_DATA[nGasTypes][nParametersEndcapMimic];
 
  //==============================================================  
  

};
CLASS_DEF(TRTDedxcorrection,105466510,1)
CONDCONT_DEF(TRTDedxcorrection,114226988);

#endif  /* TRTDEDXCORRECTION_H */
