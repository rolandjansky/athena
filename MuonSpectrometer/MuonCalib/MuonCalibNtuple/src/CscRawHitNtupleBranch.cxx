/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/CscRawHitNtupleBranch.h"
#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "MuonCalibEventBase/MuonCalibRawCscHit.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

  CscRawHitNtupleBranch::CscRawHitNtupleBranch(std::string branchName) : 
    m_branchName(branchName), branchesInit(false), index(0)
  {}

  bool CscRawHitNtupleBranch::fillBranch(const MuonCalibRawCscHit &hit ) {
    // check if branches where initialized
    if( !branchesInit )
      return false;    
    
    // check if index not out of range 
    if( index >= m_blockSize || index < 0 )
      return false;

    //    Trk::GlobalPosition global = hit.globalP
 
    // copy values 
    occupancy[index] = hit.occupancy() ;
    id[index]        = hit.identify().getIdInt();
    width[index]     = hit.width() ;
    charge[index]    = hit.charge() ;
    t[index]         = hit.t() ;
    gPosX[index]     = hit.globalPosition().x();
    gPosY[index]     = hit.globalPosition().y();
    gPosZ[index]     = hit.globalPosition().z();

    ++index;   
    return true;
  }  //end CscRawHitNtupleBranch::fillBranch

  bool CscRawHitNtupleBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree )
      return false;    

    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name ="nRCsc";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name, &index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

    // create the branches
    branchCreator.createBranch( tree, "occupancy", &occupancy, array_size + "/I" );
    branchCreator.createBranch( tree, "id",        &id,        array_size + "/I" );
    branchCreator.createBranch( tree, "width",     &width,     array_size + "/F" );
    branchCreator.createBranch( tree, "charge",    &charge,    array_size + "/F" );
    branchCreator.createBranch( tree, "t",         &t,         array_size + "/F" );
    branchCreator.createBranch( tree, "gPosX",     &gPosX,     array_size + "/F" );
    branchCreator.createBranch( tree, "gPosY",     &gPosY,     array_size + "/F" );
    branchCreator.createBranch( tree, "gPosZ",     &gPosZ,     array_size + "/F" );

    branchesInit = true;
  
    // reset branch
    reset();

    return true;
  }  //end CscRawHitNtupleBranch::createBranch

}  //namespace MuonCalib
