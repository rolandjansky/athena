/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonCalibPhiHit_EBranch.h
//   Header file for class MuonCalibPhiHit_EBranch
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// zkestere@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_PHIHIT_EBRANCH_H
#define MUONCALIB_PHIHIT_EBRANCH_H

#include <string>

class TTree;

namespace MuonCalib {

  /**@class MuonCalibPhiHit_EBranch  
     Class to create and fill a branch in a root tree which contains information
     of MuonCalib::MuonCalibHit_E s (For the PhiPatterns called MuonCalibPhiHit). All members of MuonCalib::MuonCalibHit_E are stored on this branch.

     For the content of the Extended Branches, see the  <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/MuonCalibExtraNtupleContent"><span>ExtraTree Ntuple wiki</span></a>

     @author Zdenko.Van.Kesteren@cern.ch
  */
  class MuonFixedId;
  class MuonCalibHit_E;
  
  class MuonCalibPhiHit_EBranch {
  public:  
    MuonCalibPhiHit_EBranch(std::string branchName = "phi_");            //!< default constructor 
    bool  fillBranch(const MuonCalibHit_E& hit, const int patternIndex); //!< fill content of hit into branch 
    bool  createBranch(TTree* tree);                                     //!< create branch structure in tree 
    void reset() { index = 0; }                                          //!< set hit_index to zero 
    int getBranchEntries() { return index; }                             //!< returns the number of hits currently in the branch 
  
  private:
    std::string m_branchName;          //!< name of branch in tree, per default prepended to variable names 
    bool branchesInit;                 //!< flag to check whether branches were initialized 
    bool m_first;                      //!< flag to make sure that overflow message is only printed once
    static const int blockSize = 6000; //!< quantities stored in the tree 
    int  index;                        //!< counter keeping track on the number of MuonCalib::MuonCalibHit_E s stored in the event

    int   phiPatIndex[blockSize];
    unsigned int   id[blockSize];
    float posX[blockSize];
    float posY[blockSize];
    float posZ[blockSize];
    float error[blockSize];
    
  };
}// namespace MuonCalib

#endif
