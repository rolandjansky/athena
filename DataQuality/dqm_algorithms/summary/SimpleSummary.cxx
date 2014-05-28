/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file SimpleSummary.cxx file makes a SimpleSummary for the Parameters in the Region: returns the status with the highest total weight (relative majority)
 * \author Haleh Hadavand
 */

#include <iostream>

#include <dqm_core/AlgorithmManager.h>
 
#include <dqm_algorithms/summary/SimpleSummary.h>
#include <dqm_core/Result.h> 	 
#include <dqm_core/Parameter.h>

using namespace std;

static dqm_algorithms::summary::SimpleSummary myInstance;

dqm_algorithms::summary::SimpleSummary::SimpleSummary(){
  dqm_core::AlgorithmManager::instance().registerSummaryMaker( "SimpleSummary", this );
}

dqm_algorithms::summary::SimpleSummary::~SimpleSummary(){
}

dqm_algorithms::summary::SimpleSummary *
dqm_algorithms::summary::SimpleSummary::clone(){
  return new SimpleSummary();
}

dqm_core::Result * 
dqm_algorithms::summary::SimpleSummary::execute(	const std::string & , 
							const dqm_core::Result & , 
							const dqm_core::ParametersMap & map){
    
  dqm_core::ParametersMap::const_iterator iter;

  float rweight = 0;
  float yweight = 0;
  float gweight = 0;
    
  float ngrey   = 0;
  float nblack  = 0;
    
  for (iter=map.begin();iter!=map.end();iter++){
      
    dqm_core::Result::Status status=iter->second->getResult().get()->status_;

    if ( status == dqm_core::Result::Red ) {	
      rweight+=iter->second->getWeight();
    }else if ( status == dqm_core::Result::Yellow ) {
      yweight+=iter->second->getWeight();
    }else if ( status == dqm_core::Result::Green ) {
      gweight+=iter->second->getWeight();
    }else if ( status == dqm_core::Result::Undefined ) {
      ++ngrey;
    }else if ( status==dqm_core::Result::Disabled ) {
      ++nblack;
    }
  }
  

  dqm_core::Result *newresult = new dqm_core::Result();

  if ( nblack == map.size() ) {
    newresult->status_=dqm_core::Result::Disabled;
    return newresult;
  }
  if ( ( ngrey + nblack ) == map.size() ) {
    newresult->status_=dqm_core::Result::Undefined;
    return newresult;
  }

  float weight = gweight > ((rweight >= yweight) ? rweight : yweight) ? gweight : ((rweight >= yweight) ? rweight : yweight);
     
  if ( weight == 0 ) {
    newresult->status_=dqm_core::Result::Undefined;
    return newresult;
  } 

  if ( weight == rweight ) {
    newresult->status_=dqm_core::Result::Red;
  }else if ( weight == yweight ) {
    newresult->status_=dqm_core::Result::Yellow;
  } else {
    newresult->status_=dqm_core::Result::Green;
  }
 
  return newresult;

}
