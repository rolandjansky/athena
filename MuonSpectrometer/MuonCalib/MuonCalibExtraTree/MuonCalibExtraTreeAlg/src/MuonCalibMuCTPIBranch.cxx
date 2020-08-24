/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeAlg/MuonCalibMuCTPIBranch.h"

#include "TrigT1Interfaces/RecMuonRoI.h"

#include "MuonCalibNtuple/NtupleBranchCreator.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

MuonCalibMuCTPIBranch::MuonCalibMuCTPIBranch(std::string branchName) : 
  m_branchName(branchName), m_branchesInit(false), m_first(true), m_index(0) {
}

bool  MuonCalibMuCTPIBranch::fillBranch(const LVL1::RecMuonRoI& hit, unsigned int roiBCID) {
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

  m_roiWord[m_index]         = hit.roiWord();
  m_bcID[m_index]            = roiBCID;
  m_sysID[m_index]           = hit.sysID();
  m_subsysID[m_index]        = hit.subsysID();
  m_sectorID[m_index]        = hit.sectorID();
  m_thresholdNumber[m_index] = hit.getThresholdNumber();
  m_thresholdValue[m_index]  = hit.getThresholdValue();
  m_roINumber[m_index]       = hit.getRoINumber();
  m_overlap[m_index]         = hit.getOverlap();
  m_firstCandidate[m_index]  = hit.firstCandidate();
  m_sectorOverflow[m_index]  = hit.sectorOverflow();
  m_padOverflow[m_index]     = hit.padOverflow();
  m_phi[m_index]             = hit.phi();
  m_eta[m_index]             = hit.eta();
    
  // increment hit index
  ++m_index;
  
  return true;
}  //end MuonCalibMuCTPIBranch::fillBranch

bool  MuonCalibMuCTPIBranch::createBranch(TTree* tree) {
  // check if pointer is valid
  if( !tree ){
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
  branchCreator.createBranch( tree, "roiWord",          &m_roiWord,          array_size + "/I" );
  branchCreator.createBranch( tree, "bcID",             &m_bcID,             array_size + "/I" );
  branchCreator.createBranch( tree, "sysID",            &m_sysID,            array_size + "/I" );
  branchCreator.createBranch( tree, "subsysID",         &m_subsysID,         array_size + "/I" );
  branchCreator.createBranch( tree, "sectorID",         &m_sectorID,         array_size + "/I" );
  branchCreator.createBranch( tree, "thresholdNumber",  &m_thresholdNumber,  array_size + "/I" );
  branchCreator.createBranch( tree, "thresholdValue",   &m_thresholdValue,   array_size + "/I" );
  branchCreator.createBranch( tree, "roINumber",        &m_roINumber,        array_size + "/I" );
  branchCreator.createBranch( tree, "overlap",          &m_overlap,          array_size + "/I" );
  branchCreator.createBranch( tree, "firstCandidate",   &m_firstCandidate,   array_size + "/I" );
  branchCreator.createBranch( tree, "sectorOverflow",   &m_sectorOverflow,   array_size + "/I" );
  branchCreator.createBranch( tree, "padOverflow",      &m_padOverflow,      array_size + "/I" );
  branchCreator.createBranch( tree, "phi",              &m_phi,              array_size + "/F" );
  branchCreator.createBranch( tree, "eta",              &m_eta,              array_size + "/F" );
  
  m_branchesInit = true;
  
  // reset branch
  reset();

  return true;
}  // end MuonCalibMuCTPIBranch::createBranch

}  //namespace MuonCalib
