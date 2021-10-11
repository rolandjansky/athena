/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DigitizationAlg.h"

/////////////////////////////////////////////////////////////////////////////

DigitizationAlg::DigitizationAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode DigitizationAlg::initialize()
{
  ATH_MSG_DEBUG ( "Initializing " << name() );
  //locate the PileupTools and initialize them
  ATH_CHECK(m_puTools.retrieve());
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode DigitizationAlg::execute()
{
  ATH_MSG_DEBUG ("in execute()");
  for(auto& puToolHandle : m_puTools)
    {
      // Reset the filter first
      puToolHandle->resetFilter();
      ATH_CHECK(puToolHandle->processAllSubEvents(Gaudi::Hive::currentContext()));
      // Check if the event was filtered out by the current PileUpTool.
      if (!puToolHandle->filterPassed())
        {
          ATH_MSG_VERBOSE( "Filter " << puToolHandle->name() << " failed - will stop the event" );
          this->setFilterPassed(false);
          break;
        }
    }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode DigitizationAlg::finalize() {

  ATH_MSG_INFO ("in finalize()");
  return StatusCode::SUCCESS;
}
