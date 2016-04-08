/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TString.h"

class TH1F;
class RooNDKeysPdf;
class RooRealVar;
void createTheHistograms(TString collectionName,
                         int flavourType);

void createTheHistograms(int flavourType,
                         TString collectionName)
{
  createTheHistograms( collectionName,
                       flavourType);
}



TH1F* makeHisto(RooNDKeysPdf* myKeysPdf,const char* name,int numtot,double begin,double end,RooRealVar* dep);

