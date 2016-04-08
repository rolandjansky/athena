/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeAlg/MuonCalibHit_EBranch.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"

#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

MuonCalibHit_EBranch::MuonCalibHit_EBranch(std::string branchName) : 
  m_branchName(branchName), branchesInit(false), m_first(true), index(0) {
}

bool  MuonCalibHit_EBranch::fillBranch(const MuonCalibHit_E &hit, double drifttime, const int track_index) {
  // check if branches were initialized
  if( !branchesInit ) {
    return false;    
  }

  // check if index not out of range 
  if( index >= blockSize || index < 0 ) {
    if (m_first == true) {
      m_first = false;
    }
    return false;
  }

  trackIndex[index] = track_index;

  id[index]    = (hit.identify()).getIdInt();
  posX[index]  = hit.position().x();
  posY[index]  = hit.position().y();
  posZ[index]  = hit.position().z();
  driftTime[index]   = drifttime;
  driftRadius[index] = hit.driftRadius();
  error[index] = hit.error();
  resi[index]  = hit.residual();
  pull[index]  = hit.pull();
  measType[index] = hit.type();

  // increment hit index
  ++index;
  
  return true;
}  //end MuonCalibHit_EBranch::fillBranch

bool MuonCalibHit_EBranch::createBranch(TTree* tree) {
  // check if pointer is valid
  if( !tree ){
    //std::cout << "MuonCalibHit_EBranch::createBranch  ERROR <got invalid tree pointer> " 
    //	<< std::endl;
    return false;
  }
  
  // helper class to create branches in trees
  NtupleBranchCreator branchCreator(m_branchName);

  std::string index_name = "nHits";

  // create a branch for every data member
  branchCreator.createBranch( tree, index_name, &index, "/I");

  // all entries of same size, the number of hits in the event
  std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

  // create the branches
  branchCreator.createBranch( tree, "trackIndex",  &trackIndex,  array_size + "/I" );
  branchCreator.createBranch( tree, "id",          &id,          array_size + "/i" );
  branchCreator.createBranch( tree, "posX",        &posX,        array_size + "/F" );
  branchCreator.createBranch( tree, "posY",        &posY,        array_size + "/F" );
  branchCreator.createBranch( tree, "posZ",        &posZ,        array_size + "/F" );
  branchCreator.createBranch( tree, "driftTime",   &driftTime,   array_size + "/F" );
  branchCreator.createBranch( tree, "driftRadius", &driftRadius, array_size + "/F" );
  branchCreator.createBranch( tree, "error",       &error,       array_size + "/F" );
  branchCreator.createBranch( tree, "resi",        &resi,        array_size + "/F" );
  branchCreator.createBranch( tree, "pull",        &pull,        array_size + "/F" );
  branchCreator.createBranch( tree, "type",        &measType,    array_size + "/I" );

  branchesInit = true;
  
  // reset branch
  reset();

  return true;
}  // end MuonCalibHit_EBranch::createBranch

}  //namespace MuonCalib
