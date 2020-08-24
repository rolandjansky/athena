/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/MuonCalibRpcSectorLogicBranch.h"
#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "TTree.h"
#include <iostream>

namespace MuonCalib {

  MuonCalibRpcSectorLogicBranch::MuonCalibRpcSectorLogicBranch(std::string branchName)
     : m_branchName(branchName), m_branchesInit(false), m_first(true) , m_index(0), m_index2(0), m_index3(0)
  {}        

  bool MuonCalibRpcSectorLogicBranch::fillBranch(const RpcSectorLogicContainer &slContainer) {
    // check if branches were initialized
    if( !m_branchesInit ){
      return false;    
    }

    // check if m_index not out of range 
    if( m_index >= m_blockSize || m_index < 0 ){
      if (m_first == true) {
	m_first = false;
      }
      return false;
    }
    
    // loop over container
    RpcSectorLogicContainer::const_iterator slit = slContainer.begin();
    RpcSectorLogicContainer::const_iterator slit_end = slContainer.end();
    for( ;slit!=slit_end;++slit ) {
      if(m_index >= m_blockSize || m_index<0) return false;
      const RpcSectorLogic &slLogic = **slit;

      // sector logic information
      m_sectorId[m_index] = slLogic.sectorId();
      m_bcid[m_index] = slLogic.bcid();
      m_lvl1Id[m_index] = slLogic.fel1Id();
      m_errorCode[m_index] = slLogic.errorCode();
      m_crc[m_index] = slLogic.crc();
      m_hasMoreThan2TriggerCand[m_index] = slLogic.hasMoreThan2TriggerCand();

      // loop over RpcSLTriggerHits 
      RpcSectorLogic::const_iterator hitIt = slLogic.begin();
      RpcSectorLogic::const_iterator hitIt_end = slLogic.end();
      for( ;hitIt != hitIt_end; ++hitIt ){

	if( m_index3 >= m_blockSize3 || m_index3 < 0 ) break;

	const RpcSLTriggerHit &slHit = **hitIt;
	
	m_slIndex2[m_index3]    = slLogic.sectorId();
	m_rowinBcid[m_index3]   = slHit.rowinBcid();
	m_triggerBcid[m_index3] = slHit.triggerBcid();
	m_padId[m_index3]       = slHit.padId();
	m_ptId[m_index3]        = slHit.ptId();
	m_roi[m_index3]         = slHit.roi();
	m_isInput[m_index3]     = slHit.isInput();
	
	m_outerPlane[m_index3]  = slHit.outerPlane();
	m_overlapPhi[m_index3]  = slHit.overlapPhi();
	m_overlapEta[m_index3]  = slHit.overlapEta();
	
	++m_index3;
      }
      ++m_index;
    }
    return true;
  }  //end MuonCalibRpcSectorLogicBranch::fillBranch

  bool MuonCalibRpcSectorLogicBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree ){
      return false;
    }

    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name = "_nSectors";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name, &m_index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

    // create the branches sl logic
    branchCreator.createBranch( tree, "_sectorId",  &m_sectorId,  array_size + "/I" );
    branchCreator.createBranch( tree, "_lvl1id",    &m_lvl1Id,    array_size + "/I" );
    branchCreator.createBranch( tree, "_bcid",      &m_bcid,      array_size + "/I" );
    branchCreator.createBranch( tree, "_errorCode", &m_errorCode, array_size + "/I" );
    branchCreator.createBranch( tree, "_crc",       &m_crc,       array_size + "/I" );
    branchCreator.createBranch( tree, "_hasMoreThan2TriggerCand", &m_hasMoreThan2TriggerCand, array_size + "/I" );

    // third branch for RpcSLTriggerHit properties
    std::string index_name3 = "nTriggerHits";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name3, &m_index3, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size3( std::string("[") + m_branchName + index_name3 + std::string("]") );

    branchCreator.createBranch( tree, "hit_sector",      &m_slIndex2,    array_size3 + "/I" );
    branchCreator.createBranch( tree, "hit_rowinBcid",   &m_rowinBcid,   array_size3 + "/I" );
    branchCreator.createBranch( tree, "hit_padId",       &m_padId,       array_size3 + "/I" );
    branchCreator.createBranch( tree, "hit_ptId",        &m_ptId,        array_size3 + "/I" );
    branchCreator.createBranch( tree, "hit_roi",         &m_roi,         array_size3 + "/I" );
    branchCreator.createBranch( tree, "hit_outerPlane",  &m_outerPlane,  array_size3 + "/I" );
    branchCreator.createBranch( tree, "hit_overlapPhi",  &m_overlapPhi,  array_size3 + "/I" );
    branchCreator.createBranch( tree, "hit_overlapEta",  &m_overlapEta,  array_size3 + "/I" );
    branchCreator.createBranch( tree, "hit_triggerBcid", &m_triggerBcid, array_size3 + "/I" );
    branchCreator.createBranch( tree, "hit_isInput",     &m_isInput,     array_size3 + "/I" );

    m_branchesInit = true;

    // reset branch
    reset();
  
    return true;
  }  //end MuonCalibRpcSectorLogicBranch::createBranch

} //namespace MuonCalib
