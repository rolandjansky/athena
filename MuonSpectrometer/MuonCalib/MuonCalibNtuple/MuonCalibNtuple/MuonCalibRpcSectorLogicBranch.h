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
    inline void reset() { index = 0; index2=0; index3=0; }            //!< set index to zero 
    inline const int& getBranchEntries() const{ return index; }       //!< returns the number of pats currently in the branch 
    inline  int blockSize() const{ return m_blockSize; }              //!< returns maximum number of entries stored to ntuple
  
  private:
    std::string m_branchName;             //!< name of branch in tree, per default prepended to variable names 
    bool branchesInit;                    //!< flag to check whether branches were initialized 
    bool m_first;                         //!< flag to make sure that overflow message is only printed once
    static const int m_blockSize = 100;   //!< quantities stored in the tree 
    int   index;                          //!< counter keeping track on the number of MuonCalibPattern s stored in the event

    // sector logic information
    int sectorId[m_blockSize];
    int bcid[m_blockSize];
    int lvl1Id[m_blockSize];
    int errorCode[m_blockSize];
    int crc[m_blockSize];
    int hasMoreThan2TriggerCand[m_blockSize];

    // vector properties, if value is -99999 it should be ignored
    static const int m_blockSize2 = 100;     //!< quantities stored in the tree 
    int   index2;                            //!< counter keeping track on the number of entries
    
    // RpcSLTriggerHit properties
    static const int m_blockSize3 = 100;     //!< quantities stored in the tree 
    int   index3;                            //!< counter keeping track on the number of entries

    // bc within within the readout window
    int slIndex2[m_blockSize3];            //!< index of corresponding SL
    int triggerBcid[m_blockSize3];         //!< bc of trigger hit
    int rowinBcid[m_blockSize3];
    int padId[m_blockSize3];
    int ptId[m_blockSize3];
    int roi[m_blockSize3];
    int isInput[m_blockSize3];             //! < is input or output
    
    // Info variables split
    int outerPlane[m_blockSize3];
    int overlapPhi[m_blockSize3];
    int overlapEta[m_blockSize3];
    
  };

}  //namespace MuonCalib

#endif
