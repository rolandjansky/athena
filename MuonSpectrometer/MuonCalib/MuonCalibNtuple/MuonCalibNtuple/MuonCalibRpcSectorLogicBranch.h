/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_RPCSECTORLOGICBRANCH_H
#define MUONCALIB_RPCSECTORLOGICBRANCH_H

// forward declarations
class TTree;

#include "MuonRDO/RpcSectorLogicContainer.h"

namespace MuonCalib {

  /**@class RpcSectorLogicBranch
     Class to create and fill a branch in a root tree which contains 
     information of RpcSectorLogic
     
     @author Niels.Van.Eldik@cern.ch
  */
  class MuonCalibRpcSectorLogicBranch 
  {
  public:
    MuonCalibRpcSectorLogicBranch(std::string branchName = "rpc_SL"); //!< default constructor 
    bool  fillBranch(const RpcSectorLogicContainer &slContainer);     //!< fill content of container into branch 
    bool  createBranch(TTree *tree);                                  //!< create branch structure in tree 
    inline void reset() { m_index = 0; m_index2=0; m_index3=0; }            //!< set index to zero 
    inline const int& getBranchEntries() const{ return m_index; }       //!< returns the number of pats currently in the branch 
    inline  int blockSize() const{ return m_blockSize; }              //!< returns maximum number of entries stored to ntuple
  
  private:
    std::string m_branchName;             //!< name of branch in tree, per default prepended to variable names 
    bool m_branchesInit;                    //!< flag to check whether branches were initialized 
    bool m_first;                         //!< flag to make sure that overflow message is only printed once
    static const int m_blockSize = 100;   //!< quantities stored in the tree 
    int   m_index;                          //!< counter keeping track on the number of MuonCalibPattern s stored in the event

    // sector logic information
    int m_sectorId[m_blockSize];
    int m_bcid[m_blockSize];
    int m_lvl1Id[m_blockSize];
    int m_errorCode[m_blockSize];
    int m_crc[m_blockSize];
    int m_hasMoreThan2TriggerCand[m_blockSize];

    // vector properties, if value is -99999 it should be ignored
    static const int m_blockSize2 = 100;     //!< quantities stored in the tree 
    int   m_index2;                            //!< counter keeping track on the number of entries
    
    // RpcSLTriggerHit properties
    static const int m_blockSize3 = 100;     //!< quantities stored in the tree 
    int   m_index3;                            //!< counter keeping track on the number of entries

    // bc within within the readout window
    int m_slIndex2[m_blockSize3];            //!< index of corresponding SL
    int m_triggerBcid[m_blockSize3];         //!< bc of trigger hit
    int m_rowinBcid[m_blockSize3];
    int m_padId[m_blockSize3];
    int m_ptId[m_blockSize3];
    int m_roi[m_blockSize3];
    int m_isInput[m_blockSize3];             //! < is input or output
    
    // Info variables split
    int m_outerPlane[m_blockSize3];
    int m_overlapPhi[m_blockSize3];
    int m_overlapEta[m_blockSize3];
    
  };

}  //namespace MuonCalib

#endif
