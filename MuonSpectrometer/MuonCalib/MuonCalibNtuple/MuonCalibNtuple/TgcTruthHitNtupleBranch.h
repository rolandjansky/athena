/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TgcTruthHitNtupleBranch.h
//   Header file for class TgcTruthHitNtupleBranch
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// zkestere@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_TGCTRUTHHITNTUPLEBRANCH_H
#define MUONCALIB_TGCTRUTHHITNTUPLEBRANCH_H

#include <string>

class TTree;

namespace MuonCalib {

  // forward declarations
  class MuonCalibTgcTruthHit;

  /**@class TgcTruthHitNtupleBranch
     Class to create and fill a branch in a root tree which contains
     information of MuonCalibTgcTruthHits. All members of
     MuonCalibTgcTruthHit are stored on this branch.

     See the <a
     href="https://twiki.cern.ch/twiki/bin/view/Atlas/MuonCalibNuptleContent"><span>Calibration
     Ntuple wiki</span></a> for information on ntuple variables.

     @author zkestere@nikhef.nl
  */

  class TgcTruthHitNtupleBranch {
  public:
    TgcTruthHitNtupleBranch(std::string branchName = "tgcTruth_"); //!< default constructor 
    bool  fillBranch(const MuonCalibTgcTruthHit &hit);             //!< fill content of hit into branch 
    bool  createBranch(TTree* tree);                               //!< create branch structure in tree 
    inline void reset() { index = 0; }                             //!< set hit_index to zero 
    inline const int& getBranchEntries() const { return index; }   //!< returns the number of hits currently in the branch 
    inline int blockSize() const { return m_blockSize; }           //!< returns maximum number of entries stored to ntuple
  
  private:
    std::string m_branchName;          //!< name of branch in tree, per default prepended to variable names 
    bool branchesInit;                 //!< flag to check whether branches were initialized 
    int index;                         //!< counter keeping track on the number of MuonCalibRpcTruthHit s stored in the event
    static const int m_blockSize = 3000; //!< quantities stored in the tree 

    int id[m_blockSize];
    int barCode[m_blockSize];
    double time[m_blockSize];
  };

}  //end namespace MuonCalib

#endif
