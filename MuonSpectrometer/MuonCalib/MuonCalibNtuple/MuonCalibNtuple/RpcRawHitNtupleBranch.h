/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MdtRawHitNtupleBranch.h
//   Header file for class MdtRawHitNtupleBranch
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_RPCRAWHITNTUPLEBRANCH_H
#define MUONCALIB_RPCRAWHITNTUPLEBRANCH_H

#include <string>
#include <utility>

class TTree;

namespace MuonCalib {

  class MuonCalibRawRpcHit;
  class MuonFixedId;
  /**@class RpcRawHitNtupleBranch
     Class to create and fill a branch in a root tree which contains
     information of MuonCalibRawRpcHits. All members of
     MuonCalibRawRpcHit are stored on this branch.

     See 
     <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/MuonCalibNuptleContent"><span>Calibration Ntuple wiki</span></a> 
     for information on ntuple variables.

     @author Niels.Van.Eldik@cern.ch, Zdenko.Van.Kesteren@cern.ch
  */

  class RpcRawHitNtupleBranch {
  public:
    RpcRawHitNtupleBranch(std::string branchName = "rawRpc_"); //!< default constructor 
    bool  fillBranch(const MuonCalibRawRpcHit &hit );          //!< fill content of hit into branch 
    bool  createBranch(TTree *tree);                           //!< create branch structure in tree 
    inline void reset() { index = 0; }                         //!< set hit_index to zero 
    inline const int& getBranchEntries() const{ return index; }//!< returns the number of hits currently in the branch 
    inline int  blockSize() const { return m_blockSize; }      //!< returns maximum number of entries stored to ntuple
  
  private:
    std::string m_branchName;            //!< name of branch in tree, per default prepended to variable names 
    bool branchesInit;                   //!< flag to check whether branches were initialized 
    static const int m_blockSize = 3000; //!< quantities stored in the tree 
    int   index;                         //!< counter keeping track on the number of MuonCalibRawRpcHit s stored in the event

    int   occupancy[m_blockSize];
    int   id[m_blockSize];
    float t[m_blockSize]; 
    float width[m_blockSize];
    float length[m_blockSize];
    float gPosX[m_blockSize];
    float gPosY[m_blockSize];
    float gPosZ[m_blockSize];

  };
}  //namespace MuonCalib

#endif
