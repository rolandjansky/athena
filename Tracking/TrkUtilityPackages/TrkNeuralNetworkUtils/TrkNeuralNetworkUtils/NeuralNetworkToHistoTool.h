/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
#ifndef __NeuralNetworkToHistoTool_
#define __NeuralNetworkToHistoTool_

/******************************************************
    @class NeuralNetworkToHistoTool

    Created 19-2-2008
    @author Giacinto Piacquadio (giacinto.piacquadio AT physik.uni-freiburg.de)
********************************************************/

#include <TString.h>
#include <vector>
#include <map>
#include <string>
#include "AthenaBaseComps/AthAlgTool.h"

class TH1;
class TTrainedNetwork;

static const InterfaceID IID_NeuralNetworkToHistoTool("Trk::NeuralNetworkToHistoTool", 1, 0);

namespace Trk 
{
  

class NeuralNetworkToHistoTool : public AthAlgTool
{
public:
  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_NeuralNetworkToHistoTool; };

  NeuralNetworkToHistoTool(const std::string& name,
                              const std::string& n, const IInterface* p);

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  
  ~NeuralNetworkToHistoTool();

  // old style wrappers
  std::vector<TH1*> fromTrainedNetworkToHisto(TTrainedNetwork*) const;
  TTrainedNetwork* fromHistoToTrainedNetwork(std::vector<TH1*> &) const;

  // wrappers for new NN converter tool
  // note that root's internal hist names are mangled as a workarond for 
  // root's global variable problems. 
  std::map<std::string,TH1*> histsFromNetwork(TTrainedNetwork*) const;
  TTrainedNetwork* networkFromHists(std::map<std::string,TH1*> &) const;


private:
  
};
 
}//end namespace Trk


#endif
  
  
  
