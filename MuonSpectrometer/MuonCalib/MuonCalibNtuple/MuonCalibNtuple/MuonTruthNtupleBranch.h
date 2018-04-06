/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonTruthNtupleBranch.h
//   Header file for class MuontruthNtupleBranch
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_MUONTRUTHNTUPLEBRANCH_H
#define MUONCALIB_MUONTRUTHNTUPLEBRANCH_H

#include <string>

class TTree;

namespace MuonCalib {
 

// forward declarations
class MuonCalibTruth;

  /**@class MuonTruthNtupleBranch

     Class to create and fill a branch in a root tree which contains
     information of MuonCalibTruths. All members of MuonCalibTruth are
     stored on this branch.

     See 
     <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/MuonCalibNuptleContent"><span>Calibration Ntuple wiki</span></a>
     for information on ntuple variables.

     @author Zdenko.Van.Kesteren@cern.ch
  */

  class MuonTruthNtupleBranch {
  public:
    MuonTruthNtupleBranch(std::string branchName = "truth_");    //!< default constructor 
    bool  fillBranch(const MuonCalibTruth &truth);               //!< fill content of truth into branch 
    bool  createBranch(TTree *tree);                             //!< create branch structure in tree 
    inline void reset() { m_index = 0; }                           //!< set truth_index to zero 
    inline const int& getBranchEntries() const { return m_index; } //!< returns the number of MuonCalibTruths currently in the branch 
    inline int blockSize() const { return m_blockSize; }         //!< returns maximum number of entries stored to ntuple
  
  private:
    std::string m_branchName;          //!< name of branch in tree, per default prepended to variable names 
    bool m_branchesInit;                 //!< flag to check whether branches were initialized 
    bool m_first;                        //!< flag to make sure that overflow message is only printed once
    static const int m_blockSize = 3000; //!< quantities stored in the tree 
    int   m_index;                       //!< counter keeping track on the number of MuonCalibTruth s stored in the event

    double m_kinEnergy[m_blockSize];
    double m_gPosX[m_blockSize]; 
    double m_gPosY[m_blockSize];
    double m_gPosZ[m_blockSize];
    double m_pX[m_blockSize];
    double m_pY[m_blockSize];
    double m_pZ[m_blockSize];
    int m_PDGCode[m_blockSize];
    int m_barCode[m_blockSize];
    };

}  //namespace MuonCalib
#endif
