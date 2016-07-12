/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    branchesInit(false), index(0), eventNumber(0), runNumber(0), timeStamp(0), lumiBlock(0), bcId(0), 
    dMbtsTime(0), dLarTime(0),  n_on_TAV_bits(0), n_on_TAP_bits(0)
  {}

  bool MuonEventNtupleBranch::fillBranch(const MuonCalibEventInfo &eventInfo, const MuonCalibTriggerTimeInfo &trigTimeInfo ) {
    // check if branches where initialized
    if( !branchesInit ){
      //std::cout << "MuonEventNtupleBranch::fillBranch  ERROR <branches where not initialized>"
      //	<<  std::endl;
      return false;    
    }
    
    // copy values 
    runNumber = eventInfo.runNumber() ;
    eventNumber = eventInfo.eventNumber() ; 
    timeStamp = eventInfo.timeStamp();
    lumiBlock = eventInfo.lumiBlock();
    bcId = eventInfo.bcId();
    
    dMbtsTime = trigTimeInfo.dMbtsTime();
    dLarTime  = trigTimeInfo.dLarTime();
	
    std::string event_tag = eventInfo.tag();
    unsigned int stringSize = event_tag.size();
    if(stringSize > 63) stringSize = 63;
    for(unsigned int i=0; i<stringSize; i++){
      eventTag[i] = event_tag[i]; 
    }
    eventTag[stringSize] = 0;
    n_on_TAV_bits=0;
    n_on_TAP_bits=0;
    for(unsigned int i=0; i<eventInfo.triggerBits().size(); i++) {
      if(i<eventInfo.triggerBits().size()/2) {
	if(eventInfo.triggerBits()[i] == true && n_on_TAV_bits<200) {
	  on_TAV_bits[n_on_TAV_bits]=i;
	  n_on_TAV_bits++;
	}
      } else {
	if(eventInfo.triggerBits()[i] == true && n_on_TAP_bits<200) {
	  on_TAP_bits[n_on_TAP_bits]=i - eventInfo.triggerBits().size()/2;
	  n_on_TAP_bits++;
	}
      }
    }
    
    ++index;
    return true;
  }  //end MuonEventNtupleBranch::fillBranch

  bool  MuonEventNtupleBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree ){
      //std::cout << "MuonEventNtupleBranch::createBranch  ERROR <got invalid tree pointer> " 
      //	<< std::endl;
      return false;
    }

    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name = "nEvent";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name, &index, "/I");

    // create the branches 
    branchCreator.createBranch( tree, "eventNumber",   &eventNumber,   "/I" );
    branchCreator.createBranch( tree, "runNumber",     &runNumber,     "/I" );  
    branchCreator.createBranch( tree, "timeStamp",     &timeStamp,     "/I" );
    branchCreator.createBranch( tree, "lumiBlock",     &lumiBlock,     "/I" );
    branchCreator.createBranch( tree, "bcId",          &bcId,          "/I" );
    branchCreator.createBranch( tree, "mbtsTimeDiff",  &dMbtsTime,     "/f" );
    branchCreator.createBranch( tree, "larTimeDiff",   &dLarTime,      "/f" );
    branchCreator.createBranch( tree, "eventTag",      &eventTag,      "[64]/b" ); 
    branchCreator.createBranch( tree, "n_on_TAV_bits", &n_on_TAV_bits, "/I" ); 
    branchCreator.createBranch( tree, "on_TAV_bits",   &on_TAV_bits,   (std::string("[") + m_branchName + "n_on_TAV_bits]/I").c_str());
    branchCreator.createBranch( tree, "n_on_TAP_bits", &n_on_TAP_bits, "/I" ); 
    branchCreator.createBranch( tree, "on_TAP_bits",   &on_TAP_bits,   (std::string("[") + m_branchName + "n_on_TAP_bits]/I").c_str());
    branchesInit = true;

    // reset branch
    reset();

    return true;
  }  //end MuonEventNtupleBranch::createBranch

}  //namespace MuonCalib
