/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeAlg/MuonCalibMuCTPIBranch.h"

#include "TrigT1Interfaces/RecMuonRoI.h"

#include "MuonCalibNtuple/NtupleBranchCreator.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

MuonCalibMuCTPIBranch::MuonCalibMuCTPIBranch(std::string branchName) : 
  m_branchName(branchName), branchesInit(false), m_first(true), index(0) {
}

bool  MuonCalibMuCTPIBranch::fillBranch(const LVL1::RecMuonRoI& hit, unsigned int roiBCID) {
  // check if branches where initialized
  if( !branchesInit ) {
    //  std::cout << "MuonCalibMuCTPIBranch::fillBranch  ERROR <branches where not initialized>"
    //	<<  std::endl;
    return false;    
  }

  // check if index not out of range 
  if( index >= blockSize || index < 0 ) {
    if (m_first == true) {
      //std::cout << "MuonCalibMuCTPIBranch::fillBranch  ERROR <index out of range, hit not added to ntuple> "
      //  <<  index << std::endl;
      m_first = false;
    }
    return false;
  }

  roiWord[index]         = hit.roiWord();
  bcID[index]            = roiBCID;
  sysID[index]           = hit.sysID();
  subsysID[index]        = hit.subsysID();
  sectorID[index]        = hit.sectorID();
  thresholdNumber[index] = hit.getThresholdNumber();
  thresholdValue[index]  = hit.getThresholdValue();
  roINumber[index]       = hit.getRoINumber();
  overlap[index]         = hit.getOverlap();
  firstCandidate[index]  = hit.firstCandidate();
  sectorOverflow[index]  = hit.sectorOverflow();
  padOverflow[index]     = hit.padOverflow();
  phi[index]             = hit.phi();
  eta[index]             = hit.eta();
    
  // increment hit index
  ++index;
  
  return true;
}  //end MuonCalibMuCTPIBranch::fillBranch

bool  MuonCalibMuCTPIBranch::createBranch(TTree* tree) {
  // check if pointer is valid
  if( !tree ){
    //std::cout << "MuonCalibMuCTPIBranch::createBranch  ERROR <got invalid tree pointer> " 
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
  branchCreator.createBranch( tree, "roiWord",          &roiWord,          array_size + "/I" );
  branchCreator.createBranch( tree, "bcID",             &bcID,             array_size + "/I" );
  branchCreator.createBranch( tree, "sysID",            &sysID,            array_size + "/I" );
  branchCreator.createBranch( tree, "subsysID",         &subsysID,         array_size + "/I" );
  branchCreator.createBranch( tree, "sectorID",         &sectorID,         array_size + "/I" );
  branchCreator.createBranch( tree, "thresholdNumber",  &thresholdNumber,  array_size + "/I" );
  branchCreator.createBranch( tree, "thresholdValue",   &thresholdValue,   array_size + "/I" );
  branchCreator.createBranch( tree, "roINumber",        &roINumber,        array_size + "/I" );
  branchCreator.createBranch( tree, "overlap",          &overlap,          array_size + "/I" );
  branchCreator.createBranch( tree, "firstCandidate",   &firstCandidate,   array_size + "/I" );
  branchCreator.createBranch( tree, "sectorOverflow",   &sectorOverflow,   array_size + "/I" );
  branchCreator.createBranch( tree, "padOverflow",      &padOverflow,      array_size + "/I" );
  branchCreator.createBranch( tree, "phi",              &phi,              array_size + "/F" );
  branchCreator.createBranch( tree, "eta",              &eta,              array_size + "/F" );
  
  branchesInit = true;
  
  // reset branch
  reset();

  return true;
}  // end MuonCalibMuCTPIBranch::createBranch

}  //namespace MuonCalib
