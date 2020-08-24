/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/MdtRawHitNtupleBranch.h"
#include "MuonCalibNtuple/NtupleBranchCreator.h"

#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

  MdtRawHitNtupleBranch::MdtRawHitNtupleBranch(std::string branchName) : m_branchName(branchName), m_branchesInit(false), m_index(0)
  {}

  bool MdtRawHitNtupleBranch::fillBranch( const MuonCalibRawMdtHit &hit ) {
    // check if branches were initialized
    if( !m_branchesInit ){
      return false;    
    } 

    // check if index not out of range 
    if( m_index >= m_blockSize || m_index < 0 ){
      return false;
    }  
    
    // copy values 
    m_id[m_index]         = hit.identify().getIdInt();
    m_tdc[m_index]        = hit.tdcCount();
    m_adc[m_index]        = hit.adcCount();
    m_gPosX[m_index]      = hit.globalPosition().x();
    m_gPosY[m_index]      = hit.globalPosition().y();
    m_gPosZ[m_index]      = hit.globalPosition().z();

    // increment hit index
    ++m_index;
   
    return true;
  }  //end MdtRawHitNtupleBranch::fillBranch

  bool MdtRawHitNtupleBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree ){
      return false;
    }

    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name ="nRMdt";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name, &m_index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

    // create the branches
    branchCreator.createBranch( tree, "id",        &m_id,        array_size + "/I" );
    branchCreator.createBranch( tree, "tdc",       &m_tdc,       array_size + "/I" );
    branchCreator.createBranch( tree, "adc",       &m_adc,       array_size + "/I" );
    branchCreator.createBranch( tree, "gPosX",     &m_gPosX,     array_size + "/F" );
    branchCreator.createBranch( tree, "gPosY",     &m_gPosY,     array_size + "/F" );
    branchCreator.createBranch( tree, "gPosZ",     &m_gPosZ,     array_size + "/F" );

    m_branchesInit = true;
  
    // reset branch
    reset();

    return true;
  }  //end MdtRawHitNtupleBranch::createBranch

}  //namespace MuonCalib
