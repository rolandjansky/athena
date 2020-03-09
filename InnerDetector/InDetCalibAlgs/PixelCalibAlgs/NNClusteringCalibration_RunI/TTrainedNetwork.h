/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCALIBALGS_TTRAINEDNETWORK_H
#define PIXELCALIBALGS_TTRAINEDNETWORK_H

#include "TObject.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include <math.h>
#include <vector>

/******************************************************
  @class TTrainedNetwork
  Created : 18-02-2008
  @author Giacinto Piacquadio (giacinto.piacquadio AT physik.uni-freiburg.de)
********************************************************/

class TTrainedNetwork : public TObject
{
 public:
  
  TTrainedNetwork();

  //class takes ownership of all pointers...
  TTrainedNetwork(Int_t nInput,
		  Int_t nHidden,
		  Int_t nOutput,
		  std::vector<Int_t> & nHiddenLayerSize,
		  std::vector<TVectorD*> & thresholdVectors,
		  std::vector<TMatrixD*> & weightMatrices,
		  Int_t activationFunction,
                  bool linearOutput=false,
                  bool normalizeOutput=false);

  ~TTrainedNetwork();

  void setNewWeights(std::vector<TVectorD*> & thresholdVectors,
		     std::vector<TMatrixD*> & weightMatrices);

  Int_t getnInput() const {return m_nInput;};

  Int_t getnHidden() const {return m_nHidden;};

  Int_t getnOutput() const {return m_nOutput;};

  const std::vector<Int_t> &  getnHiddenLayerSize() const {return m_nHiddenLayerSize;};

  Int_t getActivationFunction() const {return m_ActivationFunction;};

  const std::vector<TVectorD*> & getThresholdVectors() const {return m_ThresholdVectors;};

  const std::vector<TMatrixD*> & weightMatrices() const {return m_WeightMatrices;};

  std::vector<Double_t> calculateOutputValues(std::vector<Double_t> & input) const;

  bool getIfLinearOutput() const { return m_LinearOutput; };

  bool getIfNormalizeOutput() const { return m_NormalizeOutput; };

 private:

  Int_t m_nInput;
  Int_t m_nHidden;
  Int_t m_nOutput;

  std::vector<Int_t> m_nHiddenLayerSize;
  //Int_t* m_nHiddenLayerSize;

  std::vector<TVectorD*> m_ThresholdVectors;
  std::vector<TMatrixD*> m_WeightMatrices;
  //  TVectorD** m_ThresholdVectors;
  //  TMatrixD** m_WeightMatrices;
  Int_t m_ActivationFunction;

  bool m_LinearOutput;

  bool m_NormalizeOutput;

  Double_t sigmoid(Double_t x) const { return 1./(1.+exp(-2*x)); };

  ClassDef( TTrainedNetwork, 2 )

};

#endif
