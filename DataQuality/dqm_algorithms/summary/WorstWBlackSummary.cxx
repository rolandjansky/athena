/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file WorstWBlackSummary.cxx file makes a WorstWBlackSummary for the Parameters in the Region.  
 * \author Andrea Dotti
*/
#include <dqm_core/AlgorithmManager.h>
#include <dqm_core/Result.h> 	 
#include <dqm_core/Parameter.h>
#include <dqm_algorithms/summary/WorstWBlackSummary.h>

static dqm_algorithms::summary::WorstWBlackSummary myInstance;

dqm_algorithms::summary::WorstWBlackSummary::WorstWBlackSummary()
{
  dqm_core::AlgorithmManager::instance().registerSummaryMaker("WorstWBlackSummary",this);
}

dqm_algorithms::summary::WorstWBlackSummary::~WorstWBlackSummary()
{
}

dqm_algorithms::summary::WorstWBlackSummary*
dqm_algorithms::summary::WorstWBlackSummary::clone()
{
  return new WorstWBlackSummary();
}

dqm_core::Result*
dqm_algorithms::summary::WorstWBlackSummary::execute( const std::string & , 
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
    if(status==dqm_core::Result::Disabled) {
      ++black;
    } else if(status==dqm_core::Result::Red) {
      ++red;
    } else if (status==dqm_core::Result::Yellow) {
      ++yellow;
    } else if (status==dqm_core::Result::Green) {
      ++green;
    } else {
      ++undefined;
    }
  }

  dqm_core::Result *newresult = new dqm_core::Result();
  if ( black > 0) { 
    newresult->status_=dqm_core::Result::Disabled;
  } else if ( red > 0 ) {
    newresult->status_=dqm_core::Result::Red;
  } else if ( yellow > 0 ) {
    newresult->status_=dqm_core::Result::Yellow;
  } else if ( green > 0 ) {
    newresult->status_=dqm_core::Result::Green;
  } else {
    newresult->status_=dqm_core::Result::Undefined;
  }
  newresult->tags_.insert(std::make_pair("NumBlack",(double)black));
  newresult->tags_.insert(std::make_pair("NumRed",(double)red));
  newresult->tags_.insert(std::make_pair("NumYellow",(double)yellow));
  newresult->tags_.insert(std::make_pair("NumGreen",(double)green));
  newresult->tags_.insert(std::make_pair("NumUndefined",(double)undefined));
  newresult->tags_.insert(std::make_pair("NumExcluded",(double)skipped));
  return newresult;
}

