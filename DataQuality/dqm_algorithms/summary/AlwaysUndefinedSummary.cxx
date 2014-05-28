/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file AlwaysUndefinedSummary.cxx
 * \author Akimasa Ishikawa (akimasa.ishikawa@cern.ch)
 */

#include <iostream>

#include <dqm_core/AlgorithmManager.h>
 
#include <dqm_algorithms/summary/AlwaysUndefinedSummary.h>
#include <dqm_core/Result.h> 	 
#include <dqm_core/Parameter.h>

using namespace std;

static dqm_algorithms::summary::AlwaysUndefinedSummary myInstance;

dqm_algorithms::summary::AlwaysUndefinedSummary::AlwaysUndefinedSummary()
{
  dqm_core::AlgorithmManager::instance().registerSummaryMaker( "AlwaysUndefinedSummary", this );
}

dqm_algorithms::summary::AlwaysUndefinedSummary::~AlwaysUndefinedSummary()
{
}

dqm_algorithms::summary::AlwaysUndefinedSummary *
dqm_algorithms::summary::AlwaysUndefinedSummary::clone()
{
  return new AlwaysUndefinedSummary();
}

dqm_core::Result * 
dqm_algorithms::summary::AlwaysUndefinedSummary::execute(	const std::string & , 
							const dqm_core::Result & , 
							const dqm_core::ParametersMap &)
{

  dqm_core::Result *newresult = new dqm_core::Result();
  newresult->status_=dqm_core::Result::Undefined;
  return newresult;

}
