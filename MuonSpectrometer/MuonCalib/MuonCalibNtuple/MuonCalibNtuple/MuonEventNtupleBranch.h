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
    void reset() { index = 0; }                 //!< set seg_index to zero 
    int getBranchEntries() { return index; }    //!< returns the number of events currently in the branch 
  
  private:
    std::string m_branchName; //!< name of branch in tree, per default prepended to variable names 
    bool branchesInit;        //!< flag to check whether branches were initialized 
    int  index;               //!< counter keeping track on the number of MuonCalibEventInfo s stored in the event

    int eventNumber;
    int runNumber;
    int timeStamp;
    int lumiBlock;
    int bcId;
    float dMbtsTime;
    float dLarTime;
    int n_on_TAV_bits;
    int on_TAV_bits[200];
    int n_on_TAP_bits;
    int on_TAP_bits[200];

    char eventTag[64];
    };

}  //namespace MuonCalib
#endif
