/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBMUCTPIBRANCH_H
#define MUONCALIBMUCTPIBRANCH_H

#include <string>

class TTree;

namespace LVL1 {
  class RecMuonRoI;
}

namespace MuonCalib {

  /**
     MuCTPi trigger information
  */
  class MuonCalibMuCTPIBranch {
  public:
    MuonCalibMuCTPIBranch(std::string branchName = "muctpi_");  //!< default constructor 
    bool  fillBranch(const LVL1::RecMuonRoI& hit, unsigned int roiBCID);  //!< fill content of hit into branch 
    bool  createBranch(TTree* tree);                            //!< create branch structure in tree 
    void  reset() { index = 0; }                                //!< set hit_index to zero 
    int   getBranchEntries() { return index; }                  //!< returns the number of hits currently in the branch 
  
  private:
    std::string m_branchName;            //!< name of branch in tree, per default prepended to variable names 
    bool branchesInit;                   //!< flag to check whether branches were initialized 
    bool m_first;                        //!< flag to make sure that overflow message is only printed once
    static const int blockSize = 2000;   //!< quantities stored in the tree 
    int  index;                          //!< counter keeping track on the number of entries stored in the event

    int roiWord[blockSize];
    int bcID[blockSize];
    int sysID[blockSize];
    int subsysID[blockSize];
    int sectorID[blockSize];
    int thresholdNumber[blockSize];
    int thresholdValue[blockSize];
    int roINumber[blockSize];
    int overlap[blockSize];
    int firstCandidate[blockSize];
    int sectorOverflow[blockSize];
    int padOverflow[blockSize];
    float phi[blockSize];
    float eta[blockSize];

  };
}// namespace MuonCalib

#endif
