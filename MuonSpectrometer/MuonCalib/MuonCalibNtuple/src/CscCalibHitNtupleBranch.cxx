/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/CscCalibHitNtupleBranch.h"
#include "MuonCalibNtuple/NtupleBranchCreator.h"

#include "MuonCalibEventBase/CscCalibHitBase.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

  CscCalibHitNtupleBranch::CscCalibHitNtupleBranch(std::string branchName) : 
    m_branchName(branchName), m_branchesInit(false), m_index(0)
  {}

  bool CscCalibHitNtupleBranch::fillBranch(const CscCalibHitBase &hit, const int segmentIndex) {
    // check if branches where initialized
    if( !m_branchesInit ){
      return false;    
    }

    // check if m_index not out of range 
    if( m_index >= m_blockSize || m_index < 0 ){
      return false;
    }

    // copy values 
    m_segIndex[m_index]   = segmentIndex;
    m_id[m_index]         = (hit.identify()).getIdInt();
    m_nStrips[m_index]    = hit.nStrips();
    m_stripWidth[m_index] = hit.stripWidth();
    m_charge[m_index]     = hit.charge();
    m_error[m_index]      = hit.error();
    m_posX[m_index]       = hit.localPosition().x();
    m_posY[m_index]       = hit.localPosition().y();
    m_posZ[m_index]       = hit.localPosition().z();
    m_gPosX[m_index]      = hit.globalPosition().x();
    m_gPosY[m_index]      = hit.globalPosition().y();
    m_gPosZ[m_index]      = hit.globalPosition().z();

    ++m_index;    // increment hit index
    return true;
  }  //end CscCalibHitNtupleBranch::fillBranch

  bool CscCalibHitNtupleBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree ){
      return false;
    }

    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name ="nCsc";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name, &m_index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

    // create the branches
    branchCreator.createBranch( tree, "segIndex",  &m_segIndex,  array_size + "/I" );
    branchCreator.createBranch( tree, "id",        &m_id,        array_size + "/I" );
    branchCreator.createBranch( tree, "nStrips",   &m_nStrips,   array_size + "/I" );
    branchCreator.createBranch( tree, "stripWidth",&m_stripWidth,array_size + "/F" );
    branchCreator.createBranch( tree, "charge",    &m_charge,    array_size + "/I" );
    branchCreator.createBranch( tree, "error",     &m_error,     array_size + "/F" );
    branchCreator.createBranch( tree, "posX",      &m_posX,      array_size + "/F" );
    branchCreator.createBranch( tree, "posY",      &m_posY,      array_size + "/F" );
    branchCreator.createBranch( tree, "posZ",      &m_posZ,      array_size + "/F" );
    branchCreator.createBranch( tree, "gPosX",     &m_gPosX,     array_size + "/F" );
    branchCreator.createBranch( tree, "gPosY",     &m_gPosY,     array_size + "/F" );
    branchCreator.createBranch( tree, "gPosZ",     &m_gPosZ,     array_size + "/F" );

    m_branchesInit = true;
  
    reset();     // reset branch
    return true;
  }  //CscCalibHitNtupleBranch::createBranch

}  //namespace MuonCalib
