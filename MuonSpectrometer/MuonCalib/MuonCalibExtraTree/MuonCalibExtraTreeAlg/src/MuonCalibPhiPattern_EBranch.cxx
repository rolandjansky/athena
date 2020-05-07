/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeAlg/MuonCalibPhiPattern_EBranch.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTrack_E.h"

#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

MuonCalibPhiPattern_EBranch::MuonCalibPhiPattern_EBranch(std::string branchName) : 
  m_branchName(branchName), m_branchesInit(false), m_first(true), m_index(0) {    
}
  
bool  MuonCalibPhiPattern_EBranch::fillBranch(const MuonCalibTrack_E& pat) {
  // check if branches where initialized
  if( !m_branchesInit ) {
    return false;    
  }
    
  // check if index out of range 
  if( m_index >= s_blockSize || m_index < 0 ) {
    if (m_first == true) {
	m_first = false;
    }
    return false;
  }
  m_d0[m_index]  = pat.d0();
  m_z0[m_index]  = pat.z0();
  m_phi[m_index]  = pat.phi();
  m_theta[m_index]  = pat.theta();
  m_qOverP[m_index]  = pat.qOverP();  

  // increment hit index
  ++m_index;
  
  return true;
}  // end MuonCalibPhiPattern_EBranch::fillBranch

bool  MuonCalibPhiPattern_EBranch::createBranch(TTree* tree) {
  // check if pointer is valid
  if( !tree ){
    return false;
  }

  // helper class to create branches in trees
  NtupleBranchCreator branchCreator(m_branchName);

  std::string index_name ="nPhiPatterns";

  // create a branch for every data member
  branchCreator.createBranch( tree, index_name, &m_index, "/I");

  // all entries of same size, the number of hits in the event
  std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

  // create the branches
  branchCreator.createBranch( tree, "z0",      &m_z0,      array_size + "/F" );
  branchCreator.createBranch( tree, "d0",      &m_d0,      array_size + "/F" );
  branchCreator.createBranch( tree, "phi",     &m_phi,     array_size + "/F" );
  branchCreator.createBranch( tree, "theta",   &m_theta,   array_size + "/F" );
  branchCreator.createBranch( tree, "qOverP",  &m_qOverP,  array_size + "/F" );

  m_branchesInit = true;
  
  // reset branch
  reset();
  
  return true;
}  // end MuonCalibPhiPattern_EBranch::createBranch

}  //namespace MuonCalib
