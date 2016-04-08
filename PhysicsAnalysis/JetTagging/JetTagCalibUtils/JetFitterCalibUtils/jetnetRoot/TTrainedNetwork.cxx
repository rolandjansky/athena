/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTrainedNetwork.h"

ClassImp( TTrainedNetwork)

TTrainedNetwork::TTrainedNetwork()
{
  mnInput=0;
  mnHidden=0;
  mnOutput=0;
  mActivationFunction=1;
}

TTrainedNetwork::TTrainedNetwork(Int_t nInput,
		  Int_t nHidden,
		  Int_t nOutput,
		  std::vector<Int_t> & nHiddenLayerSize,
		  std::vector<TVectorD*> & thresholdVectors,
		  std::vector<TMatrixD*> & weightMatrices,
		  Int_t activationFunction)
{

  mnInput=nInput;
  mnHidden=nHidden;
  mnOutput=nOutput;
  mnHiddenLayerSize=nHiddenLayerSize;
  mThresholdVectors=thresholdVectors;
  mWeightMatrices=weightMatrices;
  mActivationFunction=activationFunction;
}

TTrainedNetwork::~TTrainedNetwork()
{
  std::vector<TVectorD*>::const_iterator vectBegin=mThresholdVectors.begin();
  std::vector<TVectorD*>::const_iterator vectEnd=mThresholdVectors.end();

  for (std::vector<TVectorD*>::const_iterator vectIter=vectBegin;
       vectIter!=vectEnd;
       ++vectIter)
  {
    delete *vectIter;
  }

  std::vector<TMatrixD*>::const_iterator matrixBegin=mWeightMatrices.begin();
  std::vector<TMatrixD*>::const_iterator matrixEnd=mWeightMatrices.end();

  for (std::vector<TMatrixD*>::const_iterator matrixIter=matrixBegin;
       matrixIter!=matrixEnd;
       ++matrixIter)
  {
    delete *matrixIter;
  }

}

void TTrainedNetwork::setNewWeights(std::vector<TVectorD*> & thresholdVectors,
				    std::vector<TMatrixD*> & weightMatrices)
{

  std::vector<TVectorD*>::const_iterator vectBegin=mThresholdVectors.begin();
  std::vector<TVectorD*>::const_iterator vectEnd=mThresholdVectors.end();

  for (std::vector<TVectorD*>::const_iterator vectIter=vectBegin;
       vectIter!=vectEnd;
       ++vectIter)
  {
    delete *vectIter;
  }

  std::vector<TMatrixD*>::const_iterator matrixBegin=mWeightMatrices.begin();
  std::vector<TMatrixD*>::const_iterator matrixEnd=mWeightMatrices.end();

  for (std::vector<TMatrixD*>::const_iterator matrixIter=matrixBegin;
       matrixIter!=matrixEnd;
       ++matrixIter)
  {
    delete *matrixIter;
  }

  mThresholdVectors.clear();
  mWeightMatrices.clear();

  mThresholdVectors=thresholdVectors;
  mWeightMatrices=weightMatrices;

}

std::vector<Double_t>  TTrainedNetwork::calculateOutputValues(std::vector<Double_t> & input) const 
{

  std::vector<Double_t> result;

  //now calculate the value using:
  TVectorD** resultVector=new TVectorD*[mnHidden+1];
  

  if (input.size()!=mnInput)
  {
    cout << " Input size: " << input.size() << " doesn't match with network: " << mnInput << endl;
    return result;
  }
  
  for (Int_t o=0;o<mnHidden+1;++o)
  {

    int sizeActualLayer=(o<mnHidden)?mnHiddenLayerSize[o]:mnOutput;
    int sizePreviousLayer=(o==0)?mnInput:mnHiddenLayerSize[o-1];

    resultVector[o]=new TVectorD(sizeActualLayer);
    
    for (Int_t s=0;s<sizeActualLayer;++s)
    {
      Double_t nodeValue=0.;
      if (o==0)
      {
        for (Int_t p=0;p<mnInput;++p)
        {
          nodeValue+=mWeightMatrices[o]->operator() (p,s)*input[p];
        }
      }
      else
      {
        for (Int_t p=0;p<mnHiddenLayerSize[o-1];++p)
        {
          nodeValue+=mWeightMatrices[o]->operator() (p,s)*resultVector[o-1]->operator()(p);
        }
      }
      nodeValue+=mThresholdVectors[o]->operator() (s);
      resultVector[o]->operator()(s) = sigmoid(nodeValue);
    }
  }      
  
  for (Int_t i=0;i<mnOutput;i++)
  {
    result.push_back(resultVector[mnHidden]->operator()(i));
  }

  for (Int_t o=0;o<mnHidden+1;++o)
  {
    int sizeActualLayer=(o<mnHidden)?mnHiddenLayerSize[o]:mnOutput;
    delete resultVector[o];
  }
  delete resultVector;

  return result;
  
}




