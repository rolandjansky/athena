/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISUTILS_AANTTREEMAP_H
#define ANALYSISUTILS_AANTTREEMAP_H

#include <map>
#include <string>

#include "TTree.h"

class AANTTreeMap
{
public:
  static void setTree (const std::string &stream, TTree *tree) { m_treeMap[stream] = tree;}
  static TTree * getTree (const std::string &stream) { return m_treeMap[stream]; }

private:
  /// tree map
  static std::map<std::string,TTree*> m_treeMap;
};

#endif
