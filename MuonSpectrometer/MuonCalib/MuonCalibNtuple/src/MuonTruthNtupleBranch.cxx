/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  MuonTruthNtupleBranch::MuonTruthNtupleBranch(std::string branchName) : m_branchName(branchName),  branchesInit(false), m_first(true), index(0)
  {}

  bool MuonTruthNtupleBranch::fillBranch(const MuonCalibTruth &truth) {
//    std::cout << "Fill Truth Branch" << std::endl;
    // check if branches where initialized
    if( !branchesInit ){
      //std::cout << "MuonTruthNtupleBranch::fillBranch  ERROR <branches where not initialized>"
      //	<<  std::endl;
      return false;    
    }

    // check if index not out of range 
    if( index >= m_blockSize || index < 0 ){
      if (m_first == true) {
	//std::cout << "MuonTruthNtupleBranch::fillBranch  ERROR <index out of range, truth not added to ntuple> "
	//  << index << std::endl;
	m_first = false;
      }
      return false;
    }

    //    TrackRecord truth( const_cast<TrackRecord&>(ctruth) ); //Looks nasty, but TrackRecord.Get*() const is not defined=

    // copy values 
    kinEnergy[index] = truth.kinEnergy();
    gPosX[index] = truth.position().x();
    gPosY[index] = truth.position().y();
    gPosZ[index] = truth.position().z();
    pX[index] = truth.momentum().x();
    pY[index] = truth.momentum().y();
    pZ[index] = truth.momentum().z();
    PDGCode[index] = truth.PDGCode();
    barCode[index] = truth.barCode();

    // increment truth index
    ++index;
  
    return true;
  }  //end MuonTruthNtupleBranch::fillBranch

  bool MuonTruthNtupleBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree ){
      // std::cout << "MuonTruthNtupleBranch::createBranch  ERROR <got invalid tree pointer> " 
      //	<< std::endl;
      return false;
    }

    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name = "nTruth";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name, &index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

    // create the branches
    branchCreator.createBranch( tree, "kinEnergy", &kinEnergy,  array_size + "/D" );
    branchCreator.createBranch( tree, "gPosX",     &gPosX,      array_size + "/D" );
    branchCreator.createBranch( tree, "gPosY",     &gPosY,      array_size + "/D" );
    branchCreator.createBranch( tree, "gPosZ",     &gPosZ,      array_size + "/D" );
    branchCreator.createBranch( tree, "pX"   ,     &pX   ,      array_size + "/D" );
    branchCreator.createBranch( tree, "pY"   ,     &pY   ,      array_size + "/D" );
    branchCreator.createBranch( tree, "pZ"   ,     &pZ   ,      array_size + "/D" );
    branchCreator.createBranch( tree, "PDGCode",   &PDGCode,    array_size + "/I" );
    branchCreator.createBranch( tree, "barCode",   &barCode,    array_size + "/I" );

    branchesInit = true;

    // reset branch
    reset();

    return true;
  }  //end MuonTruthNtupleBranch::createBranch

}  //namespace MuonCalib
