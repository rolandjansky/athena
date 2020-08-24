/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//MuonCalib packages
#include "MuonCalibNtuple/MuonEventNtupleBranch.h"
#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "MuonCalibEventBase/MuonCalibEventInfo.h"

//other packages
#include "CLHEP/Vector/ThreeVector.h"
#include "TTree.h"
#include "TString.h"

// std packages
#include <iostream>
#include <string>
#include "TString.h"

namespace MuonCalib {

  MuonEventNtupleBranch::MuonEventNtupleBranch(std::string branchName) : m_branchName(branchName),  
    m_branchesInit(false), m_index(0), m_eventNumber(0), m_runNumber(0), m_timeStamp(0), m_lumiBlock(0), m_bcId(0), 
    m_dMbtsTime(0), m_dLarTime(0),  m_n_on_TAV_bits(0), m_n_on_TAP_bits(0)
  {}

  bool MuonEventNtupleBranch::fillBranch(const MuonCalibEventInfo &eventInfo, const MuonCalibTriggerTimeInfo &trigTimeInfo ) {
    // check if branches where initialized
    if( !m_branchesInit ){
      return false;    
    }
    
    // copy values 
    m_runNumber = eventInfo.runNumber() ;
    m_eventNumber = eventInfo.eventNumber() ; 
    m_timeStamp = eventInfo.timeStamp();
    m_lumiBlock = eventInfo.lumiBlock();
    m_bcId = eventInfo.bcId();
    
    m_dMbtsTime = trigTimeInfo.dMbtsTime();
    m_dLarTime  = trigTimeInfo.dLarTime();
	
    std::string event_tag = eventInfo.tag();
    unsigned int stringSize = event_tag.size();
    if(stringSize > 63) stringSize = 63;
    for(unsigned int i=0; i<stringSize; i++){
      m_eventTag[i] = event_tag[i]; 
    }
    m_eventTag[stringSize] = 0;
    m_n_on_TAV_bits=0;
    m_n_on_TAP_bits=0;
    for(unsigned int i=0; i<eventInfo.triggerBits().size(); i++) {
      if(i<eventInfo.triggerBits().size()/2) {
	if(eventInfo.triggerBits()[i] == true && m_n_on_TAV_bits<200) {
	  m_on_TAV_bits[m_n_on_TAV_bits]=i;
	  m_n_on_TAV_bits++;
	}
      } else {
	if(eventInfo.triggerBits()[i] == true && m_n_on_TAP_bits<200) {
	  m_on_TAP_bits[m_n_on_TAP_bits]=i - eventInfo.triggerBits().size()/2;
	  m_n_on_TAP_bits++;
	}
      }
    }
    
    ++m_index;
    return true;
  }  //end MuonEventNtupleBranch::fillBranch

  bool  MuonEventNtupleBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree ){
      return false;
    }

    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name = "nEvent";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name, &m_index, "/I");

    // create the branches 
    branchCreator.createBranch( tree, "eventNumber",   &m_eventNumber,   "/I" );
    branchCreator.createBranch( tree, "runNumber",     &m_runNumber,     "/I" );  
    branchCreator.createBranch( tree, "timeStamp",     &m_timeStamp,     "/I" );
    branchCreator.createBranch( tree, "lumiBlock",     &m_lumiBlock,     "/I" );
    branchCreator.createBranch( tree, "bcId",          &m_bcId,          "/I" );
    branchCreator.createBranch( tree, "mbtsTimeDiff",  &m_dMbtsTime,     "/f" );
    branchCreator.createBranch( tree, "larTimeDiff",   &m_dLarTime,      "/f" );
    branchCreator.createBranch( tree, "eventTag",      &m_eventTag,      "[64]/b" ); 
    branchCreator.createBranch( tree, "n_on_TAV_bits", &m_n_on_TAV_bits, "/I" ); 
    branchCreator.createBranch( tree, "on_TAV_bits",   &m_on_TAV_bits,   (std::string("[") + m_branchName + "n_on_TAV_bits]/I").c_str());
    branchCreator.createBranch( tree, "n_on_TAP_bits", &m_n_on_TAP_bits, "/I" ); 
    branchCreator.createBranch( tree, "on_TAP_bits",   &m_on_TAP_bits,   (std::string("[") + m_branchName + "n_on_TAP_bits!]/I").c_str());
    m_branchesInit = true;

    // reset branch
    reset();

    return true;
  }  //end MuonEventNtupleBranch::createBranch

}  //namespace MuonCalib
