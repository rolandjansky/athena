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
    void reset() { m_index = 0; }                                          //!< set hit_index to zero 
    int getBranchEntries() { return m_index; }                             //!< returns the number of hits currently in the branch 
  
  private:
    std::string m_branchName;          //!< name of branch in tree, per default prepended to variable names 
    bool m_branchesInit;               //!< flag to check whether branches were initialized 
    bool m_first;                      //!< flag to make sure that overflow message is only printed once
    static const int s_blockSize = 6000; //!< quantities stored in the tree 
    int  m_index;                      //!< counter keeping track on the number of MuonCalib::MuonCalibHit_E s stored in the event

    int   m_phiPatIndex[s_blockSize];
    unsigned int   m_id[s_blockSize];
    float m_posX[s_blockSize];
    float m_posY[s_blockSize];
    float m_posZ[s_blockSize];
    float m_error[s_blockSize];
    
  };
}// namespace MuonCalib

#endif
