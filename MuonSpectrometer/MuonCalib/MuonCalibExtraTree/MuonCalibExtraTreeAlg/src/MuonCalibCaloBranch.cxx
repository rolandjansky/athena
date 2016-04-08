/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeAlg/MuonCalibCaloBranch.h"
#include "MuonCalibExtraTreeEvent/MuonCalibCaloHit.h"

#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "MuonCalibIdentifier/MuonFixedId.h"


#include "TTree.h"

#include <iostream>

namespace MuonCalib {

MuonCalibCaloBranch::MuonCalibCaloBranch(std::string branchName) : 
  m_branchName(branchName), branchesInit(false), m_first(true), index(0) {
}

bool  MuonCalibCaloBranch::fillBranch(const MuonCalibCaloHit& hit) {
  // check if branches were initialized
  if( !branchesInit ){
    //std::cout << "MuonCalibCaloBranch::fillBranch  ERROR <branches were not initialized>"
    //	<<  std::endl;
    return false;    
  }

  // check if index not out of range 
  if( index >= blockSize || index < 0 ) {
    if (m_first == true) {
      //std::cout << "MuonCalibCaloBranch::fillBranch  ERROR <index out of range, hit not added to ntuple> "
      //  <<  index << std::endl;
      m_first = false;
    }
    return false;
  }
    
  id[index]    = hit.identify();
  posX[index]  = hit.position().x();
  posY[index]  = hit.position().y();
  posZ[index]  = hit.position().z();
  time[index]  = hit.time();
  charge[index] = hit.charge();

  // increment hit index
  ++index;
  
  return true;
}  //end MuonCalibCaloBranch::fillBranch

bool MuonCalibCaloBranch::createBranch(TTree* tree) {
  // check if pointer is valid
  if( !tree ) {
    //std::cout << "MuonCalibCaloBranch::createBranch  ERROR <got invalid tree pointer> " 
    //	<< std::endl;
    return false;
  }

  // helper class to create branches in trees
  NtupleBranchCreator branchCreator(m_branchName);
  
  std::string index_name ="nHits";

  // create a branch for every data member
  branchCreator.createBranch( tree, index_name, &index, "/I");

  // all entries of same size, the number of hits in the event
  std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

  // create the branches
  branchCreator.createBranch( tree, "id",          &id,          array_size + "/I" );
  branchCreator.createBranch( tree, "posX",        &posX,        array_size + "/F" );
  branchCreator.createBranch( tree, "posY",        &posY,        array_size + "/F" );
  branchCreator.createBranch( tree, "posZ",        &posZ,        array_size + "/F" );
  branchCreator.createBranch( tree, "time",        &time,        array_size + "/F" );
  branchCreator.createBranch( tree, "charge",      &charge,      array_size + "/F" );

  branchesInit = true;
  
  // reset branch
  reset();
    
  return true;
}  //end MuonCalibCaloBranch::createBranch

}  //namespace MuonCalib
