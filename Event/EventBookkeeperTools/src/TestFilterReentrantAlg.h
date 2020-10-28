/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

#ifndef EVENT_BOOKKEEPER_TOOLS__TEST_FILTER_REENTRANT_ALG_H
#define EVENT_BOOKKEEPER_TOOLS__TEST_FILTER_REENTRANT_ALG_H

#include <AthenaBaseComps/AthReentrantAlgorithm.h>
#include <EventBookkeeperTools/FilterReporterParams.h>

class TestFilterReentrantAlg : public AthReentrantAlgorithm
{
public:
  TestFilterReentrantAlg(const std::string &name, ISvcLocator *pSvcLocator);
  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext &ctx) const override final;
  virtual StatusCode finalize() override final;

private:
  FilterReporterParams m_filterParams {this, "TestReentrantAlg", "Test filter in a reentrant alg"};
  Gaudi::Property<int> m_modulo {this, "Modulo", 2, ""};
};

#endif
