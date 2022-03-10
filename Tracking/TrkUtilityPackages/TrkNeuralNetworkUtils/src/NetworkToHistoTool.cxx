/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <TH1D.h>
#include <TH2D.h>
#include "TrkNeuralNetworkUtils/TTrainedNetwork.h"
#include "TrkNeuralNetworkUtils/NetworkToHistoTool.hh"
#include <cmath>
#include <stdexcept> 
#include <cassert>
#include <cstdlib> // rand
#include <boost/format.hpp>
//#include <boost/lexical_cast.hpp>

// ClassImp( TNetworkToHistoTool)

std::map<std::string,TH1*> 
NetworkToHistoTool::histsFromNetwork(const TTrainedNetwork* trainedNetwork) 
  
{

  std::map<std::string,TH1*> outputHistos;


  assert(trainedNetwork->getActivationFunction() == 1); 

  unsigned nInput=trainedNetwork->getnInput();
  std::vector<Int_t> nHiddenLayerSize=trainedNetwork->getnHiddenLayerSize();
  Int_t nHidden=nHiddenLayerSize.size();

  Int_t nOutput=trainedNetwork->getnOutput();
  
  std::vector<TVectorD*> thresholdVectors=trainedNetwork->getThresholdVectors();
  std::vector<TMatrixD*> weightMatrices=trainedNetwork->weightMatrices();

  //LayersInfo
  std::string li_string = (boost::format("LayersInfo_%i") % rand()).str(); 
  TH1D* histoLayersInfo=new TH1D(li_string.c_str(),
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

  //underflow for linear output
  if (trainedNetwork->getIfLinearOutput()){
    histoLayersInfo->SetBinContent(0,1);
  }
  //overflow for normalized output (Pott nodes)
  if (trainedNetwork->getIfNormalizeOutput()){
    histoLayersInfo->SetBinContent(nHidden+3,1);
  }


  outputHistos["LayersInfo"] = histoLayersInfo;
  
  //ThresholdInfo
  for (Int_t i=0;i<nHidden+1;++i)
  {
    std::string threName = (boost::format("Layer%i_thresholds") % i).str();
   
    Int_t layerSize=(i<nHidden)?nHiddenLayerSize[i]:nOutput;
    Int_t previousLayerSize=(i==0)?nInput:nHiddenLayerSize[i-1];

    std::string th_str = (boost::format("%s_%i") % threName % rand()).str(); 

    TH1D* histoThreshLayer=new TH1D(th_str.c_str(),
                                    threName.c_str(),
                                    layerSize,
                                    0,
                                    layerSize);
    
    for (Int_t s=0;s<layerSize;s++)
    {
      histoThreshLayer->SetBinContent(s+1,(*thresholdVectors[i])(s));
    }

    std::string weightsName = (boost::format("Layer%i_weights") % i).str();
    
    outputHistos[threName] = histoThreshLayer;

    std::string wt_str = (boost::format("%s_%i") % weightsName %
			  rand()).str(); 
    TH2D* histoWeightsLayer=new TH2D(wt_str.c_str(),
                                     weightsName.c_str(),
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
        histoWeightsLayer->SetBinContent(p+1,s+1,(*weightMatrices[i])(p,s));
      }
    }
    
    outputHistos[weightsName] = histoWeightsLayer;
    
  }

  using Input = TTrainedNetwork::Input; 
  std::vector<Input> inputs = trainedNetwork->getInputs(); 

  if (inputs.empty()) { 
    return outputHistos; 
  }
  assert(inputs.size() == nInput); 

  std::string ii_str = (boost::format("InputsInfo_%i") % rand()).str(); 
  TH2D* histoInputs = new TH2D(ii_str.c_str(), "InputsInfo",
			       nInput, 0, 1, 
			       2, 0, 1); 
  
  for (unsigned input_n = 0; input_n < nInput; input_n++ ) { 
    Input input = inputs.at(input_n); 
    histoInputs->SetBinContent(input_n + 1, 1, input.offset); 
    histoInputs->SetBinContent(input_n + 1, 2, input.scale); 
    histoInputs->GetXaxis()->SetBinLabel(input_n + 1, input.name.c_str());
  }
  outputHistos["InputsInfo"] = histoInputs; 

  return outputHistos;
  
}


TTrainedNetwork* 
NetworkToHistoTool::networkFromHists(const std::map<std::string,const TH1*>& inputHistos) 
  
{
  auto getHist = [&inputHistos] (const std::string& name) -> const TH1*
                 {
                   auto it = inputHistos.find (name);
                   if (it == inputHistos.end()) return nullptr;
                   return it->second;
                 };

  const TH1* histoLayersInfo = getHist ("LayersInfo");

  if (histoLayersInfo==nullptr)
  {
    throw std::runtime_error(" Could not find LayersInfo histogram..."); 
  }


  Int_t nHidden=histoLayersInfo->GetNbinsX()-2;
  unsigned nInput = static_cast<unsigned>
    (std::floor(histoLayersInfo->GetBinContent(1)+0.5));

  std::vector<Int_t> nHiddenLayerSize;
  nHiddenLayerSize.reserve(nHidden);

for (Int_t i=0;i<nHidden;++i)
  {
    nHiddenLayerSize.push_back( (Int_t)std::floor
				(histoLayersInfo->GetBinContent(2+i)+0.5));
  }

  Int_t nOutput=(Int_t)std::floor
    (histoLayersInfo->GetBinContent(2+nHidden)+0.5);

  unsigned options = 0; 
  if (histoLayersInfo->GetBinContent(0)>0.5)
  {
    options |= TTrainedNetwork::linearOutput;
  }
  if (histoLayersInfo->GetBinContent(nHidden+3)>0.5)
  {
    options |= TTrainedNetwork::normalizeOutput;
  }

  
  std::vector<TVectorD*> thresholdVectors;
  std::vector<TMatrixD*> weightMatrices;


  //Reconstruct thresholdInfo
  for (Int_t i=0;i<nHidden+1;++i)
  {
    std::string threName = (boost::format("Layer%i_thresholds") % i).str();

    Int_t layerSize=(i<nHidden)?nHiddenLayerSize[i]:nOutput;
    Int_t previousLayerSize=(i==0)?nInput:nHiddenLayerSize[i-1];

    TVectorD* thresholdVector=new TVectorD(layerSize);
    TMatrixD* weightMatrix=new TMatrixD(previousLayerSize,layerSize);

    const TH1* histoThreshLayer = getHist (threName);
    if (!histoThreshLayer)
      throw std::runtime_error("could not find " + threName); 

    std::string er_tpl = "inconsistency between LayersInfo and %s found: "
      "LayersInfo reports %i layers, %s has %i bins"; 

    if (layerSize != histoThreshLayer->GetNbinsX()) { 
      std::string err = (boost::format(er_tpl) % threName % layerSize % 
			 threName % histoThreshLayer->GetNbinsX()).str(); 
      throw std::runtime_error(err); 
    }

    for (Int_t s=0;s<layerSize;s++)
    {
      (*thresholdVector)(s) = histoThreshLayer->GetBinContent(s+1);
    }

    std::string weightsName = (boost::format("Layer%i_weights") % i).str();

    const TH1* histoWeightsLayer = getHist (weightsName);
    if (!histoWeightsLayer) { 
      throw std::runtime_error("could not find " + weightsName); 
    }

    if (layerSize != histoWeightsLayer->GetNbinsY()) { 
      std::string err = (boost::format(er_tpl) % weightsName % layerSize % 
			 weightsName % histoWeightsLayer->GetNbinsY()).str(); 
      throw std::runtime_error(err); 
    }


    for (Int_t s=0;s<layerSize;s++)
    {
      for (Int_t p=0;p<previousLayerSize;++p)
      {
        (*weightMatrix)(p,s) = histoWeightsLayer->GetBinContent(p+1,s+1);
      }
    }

    thresholdVectors.push_back(thresholdVector);
    weightMatrices.push_back(weightMatrix);

  }
  
  const TH1* histoInputs = getHist ("InputsInfo");
  std::vector<TTrainedNetwork::Input> inputs; 
  if (!histoInputs) { 
    for (unsigned i = 0 ; i < nInput; i++) { 
      TTrainedNetwork::Input the_input; 
      the_input.offset = 0; 
      // setting all scales to zero disables normalized output
      the_input.scale = 0; 	
      inputs.push_back(the_input); 
    }
  }
  else { 
    for (unsigned i = 0 ; i < nInput; i++) { 
      TTrainedNetwork::Input the_input; 
      the_input.name = histoInputs->GetXaxis()->GetBinLabel(i + 1); 
      the_input.offset = histoInputs->GetBinContent(i + 1, 1); 
      the_input.scale = histoInputs->GetBinContent(i + 1, 2); 
      inputs.push_back(the_input); 
    }
  }
  TTrainedNetwork* trainedNetwork = 
    new TTrainedNetwork(inputs,
			nOutput,
			thresholdVectors,
			weightMatrices, 
			TTrainedNetwork::SIGMOID, 
			options);
  return trainedNetwork;
  
}

std::vector<TH1*> NetworkToHistoTool
::fromTrainedNetworkToHisto(const TTrainedNetwork* net) 
{
  std::map<std::string, TH1*> hists = histsFromNetwork(net); 
  std::vector<TH1*> hist_vec; 
  for (std::map<std::string, TH1*>::const_iterator itr = hists.begin(); 
       itr != hists.end(); ++itr) { 
    itr->second->SetName(itr->first.c_str()); 
    hist_vec.push_back(itr->second); 
  }
  return hist_vec; 
}

TTrainedNetwork* NetworkToHistoTool
::fromHistoToTrainedNetwork(const std::vector<TH1*>& hists) 
{
  std::map<std::string, const TH1*> hist_map;
  for (const TH1* h : hists) {
    hist_map[h->GetName()] = h;
  }
  return networkFromHists(hist_map); 
}

TTrainedNetwork* NetworkToHistoTool
::fromHistoToTrainedNetwork(const std::vector<const TH1*>& hists) 
{
  std::map<std::string, const TH1*> hist_map;
  for (const TH1* h : hists) {
    hist_map[h->GetName()] = h;
  }
  return networkFromHists(hist_map); 
}
