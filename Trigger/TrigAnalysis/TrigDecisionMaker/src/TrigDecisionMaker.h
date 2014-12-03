// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// core stuff
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"

// trigger/configuration stuff
#include "TrigDecisionEvent/TrigDecision.h"


// containers
#include <vector>
#include <string>
#include <map>



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
   *  - Algorithm-derived class to run after the Steering
   *  - Finds the list of defined signatures at initialization
   *  - For each event:
   *    * Creates a TriggerDecision object
   *    * Fills it with the outcome of each defined signature
   *    * Stores it in StoreGate to be written to ESD/AOD
   *
   * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
   * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
   * @author Ricardo Goncalo <Jose.Goncalo@cern.ch>    - Royal Holloway, U. of London
   * @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - UC Irvine - AGH-UST Krakow
   */
  class TrigDecisionMaker : public AthAlgorithm
  {

    enum ResultStatus { Unknown, OK, NotRequested, NotFound, SGError, ProcError }; //!< errorCode of trigger result retrieval
    enum TrigLevel {L1=1, L2, EF, HLT};

  public:

    TrigDecisionMaker(const std::string &name, ISvcLocator *pSvcLocator); //!< std Gaudi Algorithm constructor
    ~TrigDecisionMaker();                                                 //!< std  deconstructor

    // IAlgorithm virtual methods to implement
    StatusCode initialize(); //!< std Gaudi initialize method -> read-in trigger configuration
    StatusCode execute();     //!< std Gaudi execute method  -> fill event-wise TrigDecision object, save in SG
    StatusCode finalize();    //!< std Gaudi finalize method -> print out statistics

    ResultStatus getL1Result (const LVL1CTP::Lvl1Result*& result); //!< retrieve LVL1 result (called in execute)
    ResultStatus getHLTResult(const HLT::HLTResult*&   result, TrigLevel level); //!< retrieve HLT results (called in execute)


    std::string updatedDecisionKey(); //!< make sure, we're not using a SG key twice

    char getBGByte(int BCId); //!< to get the BG byte encoded for a given BC



  private:

    bool m_doL1; //!< flag whether or not to consider L1 trigger information
    bool m_doL2; //!< flag whether or not to consider L2 trigger information
    bool m_doEF; //!< flag whether or not to consider L3 (EF) trigger information
    bool m_doHLT;//!< flag whether or not to consider merged L2EF=HLT trigger information
//    bool m_doEvtInfo;

    std::string m_trigDecisionKey; //!< SG key to save the TrigDecision object
    std::string m_l1ResultKey;     //!< SK key to retrieve the L1 result from SG
    std::string m_l2ResultKey;     //!< SK key to retrieve the L2 result from SG
    std::string m_efResultKey;     //!< SK key to retrieve the EF result from SG
    std::string m_hltResultKey;     //!< SK key to retrieve the merged HLT result from SG
//    std::string m_evtInfoKey;

    ServiceHandle<TrigConf::ITrigConfigSvc> m_trigConfigSvc; //!< handle to the full (L1 & HLT) trigger config service
    ToolHandle<HLT::ILvl1ResultAccessTool> m_lvl1Tool;  //!< tool to ease the access to the L1 results (RoIs, items, etc)

    // For statistics
    unsigned int m_nEvents; //!< statistics: number of processed events
    unsigned int m_l1_error, m_l2_error, m_ef_error, m_hlt_error, m_td_error, m_td_skip;  //!< statistics: error numbers
    unsigned int m_l1_notFound, m_l2_notFound, m_ef_notFound, m_hlt_notFound; //!< statistics: number of events where L1, HLT results were not found
    unsigned int m_l1_notReq, m_l2_notReq, m_ef_notReq, m_hlt_notReq; //!< statistics: number of events where something was not requested
    unsigned int m_l1_passed, m_l2_passed, m_ef_passed, m_hlt_passed; //!< statistics: number of events that passed the given trigger lvl
  };
}

#endif
