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
    inline void reset() { m_index_Curr = 0; m_index_Prev = 0; m_index_Next = 0; m_index = 0; } //!< set hit_index to zero 
    inline const int & getBranchEntries() const { return m_index; }  //!< returns the number of hits currently in the branch 
    inline int blockSize() const { return m_blockSize; }           //!< returns maximum number of entries stored to ntuple
  
  private:
    std::string m_branchName;          //!< name of branch in tree, per default prepended to variable names 
    bool m_branchesInit;                 //!< flag to check whether branches were initialized 
    static const int m_blockSize = 3000; //!< quantities stored in the tree 
    int   m_index;
    int   m_index_Curr;                       //!< counter keeping track on the number of MuonCalibRawTgcHit s stored in the event
    int   m_index_Prev;
    int   m_index_Next;

    int   m_bcTag[m_blockSize];    
    int   m_num_Prev[m_blockSize];  //!< for Ntuple -> MuonCalibRawTgcHit
    int   m_num_Curr[m_blockSize];  //!< for Ntuple -> MuonCalibRawTgcHit
    int   m_num_Next[m_blockSize];  //!< for Ntuple -> MuonCalibRawTgcHit

    int   m_occupancy[m_blockSize];
    int   m_id[m_blockSize];
    int   m_station[m_blockSize];
    int   m_eta[m_blockSize];
    int   m_phi[m_blockSize];
    int   m_gasGap[m_blockSize];
    int   m_isStrip[m_blockSize];
    int   m_channel[m_blockSize];
    float m_width[m_blockSize];
    float m_shortWidth[m_blockSize];
    float m_longWidth[m_blockSize];
    float m_length[m_blockSize];
    float m_gPosX[m_blockSize];
    float m_gPosY[m_blockSize];
    float m_gPosZ[m_blockSize];

  };

}  //namespace MuonCalib 

#endif
