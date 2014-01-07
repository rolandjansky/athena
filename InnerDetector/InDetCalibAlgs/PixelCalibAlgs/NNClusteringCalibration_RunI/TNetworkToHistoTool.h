/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TNetworkToHistoTool_
#define __TNetworkToHistoTool_

#include <TString.h>
#include <vector>
#include "TObject.h"

class TH1;
class TTrainedNetwork;

//by Giacinto Piacquadio (19-2-2008)

class TNetworkToHistoTool : public TObject
{
public:
  
  TNetworkToHistoTool() {};

  ~TNetworkToHistoTool() {};

  std::vector<TH1*> fromTrainedNetworkToHisto(TTrainedNetwork*) const;
  
  TTrainedNetwork* fromHistoToTrainedNetwork(std::vector<TH1*> &) const;

private:

  TH1* findHisto(TString nameOfHisto,
                  std::vector<TH1*> & inputHistos) const;
  
  ClassDef( TNetworkToHistoTool, 1 );
  
};


#endif
  
  
  
