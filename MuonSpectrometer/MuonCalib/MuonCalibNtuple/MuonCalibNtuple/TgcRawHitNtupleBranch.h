/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TgcRawHitNtupleBranch.h
// Header file for class MdtRawHitNtupleBranch
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_TGCRAWHITNTUPLEBRANCH_H
#define MUONCALIB_TGCRAWHITNTUPLEBRANCH_H

#include <string>
#include <utility>

class TTree;

namespace MuonCalib {
  
  class MuonCalibRawTgcHit;
  class MuonFixedId;
  /**@class TgcRawHitNtupleBranch
     Class to create and fill a branch in a root tree which contains
     information of MuonCalibRawTgcHits. All members of
     MuonCalibRawTgcHit are stored on this branch.

     See  
     <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/MuonCalibNuptleContent"><span>Calibration Ntuple wiki</span></a> 
     for information on ntuple variables.

     @author Niels.Van.Eldik@cern.ch, Zdenko.Van.Kesteren@cern.ch
  */

  class TgcRawHitNtupleBranch {
  public:
    TgcRawHitNtupleBranch(std::string branchName = "rawTgc_"); //!< default constructor 
    bool  fillBranch(const MuonCalibRawTgcHit &hit );          //!< fill content of hit into branch 
    bool  createBranch(TTree *tree);                           //!< create branch structure in tree 
    inline void reset() { index_Curr = 0; index_Prev = 0; index_Next = 0; index = 0; } //!< set hit_index to zero 
    inline const int & getBranchEntries() const { return index; }  //!< returns the number of hits currently in the branch 
    inline int blockSize() const { return m_blockSize; }           //!< returns maximum number of entries stored to ntuple
  
  private:
    std::string m_branchName;          //!< name of branch in tree, per default prepended to variable names 
    bool branchesInit;                 //!< flag to check whether branches were initialized 
    static const int m_blockSize = 3000; //!< quantities stored in the tree 
    int   index;
    int   index_Curr;                       //!< counter keeping track on the number of MuonCalibRawTgcHit s stored in the event
    int   index_Prev;
    int   index_Next;

    int   bcTag[m_blockSize];    
    int   num_Prev[m_blockSize];  //!< for Ntuple -> MuonCalibRawTgcHit
    int   num_Curr[m_blockSize];  //!< for Ntuple -> MuonCalibRawTgcHit
    int   num_Next[m_blockSize];  //!< for Ntuple -> MuonCalibRawTgcHit

    int   occupancy[m_blockSize];
    int   id[m_blockSize];
    int   station[m_blockSize];
    int   eta[m_blockSize];
    int   phi[m_blockSize];
    int   gasGap[m_blockSize];
    int   isStrip[m_blockSize];
    int   channel[m_blockSize];
    float width[m_blockSize];
    float shortWidth[m_blockSize];
    float longWidth[m_blockSize];
    float length[m_blockSize];
    float gPosX[m_blockSize];
    float gPosY[m_blockSize];
    float gPosZ[m_blockSize];

  };

}  //namespace MuonCalib 

#endif
