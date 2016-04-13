/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSPCAEnergyParametrization_h
#define TFCSPCAEnergyParametrization_h

#include "ISF_FastCaloSimParametrization/TFCSEnergyParametrization.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TFile.h"

class TFCSPCAEnergyParametrization:public TFCSEnergyParametrization
{
 public:
  TFCSPCAEnergyParametrization(const char* name=0, const char* title=0);
  
  // energies in calo layers should be returned in simulstate
  virtual void simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);
  
  //int n_bins() {return 0;};
  // CHANGE: return number of energy parametrization bins
  int n_bins() {return m_EigenVectors.size();};
  void P2X(int gNVariables, TMatrixD *EigenVectors,TVectorD *MeanValues, TVectorD *SigmaValues, double *p, double *x, int nTest); 
  void loadInputs(TFile* file);
  
 private:
  // PCA Matrix and NN mapping information should be stored as private member variables here
  
  std::vector<TMatrixD*> m_EigenVectors;
  std::vector<TVectorD*> m_MeanValues;
  std::vector<TVectorD*> m_SigmaValues;
  std::vector<TVectorD*> m_RelevantLayers;
  std::vector<TVectorD*> m_Gauss_means;
  std::vector<TVectorD*> m_Gauss_rms;
  
  
  
  ClassDef(TFCSPCAEnergyParametrization,1)  //TFCSPCAEnergyParametrization
 
};

#if defined(__MAKECINT__)
#pragma link C++ class TFCSPCAEnergyParametrization;
#endif

#endif

