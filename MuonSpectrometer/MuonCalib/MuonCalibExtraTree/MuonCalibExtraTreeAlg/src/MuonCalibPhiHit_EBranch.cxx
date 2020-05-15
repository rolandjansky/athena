/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeAlg/MuonCalibPhiHit_EBranch.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"

#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

MuonCalibPhiHit_EBranch::MuonCalibPhiHit_EBranch(std::string branchName) : 
  m_branchName(branchName), m_branchesInit(false), m_first(true), m_index(0) {
}

bool MuonCalibPhiHit_EBranch::fillBranch(const MuonCalibHit_E &hit, const int patternIndex) {
  // check if branches where initialized
  if( !m_branchesInit ) {
    return false;    
  }

  // check if index not out of range 
  if( m_index >= s_blockSize || m_index < 0 ) {
    if (m_first == true) {
      m_first = false;
    }
    return false;
  }

  m_phiPatIndex[m_index]   = patternIndex;
  m_id[m_index]    = (hit.identify()).getIdInt();
  m_posX[m_index]  = hit.position().x();
  m_posY[m_index]  = hit.position().y();
  m_posZ[m_index]  = hit.position().z();
  m_error[m_index]  = hit.error();
  
  // increment hit index
  ++m_index;
  
  return true;
} // end MuonCalibPhiHit_EBranch::fillBranch

bool  MuonCalibPhiHit_EBranch::createBranch(TTree* tree) {
  // check if pointer is valid
  if( !tree ){
    return false;
  }

  // helper class to create branches in trees
  NtupleBranchCreator branchCreator(m_branchName);

  std::string index_name ="nPhiHits";

  // create a branch for every data member
  branchCreator.createBranch( tree, index_name, &m_index, "/I");

  // all entries of same size, the number of hits in the event
  std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

  // create the branches
  branchCreator.createBranch( tree, "phiPatIndex",  &m_phiPatIndex,  array_size + "/I" );
  branchCreator.createBranch( tree, "id",        &m_id,        array_size + "/i" );
  branchCreator.createBranch( tree, "posX",      &m_posX,      array_size + "/F" );
  branchCreator.createBranch( tree, "posY",      &m_posY,      array_size + "/F" );
  branchCreator.createBranch( tree, "posZ",      &m_posZ,      array_size + "/F" );
  branchCreator.createBranch( tree, "error",     &m_error,      array_size + "/F" );
  
  m_branchesInit = true;
  
  // reset branch
  reset();

  return true;
}  // end MuonCalibPhiHit_EBranch::createBranch

}  //namespace MuonCalib
