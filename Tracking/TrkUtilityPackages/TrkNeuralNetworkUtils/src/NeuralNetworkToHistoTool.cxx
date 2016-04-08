/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
::fromTrainedNetworkToHisto(TTrainedNetwork* trainedNetwork) const
{
  NetworkToHistoTool tool; 
  return tool.fromTrainedNetworkToHisto(trainedNetwork); 
}

TTrainedNetwork* NeuralNetworkToHistoTool
::fromHistoToTrainedNetwork(std::vector<TH1*> & inputHistos) const
{
  NetworkToHistoTool tool; 
  return tool.fromHistoToTrainedNetwork(inputHistos); 
}

std::map<std::string,TH1*> NeuralNetworkToHistoTool
::histsFromNetwork(TTrainedNetwork* net) 
  const 
{
  NetworkToHistoTool tool; 
  return tool.histsFromNetwork(net); 
}

TTrainedNetwork* NeuralNetworkToHistoTool
::networkFromHists(std::map<std::string,TH1*>& hists) 
  const 
{
  NetworkToHistoTool tool; 
  return tool.networkFromHists(hists); 
}
}//end namespace Trk
  
  
