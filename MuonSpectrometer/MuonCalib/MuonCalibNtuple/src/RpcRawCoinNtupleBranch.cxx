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

  RpcRawCoinNtupleBranch::RpcRawCoinNtupleBranch(std::string branchName) : m_branchName(branchName), m_branchesInit(false), m_index(0)
  {}

  bool RpcRawCoinNtupleBranch::fillBranch(const MuonCalibRawRpcCoin &hit ) {
    // check if branches were initialized
    if( !m_branchesInit )
      return false;    
    
    // check if index not out of range 
    if( m_index >= m_blockSize || m_index < 0 )
      return false;

    // copy values 
    m_occupancy[m_index]  = hit.occupancy();
    m_id[m_index]         = hit.identify().getIdInt();
    m_t[m_index]          = hit.t() ;
    m_width[m_index]      = hit.width() ;
    m_length[m_index]     = hit.length() ;
    m_gPosX[m_index]      = hit.globalPosition().x();
    m_gPosY[m_index]      = hit.globalPosition().y();
    m_gPosZ[m_index]      = hit.globalPosition().z();
    m_ijk[m_index]            = hit.ijk();
    m_threshold[m_index]      = hit.threshold();
    m_overlap[m_index]        = hit.overlap();
    m_parentCmId[m_index]     = hit.parentCmId();
    m_parentPadId[m_index]    = hit.parentPadId();
    m_parentSectorId[m_index] = hit.parentSectorId();
    m_lowPtCm[m_index]        = hit.lowPtCm();

    ++m_index;
   
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
    branchCreator.createBranch( tree, index_name, &m_index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

    // create the branches
    branchCreator.createBranch( tree, "occupancy",      &m_occupancy,      array_size + "/I" );
    branchCreator.createBranch( tree, "id",             &m_id,             array_size + "/I" );
    branchCreator.createBranch( tree, "t",              &m_t,              array_size + "/F" );
    branchCreator.createBranch( tree, "width",          &m_width,          array_size + "/F" );
    branchCreator.createBranch( tree, "length",         &m_length,         array_size + "/F" );
    branchCreator.createBranch( tree, "gPosX",          &m_gPosX,          array_size + "/F" );
    branchCreator.createBranch( tree, "gPosY",          &m_gPosY,          array_size + "/F" );
    branchCreator.createBranch( tree, "gPosZ",          &m_gPosZ,          array_size + "/F" );
    branchCreator.createBranch( tree, "ijk",            &m_ijk,            array_size + "/I" );
    branchCreator.createBranch( tree, "threshold",      &m_threshold,      array_size + "/I" );
    branchCreator.createBranch( tree, "overlap",        &m_overlap,        array_size + "/I" );
    branchCreator.createBranch( tree, "parentCmId",     &m_parentCmId,     array_size + "/I" );
    branchCreator.createBranch( tree, "parentPadId",    &m_parentPadId,    array_size + "/I" );
    branchCreator.createBranch( tree, "parentSectorId", &m_parentSectorId, array_size + "/I" );
    branchCreator.createBranch( tree, "lowPtCm",        &m_lowPtCm,        array_size + "/I" );

    m_branchesInit = true;
  
    // reset branch
    reset();

    return true;
  }  //end RpcRawCoinNtupleBranch::createBranch

}  //end namespace MuonCalib 
