/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTDEDXCORRECTION_H
#define TRTDEDXCORRECTION_H


struct TRTDedxcorrection {
   
  static constexpr int nParametersTrackBaseddEdx = 100;
  static constexpr int nParametersHitBaseddEdx = 204;

  double hitOccPar[nParametersHitBaseddEdx] = {1};

  // TrackOccupancy calibration constants are separated in three arrays as we use a polynomial
  // function 2nd order of the form f(x)=a+b*x+c*x^2
  
  // TrackOccupancy calibration constants, HT hits are excluded, Pt>0.4 GeV d0<0.4 mm deltaZ0sin(theta)<0.4 mm
  double trackOccPar0NoHt[nParametersTrackBaseddEdx] = {1};
  double trackOccPar1NoHt[nParametersTrackBaseddEdx] = {1};
  double trackOccPar2NoHt[nParametersTrackBaseddEdx] = {1};

  // TrackOccupancy calibration constants, Pt>0.4 GeV d0<0.4 mm deltaZ0sin(theta)<0.4 mm
  double trackOccPar0[nParametersTrackBaseddEdx] = {1};
  double trackOccPar1[nParametersTrackBaseddEdx] = {1};
  double trackOccPar2[nParametersTrackBaseddEdx] = {1};

  static constexpr int nGasTypes = 3;
  
  double paraDivideByLengthDedxP1[nGasTypes];
  double paraDivideByLengthDedxP2[nGasTypes];
  double paraDivideByLengthDedxP3[nGasTypes];
  double paraDivideByLengthDedxP4[nGasTypes];
  double paraDivideByLengthDedxP5[nGasTypes];

  double paraDedxP1[nGasTypes];
  double paraDedxP2[nGasTypes];
  double paraDedxP3[nGasTypes];
  double paraDedxP4[nGasTypes];
  double paraDedxP5[nGasTypes];

      
  // resolution depends on the number of hits (and is different for e)
  static constexpr int nParametersResolution = 4; //Polynomial function 3rd used
  double resolution[nGasTypes][nParametersResolution];
  double resolutionElectron[nGasTypes][nParametersResolution];

  // corrections for pile-up (as a function of NVtx linear behavior observed)
  // was in principle also done separately for different detector regions
  // should be checked in more details when high pileup data available

  double normOffsetData[nGasTypes];  // offset in normalization between data and MC
  double normSlopeTot[nGasTypes];    // nvtx dependence for ToT
  double normSlopeTotDivideByLength[nGasTypes];   // nvtx dependence for ToT/L
  double normOffsetTot[nGasTypes];   // nvtx dependence for ToT
  double normOffsetTotDivideByLength[nGasTypes];  // nvtx dependence for ToT/L
  int normNzero[nGasTypes];           // for which average NVtx the fit parameters were determined

  static constexpr int nParametersLongStrawsRZ = 3240;
  static constexpr int nParametersShortStrawsRZ = 216;
  static constexpr int nParametersEndcapRZ = 336;
      
  double paraLongCorrRZ[nGasTypes][nParametersLongStrawsRZ];
  double paraShortCorrRZ[nGasTypes][nParametersShortStrawsRZ];
  double paraEndCorrRZ[nGasTypes][nParametersEndcapRZ];
  double paraLongCorrRZMC[nGasTypes][nParametersLongStrawsRZ];
  double paraShortCorrRZMC[nGasTypes][nParametersShortStrawsRZ];
  double paraEndCorrRZMC[nGasTypes][nParametersEndcapRZ];

  static constexpr int nParametersLongStrawsRZDivideByLength = 630;
  static constexpr int nParametersShortStrawsRZDivideByLength = 63;
  static constexpr int nParametersEndcapRZDivideByLength = 252;
  
  double paraLongCorrRZDivideByLengthMC[nGasTypes][nParametersLongStrawsRZDivideByLength];
  double paraShortCorrRZDivideByLengthMC[nGasTypes][nParametersShortStrawsRZDivideByLength];
  double paraEndCorrRZDivideByLengthMC[nGasTypes][nParametersEndcapRZDivideByLength];
  double paraLongCorrRZDivideByLengthDATA[nGasTypes][nParametersLongStrawsRZDivideByLength];
  double paraShortCorrRZDivideByLengthDATA[nGasTypes][nParametersShortStrawsRZDivideByLength];
  double paraEndCorrRZDivideByLengthDATA[nGasTypes][nParametersEndcapRZDivideByLength]; 

  static constexpr int nParametersLongStrawsMimic = 1800;
  static constexpr int nParametersShortStrawsMimic = 180;
  static constexpr int nParametersEndcapMimic = 560;

  float paraLongMimicToXeMC[nGasTypes][nParametersLongStrawsMimic];
  float paraLongMimicToXeDATA[nGasTypes][nParametersLongStrawsMimic];
  float paraShortMimicToXeMC[nGasTypes][nParametersShortStrawsMimic];
  float paraShortMimicToXeDATA[nGasTypes][nParametersShortStrawsMimic];
  float paraEndMimicToXeMC[nGasTypes][nParametersEndcapMimic];
  float paraEndMimicToXeDATA[nGasTypes][nParametersEndcapMimic];
 
  //==============================================================  
  

};
CLASS_DEF(TRTDedxcorrection,105466510,1)
CONDCONT_DEF(TRTDedxcorrection,114226988);

#endif  /* TRTDEDXCORRECTION_H */
