/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/RpcRawTriggerHitNtupleBranch.h"

#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "MuonCalibEventBase/MuonCalibRawRpcTriggerHit.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

  RpcRawTriggerHitNtupleBranch::RpcRawTriggerHitNtupleBranch(std::string branchName) : m_branchName(branchName), branchesInit(false), index(0)
  {}

  bool RpcRawTriggerHitNtupleBranch::fillBranch(const MuonCalibRawRpcTriggerHit &hit ) {
    // check if branches where initialized
    if( !branchesInit )
      return false;    
    
    // check if index not out of range 
    if( index >= m_blockSize || index < 0 )
      return false;

    // copy values 
    sector[index] = hit.sector();
    padId[index]  = hit.padId();
    status[index] = hit.status();
    ercode[index] = hit.errorCode();
    cmaId[index]  = hit.cmaId();
    fel1Id[index] = hit.fel1Id();
    febcId[index] = hit.febcId();
    crc[index]    = hit.crc();
    bcId[index]   = hit.bcId();
    ticks[index]  = hit.time();
    ijk[index]    = hit.ijk();
    cmachan[index] = hit.channel();
    overlap[index] = hit.overlap();
    threshold[index] = hit.threshold();

    ++index;
   
    return true;
  }  //end RpcRawTriggerHitNtupleBranch::fillBranch

  bool RpcRawTriggerHitNtupleBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree )
      return false;
    
    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);
    
    std::string index_name ="fchan";   //index counting the number of fired channels
    
    // create a branch for every data member
    branchCreator.createBranch( tree, index_name, &index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

    // create the branches
    branchCreator.createBranch( tree, "sector",    &sector,   array_size + "/I" );
    branchCreator.createBranch( tree, "padId",     &padId,    array_size + "/I" );
    branchCreator.createBranch( tree, "status",    &status,   array_size + "/I" );
    branchCreator.createBranch( tree, "ercode",    &ercode,   array_size + "/I" );
    branchCreator.createBranch( tree, "cmaId",     &cmaId,    array_size + "/I" );
    branchCreator.createBranch( tree, "fel1Id",    &fel1Id,   array_size + "/I" );
    branchCreator.createBranch( tree, "febcId",    &febcId,   array_size + "/I" );
    branchCreator.createBranch( tree, "crc",       &crc,      array_size + "/I" );  
    branchCreator.createBranch( tree, "bcId",      &bcId,     array_size + "/I" );  
    branchCreator.createBranch( tree, "ticks",     &ticks,    array_size + "/I" );  
    branchCreator.createBranch( tree, "ijk",       &ijk,      array_size + "/I" );  
    branchCreator.createBranch( tree, "cmachan",   &cmachan,  array_size + "/I" );  
    branchCreator.createBranch( tree, "overlap",   &overlap,  array_size + "/I" );  
    branchCreator.createBranch( tree, "threshold", &threshold,array_size + "/I" );  

    branchesInit = true;
  
    // reset branch
    reset();

    return true;
  }  //end RpcRawTriggerHitNtupleBranch::createBranch

}  //end namespace MuonCalib
