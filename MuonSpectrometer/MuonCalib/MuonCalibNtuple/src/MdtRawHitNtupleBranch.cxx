/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/MdtRawHitNtupleBranch.h"
#include "MuonCalibNtuple/NtupleBranchCreator.h"

#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

  MdtRawHitNtupleBranch::MdtRawHitNtupleBranch(std::string branchName) : m_branchName(branchName), branchesInit(false), index(0)
  {}

  bool MdtRawHitNtupleBranch::fillBranch( const MuonCalibRawMdtHit &hit ) {
    //std::cout << "Got into the fillBranch routine of MdtRawHitNtupleBranch" << std::endl;
    // check if branches where initialized
    if( !branchesInit ){
      //      std::cout << "MdtRawHitNtupleBranch::fillBranch  ERROR <branches where not initialized>"
      //	<<  std::endl;
      return false;    
    } 

    // check if index not out of range 
    if( index >= m_blockSize || index < 0 ){
//       std::cout << "MdtRawHitNtupleBranch::fillBranch  ERROR <index out of range, hit not added to ntuple> "
// 		<<  index << std::endl;
      return false;
    }  
    
    // copy values 
//    occupancy[index]  = hit.occupancy() ;
    id[index]         = hit.identify().getIdInt();
    tdc[index]        = hit.tdcCount();
    adc[index]        = hit.adcCount();
//    t[index]          = hit.driftTime();
//    r[index]          = hit.driftRadius();
//    dr[index]         = hit.driftRadiusError();
//    posX[index]       = hit.localPosition().x() ;
//    posY[index]       = hit.localPosition().y() ;
//    posZ[index]       = hit.localPosition().z() ;
    gPosX[index]      = hit.globalPosition().x();
    gPosY[index]      = hit.globalPosition().y();
    gPosZ[index]      = hit.globalPosition().z();

    //std::cout << "Filled the rawMdtBranch with index " << index << std::endl;
    // increment hit index
    ++index;
   
    return true;
  }  //end MdtRawHitNtupleBranch::fillBranch

  bool MdtRawHitNtupleBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree ){
      //      std::cout << "MdtRawHitNtupleBranch::createBranch  ERROR <got invalid tree pointer> " 
      //	<< std::endl;
      return false;
    }

    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name ="nRMdt";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name, &index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

    // create the branches
//    branchCreator.createBranch( tree, "occupancy", &occupancy, array_size + "/I" );
    branchCreator.createBranch( tree, "id",        &id,        array_size + "/I" );
    branchCreator.createBranch( tree, "tdc",       &tdc,       array_size + "/I" );
    branchCreator.createBranch( tree, "adc",       &adc,       array_size + "/I" );
//    branchCreator.createBranch( tree, "t",         &t,         array_size + "/F" );
//    branchCreator.createBranch( tree, "r",         &r,         array_size + "/F" );
//    branchCreator.createBranch( tree, "dr",        &dr,        array_size + "/F" );
//    branchCreator.createBranch( tree, "posX",      &posX,      array_size + "/F" );
//    branchCreator.createBranch( tree, "posY",      &posY,      array_size + "/F" );
//    branchCreator.createBranch( tree, "posZ",      &posZ,      array_size + "/F" );
    branchCreator.createBranch( tree, "gPosX",     &gPosX,     array_size + "/F" );
    branchCreator.createBranch( tree, "gPosY",     &gPosY,     array_size + "/F" );
    branchCreator.createBranch( tree, "gPosZ",     &gPosZ,     array_size + "/F" );

    branchesInit = true;
  
    // reset branch
    reset();

    return true;
  }  //end MdtRawHitNtupleBranch::createBranch

}  //namespace MuonCalib
