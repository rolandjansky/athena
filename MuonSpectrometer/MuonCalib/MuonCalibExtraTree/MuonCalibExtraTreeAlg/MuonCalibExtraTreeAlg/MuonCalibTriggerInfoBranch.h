/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonCalibHit_EBranch.h
//   Header file for class MuonCalibHit_EBranch
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// zkestere@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIBTRIGGERINFOBRANCH_H
#define MUONCALIBTRIGGERINFOBRANCH_H

#include <string>
#include <vector>

class TTree;
class CTP_RDO;

namespace MuonCalib {

  class MuonCalibTriggerInfo;

  /**
     branch to store trigger information. The first entry in the branch is the trigger triggering the event.
     If more trigger fired they will be added after the first
  */
  class MuonCalibTriggerInfoBranch {
  public:
    MuonCalibTriggerInfoBranch(std::string branchName = "ctp_");           //!< default constructor 
    bool  fillBranch(const CTP_RDO* ctp_RDO);                   //!< fill content of hit into branch 
    bool  createBranch(TTree* tree);                            //!< create branch structure in tree 
    void  reset() { index = 0; }                                //!< set hit_index to zero 
    int   getBranchEntries() { return index; }                  //!< returns the number of hits currently in the branch 
  
  private:
    std::string m_branchName;           //!< name of branch in tree, per default prepended to variable names 
    bool branchesInit;                  //!< flag to check whether branches were initialized 
    bool m_first;                       //!< flag to make sure that overflow message is only printed once
    static const int blockSize = 50*256;     //!< quantities stored in the tree 
    int  index;                         //!< counter keeping track on the number of entries stored in the event

    unsigned int m_numberBC;
    unsigned int m_L1A_BC;
    unsigned int m_timeNs;
    int m_randomTrig;
    unsigned int m_firedItemsBeforePrescale;
    int m_prescaledClock;
    int m_bcid[blockSize];
    int m_bcIndexPIT[blockSize];
    int m_pit[blockSize];
    int m_bcIndexTriggerItems[blockSize];
    int m_tbp[blockSize];
    int m_tap[blockSize];
    int m_tav[blockSize];
    int m_type[blockSize];
    float m_delay[blockSize];
  };
}// namespace MuonCalib

#endif

