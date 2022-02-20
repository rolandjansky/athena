/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisUtils/AANTTreeMap.h"

std::mutex AANTTreeMap::s_mutex;
std::map<std::string,TTree*> AANTTreeMap::s_treeMap;
