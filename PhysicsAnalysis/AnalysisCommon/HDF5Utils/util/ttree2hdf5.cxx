/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "copyRootTree.h"
#include "getTree.h"
#include "treeCopyOpts.h"

#include "H5Cpp.h"

#include "TFile.h"
#include "TChain.h"

#include <iostream>
#include <memory>

int run(int argc, char* argv[]);

int main(int argc, char* argv[]) {
  try {
    return run(argc, argv);
  } catch (std::logic_error& e) {
    std::cerr << "ERROR: " << e.what() << ", quitting." << std::endl;
    return 1;
  }
}

int run(int argc, char* argv[]) {
  using namespace H5Utils;
  AppOpts opts = getTreeCopyOpts(argc, argv);

  // Read in the root tree. We pick whatever tree is on the top level
  // of the file. If there are two we throw an error.
  std::string tree_name = opts.file.tree;
  if (tree_name.size() == 0) tree_name = getTree(opts.file.in.at(0));
  if (opts.tree.verbose) std::cout << "tree: " << tree_name << std::endl;
  std::unique_ptr<TChain> chain(new TChain(tree_name.c_str()));
  for (const auto& file_name: opts.file.in) {
    if (opts.tree.verbose) std::cout << "adding " << file_name << std::endl;
    int ret_code = chain->Add(file_name.c_str(), -1);
    if (ret_code != 1) {
      std::cerr << "Tree '" << tree_name << "' is missing from "
                << file_name << std::endl;
      return 1;
    }
  }

  // make the output file
  H5::H5File out_file(opts.file.out, H5F_ACC_TRUNC);

  // All the magic appens here
  copyRootTree(*chain, out_file, opts.tree);

  return 0;
}
