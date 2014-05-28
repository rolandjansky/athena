/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file AlwaysGreenSummary.cxx
 * \author Akimasa Ishikawa (akimasa.ishikawa@cern.ch)
 */

#include <iostream>

#include <dqm_core/AlgorithmManager.h>
 
#include <dqm_algorithms/summary/AlwaysGreenSummary.h>
#include <dqm_core/Result.h> 	 
#include <dqm_core/Parameter.h>

using namespace std;

static dqm_algorithms::summary::AlwaysGreenSummary myInstance;

dqm_algorithms::summary::AlwaysGreenSummary::AlwaysGreenSummary()
{
  dqm_core::AlgorithmManager::instance().registerSummaryMaker( "AlwaysGreenSummary", this );
}

dqm_algorithms::summary::AlwaysGreenSummary::~AlwaysGreenSummary()
{
}

dqm_algorithms::summary::AlwaysGreenSummary *
dqm_algorithms::summary::AlwaysGreenSummary::clone()
{
  return new AlwaysGreenSummary();
}

dqm_core::Result * 
dqm_algorithms::summary::AlwaysGreenSummary::execute(	const std::string & , 
							const dqm_core::Result & , 
							const dqm_core::ParametersMap &)
{

  dqm_core::Result *newresult = new dqm_core::Result();
  newresult->status_=dqm_core::Result::Green;
  return newresult;

}
