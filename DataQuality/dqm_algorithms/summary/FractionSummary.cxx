/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file FractionSummary.cxx file implements the dqm_algorithms::summary::GnamDrawersSumamry class.
 * \author A. Dotti, added by R. Calkins
*/
#include <dqm_algorithms/summary/FractionSummary.h>
#include <dqm_core/AlgorithmManager.h>


static dqm_algorithms::summary::FractionSummary myInstance;
dqm_algorithms::summary::FractionSummary::FractionSummary()
{
  m_fractionRed = 0.1;
  m_fractionYellow = 0.1;
  dqm_core::AlgorithmManager::instance().registerSummaryMaker("FractionSummary",this);
}

dqm_algorithms::summary::FractionSummary*
dqm_algorithms::summary::FractionSummary::clone()
{
  return new FractionSummary();
}

dqm_core::Result*
dqm_algorithms::summary::FractionSummary::execute(const std::string&,
						 const dqm_core::Result&,
						 const dqm_core::ParametersMap& map)
{
  int g = 0 , y = 0 , r = 0 , u = 0;
  dqm_core::ParametersMap::const_iterator iter = map.begin();
  for ( ; iter != map.end() ; ++iter)
    {
      dqm_core::Result::Status mystatus = iter->second->getResult().get()->status_;
      if      (mystatus == dqm_core::Result::Green)  { ++g; }
      else if (mystatus == dqm_core::Result::Yellow) { ++y; }
      else if (mystatus == dqm_core::Result::Red)    { ++r; }
      else                                           { ++u; }
    }
  const int total = g + y + r + u;
  const double RedFraction    = total ? ((double)r / total) : 0; // avoid to divide by zero (should never happen)
  const double YellowFraction = total ? ((double)y / total) : 0; // avoid to divide by zero (should never happen)

  dqm_core::Result* result = new dqm_core::Result(dqm_core::Result::Undefined);
  result->tags_["NGreens"]      = g;
  result->tags_["NYellows"]     = y;
  result->tags_["NReds"]        = r;
  result->tags_["NUndefineds"]  = u;
  result->tags_["RedThresh"]    = m_fractionRed;
  result->tags_["YellowThresh"] = m_fractionYellow;
  if (RedFraction > m_fractionRed)
    {
      result->status_ = dqm_core::Result::Red;
    }
  else if (YellowFraction > m_fractionYellow)
    {
      result->status_ = dqm_core::Result::Yellow;
    }
  else if (g > u)
    {
      result->status_ = dqm_core::Result::Green;
    }
  // otherwise the status remains undefined, as constructed above
  return result;
}
