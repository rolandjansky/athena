/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file WorstCaseSummary.cxx file makes a WorstCaseSummary for the Parameters in the Region.  
 * \author Andrea Dotti
*/
#include <dqm_core/AlgorithmManager.h>
#include <dqm_core/Result.h> 	 
#include <dqm_core/Parameter.h>
#include <dqm_algorithms/summary/WorstCaseSummary.h>

static dqm_algorithms::summary::WorstCaseSummary myInstance;

dqm_algorithms::summary::WorstCaseSummary::WorstCaseSummary()
{
  dqm_core::AlgorithmManager::instance().registerSummaryMaker("WorstCaseSummary",this);
}

dqm_algorithms::summary::WorstCaseSummary::~WorstCaseSummary()
{
}

dqm_algorithms::summary::WorstCaseSummary*
dqm_algorithms::summary::WorstCaseSummary::clone()
{
  return new WorstCaseSummary();
}

dqm_core::Result*
dqm_algorithms::summary::WorstCaseSummary::execute( const std::string & , 
					     const dqm_core::Result &, 
					     const dqm_core::ParametersMap & map)
{
  dqm_core::ParametersMap::const_iterator iter;
  unsigned int black=0;
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
    if(status==dqm_core::Result::Red) {
      ++red;
    } else if (status==dqm_core::Result::Yellow) {
      ++yellow;
    } else if (status==dqm_core::Result::Green) {
      ++green;
    } else if (status==dqm_core::Result::Disabled) {
      ++black;
    } else {
      ++undefined;
    }
  }

  dqm_core::Result *newresult = new dqm_core::Result();
  
    if (black == map.size() ) {
       newresult->status_=dqm_core::Result::Disabled;
       return newresult;
    }


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
  newresult->tags_.insert(std::make_pair("NumDisabled",(double)black));
  newresult->tags_.insert(std::make_pair("NumExcluded",(double)skipped));
  return newresult;
}

