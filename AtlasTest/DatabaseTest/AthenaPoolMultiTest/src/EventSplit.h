/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// EventSplit.h

#ifndef ATHENAPOOLMULTITEST_EVENTSPLIT_H
#define ATHENAPOOLMULTITEST_EVENTSPLIT_H
/**
 * @file EventSplit.h
 * @brief class definition for EventSplit
 */
 
/**
 * @class EventSplit
 * @brief Simple Filter algorithm which sets the filter flag based on 
 * the FauxTriggerMap object (created by <i>AddTrigMap</i>). The user can 
 * set the bit masks to determine the filter behavior. Reusable.
 * <br>
 * @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
 * Created May 2004 for DC2 Validation tests
 * <i>No settable properties</i>
 * $Id: EventSplit.h,v 1.1 2009-02-19 22:04:49 cranshaw Exp $
 */
             

#include "AthenaBaseComps/AthFilterAlgorithm.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include "AthenaPoolTestData/FauxTriggerMap.h"

class string;
class StoreGateSvc;

class EventSplit : public AthFilterAlgorithm {
public:
  EventSplit(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~EventSplit();
  
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
private:
  unsigned int m_l1bits;    /// level 1 bit mask 
  unsigned int m_l2bits;    /// level 2 bit mask 
  SG::ReadHandleKey<xAOD::EventInfo> m_evt
  { this, "EventInfo", "EventInfo", "" };
  SG::ReadHandleKey<FauxTriggerMap> m_rftm
  { this, "MultiTestTrigMap", "MultiTestTrigMap", "" };
};
#endif
