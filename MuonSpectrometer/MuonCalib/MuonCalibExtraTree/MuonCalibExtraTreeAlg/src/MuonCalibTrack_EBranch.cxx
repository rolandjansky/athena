/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeAlg/MuonCalibTrack_EBranch.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTrack_E.h"

#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "MuonCalibIdentifier/MuonFixedId.h"


#include "TTree.h"

#include <iostream>

namespace MuonCalib {

MuonCalibTrack_EBranch::MuonCalibTrack_EBranch(std::string branchName) : 
  m_branchName(branchName), m_branchesInit(false), m_first(true), m_index(0) {
}
  
bool  MuonCalibTrack_EBranch::fillBranch(const MuonCalibTrack_E &track) {
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
  m_x0[m_index]  = track.x0();
  m_y0[m_index]  = track.y0();
  m_z0[m_index]  = track.z0();
  m_phi[m_index]  = track.phi();
  m_theta[m_index]  = track.theta();
  m_qOverP[m_index]  = track.qOverP();
  m_author[m_index] = track.author();
  m_cov00[m_index] = track.cov00();
  m_cov01[m_index] = track.cov01();
  m_cov02[m_index] = track.cov02();
  m_cov03[m_index] = track.cov03();
  m_cov04[m_index] = track.cov04();
  m_cov11[m_index] = track.cov11();
  m_cov12[m_index] = track.cov12();
  m_cov13[m_index] = track.cov13();
  m_cov14[m_index] = track.cov14();
  m_cov22[m_index] = track.cov22();
  m_cov23[m_index] = track.cov23();
  m_cov24[m_index] = track.cov24();
  m_cov33[m_index] = track.cov33();
  m_cov34[m_index] = track.cov34();
  m_cov44[m_index] = track.cov44();
  m_chi2[m_index]  = track.chi2();
  m_ndof[m_index] = track.ndof();

  // increment hit index
  ++m_index;
  
  return true;
}  // end MuonCalibTrack_EBranch::fillBranch

bool  MuonCalibTrack_EBranch::createBranch(TTree* tree) {
  // check if pointer is valid
  if( !tree ) {
    return false;
  }

  // helper class to create branches in trees
  NtupleBranchCreator branchCreator(m_branchName);

  std::string index_name ="nTracks";

  // create a branch for every data member
  branchCreator.createBranch( tree, index_name, &m_index, "/I");

  // all entries of same size, the number of hits in the event
  std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

  // create the branches
  branchCreator.createBranch( tree, "x0",      &m_x0,      array_size + "/F" );
  branchCreator.createBranch( tree, "y0",      &m_y0,      array_size + "/F" );
  branchCreator.createBranch( tree, "z0",      &m_z0,      array_size + "/F" );
  branchCreator.createBranch( tree, "phi",     &m_phi,     array_size + "/F" );
  branchCreator.createBranch( tree, "theta",   &m_theta,   array_size + "/F" );
  branchCreator.createBranch( tree, "qOverP",  &m_qOverP,  array_size + "/F" );
//    branchCreator.createBranch( tree, "errInvP",  &m_errInvP,  array_size + "/F" );
  branchCreator.createBranch( tree, "author",  &m_author,  array_size + "/I" );
  branchCreator.createBranch( tree, "cov00",  &m_cov00,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov01",  &m_cov01,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov02",  &m_cov02,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov03",  &m_cov03,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov04",  &m_cov04,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov11",  &m_cov11,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov12",  &m_cov12,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov13",  &m_cov13,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov14",  &m_cov14,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov22",  &m_cov22,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov23",  &m_cov23,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov24",  &m_cov24,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov33",  &m_cov33,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov34",  &m_cov34,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov44",  &m_cov44,  array_size + "/F" );
  branchCreator.createBranch( tree, "chi2",    &m_chi2,  array_size + "/F" );
  branchCreator.createBranch( tree, "ndof",  &m_ndof,  array_size + "/I" );
  
  m_branchesInit = true;
  
  // reset branch
  reset();
  
  return true;
}  // end MuonCalibTrack_EBranch::createBranch

}  //namespace MuonCalib
