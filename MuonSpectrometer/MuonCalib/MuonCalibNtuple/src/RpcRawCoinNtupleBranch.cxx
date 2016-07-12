/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/RpcRawCoinNtupleBranch.h"

#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "MuonCalibEventBase/MuonCalibRawRpcCoin.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

  RpcRawCoinNtupleBranch::RpcRawCoinNtupleBranch(std::string branchName) : m_branchName(branchName), branchesInit(false), index(0)
  {}

  bool RpcRawCoinNtupleBranch::fillBranch(const MuonCalibRawRpcCoin &hit ) {
    // check if branches where initialized
    if( !branchesInit )
      return false;    
    
    // check if index not out of range 
    if( index >= m_blockSize || index < 0 )
      return false;

    // copy values 
    occupancy[index]  = hit.occupancy();
    id[index]         = hit.identify().getIdInt();
    t[index]          = hit.t() ;
    width[index]      = hit.width() ;
    length[index]     = hit.length() ;
    gPosX[index]      = hit.globalPosition().x();
    gPosY[index]      = hit.globalPosition().y();
    gPosZ[index]      = hit.globalPosition().z();
    ijk[index]            = hit.ijk();
    threshold[index]      = hit.threshold();
    overlap[index]        = hit.overlap();
    parentCmId[index]     = hit.parentCmId();
    parentPadId[index]    = hit.parentPadId();
    parentSectorId[index] = hit.parentSectorId();
    lowPtCm[index]        = hit.lowPtCm();

    ++index;
   
    return true;
  }  //end RpcRawCoinNtupleBranch::fillBranch

  bool RpcRawCoinNtupleBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree )
      return false;
    
    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name ="nRpcCoin";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name, &index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

    // create the branches
    branchCreator.createBranch( tree, "occupancy",      &occupancy,      array_size + "/I" );
    branchCreator.createBranch( tree, "id",             &id,             array_size + "/I" );
    branchCreator.createBranch( tree, "t",              &t,              array_size + "/F" );
    branchCreator.createBranch( tree, "width",          &width,          array_size + "/F" );
    branchCreator.createBranch( tree, "length",         &length,         array_size + "/F" );
    branchCreator.createBranch( tree, "gPosX",          &gPosX,          array_size + "/F" );
    branchCreator.createBranch( tree, "gPosY",          &gPosY,          array_size + "/F" );
    branchCreator.createBranch( tree, "gPosZ",          &gPosZ,          array_size + "/F" );
    branchCreator.createBranch( tree, "ijk",            &ijk,            array_size + "/I" );
    branchCreator.createBranch( tree, "threshold",      &threshold,      array_size + "/I" );
    branchCreator.createBranch( tree, "overlap",        &overlap,        array_size + "/I" );
    branchCreator.createBranch( tree, "parentCmId",     &parentCmId,     array_size + "/I" );
    branchCreator.createBranch( tree, "parentPadId",    &parentPadId,    array_size + "/I" );
    branchCreator.createBranch( tree, "parentSectorId", &parentSectorId, array_size + "/I" );
    branchCreator.createBranch( tree, "lowPtCm",        &lowPtCm,        array_size + "/I" );

    branchesInit = true;
  
    // reset branch
    reset();

    return true;
  }  //end RpcRawCoinNtupleBranch::createBranch

}  //end namespace MuonCalib 
