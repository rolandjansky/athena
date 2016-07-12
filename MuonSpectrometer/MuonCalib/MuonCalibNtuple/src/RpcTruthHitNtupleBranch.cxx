/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/RpcTruthHitNtupleBranch.h"
#include "MuonCalibNtuple/NtupleBranchCreator.h"

#include "MuonCalibEventBase/MuonCalibRpcTruthHit.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

  RpcTruthHitNtupleBranch::RpcTruthHitNtupleBranch(std::string branchName) : m_branchName(branchName), branchesInit(false), index(0)
  {}

  bool RpcTruthHitNtupleBranch::fillBranch(const MuonCalibRpcTruthHit &hit) {
    // check if branches where initialized
    if( !branchesInit ){
      //std::cout << "RpcTruthHitNtupleBranch::fillBranch  ERROR <branches where not initialized>"
      //	<<  std::endl;
      return false;    
    }

    // check if index not out of range 
    if( index >= m_blockSize || index < 0 ){
//       std::cout << "RpcTruthHitNtupleBranch::fillBranch  ERROR <index out of range, hit not added to ntuple> "
// 		<<  index << std::endl;
      return false;
    }

    // copy values 
    id[index] = hit.identify().getIdInt();
    barCode[index] = hit.barCode();
    time[index] = hit.time();

    // increment hit index
    ++index;
  
    return true;
  }  //end RpcTruthHitNtupleBranch::fillBranch

  bool RpcTruthHitNtupleBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree ){
      //std::cout << "RpcTruthHitNtupleBranch::createBranch  ERROR <got invalid tree pointer> " 
      //	<< std::endl;
      return false;
    }

    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name ="nRpcTruthHit";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name, &index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

    // create the branches
    branchCreator.createBranch( tree, "id",      &id,      array_size + "/I" );
    branchCreator.createBranch( tree, "barCode", &barCode, array_size + "/I" );
    branchCreator.createBranch( tree, "time",    &time,    array_size + "/D" );

    branchesInit = true;
  
    // reset branch
    reset();

    return true;
  }  //end RpcTruthHitNtupleBranch::createBranch

}  //end namespace MuonCalib
