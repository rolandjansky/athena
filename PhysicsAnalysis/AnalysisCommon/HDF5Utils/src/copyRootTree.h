/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COPY_ROOT_TREE_HH
#define COPY_ROOT_TREE_HH

#include "treeCopyOpts.h"

class TTree;

namespace H5 {
  class Group;
}

namespace ah5 {
  void copyRootTree(TTree& tt, H5::Group& fg, const TreeCopyOpts& opts);
}
#endif
