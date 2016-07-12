/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonPatternNtupleBranch.h
//   Header file for class MuonPatternNtupleBranch
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_MUONPATTERNNTUPLEBRANCH_H
#define MUONCALIB_MUONPATTERNNTUPLEBRANCH_H

#include <string>

// forward declarations
class TTree;

namespace MuonCalib {

  class MuonCalibPattern;

/**@class PatInfo
   Stores information associated with the MuonCalibPattern
   @author Niels.Van.Eldik@cern.ch
*/
  class PatInfo  {
  public:
    /** constructor */
    PatInfo() {}
  };

  /**@class MuonPatternNtupleBranch
     Class to create and fill a branch in a root tree which contains
     information of MuonCalibPattern s. All members of
     MuonCalibPattern are stored on this branch.

     See
     <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/MuonCalibNuptleContent"><span>Calibration Ntuple wiki</span></a>
     for information on ntuple variables.
     
     @author Niels.Van.Eldik@cern.ch, Zdenko.Van.Kesteren

     2016-03-18 E. Diehl 
     MuonPatternNtupleBranch removed from calib ntuple since Athena 20 reconstruction 
     because pattern data written to ntuples was all zero, 
     and because pattern data is not used by calib algoritms.
     However, leave class in place in case we ever want to use it.
  */
  class MuonPatternNtupleBranch {
  public:
    MuonPatternNtupleBranch(std::string branchName = "pat_");           //!< default constructor 
    bool  fillBranch(const MuonCalibPattern &pat, const PatInfo &info); //!< fill content of pat into branch 
    bool  createBranch(TTree *tree);                                    //!< create branch structure in tree 
    inline void reset() { index = 0; }                                  //!< set pat_index to zero 
    inline const int& getBranchEntries() const { return index; }        //!< returns the number of pats currently in the branch 
    inline int blockSize() const { return m_blockSize; }                //!< returns maximum number of entries stored to ntuple
  
  private:
    std::string m_branchName;           //!< name of branch in tree, per default prepended to variable names 
    bool branchesInit;                  //!< flag to check whether branches were initialized 
    bool m_first;                       //!< flag to make sure that overflow message is only printed once
    static const int m_blockSize = 100; //!< quantities stored in the tree 
    int   index;                        //!< counter keeping track on the number of MuonCalibPattern s stored in the event

    float chi2[m_blockSize];
    float z0[m_blockSize];
    float r0[m_blockSize];
    float invP[m_blockSize];
    float phi[m_blockSize];
    float theta[m_blockSize]; 
    int   nSegments[m_blockSize];
    int   nmdt[m_blockSize];
    int   nrpc[m_blockSize];
    int   ncsc[m_blockSize];
    int   ntgc[m_blockSize];
  };

}  //namespace MuonCalib
#endif
