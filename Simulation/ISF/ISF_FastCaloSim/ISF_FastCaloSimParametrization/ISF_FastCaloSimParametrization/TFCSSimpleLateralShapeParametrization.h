/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSSimpleLateralShapeParametrization_h
#define TFCSSimpleLateralShapeParametrization_h

#include "ISF_FastCaloSimParametrization/TFCSLateralShapeParametrization.h"

#include "TFile.h"
#include "TH2F.h"
#include "TF1.h"
#include "TRandom3.h"


class TFCSSimpleLateralShapeParametrization:public TFCSLateralShapeParametrization {
public:
  TFCSSimpleLateralShapeParametrization(const char* name=0, const char* title=0);

  // simulated one hit position with weight that should be put into simulstate
  // sometime later all hit weights should be resacled such that their final sum is simulstate->E(sample)
  // someone also needs to map all hits into cells
  virtual void simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);

  // Init and fill sigma
  bool Initialize(const char* filepath, const char* histname);

  bool Initialize(float input_sigma_x, float input_sigma_y)
  {
        // Setup random numbers
        rnd = new TRandom3();
        rnd->SetSeed(0);

        sigma_x = input_sigma_x;
        sigma_y = input_sigma_y;
        return true;
  };

  void getHitXY(double &x, double &y);

  float getSigma_x(){return sigma_x;};
  float getSigma_y(){return sigma_y;};
  


private:
  // simple shape information should be stored as private member variables here

  float sigma_x;
  float sigma_y;

  //float sigma2_x;
  //float sigma2_y;

  //float gaus_ratio;

  TRandom3 *rnd;


  ClassDef(TFCSSimpleLateralShapeParametrization,1)  //TFCSSimpleLateralShapeParametrization
};

#if defined(__MAKECINT__)
#pragma link C++ class TFCSSimpleLateralShapeParametrization;
#endif

#endif
