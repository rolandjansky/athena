/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//! \file PercentSummary.cxx file makes a PercentSummary for the Parameters in the Region.  
//! Valerio Consorti valerio.consorti@roma1.infn.it


#include <iostream>

#include <dqm_core/AlgorithmManager.h>

#include <dqm_algorithms/summary/PercentSummary.h>
#include <dqm_core/Result.h> 	 
#include <dqm_core/Parameter.h>

using namespace std;

static dqm_algorithms::summary::PercentSummary myInstance;

dqm_algorithms::summary::PercentSummary::PercentSummary()
{
  dqm_core::AlgorithmManager::instance().registerSummaryMaker( "PercentSummary", this );
}

dqm_algorithms::summary::PercentSummary::~PercentSummary()
{
}

dqm_algorithms::summary::PercentSummary *
dqm_algorithms::summary::PercentSummary::clone()
{
  return new PercentSummary();
}

dqm_core::Result * 
dqm_algorithms::summary::PercentSummary::execute(	const std::string & , 
						const dqm_core::Result & , 
                        			const dqm_core::ParametersMap & map)
{
    
  dqm_core::ParametersMap::const_iterator iter;
  double rweight=0;
  double yweight=0;
  double gweight=0;
  
  double red=0;
  double yellow=0;
  double green=0;
  double undefined=0;

  double weight=0;
  double y_percent=0;
  double r_percent=0;
  double ry_percent=0;

  dqm_core::Result *newresult = new dqm_core::Result();
  
  for (iter=map.begin();iter!=map.end();iter++){
    dqm_core::Result::Status status=iter->second->getResult().get()->status_;
    if(status==dqm_core::Result::Red) {	
      rweight+=iter->second->getWeight();
      red++;      
    } else if (status==dqm_core::Result::Yellow) {
        yweight+=iter->second->getWeight();
	yellow++;
    } else if (status==dqm_core::Result::Green) {
        gweight+=iter->second->getWeight();
        green++;
    } else{
        undefined++;
    }
  }

  weight=rweight+yweight+gweight;

  if( red+green+yellow == 0){
    newresult->status_=dqm_core::Result::Undefined;
    return newresult;
  } else{
      if(weight==0){
        y_percent=yellow/(red+green+yellow+undefined);
        r_percent=red/(red+green+yellow+undefined);
        ry_percent=(red+yellow)/(red+green+yellow+undefined);
      } else{
          y_percent=yweight/(red+green+yellow+undefined);
          r_percent=rweight/(red+green+yellow+undefined);
          ry_percent=(yweight+rweight)/(red+green+yellow+undefined);
      }
  }

   
     if (r_percent>=0.50 || (ry_percent >= 0.50 && r_percent>0)) {
       newresult->status_=dqm_core::Result::Red;
     }else if (y_percent >= 0.50 || (r_percent>0.25 && r_percent<0.50)) {
      newresult->status_=dqm_core::Result::Yellow;
     }else {
       newresult->status_=dqm_core::Result::Green;
     }

     return newresult;

}
