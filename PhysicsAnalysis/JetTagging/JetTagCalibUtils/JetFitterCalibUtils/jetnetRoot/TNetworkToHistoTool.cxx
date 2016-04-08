/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TH1F.h>
#include <TH2F.h>
#include "TTrainedNetwork.h"
#include "TNetworkToHistoTool.h"
#include <cmath>

ClassImp( TNetworkToHistoTool)

std::vector<TH1*> TNetworkToHistoTool::fromTrainedNetworkToHisto(TTrainedNetwork* trainedNetwork) const
{

  std::vector<TH1*> outputHistos;

  if (trainedNetwork->getActivationFunction()!=1)
  {
    cout << "ERROR: activation function is different from one. Only one is supported..." << endl;
    return outputHistos;
  }
    

  Int_t nInput=trainedNetwork->getnInput();
  vector<Int_t> nHiddenLayerSize=trainedNetwork->getnHiddenLayerSize();
  Int_t nHidden=nHiddenLayerSize.size();

  for (Int_t o=0;o<nHidden;++o)
  {
    cout << " Hidden lay: " << o << " size: " << nHiddenLayerSize[o];
  }
  
  Int_t nOutput=trainedNetwork->getnOutput();
  cout << " Output size: " << nOutput << endl;
  
  std::vector<TVectorD*> thresholdVectors=trainedNetwork->getThresholdVectors();
  std::vector<TMatrixD*> weightMatrices=trainedNetwork->weightMatrices();

  //LayersInfo

  TH1F* histoLayersInfo=new TH1F("LayersInfo",
                                 "LayersInfo",
                                 nHidden+2,
                                 0,
                                 nHidden+2);

  histoLayersInfo->SetBinContent(1,nInput);

  for (Int_t i=0;i<nHidden;++i)
  {
    histoLayersInfo->SetBinContent(2+i,nHiddenLayerSize[i]);
  }

  histoLayersInfo->SetBinContent(2+nHidden,nOutput);

  outputHistos.push_back(histoLayersInfo);

  
  //ThresholdInfo
  for (Int_t i=0;i<nHidden+1;++i)
  {
    TString threName("Layer");
    threName+=i;
    threName+="_thresholds";
   
    Int_t layerSize=(i<nHidden)?nHiddenLayerSize[i]:nOutput;
    Int_t previousLayerSize=(i==0)?nInput:nHiddenLayerSize[i-1];
    
    TH1F* histoThreshLayer=new TH1F(threName,
                                    threName,
                                    layerSize,
                                    0,
                                    layerSize);
    
    for (Int_t s=0;s<layerSize;s++)
    {
      histoThreshLayer->SetBinContent(s+1,thresholdVectors[i]->operator()(s));
    }

    TString weightsName("Layer");
    weightsName+=i;
    weightsName+="_weights";
    
    outputHistos.push_back(histoThreshLayer);

    TH2F* histoWeightsLayer=new TH2F(weightsName,
                                     weightsName,
                                     previousLayerSize,
                                     0,
                                     previousLayerSize,
                                     layerSize,
                                     0,
                                     layerSize);
    
    for (Int_t s=0;s<layerSize;s++)
    {
      for (Int_t p=0;p<previousLayerSize;++p)
      {
        histoWeightsLayer->SetBinContent(p+1,s+1,weightMatrices[i]->operator()(p,s));
      }
    }
    
    outputHistos.push_back(histoWeightsLayer);
    
  }
  

  return outputHistos;
  
}

TH1* TNetworkToHistoTool::findHisto(TString nameOfHisto,
                                     std::vector<TH1*> & inputHistos) const
{

  std::vector<TH1*>::const_iterator inputBegin=inputHistos.begin();
  std::vector<TH1*>::const_iterator inputEnd=inputHistos.end();
  
  for ( std::vector<TH1*>::const_iterator inputIter=inputBegin;inputIter!=inputEnd;++inputIter)
  {
    if ((*inputIter)->GetName()==nameOfHisto)
    {
      return (*inputIter);
    }
  }
  return 0;
}



TTrainedNetwork* TNetworkToHistoTool::fromHistoToTrainedNetwork(std::vector<TH1*> & inputHistos) const
{

  

  TH1F* histoLayersInfo=dynamic_cast<TH1F*>(findHisto("LayersInfo",inputHistos));

  if (histoLayersInfo==0)
  {
    cout << " Could not find LayersInfo histogram... Aborting " << endl;
    return 0;
  }


  Int_t nHidden=histoLayersInfo->GetNbinsX()-2;
  Int_t nInput=(Int_t)std::floor(histoLayersInfo->GetBinContent(1)+0.5);

  vector<Int_t> nHiddenLayerSize;
  for (Int_t i=0;i<nHidden;++i)
  {
    nHiddenLayerSize.push_back( (Int_t)std::floor(histoLayersInfo->GetBinContent(2+i)+0.5));
  }

  for (Int_t o=0;o<nHidden;++o)
  {
    cout << " Hidden lay: " << o << " size: " << nHiddenLayerSize[o];
  }

  Int_t nOutput=(Int_t)std::floor(histoLayersInfo->GetBinContent(2+nHidden)+0.5);
  cout << " Output size: " << nOutput << endl;
  
  std::vector<TVectorD*> thresholdVectors;
  std::vector<TMatrixD*> weightMatrices;


  //Reconstruct thresholdInfo
  for (Int_t i=0;i<nHidden+1;++i)
  {
    TString threName("Layer");
    threName+=i;
    threName+="_thresholds";

    Int_t layerSize=(i<nHidden)?nHiddenLayerSize[i]:nOutput;
    Int_t previousLayerSize=(i==0)?nInput:nHiddenLayerSize[i-1];

    TVectorD* thresholdVector=new TVectorD(layerSize);
    TMatrixD* weightMatrix=new TMatrixD(previousLayerSize,layerSize);
   
    TH1F* histoThreshLayer=dynamic_cast<TH1F*>(findHisto(threName,inputHistos));
    if (histoThreshLayer==0)
    {
      cout << " Could not find " << threName << "  histogram... Aborting (mem leak also...)" << endl;
      return 0;
    }


    for (Int_t s=0;s<layerSize;s++)
    {
      thresholdVector->operator()(s)=histoThreshLayer->GetBinContent(s+1);
    }

    TString weightsName("Layer");
    weightsName+=i;
    weightsName+="_weights";

    TH2F* histoWeightsLayer=dynamic_cast<TH2F*>(findHisto(weightsName,inputHistos));
    if (histoWeightsLayer==0)
    {
      cout << " Could not find " << weightsName << "  histogram... Aborting (mem leak also...)" << endl;
      return 0;
    }
    
    for (Int_t s=0;s<layerSize;s++)
    {
      for (Int_t p=0;p<previousLayerSize;++p)
      {
        weightMatrix->operator()(p,s)=histoWeightsLayer->GetBinContent(p+1,s+1);
      }
    }

    thresholdVectors.push_back(thresholdVector);
    weightMatrices.push_back(weightMatrix);

  }


  TTrainedNetwork* trainedNetwork=new TTrainedNetwork(nInput,
                                                      nHidden,
                                                      nOutput,
                                                      nHiddenLayerSize,
                                                      thresholdVectors,
                                                      weightMatrices,
                                                      1);
  return trainedNetwork;
  
}
        
  
  
