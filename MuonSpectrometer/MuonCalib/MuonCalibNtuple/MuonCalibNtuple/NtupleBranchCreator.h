/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_NTUPLEBRANCHCREATOR_H
#define MUONCALIB_NTUPLEBRANCHCREATOR_H

#include "TTree.h"

#include <string>

namespace MuonCalib {

/** @class NtupleBranchCreator 
    To create a branch in a given TTree (NtupleBranch). 
   @author Niels.Van.Eldik@cern.ch
*/

  class NtupleBranchCreator {
  public:
    NtupleBranchCreator(std::string name) : m_name(name) {}

    void createBranch(TTree *tree, std::string entry_name, void *address, std::string entry_size) {
      /** 
	  The entry_name is passed into the Branch-function of the tree together with the address
	
	  The entry_size is given by a string which consists out of two parts:
	  - first if the variable is an array, the size of the array
	  - second the size of the type
	
	  example:  [nhits]/F
	
      */
      std::string element_name = m_name+entry_name;
      std::string element_size = element_name + entry_size;

      tree->Branch( element_name.c_str(), address, element_size.c_str(), 2000 );
    }
  private:
    std::string m_name;
  };

}  //namespace MuonCalib

#endif
