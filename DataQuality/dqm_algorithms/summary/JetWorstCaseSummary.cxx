/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* JetWorstCaseSummary.cxx file makes a WorstCaseSummary from the LAr & TileCal flags, while returns only yellow/green flags if the Jet or CaloMon flags are red/yellow/green.  
 */
 
#include <dqm_core/AlgorithmManager.h>

#include <dqm_algorithms/summary/JetWorstCaseSummary.h>

#include <dqm_core/Result.h> 	 
#include <dqm_core/Parameter.h>

static dqm_algorithms::summary::JetWorstCaseSummary myInstance;

dqm_algorithms::summary::JetWorstCaseSummary::JetWorstCaseSummary()
{
  dqm_core::AlgorithmManager::instance().registerSummaryMaker("JetWorstCaseSummary",this);
}

dqm_algorithms::summary::JetWorstCaseSummary::~JetWorstCaseSummary()
{
}

dqm_algorithms::summary::JetWorstCaseSummary*
dqm_algorithms::summary::JetWorstCaseSummary::clone()
{
  return new JetWorstCaseSummary();
}

dqm_core::Result*
dqm_algorithms::summary::JetWorstCaseSummary::execute( const std::string & , 
					     const dqm_core::Result &, 
					     const dqm_core::ParametersMap & map)
{
  dqm_core::ParametersMap::const_iterator iter;
  unsigned int red=0;
  unsigned int yellow=0;
  unsigned int green=0;
  unsigned int undefined = 0;
  unsigned int skipped = 0;

  for (iter=map.begin();iter!=map.end();iter++){

    //If weight is 0 skip this result. Allow to "turn off" 
    // results in summary makers
    if ( iter->second->getWeight() == 0 ) 
      {
	ERS_DEBUG(2,"Skip result (weight 0): "<<iter->first);
	++skipped;
	continue;
      }
    dqm_core::Result::Status status=iter->second->getResult().get()->status_;

    if( (status==dqm_core::Result::Red && (iter->first == "JetBarrel" || iter->first == "JetASide" || iter->first == "JetCSide" ||iter->first == "CaloMonBAR" || iter->first == "CaloMonECA" || iter->first == "CaloMonECC" )) || status==dqm_core::Result::Yellow) {
      ++yellow;
     
    } else if(status==dqm_core::Result::Red) {
      ++red;
    } else if (status==dqm_core::Result::Green) {
      ++green;
    } else {
      ++undefined;
    }
  }

  dqm_core::Result *newresult = new dqm_core::Result();

  
  if ( red > 0 ) {
    newresult->status_=dqm_core::Result::Red;
  } else if ( yellow > 0 ) {
    newresult->status_=dqm_core::Result::Yellow;
  } else if ( green > 0 ) {
    newresult->status_=dqm_core::Result::Green;
  } else {
    newresult->status_=dqm_core::Result::Undefined;
  }
  newresult->tags_.insert(std::make_pair("NumRed",(double)red));
  newresult->tags_.insert(std::make_pair("NumYellow",(double)yellow));
  newresult->tags_.insert(std::make_pair("NumGreen",(double)green));
  newresult->tags_.insert(std::make_pair("NumUndefined",(double)undefined));
  newresult->tags_.insert(std::make_pair("NumExcluded",(double)skipped));
  return newresult;
}

