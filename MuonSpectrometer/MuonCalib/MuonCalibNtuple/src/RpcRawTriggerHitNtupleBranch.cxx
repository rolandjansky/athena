/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/RpcRawTriggerHitNtupleBranch.h"

#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "MuonCalibEventBase/MuonCalibRawRpcTriggerHit.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

  RpcRawTriggerHitNtupleBranch::RpcRawTriggerHitNtupleBranch(std::string branchName) : m_branchName(branchName), m_branchesInit(false), m_index(0)
  {}

  bool RpcRawTriggerHitNtupleBranch::fillBranch(const MuonCalibRawRpcTriggerHit &hit ) {
    // check if branches were initialized
    if( !m_branchesInit )
      return false;    
    
    // check if index not out of range 
    if( m_index >= m_blockSize || m_index < 0 )
      return false;

    // copy values 
    m_sector[m_index] = hit.sector();
    m_padId[m_index]  = hit.padId();
    m_status[m_index] = hit.status();
    m_ercode[m_index] = hit.errorCode();
    m_cmaId[m_index]  = hit.cmaId();
    m_fel1Id[m_index] = hit.fel1Id();
    m_febcId[m_index] = hit.febcId();
    m_crc[m_index]    = hit.crc();
    m_bcId[m_index]   = hit.bcId();
    m_ticks[m_index]  = hit.time();
    m_ijk[m_index]    = hit.ijk();
    m_cmachan[m_index] = hit.channel();
    m_overlap[m_index] = hit.overlap();
    m_threshold[m_index] = hit.threshold();

    ++m_index;
   
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
    branchCreator.createBranch( tree, index_name, &m_index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

    // create the branches
    branchCreator.createBranch( tree, "sector",    &m_sector,   array_size + "/I" );
    branchCreator.createBranch( tree, "padId",     &m_padId,    array_size + "/I" );
    branchCreator.createBranch( tree, "status",    &m_status,   array_size + "/I" );
    branchCreator.createBranch( tree, "ercode",    &m_ercode,   array_size + "/I" );
    branchCreator.createBranch( tree, "cmaId",     &m_cmaId,    array_size + "/I" );
    branchCreator.createBranch( tree, "fel1Id",    &m_fel1Id,   array_size + "/I" );
    branchCreator.createBranch( tree, "febcId",    &m_febcId,   array_size + "/I" );
    branchCreator.createBranch( tree, "crc",       &m_crc,      array_size + "/I" );  
    branchCreator.createBranch( tree, "bcId",      &m_bcId,     array_size + "/I" );  
    branchCreator.createBranch( tree, "ticks",     &m_ticks,    array_size + "/I" );  
    branchCreator.createBranch( tree, "ijk",       &m_ijk,      array_size + "/I" );  
    branchCreator.createBranch( tree, "cmachan",   &m_cmachan,  array_size + "/I" );  
    branchCreator.createBranch( tree, "overlap",   &m_overlap,  array_size + "/I" );  
    branchCreator.createBranch( tree, "threshold", &m_threshold,array_size + "/I" );  

    m_branchesInit = true;
  
    // reset branch
    reset();

    return true;
  }  //end RpcRawTriggerHitNtupleBranch::createBranch

}  //end namespace MuonCalib
