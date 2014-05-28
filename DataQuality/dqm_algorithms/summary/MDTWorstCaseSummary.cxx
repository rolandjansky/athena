/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* MDTWorstCaseSummary.cxx file makes a WorstCaseSummary from the LowStat MDT flags.  This loops over all results in MDT[BA][BC][EA][EC] and determines looks at ML Coverage plots, it
then computes the region coverage as a percentage of NBinsOn/TotalBins.  
 */
 
#include <dqm_core/AlgorithmManager.h>

#include <dqm_algorithms/summary/MDTWorstCaseSummary.h>

#include <string>

#include <dqm_core/Result.h> 	 
#include <dqm_core/Parameter.h>

static dqm_algorithms::summary::MDTWorstCaseSummary myInstance;

dqm_algorithms::summary::MDTWorstCaseSummary::MDTWorstCaseSummary()
{
  dqm_core::AlgorithmManager::instance().registerSummaryMaker("MDTWorstCaseSummary",this);
}

dqm_algorithms::summary::MDTWorstCaseSummary::~MDTWorstCaseSummary()
{
}

dqm_algorithms::summary::MDTWorstCaseSummary*
dqm_algorithms::summary::MDTWorstCaseSummary::clone()
{
  return new MDTWorstCaseSummary();
}

dqm_core::Result*
dqm_algorithms::summary::MDTWorstCaseSummary::execute( const std::string & , 
					     const dqm_core::Result &, 
					     const dqm_core::ParametersMap & map)
{
  dqm_core::ParametersMap::const_iterator iter;

  double TotalBins = 0;
  int NBins = 0;

  for (iter=map.begin();iter!=map.end();iter++){
    
    std::string name = (*iter).first;
    if(name.find("NumberOfHitsIn") == std::string::npos) continue;

    std::map<std::string,double> theTags = (*iter).second->getResult()->tags_;

    int nBins = 0;
    double ratio = 1.;
    double effCut = 1.;
    for(std::map<std::string,double>::const_iterator mitr = theTags.begin(); mitr != theTags.end(); ++mitr){
      std::string parameter = (*mitr).first;
      if(parameter == "NBins") {
	nBins = (*mitr).second;
      }
      if(parameter == "NBins_%"){
	ratio = (*mitr).second;
      }
      if(parameter == "Effective_BinThreshold") effCut = (*mitr).second;
    }
    
    if(ratio > 0 && effCut > 1.1) {
      TotalBins += nBins/ratio*100;
      NBins += nBins;
    }

  }

  dqm_core::Result *newresult = new dqm_core::Result();

  
  if( TotalBins < 100) newresult->status_ = dqm_core::Result::Undefined;
  else if( NBins*1./TotalBins < 0.9) newresult->status_ = dqm_core::Result::Red;
  else newresult->status_ = dqm_core::Result::Green;
  
  return newresult;

}

