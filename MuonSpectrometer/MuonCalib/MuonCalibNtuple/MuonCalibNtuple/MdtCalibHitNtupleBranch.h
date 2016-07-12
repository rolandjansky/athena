/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MdtCalibHitNtupleBranch.h
//   Header file for class MdtCalibHitNtupleBranch
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_MDTCALIBHITNTUPLEBRANCH_H
#define MUONCALIB_MDTCALIBHITNTUPLEBRANCH_H

#include <string>

class TTree;

namespace MuonCalib {

  // forward declarations
  class MdtCalibHitBase;
  
  /**@class MdtCalibHitNtupleBranch
     Class to create and fill a branch in a root tree which contains information
     of MdtCalibHitBase s. All members of MdtCalibHitBase are stored on this branch.

     See the <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/MuonCalibNuptleContent"><span>Calibration Ntuple wiki</span></a> for information on ntuple variables.

     @author Niels.Van.Eldik@cern.ch, Zdenko.Van.Kesteren@cern.ch
  */
  
  class MdtCalibHitNtupleBranch {
  public:
    MdtCalibHitNtupleBranch(std::string branchName = "mdt_");         //!< default constructor 
    bool  fillBranch(const MdtCalibHitBase &hit, const int segIndex); //!< fill content of hit into branch 
    bool  createBranch(TTree *tree);                                  //!< create branch structure in tree 
    inline void  reset() { index = 0; }                               //!< set hit_index to zero 
    inline const int& getBranchEntries() const { return index; }      //!< returns the number of hits currently in the branch 
    inline int blockSize() const { return m_blockSize; }              //!< returns maximum number of entries stored to ntuple
  
  private:
    std::string m_branchName;                                         //!< name of branch in tree, per default prepended to variable names 
    bool branchesInit;                                                //!< flag to check whether branches were initialized 
    static const int m_blockSize = 3000;                              //!< quantities stored in the tree 
    int   index;                                                      //!< counter keeping track on the number of MdtCalibHitBase s stored in the event

    int   segIndex[m_blockSize];
    int   id[m_blockSize];
    int   tdc[m_blockSize];
    int   adc[m_blockSize];
    float t[m_blockSize];
    float r[m_blockSize]; 
    float dr[m_blockSize];
    float rTrk[m_blockSize]; 
    float drTrk[m_blockSize];
    float resi[m_blockSize];
    float distRO[m_blockSize];
    float slewTime[m_blockSize];
    float lorTime[m_blockSize];
    float propTime[m_blockSize];
    float tof[m_blockSize];
    float bFieldPerp[m_blockSize];
    float bFieldPara[m_blockSize];
    float temperature[m_blockSize];
    float projSag[m_blockSize];
    float posX[m_blockSize];
    float posY[m_blockSize];
    float posZ[m_blockSize];
    float gPosX[m_blockSize];
    float gPosY[m_blockSize];
    float gPosZ[m_blockSize];
    float closesApproachX[m_blockSize];
    float closesApproachY[m_blockSize];
    float closesApproachZ[m_blockSize];
    float gClosesApproachX[m_blockSize];
    float gClosesApproachY[m_blockSize];
    float gClosesApproachZ[m_blockSize];
    float tube_t0[m_blockSize];
    float tube_adccal[m_blockSize];
    float xtwin[m_blockSize];
    float temperature_time[m_blockSize];
    float wire_sag_time[m_blockSize];
    bool segment_t0_applied[m_blockSize];
  };
}  //namespace MuonCalib

#endif
