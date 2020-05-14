/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/MdtTruthHitNtupleBranch.h"
#include "MuonCalibNtuple/NtupleBranchCreator.h"

#include "MuonCalibEventBase/MuonCalibMdtTruthHit.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

  MdtTruthHitNtupleBranch::MdtTruthHitNtupleBranch(std::string branchName) : m_branchName(branchName), m_branchesInit(false), m_index(0)
  {}

  bool MdtTruthHitNtupleBranch::fillBranch(const MuonCalibMdtTruthHit &hit) {
    // check if branches were initialized
    if( !m_branchesInit ){
      return false;    
    }

    // check if index not out of range 
    if( m_index >= m_blockSize || m_index < 0 ){
      return false;
    }

    // copy values 
    m_id[m_index] = hit.identify().getIdInt();
    m_barCode[m_index] = hit.barCode();
    m_driftRadius[m_index] = hit.driftRadius();
    m_positionAlongTube[m_index] = hit.positionAlongTube();
    m_gpositionX[m_index] = hit.gpositionX();
    m_gpositionY[m_index] = hit.gpositionY();
    m_gpositionZ[m_index] = hit.gpositionZ();
    m_time[m_index] = hit.time();

    // increment hit index
    ++m_index;
  
    return true;
  }  //end MdtTruthHitNtupleBranch::fillBranch

  bool MdtTruthHitNtupleBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree ){
      return false;
    }

    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name ="nMdtTruthHit";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name, &m_index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

    // create the branches
    branchCreator.createBranch( tree, "id",          &m_id,                array_size + "/I" );
    branchCreator.createBranch( tree, "barCode",     &m_barCode,           array_size + "/I" );
    branchCreator.createBranch( tree, "driftRadius", &m_driftRadius,       array_size + "/D" );
    branchCreator.createBranch( tree, "posAlongTube",&m_positionAlongTube, array_size + "/D" );
    branchCreator.createBranch( tree, "gPosX",       &m_gpositionX,        array_size + "/D" );
    branchCreator.createBranch( tree, "gPosY",       &m_gpositionY,        array_size + "/D" );
    branchCreator.createBranch( tree, "gPosZ",       &m_gpositionZ,        array_size + "/D" );
    branchCreator.createBranch( tree, "time",        &m_time,              array_size + "/D" );

    m_branchesInit = true;
  
    // reset branch
    reset();

    return true;
  }  //end MdtTruthHitNtupleBranch::createBranch

}  //namespace MuonCalib
