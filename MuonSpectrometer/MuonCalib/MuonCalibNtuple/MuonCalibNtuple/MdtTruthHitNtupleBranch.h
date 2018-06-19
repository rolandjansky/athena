/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MdtTruthHitNtupleBranch.h
//   Header file for class MdtTruthHitNtupleBranch
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// zkestere@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_MDTTRUTHHITNTUPLEBRANCH_H
#define MUONCALIB_MDTTRUTHHITNTUPLEBRANCH_H

#include <string>

class TTree;

namespace MuonCalib {

  // forward declarations
  class MuonCalibMdtTruthHit;

  /**@class MdtTruthHitNtupleBranch
     Class to create and fill a branch in a root tree which contains information
     of MuonCalibMdtTruthHits. All members of the MuonCalibMdtTruthHit are stored on the branch.

     See the <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/MuonCalibNuptleContent"><span>Calibration Ntuple wiki</span></a> for information on the ntuple variables.

     @author zkestere@nikhef.nl
  */

  class MdtTruthHitNtupleBranch {
  public:
    MdtTruthHitNtupleBranch(std::string branchName = "mdtTruth_"); //!< default constructor 
    bool  fillBranch(const MuonCalibMdtTruthHit &hit);             //!< fill content of hit into branch 
    bool  createBranch(TTree *tree);                               //!< create branch structure in tree 
    inline void reset() { m_index = 0; }                             //!< set hit_index to zero 
    inline const int& getBranchEntries() const { return m_index; }   //!< returns the number of hits currently in the branch 
    inline int  blockSize() const { return m_blockSize; }          //!< returns maximum number of entries stored to ntuple
  
  private:
    std::string m_branchName;          //!< name of branch in tree, per default prepended to variable names 
    bool m_branchesInit;                 //!< flag to check whether branches were initialized 
    int m_index;                         //!< counter keeping track on the number of MuonCalibMdtTruthHit s stored in the event

    static const int m_blockSize = 3000; //!< max number of MuonCalibMdtTruthHit s to be stored on branch.
    int m_id[m_blockSize];
    int m_barCode[m_blockSize];
    double m_driftRadius[m_blockSize];
    double m_positionAlongTube[m_blockSize];
    double m_gpositionX[m_blockSize];
    double m_gpositionY[m_blockSize];
    double m_gpositionZ[m_blockSize];
    double m_time[m_blockSize];
    
  };
}  //namespace MuonCalib

#endif
