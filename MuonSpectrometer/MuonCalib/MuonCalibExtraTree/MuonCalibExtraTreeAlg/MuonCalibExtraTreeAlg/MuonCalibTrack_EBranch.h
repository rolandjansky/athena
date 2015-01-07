/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonCalibTrack_EBranch.h
//   Header file for class MuonCalibTrack_EBranch
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// zkestere@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_TRACKBRANCH_H
#define MUONCALIB_TRACKBRANCH_H

#include <string>

class TTree;

namespace MuonCalib {

  /**@class MuonCalibTrack_EBranch
     Class to create and fill a branch in a root tree which contains information
     of MuonCalib::MuonCalibTrack_E s . All members of MuonCalib::MuonCalibTrack_E are stored on this branch.

     For the content of the Extended Branches, see the  <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/MuonCalibExtraNtupleContent"><span>ExtraTree Ntuple wiki</span></a>
     @author Zdenko.Van.Kesteren@cern.ch
  */
  class MuonCalibTrack_E;
  
  class MuonCalibTrack_EBranch {
  public:
    MuonCalibTrack_EBranch(std::string branchName = "trk_"); //!< default constructor 
    bool  fillBranch(const MuonCalibTrack_E& track) ;        //!< fill content of hit into branch 
    bool  createBranch(TTree* tree);                         //!< create branch structure in tree 
    void reset() { index = 0; }                              //!< set hit_index to zero 
    int getBranchEntries() { return index; }                 //!< returns the number of hits currently in the branch 
  
  private:
    std::string m_branchName;          //!< name of branch in tree, per default prepended to variable names 
    bool branchesInit;                 //!< flag to check whether branches were initialized 
    bool m_first;                      //!< flag to make sure that overflow message is only printed once
    static const int blockSize = 3000; //!< quantities stored in the tree 
    int  index;                        //!< counter keeping track on the number of MuonCalib::MuonCalibTrack_E s stored in the event

    float x0[blockSize];
    float y0[blockSize];
    float z0[blockSize];
    float phi[blockSize];
    float theta[blockSize];
    float qOverP[blockSize];
//    float errInvP[blockSize];
    int author[blockSize];
    float cov00[blockSize];
    float cov01[blockSize];
    float cov02[blockSize];
    float cov03[blockSize];
    float cov04[blockSize];
    float cov11[blockSize];
    float cov12[blockSize];
    float cov13[blockSize];
    float cov14[blockSize];
    float cov22[blockSize];
    float cov23[blockSize];
    float cov24[blockSize];
    float cov33[blockSize];
    float cov34[blockSize];
    float cov44[blockSize];
    float chi2[blockSize];
    int   ndof[blockSize];

  };
}// namespace MuonCalib

#endif
