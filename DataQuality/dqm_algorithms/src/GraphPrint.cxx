/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! author Jahred Adelman
*/
#include <dqm_algorithms/GraphPrint.h>
#include <TGraphErrors.h>
#include <TMath.h>
#include <TClass.h>
#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <dqm_core/Result.h>
#include <ers/ers.h>
#include <string>
#include <sstream>
#include <iostream>
#include <dqm_core/AlgorithmManager.h>

static dqm_algorithms::GraphPrint myInstance;

dqm_algorithms::GraphPrint::GraphPrint() {
  dqm_core::AlgorithmManager::instance().registerAlgorithm("GraphPrint",this);
}

dqm_algorithms::GraphPrint::~GraphPrint() {}

dqm_algorithms::GraphPrint* dqm_algorithms::GraphPrint::clone(){
  return new GraphPrint();
}

dqm_core::Result* dqm_algorithms::GraphPrint::execute(const std::string & name,
						     const TObject & obj, 
						      const dqm_core::AlgorithmConfig & /* config */ )
{
  TGraph* graph;
  if ( obj.IsA()->InheritsFrom("TGraph") )
    {
      ERS_DEBUG(2,"Got TGraph called: "<<obj.GetName()<<" of type:"<<obj.IsA()->GetName());
      graph = (TGraph*)&obj;
    } 
  else
    {
      throw dqm_core::BadConfig(ERS_HERE,name,"Object is not a TGraph ");
    }
  Int_t npoints = graph->GetN();

  // Everything is now configured correctly, we can start
  dqm_core::Result* result = new dqm_core::Result;
  for ( Int_t bin = 0 ; bin < npoints ; ++bin) //Loop on all points
    {
       Double_t x = 0, y = 0;
       graph->GetPoint(bin,x,y);
      // Check point error bars
       ERS_DEBUG(3,"Looking at point "<<bin<< " and I see x = " << x << " and y = " << y);
       std::string binname = Form("Candidate %d in LB %.0f, Event", bin, x);
       //std::string binname = Form("Candidate %d LB %d Event:", bin, int(x));
       result->tags_[binname.c_str()] = y;
	}
  
  result->status_=dqm_core::Result::Green;
  return result;
}

void
dqm_algorithms::GraphPrint::printDescription(std::ostream& out)
{
   out << " Just dump all the bin contents of a TGraph!" << std::endl;
}
