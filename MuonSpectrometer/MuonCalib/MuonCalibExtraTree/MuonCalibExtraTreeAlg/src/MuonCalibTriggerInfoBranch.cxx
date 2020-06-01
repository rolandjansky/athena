/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeAlg/MuonCalibTriggerInfoBranch.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTriggerInfo.h"
#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "TrigT1Result/CTP_Decoder.h"
#include "TrigT1Result/CTP_RDO.h"

#include "TTree.h"
#include <iostream>
#include <set>
#include <map>

namespace MuonCalib {

MuonCalibTriggerInfoBranch::MuonCalibTriggerInfoBranch(std::string branchName) : 
  m_branchName(branchName), m_branchesInit(false), m_first(true), m_index(0), m_numberBC(0),
  m_L1A_BC(0), m_timeNs(0), m_randomTrig(0), m_firedItemsBeforePrescale(0), m_prescaledClock(0) {
}

bool  MuonCalibTriggerInfoBranch::fillBranch(const CTP_RDO* ctpRDO) {
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
    
  CTP_Decoder ctp;
  ctp.setRDO(ctpRDO);
  const std::vector<CTP_BC>& BCs = ctp.getBunchCrossings();

  if(BCs.empty()) return false;

  // now get the data
  unsigned int storeBunch = ctpRDO->getL1AcceptBunchPosition();
  const CTP_BC& bunch = BCs.at(storeBunch);
  m_numberBC = ctpRDO->getNumberOfBunches();
  m_L1A_BC = 0;
  m_timeNs   = ctpRDO->getTimeNanoSec();
  m_randomTrig = 0;
  m_firedItemsBeforePrescale = 0;
  m_prescaledClock = 0;
  
  for (int i = 0 ; i < s_blockSize; i++) {
    m_bcid[i] = 0;
    m_bcIndexPIT[i] = 0;
    m_pit[i] = 0;
    m_bcIndexTriggerItems[i] = 0;
    m_tbp[i] = 0;
    m_tap[i] = 0;
    m_tav[i] = 0;
    m_type[i] = 0;
    m_delay[i] = 0;
  }

  if(m_numberBC == 0) return true;

  int index_bcid = 0; 
  int index_pit = 0; 
  int index_tbp = 0; 
  int index0 = 0; 
  std::set<unsigned int> firingItems;
  unsigned int bunchIndex = 0;
  // now loop over the bunches that were read-out
  for(std::vector<CTP_BC>::const_iterator it = BCs.begin(); it != BCs.end(); ++it, ++bunchIndex) {
    if(bunchIndex==storeBunch) {
      m_randomTrig = it->getRandomTrig();
      m_prescaledClock = it->getPrescaledClock();
      m_firedItemsBeforePrescale += (it->getTBP().count());
      m_L1A_BC   = it->getBCID();
    }
    
    m_bcid[index_bcid] = it->getBCID();
    index_bcid++;
    //Oct, 2014 PIT changed to TIP which is the old PIT and the new direct
    //inputs from TOPO and ALFA (Joerg Stelzer)
    for(size_t pitNum = 0; pitNum < it->getTIP().size(); ++pitNum) {  
      if(it->getTIP().test(pitNum)) {
	m_pit[index_pit] = pitNum;
	m_bcIndexPIT[index_pit] = bunchIndex;
	index_pit++; 
      }
    }
    for(size_t tbpNum = 0; tbpNum < it->getTBP().size(); ++tbpNum) {
      if(it->getTBP().test(tbpNum)) {
	m_tbp[index_tbp] = tbpNum;
	m_bcIndexTriggerItems[index_tbp] = bunchIndex;
	if(it->getTAP().test(tbpNum)) m_tap[index_tbp] = tbpNum;
	else m_tap[index_tbp] =-1;
	if(it->getTAV().test(tbpNum)) m_tav[index_tbp] = tbpNum;
	else m_tav[index_tbp] =-1;
	index_tbp++; 
      }
    }
    
    //backward compatibility with our analysis code
    if(bunch.getTBP().any()) {
      for(unsigned int i = 0; i < bunch.getTBP().size() ; ++i) {
	if(bunch.getTBP().test(i)) {
	  firingItems.insert(i);
	}
      }
    }
      
    double timeBC = bunchIndex*25.;
    timeBC = timeBC - 375.;
    // now loop over our items
    for( int item=0;item<NUMBEROFKNOWNTRIGGERS;++item ){
      MuonCalibTriggerType trtype = (MuonCalibTriggerType)item;
      std::vector<int> bits = MuonCalibTriggerTypeHelper::triggerBits(trtype);
      std::vector<int>::iterator bit = bits.begin();
      std::vector<int>::iterator bit_end = bits.end();
      for( ;bit!=bit_end;++bit ){
	bool bunchContainsItem = firingItems.count(*bit);
	if( bunchContainsItem ) {
	  m_type[index0] = trtype;
	  m_delay[index0] = timeBC;
	  index0++;
	}
      }
    }
  }
  
  int indexm = index0;
  if (index_pit > indexm) indexm = index_pit;
  if (index_tbp > indexm) indexm = index_tbp;
  if (index_bcid > indexm) indexm = index_bcid;
  m_index += indexm;
    
  return true;
}  // end MuonCalibTriggerInfoBranch::fillBranch
  
bool  MuonCalibTriggerInfoBranch::createBranch(TTree* tree) {
  // check if pointer is valid
  if( !tree ) {
    return false;
  }

  // helper class to create branches in trees
  NtupleBranchCreator branchCreator(m_branchName);

  std::string index_name ="nTriggerInfo";

  // create a branch for every data member
  branchCreator.createBranch( tree, index_name, &m_index, "/I");

  // all entries of same size, the number of hits in the event
  std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

  // create the branches
  branchCreator.createBranch( tree, "numberBC", &m_numberBC, "/i");
  branchCreator.createBranch( tree, "L1A_BC", &m_L1A_BC, "/i");
  branchCreator.createBranch( tree, "timeNs", &m_timeNs, "/i");
  branchCreator.createBranch( tree, "randomTrig", &m_randomTrig, "/I");
  branchCreator.createBranch( tree, "firedItemsBeforePrescale", &m_firedItemsBeforePrescale, "/i");
  branchCreator.createBranch( tree, "prescaledClock", &m_prescaledClock, "/i");

  // Below array structure max 30*256 
  // bcid [ max 30 ]
  branchCreator.createBranch( tree, "bcid", &m_bcid, array_size +"/I");
  branchCreator.createBranch( tree, "bcIndexTriggerItems", &m_bcIndexTriggerItems,array_size +"/I");
  branchCreator.createBranch( tree, "bcIndexPIT", &m_bcIndexPIT,array_size +"/I");
  branchCreator.createBranch( tree, "pit", &m_pit,array_size +"/I");
  branchCreator.createBranch( tree, "tbp", &m_tbp, array_size +"/I");
  branchCreator.createBranch( tree, "tap", &m_tap, array_size +"/I");
  branchCreator.createBranch( tree, "tav", &m_tav, array_size +"/I");
  branchCreator.createBranch( tree, "type", &m_type, array_size +"/I");
  branchCreator.createBranch( tree, "delay", &m_delay, array_size +"/F");

  m_branchesInit = true;
    
  // reset branch
  reset();
  
  return true;
}  // end MuonCalibTriggerInfoBranch::createBranch

}  //namespace MuonCalib
