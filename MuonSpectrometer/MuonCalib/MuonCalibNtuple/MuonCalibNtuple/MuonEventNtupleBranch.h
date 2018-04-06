/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonEventNtupleBranch.h
//   Header file for class MuoneventNtupleBranch
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_MUONEVENTNTUPLEBRANCH_H
#define MUONCALIB_MUONEVENTNTUPLEBRANCH_H


#include <string>
#include "MuonCalibEventBase/MuonCalibEventInfo.h"
#include "MuonCalibEventBase/MuonCalibTriggerTimeInfo.h"

class TTree;
 
// forward declarations
 
namespace MuonCalib {
 
  /**@class MuonEventNtupleBranch
     Class to create and fill a branch in a root tree which contains
     information of MuonCalibEventInfo. All members of the
     MuonCalibEventInfo are stored on the branch.

     See
     <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/MuonCalibNuptleContent"><span>Calibration Ntuple wiki</span></a>
     for information on the ntuple variables.

     @author Zdenko.Van.Kesteren@cern.ch
  */
  class MuonEventNtupleBranch 
  {
  public:
    MuonEventNtupleBranch(std::string branchName = "event_");    //!< default constructor 
    bool  fillBranch(const MuonCalibEventInfo &eventInfo, const MuonCalibTriggerTimeInfo &trigTimeInfo ); //!< fill content of eventInfo into branch 
    bool  createBranch(TTree *tree);            //!< create branch structure in tree 
    void reset() { m_index = 0; }                 //!< set seg_index to zero 
    int getBranchEntries() { return m_index; }    //!< returns the number of events currently in the branch 
  
  private:
    std::string m_branchName; //!< name of branch in tree, per default prepended to variable names 
    bool m_branchesInit;        //!< a flag to check whether branches were initialized 
    int  m_index;               //!< counter keeping track on the number of MuonCalibEventInfo s stored in the event

    int m_eventNumber;
    int m_runNumber;
    int m_timeStamp;
    int m_lumiBlock;
    int m_bcId;
    float m_dMbtsTime;
    float m_dLarTime;
    int m_n_on_TAV_bits;
    int m_on_TAV_bits[200];
    int m_n_on_TAP_bits;
    int m_on_TAP_bits[200];

    char m_eventTag[64];
    };

}  //namespace MuonCalib
#endif
