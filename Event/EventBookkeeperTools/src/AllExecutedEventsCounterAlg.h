/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

#ifndef EVENT_BOOKKEEPER_TOOLS__ALL_EXECUTED_EVENTS_COUNTER_ALG_H
#define EVENT_BOOKKEEPER_TOOLS__ALL_EXECUTED_EVENTS_COUNTER_ALG_H

#include <AthenaBaseComps/AthReentrantAlgorithm.h>
#include <CxxUtils/checker_macros.h>
#include <EventBookkeeperTools/FilterReporterParams.h>


class AllExecutedEventsCounterAlg : public AthReentrantAlgorithm
{
public:

  AllExecutedEventsCounterAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize ATLAS_NOT_THREAD_SAFE () override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;
  virtual StatusCode finalize() override final;

private:
  FilterReporterParams m_filterParams {this, "AllExecutedEvents", "Number of processed events before any cut"};
};

#endif
