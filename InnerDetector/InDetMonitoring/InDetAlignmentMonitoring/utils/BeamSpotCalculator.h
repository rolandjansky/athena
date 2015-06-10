/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////
//  BeamSpotCalculator
////////////////////////
// Wrote by Javier Jimenez Pena

#include <iostream>
#include <string>

#include "TFile.h"
#include "TH3F.h"
#include "TF2.h"
#include "TProfile2D.h"
#include <TStyle.h>

//
// variables
//

TFile*  _InputFile;
TH3F*   _hd0phi0z0;
TProfile2D* _hd0phi0z0_prof;
TH1D*   _hd0_vs_phi0_prof;

TH1F*  _hbsX;  //TAG BeamSpot Histograms
TH1F*  _hbsY;
TH1F*  _hbsTiltX;
TH1F*  _hbsTiltY;

double GeneralParam[4], GeneralParamErrors[4]; //For saving the parameters of the 2-D fit
  

double ZZ;
//
// Prototypes
//
void   Initialize (string inputfile, string trkcollection);
void   AnalyseData (Double_t min, Double_t max);
void   SliceBySliceAnalysis (Double_t min , Double_t max);
void   TagBeamSpot();
void   DrawLine(int number);
void   DrawLineTilt(int number);

//Fit Functions
double d0_function (double *x, double *par);
double d0_function_slice(double *x, double *par);

//vectors to save the results of slice by slice fits.
const int vsize=50;
double m_z0[vsize],m_z0_error[vsize],m_x0[vsize],m_y0[vsize],m_x0_error[vsize],m_y0_error[vsize]; 




//////
