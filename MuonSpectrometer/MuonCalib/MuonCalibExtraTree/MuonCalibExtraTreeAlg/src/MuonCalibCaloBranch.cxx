/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeAlg/MuonCalibCaloBranch.h"
#include "MuonCalibExtraTreeEvent/MuonCalibCaloHit.h"
#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

#include "TTree.h"
#include <iostream>

namespace MuonCalib {

MuonCalibCaloBranch::MuonCalibCaloBranch(std::string branchName) : 
  m_branchName(branchName), m_branchesInit(false), m_first(true), m_index(0) {
}

bool  MuonCalibCaloBranch::fillBranch(const MuonCalibCaloHit& hit) {
  // check if branches were initialized
  if( !m_branchesInit ){
    return false;    
  }

  // check if index not out of range 
  if( m_index >= s_blockSize || m_index < 0 ) {
    if (m_first == true) {
      m_first = false;
    }
    return false;
  }
    
  m_id[m_index]    = hit.identify();
  m_posX[m_index]  = hit.position().x();
  m_posY[m_index]  = hit.position().y();
  m_posZ[m_index]  = hit.position().z();
  m_time[m_index]  = hit.time();
  m_charge[m_index] = hit.charge();

  // increment hit index
  ++m_index;
  
  return true;
}  //end MuonCalibCaloBranch::fillBranch

bool MuonCalibCaloBranch::createBranch(TTree* tree) {
  // check if pointer is valid
  if( !tree ) {
    return false;
  }

  // helper class to create branches in trees
  NtupleBranchCreator branchCreator(m_branchName);
  
  std::string index_name ="nHits";

  // create a branch for every data member
  branchCreator.createBranch( tree, index_name, &m_index, "/I");

  // all entries of same size, the number of hits in the event
  std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

  // create the branches
  branchCreator.createBranch( tree, "id",          &m_id,          array_size + "/I" );
  branchCreator.createBranch( tree, "posX",        &m_posX,        array_size + "/F" );
  branchCreator.createBranch( tree, "posY",        &m_posY,        array_size + "/F" );
  branchCreator.createBranch( tree, "posZ",        &m_posZ,        array_size + "/F" );
  branchCreator.createBranch( tree, "time",        &m_time,        array_size + "/F" );
  branchCreator.createBranch( tree, "charge",      &m_charge,      array_size + "/F" );

  m_branchesInit = true;
  
  // reset branch
  reset();
    
  return true;
}  //end MuonCalibCaloBranch::createBranch

}  //namespace MuonCalib
