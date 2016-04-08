/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeAlg/MuonCalibTrack_EBranch.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTrack_E.h"

#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "MuonCalibIdentifier/MuonFixedId.h"


#include "TTree.h"

#include <iostream>

namespace MuonCalib {

MuonCalibTrack_EBranch::MuonCalibTrack_EBranch(std::string branchName) : 
  m_branchName(branchName), branchesInit(false), m_first(true), index(0) {
}
  
bool  MuonCalibTrack_EBranch::fillBranch(const MuonCalibTrack_E &track) {
  // check if branches where initialized
  if( !branchesInit ) {
    //std::cout << "MuonCalibTrack_EBranch::fillBranch  ERROR <branches where not initialized>"
    //	<<  std::endl;
    return false;    
  }

  // check if index not out of range 
  if( index >= blockSize || index < 0 ) {
    if (m_first == true) {
      //std::cout << "MuonCalibTrack_EBranch::fillBranch  ERROR <index out of range, hit not added to ntuple> "
      //  <<  index << std::endl;
      m_first = false;
    }
    return false;
  }
  x0[index]  = track.x0();
  y0[index]  = track.y0();
  z0[index]  = track.z0();
  phi[index]  = track.phi();
  theta[index]  = track.theta();
  qOverP[index]  = track.qOverP();
//    errInvP[index] = track.errInvP();
  author[index] = track.author();
  cov00[index] = track.cov00();
  cov01[index] = track.cov01();
  cov02[index] = track.cov02();
  cov03[index] = track.cov03();
  cov04[index] = track.cov04();
  cov11[index] = track.cov11();
  cov12[index] = track.cov12();
  cov13[index] = track.cov13();
  cov14[index] = track.cov14();
  cov22[index] = track.cov22();
  cov23[index] = track.cov23();
  cov24[index] = track.cov24();
  cov33[index] = track.cov33();
  cov34[index] = track.cov34();
  cov44[index] = track.cov44();
  chi2[index]  = track.chi2();
  ndof[index] = track.ndof();

  // increment hit index
  ++index;
  
  return true;
}  // end MuonCalibTrack_EBranch::fillBranch

bool  MuonCalibTrack_EBranch::createBranch(TTree* tree) {
  // check if pointer is valid
  if( !tree ) {
    //std::cout << "MuonCalibTrack_EBranch::createBranch  ERROR <got invalid tree pointer> " 
    //	<< std::endl;
    return false;
  }

  // helper class to create branches in trees
  NtupleBranchCreator branchCreator(m_branchName);

  std::string index_name ="nTracks";

  // create a branch for every data member
  branchCreator.createBranch( tree, index_name, &index, "/I");

  // all entries of same size, the number of hits in the event
  std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

  // create the branches
  branchCreator.createBranch( tree, "x0",      &x0,      array_size + "/F" );
  branchCreator.createBranch( tree, "y0",      &y0,      array_size + "/F" );
  branchCreator.createBranch( tree, "z0",      &z0,      array_size + "/F" );
  branchCreator.createBranch( tree, "phi",     &phi,     array_size + "/F" );
  branchCreator.createBranch( tree, "theta",   &theta,   array_size + "/F" );
  branchCreator.createBranch( tree, "qOverP",  &qOverP,  array_size + "/F" );
//    branchCreator.createBranch( tree, "errInvP",  &errInvP,  array_size + "/F" );
  branchCreator.createBranch( tree, "author",  &author,  array_size + "/I" );
  branchCreator.createBranch( tree, "cov00",  &cov00,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov01",  &cov01,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov02",  &cov02,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov03",  &cov03,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov04",  &cov04,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov11",  &cov11,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov12",  &cov12,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov13",  &cov13,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov14",  &cov14,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov22",  &cov22,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov23",  &cov23,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov24",  &cov24,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov33",  &cov33,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov34",  &cov34,  array_size + "/F" );
  branchCreator.createBranch( tree, "cov44",  &cov44,  array_size + "/F" );
  branchCreator.createBranch( tree, "chi2",    &chi2,  array_size + "/F" );
  branchCreator.createBranch( tree, "ndof",  &ndof,  array_size + "/I" );
  
  branchesInit = true;
  
  // reset branch
  reset();
  
  return true;
}  // end MuonCalibTrack_EBranch::createBranch

}  //namespace MuonCalib
