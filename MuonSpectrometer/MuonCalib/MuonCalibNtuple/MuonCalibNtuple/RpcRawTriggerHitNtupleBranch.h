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
    inline void reset() { m_index = 0; }                             //!< set hit_index to zero 
    inline const int& getBranchEntries() const{ return m_index; }    //!< returns the number of hits currently in the branch 
    inline int  blockSize() const { return m_blockSize; }          //!< returns maximum number of entries stored to ntuple
    
  private:
    std::string m_branchName;             //!< name of branch in tree, per default prepended to variable names 
    bool m_branchesInit;                    //!< flag to check whether branches were initialized 
    static const int m_blockSize = 3000;  //!< quantities stored in the tree 
    int   m_index;                          //!< counter keeping track on the number of MuonCalibRawRpcHit s stored in the event
    
    int m_sector[m_blockSize];
    int m_padId[m_blockSize];
    int m_status[m_blockSize];
    int m_ercode[m_blockSize];
    int m_cmaId[m_blockSize];
    int m_fel1Id[m_blockSize];
    int m_febcId[m_blockSize];
    int m_crc[m_blockSize];
    int m_bcId[m_blockSize];
    int m_ticks[m_blockSize];
    int m_ijk[m_blockSize];
    int m_cmachan[m_blockSize];
    int m_overlap[m_blockSize];
    int m_threshold[m_blockSize];
  };
  
}  //namespace MuonCalib

#endif
