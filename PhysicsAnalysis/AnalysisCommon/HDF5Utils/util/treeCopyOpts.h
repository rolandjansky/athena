/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TREE_COPY_OPTS_HH
#define TREE_COPY_OPTS_HH

#include <string>
#include <vector>

#include "TTreeFormula.h"

namespace H5Utils {

  const size_t CHUNK_SIZE = 128;

  struct TreeCopyOpts
  {
    std::string branch_regex;
    std::vector<size_t> vector_lengths;
    size_t chunk_size;
    size_t n_entries;
    bool verbose;
    int print_interval;
    std::string selection;
  };

  struct IOOpts
  {
    std::vector<std::string> in;
    std::string out;
    std::string tree;
  };

  struct AppOpts
  {
    TreeCopyOpts tree;
    IOOpts file;
  };

  AppOpts getTreeCopyOpts(int argc, char* argv[]);

}

#endif
