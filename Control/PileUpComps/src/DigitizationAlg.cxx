/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DigitizationAlg.h"

#include "PileUpTools/IPileUpTool.h"

/////////////////////////////////////////////////////////////////////////////

DigitizationAlg::DigitizationAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_puTools(/*this*/) // TODO make the PileUpTools private
{
  declareProperty("PileUpTools", m_puTools, "IPileUpTools to be run for each event");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode DigitizationAlg::initialize()
{
  ATH_MSG_DEBUG ( "Initializing " << name() << " - package version " << PACKAGE_VERSION );
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
      ATH_CHECK(puToolHandle->processAllSubEvents());
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
