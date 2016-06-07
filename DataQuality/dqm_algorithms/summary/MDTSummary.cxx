/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file MDTSummary.cxx file implements the dqm_algorithms::summary::GnamDrawersSumamry class.
 * \author A. Dotti, added by R. Calkins
*/
#include <dqm_algorithms/summary/MDTSummary.h>
#include <dqm_core/AlgorithmManager.h>


static dqm_algorithms::summary::MDTSummary myInstance;
dqm_algorithms::summary::MDTSummary::MDTSummary()
{
  m_thresholdRed = 10;
  m_thresholdYellow = 5;
  dqm_core::AlgorithmManager::instance().registerSummaryMaker("MDTSummary",this);
}

dqm_algorithms::summary::MDTSummary*
dqm_algorithms::summary::MDTSummary::clone()
{
  return new MDTSummary();
}

dqm_core::Result*
dqm_algorithms::summary::MDTSummary::execute(const std::string&,
						 const dqm_core::Result&,
						 const dqm_core::ParametersMap& map)
{
  int y = 0 , r = 0;
  dqm_core::ParametersMap::const_iterator iter = map.begin();
  for ( ; iter != map.end() ; ++iter)
    {
      dqm_core::Result::Status mystatus = iter->second->getResult().get()->status_;
      if      (mystatus == dqm_core::Result::Red)    { ++r; }
      else if (mystatus == dqm_core::Result::Yellow) { ++y; }
    }
  const double n_problems = (double)r + 0.5*(double)y;

  dqm_core::Result* result = new dqm_core::Result(dqm_core::Result::Undefined);
  result->tags_["NYellows"]     = y;
  result->tags_["NReds"]        = r;
  result->tags_["RedThresh"]    = m_thresholdRed;
  result->tags_["YellowThresh"] = m_thresholdYellow;
  if ( n_problems > m_thresholdRed)
    {
      result->status_ = dqm_core::Result::Red;
    }
  else if (n_problems > m_thresholdYellow)
    {
      result->status_ = dqm_core::Result::Yellow;
    }
  else 
    {
      result->status_ = dqm_core::Result::Green;
    }
  // otherwise the status remains undefined, as constructed above
  return result;
}
