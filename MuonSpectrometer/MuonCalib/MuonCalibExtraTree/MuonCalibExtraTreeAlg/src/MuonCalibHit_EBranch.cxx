/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeAlg/MuonCalibHit_EBranch.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"

#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

MuonCalibHit_EBranch::MuonCalibHit_EBranch(std::string branchName) : 
  m_branchName(branchName), m_branchesInit(false), m_first(true), m_index(0) {
}

bool  MuonCalibHit_EBranch::fillBranch(const MuonCalibHit_E &hit, double drifttime, const int track_index) {
  // check if branches were initialized
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

  m_trackIndex[m_index] = track_index;

  m_id[m_index]    = (hit.identify()).getIdInt();
  m_posX[m_index]  = hit.position().x();
  m_posY[m_index]  = hit.position().y();
  m_posZ[m_index]  = hit.position().z();
  m_driftTime[m_index]   = drifttime;
  m_driftRadius[m_index] = hit.driftRadius();
  m_error[m_index] = hit.error();
  m_resi[m_index]  = hit.residual();
  m_pull[m_index]  = hit.pull();
  m_measType[m_index] = hit.type();

  // increment hit index
  ++m_index;
  
  return true;
}  //end MuonCalibHit_EBranch::fillBranch

bool MuonCalibHit_EBranch::createBranch(TTree* tree) {
  // check if pointer is valid
  if( !tree ){
    return false;
  }
  
  // helper class to create branches in trees
  NtupleBranchCreator branchCreator(m_branchName);

  std::string index_name = "nHits";

  // create a branch for every data member
  branchCreator.createBranch( tree, index_name, &m_index, "/I");

  // all entries of same size, the number of hits in the event
  std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

  // create the branches
  branchCreator.createBranch( tree, "trackIndex",  &m_trackIndex,  array_size + "/I" );
  branchCreator.createBranch( tree, "id",          &m_id,          array_size + "/i" );
  branchCreator.createBranch( tree, "posX",        &m_posX,        array_size + "/F" );
  branchCreator.createBranch( tree, "posY",        &m_posY,        array_size + "/F" );
  branchCreator.createBranch( tree, "posZ",        &m_posZ,        array_size + "/F" );
  branchCreator.createBranch( tree, "driftTime",   &m_driftTime,   array_size + "/F" );
  branchCreator.createBranch( tree, "driftRadius", &m_driftRadius, array_size + "/F" );
  branchCreator.createBranch( tree, "error",       &m_error,       array_size + "/F" );
  branchCreator.createBranch( tree, "resi",        &m_resi,        array_size + "/F" );
  branchCreator.createBranch( tree, "pull",        &m_pull,        array_size + "/F" );
  branchCreator.createBranch( tree, "type",        &m_measType,    array_size + "/I" );

  m_branchesInit = true;
  
  // reset branch
  reset();

  return true;
}  // end MuonCalibHit_EBranch::createBranch

}  //namespace MuonCalib
