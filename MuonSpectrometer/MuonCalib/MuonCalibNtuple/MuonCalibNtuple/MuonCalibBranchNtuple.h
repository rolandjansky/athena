/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBBRANCHNTUPLE_H
#define MUONCALIBBRANCHNTUPLE_H

#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibEventBase/MuonCalibPattern.h"

//include all Ntuple Branch classes
#include "MuonCalibNtuple/MdtCalibHitNtupleBranch.h"
#include "MuonCalibNtuple/RpcCalibHitNtupleBranch.h"
#include "MuonCalibNtuple/TgcCalibHitNtupleBranch.h"
#include "MuonCalibNtuple/CscCalibHitNtupleBranch.h"
#include "MuonCalibNtuple/MuonEventNtupleBranch.h"
#include "MuonCalibNtuple/MuonTruthNtupleBranch.h"
#include "MuonCalibNtuple/MdtTruthHitNtupleBranch.h"
#include "MuonCalibNtuple/RpcTruthHitNtupleBranch.h"
#include "MuonCalibNtuple/TgcTruthHitNtupleBranch.h"
#include "MuonCalibNtuple/CscTruthHitNtupleBranch.h"
#include "MuonCalibNtuple/MuonSegmentNtupleBranch.h"
#include "MuonCalibNtuple/MuonPatternNtupleBranch.h"
#include "MuonCalibNtuple/MdtRawHitNtupleBranch.h"
#include "MuonCalibNtuple/RpcRawHitNtupleBranch.h"
#include "MuonCalibNtuple/RpcRawTriggerHitNtupleBranch.h"
#include "MuonCalibNtuple/TgcRawHitNtupleBranch.h"
#include "MuonCalibNtuple/TgcRawCoinNtupleBranch.h"
#include "MuonCalibNtuple/RpcRawCoinNtupleBranch.h"
#include "MuonCalibNtuple/CscRawHitNtupleBranch.h"
#include "MuonCalibNtuple/MuonCalibRpcSectorLogicBranch.h"

#include "MdtCalibFitters/LocalSegmentResolver.h"

//#include "TrackRecord/TrackRecord.h"
#include <map>
#include <string>

class TFile;
class TTree;
class TDirectory;

namespace MuonCalib {
  class MuonFixedId;
  class IMdtSegmentFitter;
  class MuonCalibEventInfo;
  class MuonCalibTruth;
  class MuonCalibRawHitCollection;
  class MuonCalibRawTriggerHitCollection;
  class MuonCalibMdtTruthHit;
  class MuonCalibRpcTruthHit;
  class MuonCalibTgcTruthHit;
  class MuonCalibCscTruthHit;

  /**
     @class  MuonCalibBranchNtuple
     Class to handle ntuple containing MuonCalibEvents. Steers the
     separate NtupleBranches. Uses FillToBranch class for
     short-notation filling of certain NtupleBranches.
  */

  class MuonCalibBranchNtuple {
  public:
    MuonCalibBranchNtuple(bool booTree=true, bool doRawTgc=false);                        //!< default constructor
    MuonCalibBranchNtuple(std::string RegionName, bool booTree=true, bool doRawTgc=false);  //!< constructor initializing specific region
    ~MuonCalibBranchNtuple();                       //!< destructor

    void handleRawHits( const MuonCalibRawHitCollection &rawHits ); //!< fills branch with raw hit collection. Calls the specific MuonCalibRawXxxNtupleBranch fill routine per MuonCalibRawXxxHit on the collection.
    void handleRawTriggerHits( const MuonCalibRawTriggerHitCollection &rawTriggerHits ); //!< fills branch with raw trigger hit collection. Calls the specific MuonCalibRawTriggerXxxNtupleBranch fill routine per MuonCalibRawTriggerXxxHit on the collection.
    void handlePattern(const MuonCalibPattern &pat);                //!< fills branch with MuonCalibPattern information, calling handleSegment per MuonCalibSegment on the MuonCalibPattern
    void handleSegment(const MuonCalibSegment &seg);                //!< calls fillSegmentToTree
    void fillTruthToTree( const MuonCalibTruth &truth);             //!< fills branch with MuonCalibTruth information
    void fillMdtTruthToTree( const MuonCalibMdtTruthHit &mdtTruth );//!< fills branch with MuonCalibMdtTruthHit information
    void fillRpcTruthToTree( const MuonCalibRpcTruthHit &rpcTruth );//!< fills branch with MuonCalibRpcTruthHit information
    void fillTgcTruthToTree( const MuonCalibTgcTruthHit &tgcTruth );//!< fills branch with MuonCalibTgcTruthHit information
    void fillCscTruthToTree( const MuonCalibCscTruthHit &cscTruth );//!< fills branch with MuonCalibCscTruthHit information
    void fillRpcSectorLogic( const RpcSectorLogicContainer &rpcSlContainer );//!< fills branch with MuonCalibCscTruthHit information
    void fillEventToTree( const MuonCalibEventInfo &eventInfo, const MuonCalibTriggerTimeInfo &triggerTimeInfo = MuonCalibTriggerTimeInfo());  //!< fills branch with MuonCalibEventInfo information
    void setEvent(unsigned int n);                                  //!< sets eventcounter to n and resets all NtupleBranches
    void finishEvent();                                             //!< outputs summary information of the NtupleBranches and fills Tree

    void setPrintLevel(int level);                                  //!< set print level

    void  writeTree();                                              //!< Writes the Tree

    inline TTree * getTTree() { return m_tree; }

  private:
    void fillSegmentToTree(const MuonCalibSegment &seg);            //!< fills branch with MuonCalibSegment information. Per segment, it fills XxxCalibHit information on the appropriate XxxCalibHitNtupleBranches.

    void  openFile();                           //!< creates a new TFile "segments.root" and sets m_directory member to this TFile
    void  closeFile();                          //!< wries file, closes file, deletes file
    void  bookTree(bool doTruth=true, bool doRawTgc=false);                           //!< creates TTree in TFile, creates all NtupleBranches
    void  createTruthBranch(TTree *tree);       //!< creates MuonCalibTruthNtupleBranch instance
    void  createMdtTruthHitBranch(TTree *tree); //!< creates MdtCalibTruthHitNtupleBranch instance
    void  createRpcTruthHitBranch(TTree *tree); //!< creates RpcCalibTruthHitNtupleBranch instance
    void  createTgcTruthHitBranch(TTree *tree); //!< creates TgcCalibTruthHitNtupleBranch instance
    void  createCscTruthHitBranch(TTree *tree); //!< creates CscCalibTruthHitNtupleBranch instance
    void  createEventBranch(TTree *tree);       //!< creates MuonCalibEventInfoNtupleBranch instance
    void  createPatternBranch(TTree *tree);     //!< creates MuonCalibPatternNtupleBranch instance
    void  createSegmentBranch(TTree *tree);     //!< creates MuonCalibSegmetnNtupleBranch instance
    void  createMdtCalibHitBranch(TTree *tree); //!< creates MdtCalibTruthHitNtupleBranch instance
    void  createRpcCalibHitBranch(TTree *tree); //!< creates RpcCalibTruthHitNtupleBranch instance
    void  createTgcCalibHitBranch(TTree *tree); //!< creates TgcCalibTruthHitNtupleBranch instance
    void  createCscCalibHitBranch(TTree *tree); //!< creates CscCalibTruthHitNtupleBranch instance
    void  createMdtRawHitBranch(TTree *tree);   //!< creates MuonCalibRawMdtHitNtupleBranch instance
    void  createRpcRawHitBranch(TTree *tree);   //!< creates MuonCalibRawRpcHitNtupleBranch instance
    void  createTgcRawHitBranch(TTree *tree);   //!< creates MuonCalibRawTgcHitNtupleBranch instance
    void  createTgcRawCoinBranch(TTree *tree);  //!< creates MuonCalibRawTgcCoinNtupleBranch instance
    void  createRpcRawCoinBranch(TTree *tree);  //!< creates MuonCalibRawRpcCoinNtupleBranch instance
    void  createCscRawHitBranch(TTree *tree);   //!< creates MuonCalibRawCscHitNtupleBranch instance
    void  createRpcRawTriggerHitBranch(TTree *tree);   //!< creates MuonCalibRawRpcHitNtupleBranch instance

    IMdtSegmentFitter* createFitter();          //!< creates a DCSLFitter instance

    TFile      *m_file;                         //!< TFile member
    TDirectory *m_directory;                    //!< TDirectory member
    TTree      *m_tree;                         //!< TTree to store all branches in
    IMdtSegmentFitter *m_fitter;                //!< DCSLFitter instance
    LocalSegmentResolver m_resolver;            //!< Segment resolver instance

    int   event;                                //!< event counter

    MuonEventNtupleBranch   eventBranch;

    MuonTruthNtupleBranch   *truthBranch;
    MdtTruthHitNtupleBranch *mdtTruthBranch;
    RpcTruthHitNtupleBranch *rpcTruthBranch;
    TgcTruthHitNtupleBranch *tgcTruthBranch;
    CscTruthHitNtupleBranch *cscTruthBranch;

    MuonPatternNtupleBranch patBranch;
    MuonSegmentNtupleBranch segBranch;

    MdtCalibHitNtupleBranch mdtHitBranch;
    RpcCalibHitNtupleBranch rpcHitBranch;
    TgcCalibHitNtupleBranch tgcHitBranch;
    CscCalibHitNtupleBranch cscHitBranch;

    MdtRawHitNtupleBranch rawMdtHitBranch;
    RpcRawHitNtupleBranch rawRpcHitBranch;
    TgcRawHitNtupleBranch rawTgcHitBranch;
    TgcRawCoinNtupleBranch rawTgcCoinBranch;
    RpcRawCoinNtupleBranch rawRpcCoinBranch;
    CscRawHitNtupleBranch rawCscHitBranch;
    MuonCalibRpcSectorLogicBranch rpcSlBranch;
    RpcRawTriggerHitNtupleBranch rawRpcTriggerHitBranch;

    int m_printLevel;                         //!< print level

    // hidden assignment operator and copy constructor
    MuonCalibBranchNtuple & operator=(const MuonCalibBranchNtuple &right);
    MuonCalibBranchNtuple(const MuonCalibBranchNtuple&);

  };

}  //namespace MuonCalib

#endif
