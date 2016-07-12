/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TgcCalibHitNtupleBranch.h
//   Header file for class TgcCalibHitNtupleBranch
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// zkestere@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_TGCCALIBHITNTUPLEBRANCH_H
#define MUONCALIB_TGCCALIBHITNTUPLEBRANCH_H

#include <string>

class TTree;

namespace MuonCalib {

// forward declarations
class TgcCalibHitBase;

  /**@class TgcCalibHitNtupleBranch
     Class to create and fill a branch in a root tree which contains
     information of TgcCalibHits. All members of TgcCalibHitBase are
     stored on this branch.

     See 
     <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/MuonCalibNuptleContent"><span>Calibration Ntuple wiki</span></a> 
     for information on ntuple variables.

     @author zkestere@nikhef.nl
  */

  class TgcCalibHitNtupleBranch {
  public:
    TgcCalibHitNtupleBranch(std::string branchName = "tgc_");         //!< default constructor 
    bool  fillBranch(const TgcCalibHitBase &hit, const int segIndex); //!< fill content of hit into branch 
    bool  createBranch(TTree *tree);                                  //!< create branch structure in tree 
    inline void reset() { index = 0; }                                //!< set hit_index to zero 
    inline const int& getBranchEntries() const { return index; }      //!< returns the number of hits currently in the branch 
    inline int blockSize() const { return m_blockSize; }              //!< returns maximum number of entries stored to ntuple
    
  private:
    std::string m_branchName;          //!< name of branch in tree, per default prepended to variable names 
    bool branchesInit;                 //!< flag to check whether branches were initialized 
    static const int m_blockSize = 3000; //!< quantities stored in the tree 
    int   index;                       //!< counter keeping track on the number of TgcCalibHitBase s stored in the event

    int   segIndex[m_blockSize];
    int   id[m_blockSize];
    float stripWidth[m_blockSize];
    float stripLength[m_blockSize];
    int   nStrips[m_blockSize];
    float error[m_blockSize];
    float posX[m_blockSize];
    float posY[m_blockSize];
    float posZ[m_blockSize];
    float gPosX[m_blockSize];
    float gPosY[m_blockSize];
    float gPosZ[m_blockSize];
  };

}  //namespace MuonCalib

#endif
