// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         TrigDecisionMaker.h
 **
 **   Description:  - Algorithm-derived class to run after the Steering
 **                 - Finds the list of defined signatures at initialization
 **                 - For each event:
 **                   * Creates a TriggerDecision object
 **                   * Fills it with the outcome of each defined signature
 **                   * Stores it in StoreGate to be written to ESD/AOD
 **
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Ricardo Goncalo <Jose.Goncalo@cern.ch>    - Royal Holloway, U. of London
 **
 **   Created:      Tue May  09 14:55:56 GMT 2006
 **   Modified:
 **
 **************************************************************************/
#ifndef TrigDecisionMaker_TrigDecisionMaker_H
#define TrigDecisionMaker_TrigDecisionMaker_H

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// core stuff
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"

// trigger/configuration stuff
#include "TrigDecisionEvent/TrigDecision.h"

#include "TrigT1Result/RoIBResult.h"


// containers
#include <vector>
#include <string>
#include <map>
#include <atomic>



namespace HLT {
  class ILvl1ResultAccessTool;
  class HLTResult;
}

namespace LVL1CTP {
  class Lvl1Result;
}

namespace TrigConf {
  class ITrigConfigSvc;
}

class MsgStream;

namespace TrigDec {

  /**
   * @class TrigDecisionMaker
   *  Note: Legacy class. Used to process Run 1 and Run 2 RAW data.
   *  - Algorithm-derived class to run after the Steering
   *  - Finds the list of defined signatures at initialization
   *  - For each event:
   *    * Creates a TriggerDecision object
   *    * Fills it with the outcome of each defined signature
   *    * Stores it in StoreGate to be written to ESD/AOD
   *
   * @see TrigDecisionMakerMT
   * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
   * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
   * @author Ricardo Goncalo <Jose.Goncalo@cern.ch>    - Royal Holloway, U. of London
   * @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - UC Irvine - AGH-UST Krakow
   */
  class TrigDecisionMaker : public AthReentrantAlgorithm
  {

    enum ResultStatus { Unknown, OK, NotRequested, NotFound, SGError, ProcError }; //!< errorCode of trigger result retrieval
    enum TrigLevel {L1=1, L2, EF, HLT};

  public:

    TrigDecisionMaker(const std::string &name, ISvcLocator *pSvcLocator); //!< std Gaudi Algorithm constructor
    virtual ~TrigDecisionMaker();                                                 //!< std  deconstructor

    // IAlgorithm virtual methods to implement
    virtual StatusCode initialize() override; //!< std Gaudi initialize method -> read-in trigger configuration
    virtual StatusCode execute(const EventContext& ctx) const override;     //!< std Gaudi execute method  -> fill event-wise TrigDecision object, save in SG
    virtual StatusCode finalize() override;    //!< std Gaudi finalize method -> print out statistics

    ResultStatus getL1Result (const LVL1CTP::Lvl1Result*& result, const EventContext& ctx) const; //!< retrieve LVL1 result (called in execute)
    ResultStatus getHLTResult(const HLT::HLTResult*&   result, TrigLevel level, const EventContext& ctx) const; //!< retrieve HLT results (called in execute)

    char getBGByte(int BCId) const; //!< to get the BG byte encoded for a given BC

  private:

    Gaudi::Property<bool> m_doL1{this, "doL1", true, "flag whether or not to consider L1 trigger information"};
    Gaudi::Property<bool> m_doL2{this, "doL2", true, "flag whether or not to consider L2 trigger information"};
    Gaudi::Property<bool> m_doEF{this, "doEF", true, "flag whether or not to consider L3 (EF) trigger information"};
    Gaudi::Property<bool> m_doHLT{this, "doHLT", true, "flag whether or not to consider merged L2EF=HLT trigger information"};

//    bool m_doEvtInfo;

    SG::WriteHandleKey<TrigDecision> m_trigDecisionKey{this, "TrigDecisionKey", "TrigDecision", "SG key to save the TrigDecision object" };
    SG::ReadHandleKey<ROIB::RoIBResult> m_l1roibResultKey{this, "L1ROIBResultKey", "RoIBResult", "SK key to retrieve the L1 ROIB result from SG" };
    SG::ReadHandleKey<LVL1CTP::Lvl1Result> m_l1ResultKey{this, "L1ResultKey", "Lvl1Result", "SK key to retrieve the L1 result from SG" };
    SG::ReadHandleKey<HLT::HLTResult> m_l2ResultKey{this, "L2ResultKey", "HLTResult_L2", "SK key to retrieve the L2 result from SG" };
    SG::ReadHandleKey<HLT::HLTResult> m_efResultKey{this, "EFResultKey", "HLTResult_EF", "SK key to retrieve the EF result from SG" };
    SG::ReadHandleKey<HLT::HLTResult> m_hltResultKey{this, "HLTResultKey", "HLTResult_HLT", " SK key to retrieve the merged HLT result from SG" };

    ServiceHandle<TrigConf::ITrigConfigSvc> m_trigConfigSvc; //!< handle to the full (L1 & HLT) trigger config service
    ToolHandle<HLT::ILvl1ResultAccessTool> m_lvl1Tool;  //!< tool to ease the access to the L1 results (RoIs, items, etc)

    // For statistics
    mutable std::atomic<unsigned int> m_nEvents; //!< statistics: number of processed events
    mutable std::atomic<unsigned int> m_l1_error, m_l2_error, m_ef_error, m_hlt_error, m_td_error, m_td_skip;  //!< statistics: error numbers
    mutable std::atomic<unsigned int> m_l1_notFound, m_l2_notFound, m_ef_notFound, m_hlt_notFound; //!< statistics: number of events where L1, HLT results were not found
    mutable std::atomic<unsigned int> m_l1_notReq, m_l2_notReq, m_ef_notReq, m_hlt_notReq; //!< statistics: number of events where something was not requested
    mutable std::atomic<unsigned int> m_l1_passed, m_l2_passed, m_ef_passed, m_hlt_passed; //!< statistics: number of events that passed the given trigger lvl
  };
}

#endif
