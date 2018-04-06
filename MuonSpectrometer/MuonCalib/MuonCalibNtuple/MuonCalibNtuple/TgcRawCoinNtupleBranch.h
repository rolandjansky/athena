/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TgcRawCoinNtupleBranch.h
// Header file for class MdtRawCoinNtupleBranch
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// kubota@icepp.s.u-tokyo.ac.jp
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_TGCRAWCOINNTUPLEBRANCH_H
#define MUONCALIB_TGCRAWCOINNTUPLEBRANCH_H

#include <string>
#include <utility>

class TTree;

namespace MuonCalib {
  
  class MuonCalibRawTgcCoin;
  class MuonFixedId;
  /**@class TgcRawCoinNtupleBranch
     Class to create and fill a branch in a root tree which contains
     information of MuonCalibRawTgcCoins. All members of
     MuonCalibRawTgcCoin are stored on this branch.

     See 
     <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/MuonCalibNuptleContent"><span>Calibration Ntuple wiki</span></a> 
     for information on ntuple variables.

     @author Niels.Van.Eldik@cern.ch, Zdenko.Van.Kesteren@cern.ch
  */

  class TgcRawCoinNtupleBranch 
  {
  public:
    TgcRawCoinNtupleBranch(std::string branchName = "rawTgcCoin_"); //!< default constructor 
    bool  fillBranch(const MuonCalibRawTgcCoin &coin );             //!< fill content of coin into branch 
    bool  createBranch(TTree *tree);                                //!< create branch structure in tree 
    inline void reset() { m_index = 0; m_index_Curr=0; m_index_Prev = 0; m_index_Next = 0;
                   m_index_tracklet_Curr = 0; m_index_tracklet_Prev = 0; m_index_tracklet_Next = 0; 
                   m_index_highpt_Curr = 0; m_index_highpt_Prev = 0; m_index_highpt_Next = 0; 
                   m_index_sl_Curr = 0; m_index_sl_Prev = 0; m_index_sl_Next = 0; 
		   m_index_tracklet = 0; m_index_highpt = 0; m_index_sl = 0; }//!< set coin_index to zero 
    inline const int& getBranchEntries() const{ return m_index; }         //!< returns the number of coins currently in the branch 
    inline int  blockSize() const{ return m_blockSize; }                //!< returns maximum number of entries stored to ntuple
  
  private:
    std::string m_branchName;          //!< name of branch in tree, per default prepended to variable names 
    bool m_branchesInit;                 //!< flag to check whether branches were initialized 
    static const int m_blockSize = 3000; //!< quantities stored in the tree 
    int   m_index;
    int   m_index_Curr;                       //!< counter keeping track on the number of MuonCalibRawTgcCoin s stored in the event
    int   m_index_Prev;
    int   m_index_Next;
    int   m_index_tracklet;
    int   m_index_highpt;
    int   m_index_sl;
    int   m_index_tracklet_Curr;
    int   m_index_tracklet_Prev;
    int   m_index_tracklet_Next;
    int   m_index_highpt_Curr;
    int   m_index_highpt_Prev;
    int   m_index_highpt_Next;
    int   m_index_sl_Curr;
    int   m_index_sl_Prev;
    int   m_index_sl_Next;

    int m_type_bcTag[m_blockSize]; 
    /* for NTuple -> MuonCalibRawTgcCoin 
       (1: prev-tracklet, 2: prev-highpt, 3:prev-sl,
        4: curr-tracklet, 5: curr-highpt, 6:curr-sl,
	7: next-tracklet, 8: next-highpt, 9:next-sl)
     */
    int m_num_tracklet_Prev[m_blockSize];   //!< for Ntuple -> MuonCalibRawTgcCoin
    int m_num_tracklet_Curr[m_blockSize];   //!< for Ntuple -> MuonCalibRawTgcCoin
    int m_num_tracklet_Next[m_blockSize];   //!< for Ntuple -> MuonCalibRawTgcCoin
    int m_num_highpt_Prev[m_blockSize];   //!< for Ntuple -> MuonCalibRawTgcCoin
    int m_num_highpt_Curr[m_blockSize];   //!< for Ntuple -> MuonCalibRawTgcCoin
    int m_num_highpt_Next[m_blockSize];   //!< for Ntuple -> MuonCalibRawTgcCoin
    int m_num_sl_Prev[m_blockSize];   //!< for Ntuple -> MuonCalibRawTgcCoin
    int m_num_sl_Curr[m_blockSize];   //!< for Ntuple -> MuonCalibRawTgcCoin
    int m_num_sl_Next[m_blockSize];   //!< for Ntuple -> MuonCalibRawTgcCoin

    int m_type[m_blockSize];
    int m_type_Prev[m_blockSize];
    int m_type_Next[m_blockSize];
    int m_bcTag_tracklet[m_blockSize];
    int m_bcTag_highpt[m_blockSize];
    int m_bcTag_sl[m_blockSize];
    
    int m_id_tracklet[m_blockSize];
    int m_eta_tracklet[m_blockSize];
    int m_phi_tracklet[m_blockSize];
    int m_sector_tracklet[m_blockSize];
    int m_isForward_tracklet[m_blockSize];
    int m_isStrip_tracklet[m_blockSize];
    int m_trackletId_tracklet[m_blockSize];
    int m_delta_tracklet[m_blockSize];
    int m_sub_tracklet[m_blockSize];
    float m_gPosInX_tracklet[m_blockSize];
    float m_gPosInY_tracklet[m_blockSize];
    float m_gPosInZ_tracklet[m_blockSize];
    float m_gPosOutX_tracklet[m_blockSize];
    float m_gPosOutY_tracklet[m_blockSize];
    float m_gPosOutZ_tracklet[m_blockSize];
    float m_widthIn_tracklet[m_blockSize];
    float m_widthOut_tracklet[m_blockSize];

    int m_id_highpt[m_blockSize];
    int m_eta_highpt[m_blockSize];
    int m_phi_highpt[m_blockSize];
    int m_sector_highpt[m_blockSize];
    int m_isForward_highpt[m_blockSize];
    int m_isStrip_highpt[m_blockSize];
    int m_trackletId_highpt[m_blockSize];
    int m_delta_highpt[m_blockSize];
    int m_sub_highpt[m_blockSize];
    float m_gPosInX_highpt[m_blockSize];
    float m_gPosInY_highpt[m_blockSize];
    float m_gPosInZ_highpt[m_blockSize];
    float m_gPosOutX_highpt[m_blockSize];
    float m_gPosOutY_highpt[m_blockSize];
    float m_gPosOutZ_highpt[m_blockSize];
    float m_widthIn_highpt[m_blockSize];
    float m_widthOut_highpt[m_blockSize];

    int m_id_sl[m_blockSize];
    int m_eta_sl[m_blockSize];
    int m_phi_sl[m_blockSize];
    int m_sector_sl[m_blockSize];
    int m_isForward_sl[m_blockSize];
    int m_trackletId_sl[m_blockSize];
    int m_trackletIdStrip_sl[m_blockSize];
    int m_roi_sl[m_blockSize];
    int m_pt_sl[m_blockSize];
    int m_isPositiveDeltaR_sl[m_blockSize];
    float m_gPosOutX_sl[m_blockSize];
    float m_gPosOutY_sl[m_blockSize];
    float m_gPosOutZ_sl[m_blockSize];
    float m_widthR_sl[m_blockSize];
    float m_widthPhi_sl[m_blockSize];

  };

}  //namespace MuonCalib

#endif
