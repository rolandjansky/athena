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

#ifndef MUONCALIB_HIT_EBRANCH_H
#define MUONCALIB_HIT_EBRANCH_H

#include <string>

class TTree;

namespace MuonCalib {

  /**@class MuonCalibHit_EBranch
     Class to create and fill a branch in a root tree which contains information
     of MuonCalib::MuonCalibHit_E s. All members of MuonCalib::MuonCalibHit_E are stored on this branch.

     For the content of the Extended Branches, see the  <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/MuonCalibExtraNtupleContent"><span>ExtraTree Ntuple wiki</span></a>

     @author Zdenko.Van.Kesteren@cern.ch
  */
  class MuonFixedId;
  class MuonCalibHit_E;
  
  class MuonCalibHit_EBranch {
  public:
    MuonCalibHit_EBranch(std::string branchName = "trkHit_");          //!< default constructor 
    bool fillBranch(const MuonCalibHit_E &hit, const double drifttime, const int track_index); //!< fill content of hit into branch 
    bool createBranch(TTree* tree);                                    //!< create branch structure in tree 
    void reset() { index = 0; }                                        //!< set hit_index to zero 
    int getBranchEntries() { return index; }                           //!< returns the number of hits currently in the branch 
  
  private:
    std::string m_branchName;           //!< name of branch in tree, per default prepended to variable names 
    bool branchesInit;                  //!< flag to check whether branches were initialized 
    bool m_first;                       //!< flag to make sure that overflow message is only printed once
    static const int blockSize = 3000;  //!< quantities stored in the tree 
    int  index;                         //!< counter keeping track on the number of MuonCalib::MuonCalibHit_E s stored in the event

    int   trackIndex[blockSize];
    unsigned int id[blockSize];
    float posX[blockSize];
    float posY[blockSize];
    float posZ[blockSize];
    float driftRadius[blockSize];
    float driftTime[blockSize];
    float error[blockSize];
    float resi[blockSize];
    float pull[blockSize];
    int   measType[blockSize];
  };
}// namespace MuonCalib

#endif
