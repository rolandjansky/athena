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
    inline void reset() { m_index = 0; }                                //!< set hit_index to zero 
    inline const int& getBranchEntries() const { return m_index; }      //!< returns the number of hits currently in the branch 
    inline int blockSize() const { return m_blockSize; }              //!< returns maximum number of entries stored to ntuple
    
  private:
    std::string m_branchName;          //!< name of branch in tree, per default prepended to variable names 
    bool m_branchesInit;                 //!< flag to check whether branches were initialized 
    static const int m_blockSize = 3000; //!< quantities stored in the tree 
    int   m_index;                       //!< counter keeping track on the number of TgcCalibHitBase s stored in the event

    int   m_segIndex[m_blockSize];
    int   m_id[m_blockSize];
    float m_stripWidth[m_blockSize];
    float m_stripLength[m_blockSize];
    int   m_nStrips[m_blockSize];
    float m_error[m_blockSize];
    float m_posX[m_blockSize];
    float m_posY[m_blockSize];
    float m_posZ[m_blockSize];
    float m_gPosX[m_blockSize];
    float m_gPosY[m_blockSize];
    float m_gPosZ[m_blockSize];
  };

}  //namespace MuonCalib

#endif
