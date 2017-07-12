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
  
  int n_pcabins()        { return m_numberpcabins; };
  IntArray* get_layers() { return m_RelevantLayers; };
  
  void P2X(TVectorD*, TVectorD* , TMatrixD* , int, double* , double* , int);
  void loadInputs(TFile* file);
  void loadInputs(TFile* file,std::string);
  
 private:
  // PCA Matrix and NN mapping information should be stored as private member variables here
  
  IntArray* m_RelevantLayers;

  std::vector<TMatrixDSym*> m_symCov;
  std::vector<TMatrixD*>    m_EV;
  std::vector<TVectorD*>    m_MeanValues;
  std::vector<TVectorD*>    m_SigmaValues;
  std::vector<TVectorD*>    m_Gauss_means;
  std::vector<TVectorD*>    m_Gauss_rms;
  std::vector<TVectorD*>    m_LowerBounds;
  std::vector<std::vector<TFCS1DFunction*> > m_cumulative;
  
  int m_numberpcabins;
  
  ClassDef(TFCSPCAEnergyParametrization,1)  //TFCSPCAEnergyParametrization
 
};

#if defined(__MAKECINT__)
#pragma link C++ class TFCSPCAEnergyParametrization+;
#endif

#endif
