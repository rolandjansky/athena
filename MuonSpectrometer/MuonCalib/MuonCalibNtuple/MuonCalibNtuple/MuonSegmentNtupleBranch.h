/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonSegmentNtupleBranch.h
//   Header file for class MuonSegmentNtupleBranch
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_MUONSEGMENTNTUPLEBRANCH_H
#define MUONCALIB_MUONSEGMENTNTUPLEBRANCH_H

#include <string>

class TTree;

namespace MuonCalib {

  // forward declarations
  class MuonCalibSegment;

  /**@class MuonSegmentNtupleBranch

     Class to create and fill a branch in a root tree which contains
     information of MuonCalibSegments. All members of MuonCalibSegment
     are stored on this branch.

     See 
     <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/MuonCalibNuptleContent"><span>Calibration Ntuple wiki</span></a>
     for information on ntuple variables.
     
     @author Niels.Van.Eldik@cern.ch, Zdenko.Van.Kesteren@cern.ch
  */

  class MuonSegmentNtupleBranch {
  public:
    MuonSegmentNtupleBranch(std::string branchName = "seg_");           //!< default constructor 
    bool  fillBranch(const MuonCalibSegment &seg );               //!< fill content of seg into branch 
    //    bool  fillBranch(const MuonCalibSegment &seg, const int patIndex ); //!< fill content of seg into branch 
    bool  createBranch(TTree *tree);                                    //!< create branch structure in tree 
    inline void reset() { m_index = 0; }                                  //!< set seg_index to zero 
    inline const int& getBranchEntries() const { return m_index; }        //!< returns the number of segs currently in the branch 
    inline int blockSize() const { return m_blockSize; }                //!< returns maximum number of entries stored to ntuple  

  private:
    std::string m_branchName;            //!< name of branch in tree, per default prepended to variable names 
    bool m_branchesInit;                   //!< flag to check whether branches were initialized 
    bool m_first;                        //!< flag to make sure that overflow message is only printed once

    static const int m_blockSize = 400;  //!< quantities stored in the tree 
    int   m_index;                         //!< counter keeping track on the number of MuonCalibSegment s stored in the event

    //    int   m_patIndex[m_blockSize];   //no longer meaningful after Athena 17
    int   m_quality[m_blockSize];
    int   m_author[m_blockSize];
    float m_chi2[m_blockSize];
    float m_fittedT0[m_blockSize];
    float m_gPosX[m_blockSize];
    float m_gPosY[m_blockSize];
    float m_gPosZ[m_blockSize];
    float m_gDirX[m_blockSize];
    float m_gDirY[m_blockSize]; 
    float m_gDirZ[m_blockSize];
    float m_posX[m_blockSize];
    float m_posY[m_blockSize];
    float m_posZ[m_blockSize];
    float m_dirX[m_blockSize];
    float m_dirY[m_blockSize]; 
    float m_dirZ[m_blockSize];
    int   m_nHits[m_blockSize];
    int   m_nMdtHits[m_blockSize];
    int   m_nRpcHits[m_blockSize];
    int   m_nCscHits[m_blockSize];
    int   m_nTgcHits[m_blockSize];
    double m_transPhi[m_blockSize];
    double m_transTheta[m_blockSize];
    double m_transPsi[m_blockSize];
    double m_transX[m_blockSize];
    double m_transY[m_blockSize];
    double m_transZ[m_blockSize];
  };

}  //namespace MuonCalib
#endif
