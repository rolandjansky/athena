/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RpcRawTriggerHitNtupleBranch.h
//   Header file for class RpcRawTriggerHitNtupleBranch
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Giuseppe.Salamanna@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_RPCRAWTRIGGERHITNTUPLEBRANCH_H
#define MUONCALIB_RPCRAWTRIGGERHITNTUPLEBRANCH_H

#include <string>
#include <utility>

class TTree;

namespace MuonCalib {

  class MuonCalibRawRpcTriggerHit;
  
  class RpcRawTriggerHitNtupleBranch {
  public:
    RpcRawTriggerHitNtupleBranch(std::string branchName = "rpc_"); //!< default constructor 
    bool  fillBranch(const MuonCalibRawRpcTriggerHit &hit );       //!< fill content of hit into branch 
    bool  createBranch(TTree *tree);                               //!< create branch structure in tree 
    inline void reset() { index = 0; }                             //!< set hit_index to zero 
    inline const int& getBranchEntries() const{ return index; }    //!< returns the number of hits currently in the branch 
    inline int  blockSize() const { return m_blockSize; }          //!< returns maximum number of entries stored to ntuple
    
  private:
    std::string m_branchName;             //!< name of branch in tree, per default prepended to variable names 
    bool branchesInit;                    //!< flag to check whether branches were initialized 
    static const int m_blockSize = 3000;  //!< quantities stored in the tree 
    int   index;                          //!< counter keeping track on the number of MuonCalibRawRpcHit s stored in the event
    
    int sector[m_blockSize];
    int padId[m_blockSize];
    int status[m_blockSize];
    int ercode[m_blockSize];
    int cmaId[m_blockSize];
    int fel1Id[m_blockSize];
    int febcId[m_blockSize];
    int crc[m_blockSize];
    int bcId[m_blockSize];
    int ticks[m_blockSize];
    int ijk[m_blockSize];
    int cmachan[m_blockSize];
    int overlap[m_blockSize];
    int threshold[m_blockSize];
  };
  
}  //namespace MuonCalib

#endif
