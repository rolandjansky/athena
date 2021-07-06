/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARROD_LARFEBERRORSUMMARYMAKER
#define LARROD_LARFEBERRORSUMMARYMAKER


/********************************************************************

NAME:     LArFebSummaryMaker
          Algorithm that makes LArFebSummary 

********************************************************************/

#include <AthenaBaseComps/AthReentrantAlgorithm.h>
#include <GaudiKernel/ServiceHandle.h>
#include <GaudiKernel/ToolHandle.h>

#include "LArRawEvent/LArFebHeaderContainer.h"
#include "LArRawEvent/LArFebErrorSummary.h"
#include "LArRecConditions/LArBadChannelCont.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <array>
#include <set>

class LArOnlineID; 

class LArFebErrorSummaryMaker : public AthReentrantAlgorithm
{
 public:

  using AthReentrantAlgorithm::AthReentrantAlgorithm; 
  virtual ~LArFebErrorSummaryMaker() = default;

  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;
  virtual StatusCode finalize() override final;

 private:

  //Atomic counters:
  mutable std::atomic<int> m_missingFebsWarns{0}; //counter for missing FEB warnings
  mutable std::array<std::atomic<unsigned>, LArFebErrorSummary::N_LArFebErrorType > m_errors; //error types accumulator

  //The following variables are set in initialize:
  std::set<unsigned int> m_all_febs ; 
  bool m_isHec=false;
  bool m_isFcal=false;
  bool m_isEmb=false;
  bool m_isEmec=false;
  bool m_isEmPS=false;
  bool m_isAside=false;
  bool m_isCside=false;

  const LArOnlineID* m_onlineHelper=nullptr;

  // properties:
  Gaudi::Property<int> m_warnLimit{ this, "warnLimit", 10, "Limit the number of warning messages for missing input" };
  Gaudi::Property<bool> m_checkAllFeb{ this, "CheckAllFEB", true, "Check all FEBS ?" };
  Gaudi::Property<std::string> m_partition{ this, "PartitionId", "", "Should contain DAQ partition (+ eventually the EventBuilder)" };
  Gaudi::Property<std::vector<unsigned int> > m_knownEvtId{ this, "MaskFebEvtId", {}, "ignore these FEBs for EvtId" };
  Gaudi::Property<std::vector<unsigned int> > m_knownSCACStatus{ this, "MaskFebScacStatus", {}, "ignore these FEBs for ScacStatus" };
  Gaudi::Property<std::vector<unsigned int> > m_knownZeroSample{ this, "MaskFebZeroSample", {}, "ignore these FEBs for ZeroSample" };

  SG::ReadCondHandleKey<LArBadFebCont> m_bfKey{this,"BFKey","LArBadFeb","Key of the BadFebContainer in the conditions store"};
  SG::ReadHandleKey<LArFebHeaderContainer> m_readKey{this,"ReadKey","LArFebHeader"};
  SG::WriteHandleKey<LArFebErrorSummary> m_writeKey{this,"WriteKey","LArFebErrorSummary"};

  // methods:
  bool masked (unsigned int hid, const std::vector<unsigned int>& v_feb) const; 
};
#endif










