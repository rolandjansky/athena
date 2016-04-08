/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeAlg/MuonCalibPhiHit_EBranch.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"

#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

MuonCalibPhiHit_EBranch::MuonCalibPhiHit_EBranch(std::string branchName) : 
  m_branchName(branchName), branchesInit(false), m_first(true), index(0) {
}

bool MuonCalibPhiHit_EBranch::fillBranch(const MuonCalibHit_E &hit, const int patternIndex) {
  // check if branches where initialized
  if( !branchesInit ) {
    //std::cout << "MuonCalibPhiHit_EBranch::fillBranch  ERROR <branches where not initialized>"
    //	<<  std::endl;
    return false;    
  }

  // check if index not out of range 
  if( index >= blockSize || index < 0 ) {
    if (m_first == true) {
      //std::cout << "MuonCalibPhiHit_EBranch::fillBranch  ERROR <index out of range, hit not added to ntuple> "
      //  <<  index << std::endl;
      m_first = false;
    }
    return false;
  }

  phiPatIndex[index]   = patternIndex;
    
  //std::cout << " ***************** patHit index: " << patternIndex << std::endl;

  id[index]    = (hit.identify()).getIdInt();
  posX[index]  = hit.position().x();
  posY[index]  = hit.position().y();
  posZ[index]  = hit.position().z();
  error[index]  = hit.error();
  
  // increment hit index
  ++index;
  
  return true;
} // end MuonCalibPhiHit_EBranch::fillBranch

bool  MuonCalibPhiHit_EBranch::createBranch(TTree* tree) {
  // check if pointer is valid
  if( !tree ){
    //std::cout << "MuonCalibPhiHit_EBranch::createBranch  ERROR <got invalid tree pointer> " 
    //	<< std::endl;
    return false;
  }

  // helper class to create branches in trees
  NtupleBranchCreator branchCreator(m_branchName);

  std::string index_name ="nPhiHits";

  // create a branch for every data member
  branchCreator.createBranch( tree, index_name, &index, "/I");

  // all entries of same size, the number of hits in the event
  std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

  // create the branches
  branchCreator.createBranch( tree, "phiPatIndex",  &phiPatIndex,  array_size + "/I" );
  branchCreator.createBranch( tree, "id",        &id,        array_size + "/i" );
  branchCreator.createBranch( tree, "posX",      &posX,      array_size + "/F" );
  branchCreator.createBranch( tree, "posY",      &posY,      array_size + "/F" );
  branchCreator.createBranch( tree, "posZ",      &posZ,      array_size + "/F" );
  branchCreator.createBranch( tree, "error",     &error,      array_size + "/F" );
  
  branchesInit = true;
  
  // reset branch
  reset();

  return true;
}  // end MuonCalibPhiHit_EBranch::createBranch

}  //namespace MuonCalib
