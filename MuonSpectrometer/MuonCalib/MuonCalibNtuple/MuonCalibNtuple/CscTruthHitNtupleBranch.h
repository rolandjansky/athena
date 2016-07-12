/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CscTruthHitNtupleBranch.h
//   Header file for class CscTruthHitNtupleBranch
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// zkestere@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_CSCTRUTHHITNTUPLEBRANCH_H
#define MUONCALIB_CSCTRUTHHITNTUPLEBRANCH_H

#include <string>

class TTree;

namespace MuonCalib {

  // forward declarations
  class MuonCalibCscTruthHit;

  /**@class CscTruthHitNtupleBranch
     Class to create and fill a branch in a root tree which contains
     information of MuonCalibCscTruthHits. All members of
     MuonCalibCscTruthHits are stored on this branch.

     See
     <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/MuonCalibNuptleContent"><span>Calibration Ntuple wiki</span></a>
     for information on ntuple variables.
    
     @author Zdenko.Van.Kesteren@cern.ch
  */

  class CscTruthHitNtupleBranch {
  public:
    CscTruthHitNtupleBranch(std::string branchName = "cscTruth_"); //!< default constructor 

    bool  fillBranch(const MuonCalibCscTruthHit &hit);             //!< fill content of hit into branch 
    bool  createBranch(TTree *tree);                               //!< create branch structure in tree 
    inline void reset() { index = 0; }                             //!< set hit_index to zero 
    inline const int& getBranchEntries() const { return index; }   //!< returns the number of hits currently in the branch 
    inline int blockSize() const { return m_blockSize; }           //!< returns maximum number of entries stored to ntuple
  
  private:
    std::string m_branchName;                                      //!< name of branch in tree, per default prepended to variable names 
    bool branchesInit;                                             //!< flag to check whether branches were initialized 
    int index;                                                     //!< counter keeping track on the number of MuonCalibRawCscHits stored in the event
    static const int m_blockSize = 3000;                           //!< number of quantities stored in the tree 

    int id[m_blockSize];
    int barCode[m_blockSize];
    double time[m_blockSize];

  };
}  //namespace MuonCalib

#endif

