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
        m_rnd = new TRandom3();
        m_rnd->SetSeed(0);

        m_sigmaX = input_sigma_x;
        m_sigmaY = input_sigma_y;
        return true;
  };

  void getHitXY(double &x, double &y);

  float getSigma_x(){return m_sigmaX;};
  float getSigma_y(){return m_sigmaY;};
  


private:
  // simple shape information should be stored as private member variables here

  float m_sigmaX;
  float m_sigmaY;

  //float sigma2_x;
  //float sigma2_y;

  //float gaus_ratio;

  TRandom3 *m_rnd;


  ClassDef(TFCSSimpleLateralShapeParametrization,1)  //TFCSSimpleLateralShapeParametrization
};

#endif
