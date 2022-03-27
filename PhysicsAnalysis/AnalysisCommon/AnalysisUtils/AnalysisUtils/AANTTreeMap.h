/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISUTILS_AANTTREEMAP_H
#define ANALYSISUTILS_AANTTREEMAP_H

#include <map>
#include <string>
#include <mutex>

#include "TTree.h"
#include "CxxUtils/checker_macros.h"

class AANTTreeMap
{
public:
  static void setTree (const std::string &stream, TTree *tree)
  {
    std::scoped_lock lock (s_mutex);
    s_treeMap[stream] = tree;
  }
  static TTree * getTree (const std::string &stream)
  {
    std::scoped_lock lock (s_mutex);
    return s_treeMap[stream];
  }

private:
  /// tree map
  static std::mutex s_mutex;
  static std::map<std::string,TTree*> s_treeMap ATLAS_THREAD_SAFE;
};

#endif
