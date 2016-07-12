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
    inline void reset() { index = 0; index_Curr=0; index_Prev = 0; index_Next = 0;
                   index_tracklet_Curr = 0; index_tracklet_Prev = 0; index_tracklet_Next = 0; 
                   index_highpt_Curr = 0; index_highpt_Prev = 0; index_highpt_Next = 0; 
                   index_sl_Curr = 0; index_sl_Prev = 0; index_sl_Next = 0; 
		   index_tracklet = 0; index_highpt = 0; index_sl = 0; }//!< set coin_index to zero 
    inline const int& getBranchEntries() const{ return index; }         //!< returns the number of coins currently in the branch 
    inline int  blockSize() const{ return m_blockSize; }                //!< returns maximum number of entries stored to ntuple
  
  private:
    std::string m_branchName;          //!< name of branch in tree, per default prepended to variable names 
    bool branchesInit;                 //!< flag to check whether branches were initialized 
    static const int m_blockSize = 3000; //!< quantities stored in the tree 
    int   index;
    int   index_Curr;                       //!< counter keeping track on the number of MuonCalibRawTgcCoin s stored in the event
    int   index_Prev;
    int   index_Next;
    int   index_tracklet;
    int   index_highpt;
    int   index_sl;
    int   index_tracklet_Curr;
    int   index_tracklet_Prev;
    int   index_tracklet_Next;
    int   index_highpt_Curr;
    int   index_highpt_Prev;
    int   index_highpt_Next;
    int   index_sl_Curr;
    int   index_sl_Prev;
    int   index_sl_Next;

    int type_bcTag[m_blockSize]; 
    /* for NTuple -> MuonCalibRawTgcCoin 
       (1: prev-tracklet, 2: prev-highpt, 3:prev-sl,
        4: curr-tracklet, 5: curr-highpt, 6:curr-sl,
	7: next-tracklet, 8: next-highpt, 9:next-sl)
     */
    int num_tracklet_Prev[m_blockSize];   //!< for Ntuple -> MuonCalibRawTgcCoin
    int num_tracklet_Curr[m_blockSize];   //!< for Ntuple -> MuonCalibRawTgcCoin
    int num_tracklet_Next[m_blockSize];   //!< for Ntuple -> MuonCalibRawTgcCoin
    int num_highpt_Prev[m_blockSize];   //!< for Ntuple -> MuonCalibRawTgcCoin
    int num_highpt_Curr[m_blockSize];   //!< for Ntuple -> MuonCalibRawTgcCoin
    int num_highpt_Next[m_blockSize];   //!< for Ntuple -> MuonCalibRawTgcCoin
    int num_sl_Prev[m_blockSize];   //!< for Ntuple -> MuonCalibRawTgcCoin
    int num_sl_Curr[m_blockSize];   //!< for Ntuple -> MuonCalibRawTgcCoin
    int num_sl_Next[m_blockSize];   //!< for Ntuple -> MuonCalibRawTgcCoin

    int type[m_blockSize];
    int type_Prev[m_blockSize];
    int type_Next[m_blockSize];
    int bcTag_tracklet[m_blockSize];
    int bcTag_highpt[m_blockSize];
    int bcTag_sl[m_blockSize];
    
    int id_tracklet[m_blockSize];
    int eta_tracklet[m_blockSize];
    int phi_tracklet[m_blockSize];
    int sector_tracklet[m_blockSize];
    int isForward_tracklet[m_blockSize];
    int isStrip_tracklet[m_blockSize];
    int trackletId_tracklet[m_blockSize];
    int delta_tracklet[m_blockSize];
    int sub_tracklet[m_blockSize];
    float gPosInX_tracklet[m_blockSize];
    float gPosInY_tracklet[m_blockSize];
    float gPosInZ_tracklet[m_blockSize];
    float gPosOutX_tracklet[m_blockSize];
    float gPosOutY_tracklet[m_blockSize];
    float gPosOutZ_tracklet[m_blockSize];
    float widthIn_tracklet[m_blockSize];
    float widthOut_tracklet[m_blockSize];

    int id_highpt[m_blockSize];
    int eta_highpt[m_blockSize];
    int phi_highpt[m_blockSize];
    int sector_highpt[m_blockSize];
    int isForward_highpt[m_blockSize];
    int isStrip_highpt[m_blockSize];
    int trackletId_highpt[m_blockSize];
    int delta_highpt[m_blockSize];
    int sub_highpt[m_blockSize];
    float gPosInX_highpt[m_blockSize];
    float gPosInY_highpt[m_blockSize];
    float gPosInZ_highpt[m_blockSize];
    float gPosOutX_highpt[m_blockSize];
    float gPosOutY_highpt[m_blockSize];
    float gPosOutZ_highpt[m_blockSize];
    float widthIn_highpt[m_blockSize];
    float widthOut_highpt[m_blockSize];

    int id_sl[m_blockSize];
    int eta_sl[m_blockSize];
    int phi_sl[m_blockSize];
    int sector_sl[m_blockSize];
    int isForward_sl[m_blockSize];
    int trackletId_sl[m_blockSize];
    int trackletIdStrip_sl[m_blockSize];
    int roi_sl[m_blockSize];
    int pt_sl[m_blockSize];
    int isPositiveDeltaR_sl[m_blockSize];
    float gPosOutX_sl[m_blockSize];
    float gPosOutY_sl[m_blockSize];
    float gPosOutZ_sl[m_blockSize];
    float widthR_sl[m_blockSize];
    float widthPhi_sl[m_blockSize];

  };

}  //namespace MuonCalib

#endif
