/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTCHAMP_H
#define TRTCHAMP_H

#include <vector>
#include <string>

// Root libraries
#include "TMath.h"
#include "TApplication.h"
#include "TFile.h"
#include "TH1.h"
#include "TMinuit.h"

#include "CxxUtils/checker_macros.h"

//-----------------------------------------------------------------------------------------------//
//                                                                                               //
//     TRTChamp / TrtToolBetaLiklihood                                                           //
//     Authors: C. Gay, B. Mills, S. Schramm 2009-2011 Univ. British Columbia / ATLAS Canada     //
//     Contact developers at mills.wj@gmail.com                                                  //
//     Use default calibrations at your own risk!                                                //
//                                                                                               //
//-----------------------------------------------------------------------------------------------//

class ATLAS_NOT_THREAD_SAFE TrtToolBetaLiklihood { // This class uses global variables and function for TMinuit. However, they are not protected by mutex. This class is not thread safe.
  
public:
  TrtToolBetaLiklihood();
  
  // Some control parameters gathered here for convenience:
  // Values are given in TrtToolsBetaLiklihood.cpp at the top of the file
  // Done this way to fix the one-definition rule of C++
  // hit radius bins
  static const int NRFEBINS;
  static const double RFEMIN;
  static const double RFEMAX;
  
  //eta bins
  static const int NETABINS;
  static const double ETAMIN;
  static const double ETAMAX;
  static const double etadelta;
  static const double raddelta;

  //r(t) constants
  static const double RT0;
  static const double RT1;
  static const double RT2;
  static const double RT3;
  
  //struct to collect all the info parsed from a raw TRT hit:
  struct HIT {
    int LBit;   
    int FBit;
    int LGap;
    int BoT;
    int becIdx;  
    float t0;
    int nHT;
    float pathlen;
    float dlD;
    float dlR;   
    float hitR;
    float fitR;
    int cxnFail;
    float estDrift;
    int isTube;
  };
  
  
  //user's function for getting the ML estimate of beta from TRT FE info:
  std::vector<float> TRT_FEbeta(std::vector<int> TRT_bitpattern, std::vector<int> TRT_bec, std::vector<int> TRT_strawlayer, 
                   std::vector<int> TRT_layer, std::vector<float> TRT_t0, std::vector<float> TRT_estDrift, std::vector<float> TRT_R, 
                   std::vector<float> TRT_R_track, std::vector<float> TrackX, std::vector<float>TrackY, 
                   std::vector<float> TrackZ, float RecPt, float RecEta, std::vector<int> TRT_isTube);
  float TRT_CorrectFEbeta(float beta, float eta);                                                                 //applies bias correction to extracted FE beta.  

  void TRT_LoadPriors(std::string priorFileName);                                                                 //loads priors, LBitTemplates.root in the run dir
  
  // Load the default prior values
  int TRT_LoadDefaultPriors();
  // Specify how many prior values are expected
  unsigned int TRT_NumPriors();
  // Update a specific prior value
  void TRT_UpdatePriorValues(int radiusIndex, int etaIndex, int barrelOrEndcap, double* bitValues);

  //Prior generation - out of date, do not use!
  void  TRT_SetupPriors();                                                                                        //declare the empty histos for a new batch of priors
  void  TRT_FillPriors(std::vector<int> TRT_bitpattern, std::vector<int> TRT_bec, std::vector<int> TRT_strawlayer, 
                       std::vector<int> TRT_layer, std::vector<float> TRT_t0, std::vector<float> TRT_R,
                       std::vector<float> TRT_R_track, std::vector<float> TrackX, std::vector<float> TrackY, 
                       std::vector<float> TrackZ, float RecPt, float RecEta); 
  void TRT_SavePriors();

private:
  //internal functions, shouldn't be called except as per TRT_FEbeta:
  void TRT_SelectFEhits();                                                                                        //chooses decent looking hits to pass on to fitting
  void TRT_BitPattern(int hitP, int *lowPat, int *highPat, HIT &hit);                                             //parses TRT hits into useful info.  
  void TRT_PropLenCorr(HIT hit, int *corrLBits, float *corrLBitWeight, int *corrFBits, float *corrFBitWeight);    //fixes reflection in electronics.  
  void TRT_FEbeta_min ATLAS_NOT_THREAD_SAFE
    (float &beta, float &betaSigmaLo, float &betaSigmaUp);                                       //handles MINUIT for the TRT FE beta fit. 
  void TRT_SelectFEpriors(TrtToolBetaLiklihood::HIT hit, int *corrLBits, float *corrLBitWeight);
  //void TRT_solveCubic(float DCrad, float &estDrift);
};

#endif
