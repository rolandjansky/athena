/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MdtRawHitNtupleBranch.h
//   Header file for class MdtRawHitNtupleBranch
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// zkestere@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_MDTRAWHITNTUPLEBRANCH_H
#define MUONCALIB_MDTRAWHITNTUPLEBRANCH_H

#include <string>
#include <utility>

class TTree;

namespace MuonCalib {

  class MuonCalibRawMdtHit;
  class MuonFixedId;
  /**@class MdtRawHitNtupleBranch
     Class to create and fill a branch in a root tree which contains
     information of MuonCalibRawMdtHits. All members of the
     MuonCalibRawMdtHit are stored on the branch.

     See <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/MuonCalibNuptleContent"><span>Calibration Ntuple wiki</span></a>
     for information on the ntuple variables.

     @author Zdenko.Van.Kesteren@cern.ch
  */

  class MdtRawHitNtupleBranch {
  public:
    MdtRawHitNtupleBranch(std::string branchName = "rawMdt_");  //!< default constructor 
    bool  fillBranch( const MuonCalibRawMdtHit &hit );          //!< fill content of hit into branch 
    bool  createBranch(TTree *tree);                            //!< create branch structure in tree 
    void reset() { index = 0; }                                 //!< set hit_index to zero 
    inline const int& getBranchEntries() const { return index; }//!< returns the number of hits currently in the branch 
    inline int blockSize() const { return m_blockSize; }        //!< returns maximum number of entries stored to ntuple
  
  private:
    std::string m_branchName;                           //!< name of branch in tree, per default prepended to variable names 
    bool branchesInit;                                  //!< flag to check whether branches were initialized 
    static const int m_blockSize = 13000;               //!< quantities stored in the tree 
    int   index;                                        //!< counter keeping track on the number of MuonCalibRawMdtHit s stored in the event

//    int   occupancy[m_blockSize];
    int   id[m_blockSize];
    int   tdc[m_blockSize];
    int   adc[m_blockSize];
//    float t[m_blockSize];
//    float r[m_blockSize]; 
//    float dr[m_blockSize];
//    float posX[m_blockSize];
//    float posY[m_blockSize];
//    float posZ[m_blockSize];
    float gPosX[m_blockSize];
    float gPosY[m_blockSize];
    float gPosZ[m_blockSize];

  };
}  //namespace MuonCalib

#endif
