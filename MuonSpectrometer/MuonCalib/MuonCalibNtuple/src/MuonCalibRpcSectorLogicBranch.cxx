/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/MuonCalibRpcSectorLogicBranch.h"

#include "MuonCalibNtuple/NtupleBranchCreator.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

  MuonCalibRpcSectorLogicBranch::MuonCalibRpcSectorLogicBranch(std::string branchName)
     : m_branchName(branchName), branchesInit(false), m_first(true) , index(0), index2(0), index3(0)
  {}        

  bool MuonCalibRpcSectorLogicBranch::fillBranch(const RpcSectorLogicContainer &slContainer) {
    // check if branches where initialized
    if( !branchesInit ){
      //std::cout << "MuonCalibRpcSectorLogicBranch::fillBranch "
      //	<< " ERROR <branches where not initialized>"
      //	<<  std::endl;
      return false;    
    }

    // check if index not out of range 
    if( index >= m_blockSize || index < 0 ){
      if (m_first == true) {
	//std::cout << "MuonCalibRpcSectorLogicBranch::fillBranch  " 
	//<< " ERROR <index out of range, entry not added to ntuple> "
	//  <<  index << std::endl;
	m_first = false;
      }
      return false;
    }
    
    // loop over container
    RpcSectorLogicContainer::const_iterator slit = slContainer.begin();
    RpcSectorLogicContainer::const_iterator slit_end = slContainer.end();
    for( ;slit!=slit_end;++slit ) {
      if(index >= m_blockSize || index<0) return false;
      const RpcSectorLogic &slLogic = **slit;

      // sector logic information
      sectorId[index] = slLogic.sectorId();
      bcid[index] = slLogic.bcid();
      lvl1Id[index] = slLogic.fel1Id();
      errorCode[index] = slLogic.errorCode();
      crc[index] = slLogic.crc();
      hasMoreThan2TriggerCand[index] = slLogic.hasMoreThan2TriggerCand();

      // loop over RpcSLTriggerHits 
      RpcSectorLogic::const_iterator hitIt = slLogic.begin();
      RpcSectorLogic::const_iterator hitIt_end = slLogic.end();
      for( ;hitIt != hitIt_end; ++hitIt ){

	if( index3 >= m_blockSize3 || index3 < 0 ) break;

	const RpcSLTriggerHit &slHit = **hitIt;
	
	slIndex2[index3]    = slLogic.sectorId();
	rowinBcid[index3]   = slHit.rowinBcid();
	triggerBcid[index3] = slHit.triggerBcid();
	padId[index3]       = slHit.padId();
	ptId[index3]        = slHit.ptId();
	roi[index3]         = slHit.roi();
	isInput[index3]     = slHit.isInput();
	
	outerPlane[index3]  = slHit.outerPlane();
	overlapPhi[index3]  = slHit.overlapPhi();
	overlapEta[index3]  = slHit.overlapEta();
	
	++index3;
      }
      ++index;
    }
    return true;
  }  //end MuonCalibRpcSectorLogicBranch::fillBranch

  bool MuonCalibRpcSectorLogicBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree ){
      //std::cout << "MuonPatternNtupleBranch::createBranch  ERROR <got invalid tree pointer> " 
      //<< std::endl;
      return false;
    }

    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name = "_nSectors";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name, &index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

    // create the branches sl logic
    branchCreator.createBranch( tree, "_sectorId",  &sectorId,  array_size + "/I" );
    branchCreator.createBranch( tree, "_lvl1id",    &lvl1Id,    array_size + "/I" );
    branchCreator.createBranch( tree, "_bcid",      &bcid,      array_size + "/I" );
    branchCreator.createBranch( tree, "_errorCode", &errorCode, array_size + "/I" );
    branchCreator.createBranch( tree, "_crc",       &crc,       array_size + "/I" );
    branchCreator.createBranch( tree, "_hasMoreThan2TriggerCand", &hasMoreThan2TriggerCand, array_size + "/I" );

    /**
    // second branch for vector properties
    std::string index_name2 = "nLogicsTimes";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name2, &index2, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size2( std::string("[") + m_branchName + index_name2 + std::string("]") );

    branchCreator.createBranch( tree, "slIndex",       &slIndex,      array_size2 + "/I" );          
    branchCreator.createBranch( tree, "triggerRates",  &triggerRates, array_size2 + "/F" );
    branchCreator.createBranch( tree, "counters",      &counters,     array_size2 + "/I" );
    */

    // third branch for RpcSLTriggerHit properties
    std::string index_name3 = "nTriggerHits";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name3, &index3, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size3( std::string("[") + m_branchName + index_name3 + std::string("]") );

    branchCreator.createBranch( tree, "hit_sector",      &slIndex2,    array_size3 + "/I" );
    branchCreator.createBranch( tree, "hit_rowinBcid",   &rowinBcid,   array_size3 + "/I" );
    branchCreator.createBranch( tree, "hit_padId",       &padId,       array_size3 + "/I" );
    branchCreator.createBranch( tree, "hit_ptId",        &ptId,        array_size3 + "/I" );
    branchCreator.createBranch( tree, "hit_roi",         &roi,         array_size3 + "/I" );
    branchCreator.createBranch( tree, "hit_outerPlane",  &outerPlane,  array_size3 + "/I" );
    branchCreator.createBranch( tree, "hit_overlapPhi",  &overlapPhi,  array_size3 + "/I" );
    branchCreator.createBranch( tree, "hit_overlapEta",  &overlapEta,  array_size3 + "/I" );
    branchCreator.createBranch( tree, "hit_triggerBcid", &triggerBcid, array_size3 + "/I" );
    branchCreator.createBranch( tree, "hit_isInput",     &isInput,     array_size3 + "/I" );

    branchesInit = true;

    // reset branch
    reset();
  
    return true;
  }  //end MuonCalibRpcSectorLogicBranch::createBranch

} //namespace MuonCalib
