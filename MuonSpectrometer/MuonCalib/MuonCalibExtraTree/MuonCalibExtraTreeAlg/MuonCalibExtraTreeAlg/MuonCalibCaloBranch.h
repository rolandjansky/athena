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

#ifndef MUONCALIBMBTSBRANCH_H
#define MUONCALIBMBTSBRANCH_H

#include <string>

class TTree;

namespace MuonCalib {

  class MuonCalibCaloHit;

  /**@class MuonCalibCaloBranch
     Calo trigger information
  */
  class MuonCalibCaloBranch {
  public:
    MuonCalibCaloBranch(std::string branchName = "mbts_");      //!< default constructor 
    bool  fillBranch(const MuonCalibCaloHit& hit);              //!< fill content of hit into branch 
    bool  createBranch(TTree* tree);                            //!< create branch structure in tree 
    void  reset() { m_index = 0; }                              //!< set hit_index to zero 
    int   getBranchEntries() { return m_index; }                //!< returns the number of hits currently in the branch 
  
  private:
    std::string m_branchName;           //!< name of branch in tree, per default prepended to variable names 
    bool m_branchesInit;                //!< flag to check whether branches were initialized 
    bool m_first;                       //!< flag to make sure that overflow message is only printed once
    static const int s_blockSize = 100;   //!< quantities stored in the tree 
    int  m_index;                         //!< counter keeping track on the number of entries stored in the event

    unsigned int m_id[s_blockSize];
    float m_posX[s_blockSize];
    float m_posY[s_blockSize];
    float m_posZ[s_blockSize];
    float m_time[s_blockSize];
    float m_charge[s_blockSize];
  };
}// namespace MuonCalib

#endif
