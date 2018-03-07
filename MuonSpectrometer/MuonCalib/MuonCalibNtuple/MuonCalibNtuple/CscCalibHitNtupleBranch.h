/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CscCalibHitNtupleBranch.h
//   Header file for class CscCalibHitNtupleBranch
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// zkestere@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_CSCCALIBHITNTUPLEBRANCH_H
#define MUONCALIB_CSCCALIBHITNTUPLEBRANCH_H

#include <string>

class TTree;

namespace MuonCalib {

  // forward declarations
class CscCalibHitBase;

  /**@class CscCalibHitNtupleBranch

     Class to create and fill a branch in a root tree which contains
     information of CscCalibHits. All members of CscCalibHitBase are
     stored on this branch.

     See 
     <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/MuonCalibNuptleContent"><span>Calibration Ntuple wiki</span></a>
     for information on ntuple variables.
     
     @author zkestere@nikhef.nl
  */

  class CscCalibHitNtupleBranch {
  public:
    CscCalibHitNtupleBranch(std::string branchName = "csc_");         //!< default constructor

    bool  fillBranch(const CscCalibHitBase &hit, const int segIndex); //!< fill content of hit into branch 
    bool  createBranch(TTree *tree);                                  //!< create branch structure in tree 
    inline void reset() { m_index = 0; }                                //!< set hit_index to zero 
    inline const int &getBranchEntries() const { return m_index; }      //!< returns the number of hits currently in the branch 
    inline  int  blockSize() const { return m_blockSize; }            //!< returns maximum number of entries stored to ntuple
  
  private:
    std::string m_branchName;           //!< name of branch in tree, per default prepended to variable names 
    bool m_branchesInit;                  //!< flag to check whether branches were initialized 
    static const int m_blockSize = 3000;  //!< quantities stored in the tree 
    int   m_index ;                       //!< counter keeping track on the number of CscCalibHitBase s stored in the event

    int   m_segIndex[m_blockSize];
    int   m_id[m_blockSize];
    int   m_nStrips[m_blockSize];
    float m_stripWidth[m_blockSize];
    int   m_charge[m_blockSize];
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
