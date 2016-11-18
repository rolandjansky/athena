/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCSPCAEnergyParametrization_h
#define TFCSPCAEnergyParametrization_h

#include "ISF_FastCaloSimEvent/TFCSEnergyParametrization.h"
#include "ISF_FastCaloSimEvent/IntArray.h"
#include "ISF_FastCaloSimEvent/TFCS1DFunction.h"
#include "TMatrixF.h"
#include "TMatrixDSym.h"
#include "TVectorF.h"
#include "TFile.h"

class TFCSPCAEnergyParametrization:public TFCSEnergyParametrization
{
 public:
  TFCSPCAEnergyParametrization(const char* name=0, const char* title=0);
  
  // energies in calo layers should be returned in simulstate
  virtual void simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol);
  
  //int n_bins() {return 0;};
  // CHANGE: return number of energy parametrization bins
  int n_bins() { return (m_RelevantLayers->GetSize()+1); };
  void P2X(TMatrixD, int gNVariables, double *p, double *x, int nTest); 
  void P2X(int gNVariables, double *p, double *x, int nTest); 
  void loadInputs(TFile* file, int);
  void loadInputs(TFile* file);
  double InvCumulant(TH1D* hist,double y);
	  
 private:
  // PCA Matrix and NN mapping information should be stored as private member variables here
  
  TMatrixDSym* m_symCov;
  TMatrixD* m_EV;
  TVectorD* m_MeanValues;
  TVectorD* m_SigmaValues;
  TVectorD* m_Gauss_means;
  TVectorD* m_Gauss_rms; //Gauss nur einmal speichern!
  IntArray* m_RelevantLayers;
  TVectorD* m_LowerBounds;
  std::vector<TFCS1DFunction*> m_cumulative;
  std::vector<TH1D*> h_cumulative;
  
  ClassDef(TFCSPCAEnergyParametrization,1)  //TFCSPCAEnergyParametrization
 
};

//#if defined(__MAKECINT__)
//#pragma link C++ class TFCSPCAEnergyParametrization+;
//#endif

#endif

