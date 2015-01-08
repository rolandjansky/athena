/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRec/JetSorter.h"

/////////////////////////////////////////
// Comparison functions
bool ptCompare(const xAOD::Jet* j1, const xAOD::Jet* j2){ return j1->pt() > j2->pt() ;} 

// A global map for comparison functions
typedef bool (*compFunc)(const xAOD::Jet*, const xAOD::Jet*);
std::map<std::string ,  compFunc> g_compFuncMap;

// init the global map
void initFuncMap(){
  static bool inited = false;
  if(inited) return;
  g_compFuncMap["ByPtDown"] = ptCompare;
}
/////////////////////////////////////////

  
JetSorter::JetSorter(const std::string& n) : 
  asg::AsgTool(n)
  ,m_sortType("ByPtDown")
{
  //declareInterface<IJetModifier>(this);
  declareProperty("SortingType", m_sortType);
  initFuncMap();
}

int JetSorter::modify(xAOD::JetContainer& jets) const {
  ATH_MSG_DEBUG(" Sorting : "<< m_sortType  << "  "<< jets.size() );
  // leave early if empty otherwise sort() throws an exception.
  if(jets.empty() ) return 1;

  jets.sort( *g_compFuncMap[ m_sortType ] );
  return 1;
}
