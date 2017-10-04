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
    void  reset() { m_index = 0; }                              //!< set hit_index to zero 
    int   getBranchEntries() { return m_index; }                //!< returns the number of hits currently in the branch 
  
  private:
    std::string m_branchName;            //!< name of branch in tree, per default prepended to variable names 
    bool m_branchesInit;                 //!< flag to check whether branches were initialized 
    bool m_first;                        //!< flag to make sure that overflow message is only printed once
    static const int s_blockSize = 2000;   //!< quantities stored in the tree 
    int  m_index;                        //!< counter keeping track on the number of entries stored in the event

    int m_roiWord[s_blockSize];
    int m_bcID[s_blockSize];
    int m_sysID[s_blockSize];
    int m_subsysID[s_blockSize];
    int m_sectorID[s_blockSize];
    int m_thresholdNumber[s_blockSize];
    int m_thresholdValue[s_blockSize];
    int m_roINumber[s_blockSize];
    int m_overlap[s_blockSize];
    int m_firstCandidate[s_blockSize];
    int m_sectorOverflow[s_blockSize];
    int m_padOverflow[s_blockSize];
    float m_phi[s_blockSize];
    float m_eta[s_blockSize];

  };
}// namespace MuonCalib

#endif
