/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COPY_ROOT_TREE_HH
#define COPY_ROOT_TREE_HH

#include "tree_copy_opts.hh"

class TTree;

namespace H5 {
  class Group;
}

namespace h5 {
  void copy_root_tree(TTree& tt, H5::Group& fg, const TreeCopyOpts& opts);
}
#endif
