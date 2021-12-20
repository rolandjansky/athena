/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkNeuralNetworkUtils/NeuralNetworkToHistoTool.h"

#include "GaudiKernel/MsgStream.h"
#include "TrkNeuralNetworkUtils/TTrainedNetwork.h"
#include "TrkNeuralNetworkUtils/NetworkToHistoTool.hh"

#include <TH1F.h>
#include <TH2F.h>
#include <cmath>

namespace Trk 
{
  

NeuralNetworkToHistoTool::NeuralNetworkToHistoTool(const std::string& name,
                                                         const std::string& n, const IInterface* p):
        AthAlgTool(name, n,p)
{
  declareInterface<NeuralNetworkToHistoTool>(this);
}

NeuralNetworkToHistoTool::~NeuralNetworkToHistoTool()
{}

StatusCode NeuralNetworkToHistoTool::initialize() {

  ATH_MSG_INFO(" Initialization of NeuralNetworkToHistoTool succesfull");
  return StatusCode::SUCCESS;
}

StatusCode  NeuralNetworkToHistoTool::finalize() {

  ATH_MSG_INFO(" Finalization of NeuralNetworkToHistoTool succesfull");
  return StatusCode::SUCCESS;
}


std::vector<TH1*> NeuralNetworkToHistoTool
::fromTrainedNetworkToHisto(TTrainedNetwork* trainedNetwork) 
{
  NetworkToHistoTool tool; 
  return tool.fromTrainedNetworkToHisto(trainedNetwork); 
}

TTrainedNetwork* NeuralNetworkToHistoTool
::fromHistoToTrainedNetwork(const std::vector<TH1*> & inputHistos) 
{
  NetworkToHistoTool tool; 
  return tool.fromHistoToTrainedNetwork(inputHistos); 
}

TTrainedNetwork* NeuralNetworkToHistoTool
::fromHistoToTrainedNetwork(const std::vector<const TH1*> & inputHistos) 
{
  NetworkToHistoTool tool; 
  return tool.fromHistoToTrainedNetwork(inputHistos); 
}

std::map<std::string,TH1*> NeuralNetworkToHistoTool
::histsFromNetwork(TTrainedNetwork* net) 
  
{
  NetworkToHistoTool tool; 
  return NetworkToHistoTool::histsFromNetwork(net); 
}

TTrainedNetwork* NeuralNetworkToHistoTool
::networkFromHists(const std::map<std::string,const TH1*>& hists) 
  
{
  NetworkToHistoTool tool; 
  return NetworkToHistoTool::networkFromHists(hists); 
}
}//end namespace Trk
  
  
