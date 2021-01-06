/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TTrainedNetwork.h"
#include <iostream>

#include <vector>

using namespace std;

TTrainedNetwork::TTrainedNetwork()
  : m_nInput(0),
    m_nHidden(0),
    m_nOutput(0),
    m_ActivationFunction(1),
    m_LinearOutput(false),
    m_NormalizeOutput (false)
{
}

TTrainedNetwork::TTrainedNetwork(Int_t nInput,
                                 Int_t nHidden,
                                 Int_t nOutput,
                                 std::vector<Int_t> & nHiddenLayerSize,
                                 std::vector<TVectorD*> & thresholdVectors,
                                 std::vector<TMatrixD*> & weightMatrices,
                                 Int_t activationFunction,
                                 bool linearOutput,
                                 bool normalizeOutput)
  : m_nInput (nInput),
    m_nHidden (nHidden),
    m_nOutput (nOutput),
    m_nHiddenLayerSize (nHiddenLayerSize),
    m_ThresholdVectors (thresholdVectors),
    m_WeightMatrices (weightMatrices),
    m_ActivationFunction (activationFunction),
    m_LinearOutput (linearOutput),
    m_NormalizeOutput (normalizeOutput)
{
}

TTrainedNetwork::~TTrainedNetwork()
{
  std::vector<TVectorD*>::const_iterator vectBegin=m_ThresholdVectors.begin();
  std::vector<TVectorD*>::const_iterator vectEnd=m_ThresholdVectors.end();

  for (std::vector<TVectorD*>::const_iterator vectIter=vectBegin;
       vectIter!=vectEnd;
       ++vectIter)
  {
    delete *vectIter;
  }

  std::vector<TMatrixD*>::const_iterator matrixBegin=m_WeightMatrices.begin();
  std::vector<TMatrixD*>::const_iterator matrixEnd=m_WeightMatrices.end();

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

  std::vector<TVectorD*>::const_iterator vectBegin=m_ThresholdVectors.begin();
  std::vector<TVectorD*>::const_iterator vectEnd=m_ThresholdVectors.end();

  for (std::vector<TVectorD*>::const_iterator vectIter=vectBegin;
       vectIter!=vectEnd;
       ++vectIter)
  {
    delete *vectIter;
  }

  std::vector<TMatrixD*>::const_iterator matrixBegin=m_WeightMatrices.begin();
  std::vector<TMatrixD*>::const_iterator matrixEnd=m_WeightMatrices.end();

  for (std::vector<TMatrixD*>::const_iterator matrixIter=matrixBegin;
       matrixIter!=matrixEnd;
       ++matrixIter)
  {
    delete *matrixIter;
  }

  m_ThresholdVectors.clear();
  m_WeightMatrices.clear();

  m_ThresholdVectors=thresholdVectors;
  m_WeightMatrices=weightMatrices;

}

std::vector<Double_t>  TTrainedNetwork::calculateOutputValues(std::vector<Double_t> & input) const 
{

  std::vector<Double_t> result;

  //now calculate the value using:
  TVectorD** resultVector=new TVectorD*[m_nHidden+1];
  

  if ((int)input.size()!=m_nInput)
  {
    std::cout << " Input size: " << input.size() << " doesn't match with network: " << m_nInput << std::endl;
    delete[] resultVector;
    return result;
  }
  
  for (Int_t o=0;o<m_nHidden+1;++o)
  {

    int sizeActualLayer=(o<m_nHidden)?m_nHiddenLayerSize[o]:m_nOutput;
//    int sizePreviousLayer=(o==0)?m_nInput:m_nHiddenLayerSize[o-1];

    resultVector[o]=new TVectorD(sizeActualLayer);
    
    for (Int_t s=0;s<sizeActualLayer;++s)
    {
      Double_t nodeValue=0.;
      if (o==0)
      {
        for (Int_t p=0;p<m_nInput;++p)
        {
          nodeValue+=m_WeightMatrices[o]->operator() (p,s)*input[p];
        }
      }
      else
      {
        for (Int_t p=0;p<m_nHiddenLayerSize[o-1];++p)
        {
          nodeValue+=m_WeightMatrices[o]->operator() (p,s)*resultVector[o-1]->operator()(p);
        }
      }
      nodeValue+=m_ThresholdVectors[o]->operator() (s);
      if (o!=m_nHidden || (!m_LinearOutput))
      {
        resultVector[o]->operator()(s) = sigmoid(nodeValue);
      }
      else
      {
        resultVector[o]->operator()(s) = nodeValue;
      }
    }
  }      
  
  double sumLastLayer=0;

  if (m_NormalizeOutput)
  {
    for (Int_t i=0;i<m_nOutput;i++)
    {
      sumLastLayer+=resultVector[m_nHidden]->operator()(i);
    }
  }
  

  for (Int_t i=0;i<m_nOutput;i++)
  {
    if (!m_NormalizeOutput)
    {
      result.push_back(resultVector[m_nHidden]->operator()(i));
    }
    else
    {
      result.push_back(resultVector[m_nHidden]->operator()(i)/sumLastLayer);
    }
  }

  for (Int_t o=0;o<m_nHidden+1;++o)
  {
    //int sizeActualLayer=(o<m_nHidden)?m_nHiddenLayerSize[o]:m_nOutput;
    delete resultVector[o];
  }
  delete[] resultVector;

  return result;
  
}

ClassImp( TTrainedNetwork)




