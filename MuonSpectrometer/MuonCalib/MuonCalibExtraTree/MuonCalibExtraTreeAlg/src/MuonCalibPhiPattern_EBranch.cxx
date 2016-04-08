/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeAlg/MuonCalibPhiPattern_EBranch.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTrack_E.h"

#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

MuonCalibPhiPattern_EBranch::MuonCalibPhiPattern_EBranch(std::string branchName) : 
  m_branchName(branchName), branchesInit(false), m_first(true), index(0) {    
}
  
bool  MuonCalibPhiPattern_EBranch::fillBranch(const MuonCalibTrack_E& pat) {
  // check if branches where initialized
  if( !branchesInit ) {
    //std::cout << "MuonCalibPhiPattern_EBranch::fillBranch  ERROR <branches where not initialized>"
    //	<<  std::endl;
    return false;    
  }
    
  // check if index out of range 
  if( index >= blockSize || index < 0 ) {
    if (m_first == true) {
      //std::cout << "MuonCalibPhiPattern_EBranch::fillBranch  ERROR <index out of range, hit not added to ntuple> "
      //  <<  index << std::endl;
	m_first = false;
    }
    return false;
  }
  d0[index]  = pat.d0();
  z0[index]  = pat.z0();
  phi[index]  = pat.phi();
  theta[index]  = pat.theta();
  qOverP[index]  = pat.qOverP();  

  // increment hit index
  ++index;
  
  return true;
}  // end MuonCalibPhiPattern_EBranch::fillBranch

bool  MuonCalibPhiPattern_EBranch::createBranch(TTree* tree) {
  // check if pointer is valid
  if( !tree ){
    //std::cout << "MuonCalibPhiPattern_EBranch::createBranch  ERROR <got invalid tree pointer> " 
    //	<< std::endl;
    return false;
  }

  // helper class to create branches in trees
  NtupleBranchCreator branchCreator(m_branchName);

  std::string index_name ="nPhiPatterns";

  // create a branch for every data member
  branchCreator.createBranch( tree, index_name, &index, "/I");

  // all entries of same size, the number of hits in the event
  std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

  // create the branches
  branchCreator.createBranch( tree, "z0",      &z0,      array_size + "/F" );
  branchCreator.createBranch( tree, "d0",      &d0,      array_size + "/F" );
  branchCreator.createBranch( tree, "phi",     &phi,     array_size + "/F" );
  branchCreator.createBranch( tree, "theta",   &theta,   array_size + "/F" );
  branchCreator.createBranch( tree, "qOverP",  &qOverP,  array_size + "/F" );

  branchesInit = true;
  
  // reset branch
  reset();
  
  return true;
}  // end MuonCalibPhiPattern_EBranch::createBranch

}  //namespace MuonCalib
