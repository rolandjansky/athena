/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//-*-c++-*-
#ifndef NETWORK_TO_HISTO_TOOL_H
#define NETWORK_TO_HISTO_TOOL_H

#include <string>
#include <map>
#include <vector>

class TH1;
class TTrainedNetwork;


class NetworkToHistoTool 
{
public:
  
  NetworkToHistoTool() {};

  ~NetworkToHistoTool() {};

  static std::map<std::string,TH1*> histsFromNetwork(const TTrainedNetwork*) ;
  static TTrainedNetwork* networkFromHists(const std::map<std::string,const TH1*>&) ;

  static std::vector<TH1*> fromTrainedNetworkToHisto(const TTrainedNetwork*) ;
  static TTrainedNetwork* fromHistoToTrainedNetwork(const std::vector<TH1*>&) ; 
  static TTrainedNetwork* fromHistoToTrainedNetwork(const std::vector<const TH1*>&) ; 

private:
  
  
};


#endif
  
  
  
