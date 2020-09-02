/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/TgcTruthHitNtupleBranch.h"
#include "MuonCalibNtuple/NtupleBranchCreator.h"

#include "MuonCalibEventBase/MuonCalibTgcTruthHit.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

  TgcTruthHitNtupleBranch::TgcTruthHitNtupleBranch(std::string branchName) : m_branchName(branchName), m_branchesInit(false), m_index(0)
  {}

  bool  TgcTruthHitNtupleBranch::fillBranch( const MuonCalibTgcTruthHit &hit ) {
    // check if branches were initialized
    if( !m_branchesInit ) {
      return false;    
    }

    // check if index not out of range 
    if( m_index >= m_blockSize || m_index < 0 ) {
      return false;
    }

    // copy values 
    m_id[m_index] = hit.identify().getIdInt();
    m_barCode[m_index] = hit.barCode();
    m_time[m_index] = hit.time();

    // increment hit index
    ++m_index;
  
    return true;
  }

  bool  TgcTruthHitNtupleBranch::createBranch( TTree *tree ) {
    // check if pointer is valid
    if( !tree ){
      return false;
    }

    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name ="nTgcTruthHit";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name, &m_index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

    // create the branches
    branchCreator.createBranch( tree, "id",      &m_id,      array_size + "/I" );
    branchCreator.createBranch( tree, "barCode", &m_barCode, array_size + "/I" );
    branchCreator.createBranch( tree, "time",    &m_time,    array_size + "/D" );

    m_branchesInit = true;
  
    // reset branch
    reset();

    return true;
  }

}  //namespace MuonCalib
