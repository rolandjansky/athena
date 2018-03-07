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
    inline void  reset() { m_index = 0; }                               //!< set hit_index to zero 
    inline const int& getBranchEntries() const { return m_index; }      //!< returns the number of hits currently in the branch 
    inline int blockSize() const { return m_blockSize; }              //!< returns maximum number of entries stored to ntuple
  
  private:
    std::string m_branchName;                                         //!< name of branch in tree, per default prepended to variable names 
    bool m_branchesInit;                                                //!< flag to check whether branches were initialized 
    static const int m_blockSize = 3000;                              //!< quantities stored in the tree 
    int   m_index;                                                      //!< counter keeping track on the number of MdtCalibHitBase s stored in the event

    int   m_segIndex[m_blockSize];
    int   m_id[m_blockSize];
    int   m_tdc[m_blockSize];
    int   m_adc[m_blockSize];
    float m_t[m_blockSize];
    float m_r[m_blockSize]; 
    float m_dr[m_blockSize];
    float m_rTrk[m_blockSize]; 
    float m_drTrk[m_blockSize];
    float m_resi[m_blockSize];
    float m_distRO[m_blockSize];
    float m_slewTime[m_blockSize];
    float m_lorTime[m_blockSize];
    float m_propTime[m_blockSize];
    float m_tof[m_blockSize];
    float m_bFieldPerp[m_blockSize];
    float m_bFieldPara[m_blockSize];
    float m_temperature[m_blockSize];
    float m_projSag[m_blockSize];
    float m_posX[m_blockSize];
    float m_posY[m_blockSize];
    float m_posZ[m_blockSize];
    float m_gPosX[m_blockSize];
    float m_gPosY[m_blockSize];
    float m_gPosZ[m_blockSize];
    float m_closesApproachX[m_blockSize];
    float m_closesApproachY[m_blockSize];
    float m_closesApproachZ[m_blockSize];
    float m_gClosesApproachX[m_blockSize];
    float m_gClosesApproachY[m_blockSize];
    float m_gClosesApproachZ[m_blockSize];
    float m_tube_t0[m_blockSize];
    float m_tube_adccal[m_blockSize];
    float m_xtwin[m_blockSize];
    float m_temperature_time[m_blockSize];
    float m_wire_sag_time[m_blockSize];
    bool m_segment_t0_applied[m_blockSize];
  };
}  //namespace MuonCalib

#endif
