/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRec/JetSorter.h"

/////////////////////////////////////////
// Comparison functions
bool ptCompare(const xAOD::Jet* j1, const xAOD::Jet* j2){ return j1->pt() > j2->pt() ;} 

// A global map for comparison functions
typedef bool (*compFunc)(const xAOD::Jet*, const xAOD::Jet*);
const std::map<std::string, compFunc> g_compFuncMap = {
  {"ByPtDown", ptCompare}
};

JetSorter::JetSorter(const std::string& n) :
  asg::AsgTool(n)
  ,m_sortType("ByPtDown")
{
  declareProperty("SortingType", m_sortType);
}

StatusCode JetSorter::modify(xAOD::JetContainer& jets) const {
  ATH_MSG_DEBUG(" Sorting : "<< m_sortType  << "  "<< jets.size() );
  // leave early if empty otherwise sort() throws an exception.
  if(jets.empty() ) return StatusCode::SUCCESS;

  const auto itr = g_compFuncMap.find(m_sortType);
  if (itr == g_compFuncMap.end()) {
    ATH_MSG_ERROR("Unsupported SortingType " << m_sortType);
    return StatusCode::FAILURE;
  }
  jets.sort( *itr->second );
  return StatusCode::SUCCESS;
}
