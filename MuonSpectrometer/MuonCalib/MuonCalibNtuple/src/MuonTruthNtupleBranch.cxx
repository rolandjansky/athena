/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//MuonCalib packages
#include "MuonCalibNtuple/MuonTruthNtupleBranch.h"
#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "MuonCalibEventBase/MuonCalibTruth.h"

//other packages
#include "CLHEP/Vector/ThreeVector.h"
#include "TTree.h"

// std packages
#include <iostream>

namespace MuonCalib {

  MuonTruthNtupleBranch::MuonTruthNtupleBranch(std::string branchName) : m_branchName(branchName),  m_branchesInit(false), m_first(true), m_index(0)
  {}

  bool MuonTruthNtupleBranch::fillBranch(const MuonCalibTruth &truth) {
    // check if branches were initialized
    if( !m_branchesInit ){
      return false;    
    }

    // check if index not out of range 
    if( m_index >= m_blockSize || m_index < 0 ){
      if (m_first == true) {
	m_first = false;
      }
      return false;
    }

    //    TrackRecord truth( const_cast<TrackRecord&>(ctruth) ); //Looks nasty, but TrackRecord.Get*() const is not defined=

    // copy values 
    m_kinEnergy[m_index] = truth.kinEnergy();
    m_gPosX[m_index] = truth.position().x();
    m_gPosY[m_index] = truth.position().y();
    m_gPosZ[m_index] = truth.position().z();
    m_pX[m_index] = truth.momentum().x();
    m_pY[m_index] = truth.momentum().y();
    m_pZ[m_index] = truth.momentum().z();
    m_PDGCode[m_index] = truth.PDGCode();
    m_barCode[m_index] = truth.barCode();

    // increment truth index
    ++m_index;
  
    return true;
  }  //end MuonTruthNtupleBranch::fillBranch

  bool MuonTruthNtupleBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree ){
      return false;
    }

    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name = "nTruth";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name, &m_index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

    // create the branches
    branchCreator.createBranch( tree, "kinEnergy", &m_kinEnergy,  array_size + "/D" );
    branchCreator.createBranch( tree, "gPosX",     &m_gPosX,      array_size + "/D" );
    branchCreator.createBranch( tree, "gPosY",     &m_gPosY,      array_size + "/D" );
    branchCreator.createBranch( tree, "gPosZ",     &m_gPosZ,      array_size + "/D" );
    branchCreator.createBranch( tree, "pX"   ,     &m_pX   ,      array_size + "/D" );
    branchCreator.createBranch( tree, "pY"   ,     &m_pY   ,      array_size + "/D" );
    branchCreator.createBranch( tree, "pZ"   ,     &m_pZ   ,      array_size + "/D" );
    branchCreator.createBranch( tree, "PDGCode",   &m_PDGCode,    array_size + "/I" );
    branchCreator.createBranch( tree, "barCode",   &m_barCode,    array_size + "/I" );

    m_branchesInit = true;

    // reset branch
    reset();

    return true;
  }  //end MuonTruthNtupleBranch::createBranch

}  //namespace MuonCalib
