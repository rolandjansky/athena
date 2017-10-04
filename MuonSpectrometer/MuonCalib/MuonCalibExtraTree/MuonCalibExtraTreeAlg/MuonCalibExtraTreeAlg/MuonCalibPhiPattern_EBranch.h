/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonCalibPhiPattern_EBranch.h
//   Header file for class MuonCalibPhiPattern_EBranch
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// zkestere@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_PHIPATTERN_EBRANCH_H
#define MUONCALIB_PHIPATTERN_EBRANCH_H

#include <string>

class TTree;

namespace MuonCalib {

  /**@class MuonCalibPhiPattern_EBranch
     Class to create and fill a branch in a root tree which contains information
     of MuonCalib::MuonCalibTrack_E s (In this case, a PhiPattern is described by a MuonCalib::MuonCalibTrack_E). All members of MuonCalib::MuonCalibTrack_E are stored on this branch.

     For the content of the Extended Branches, see the  <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/MuonCalibExtraNtupleContent"><span>ExtraTree Ntuple wiki</span></a>

     @author Zdenko.Van.Kesteren@cern.ch
  */
  class MuonCalibTrack_E;
  
  class MuonCalibPhiPattern_EBranch {
  public:
    MuonCalibPhiPattern_EBranch(std::string branchName = "phiPat_"); //!< default constructor 
    bool  fillBranch(const MuonCalibTrack_E& pat) ;                  //!< fill content of hit into branch 
    bool  createBranch(TTree* tree);                                 //!< create branch structure in tree 
    void reset() { m_index = 0; }                                    //!< set hit_index to zero 
    int getBranchEntries() { return m_index; }                       //!< returns the number of hits currently in the branch 
  
  private:
    std::string m_branchName;          //!< name of branch in tree, per default prepended to variable names 
    bool m_branchesInit;               //!< flag to check whether branches were initialized 
    bool m_first;                      //!< flag to make sure that overflow message is only printed once
    static const int s_blockSize = 3000; //!< quantities stored in the tree 
    int   m_index;                       //!< counter keeping track on the number of MuonCalib::MuonCalibTrack_E s stored in the event

    float m_z0[s_blockSize];
    float m_d0[s_blockSize];
    float m_phi[s_blockSize];
    float m_theta[s_blockSize];
    float m_qOverP[s_blockSize];

  };
}// namespace MuonCalib

#endif
