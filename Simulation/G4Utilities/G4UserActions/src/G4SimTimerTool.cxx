/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "G4UserActions/G4SimTimerTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  G4SimTimerTool::G4SimTimerTool(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent)
    : ActionToolBaseReport<G4SimTimer>(type, name, parent)
  {
    declareInterface<IBeginEventActionTool>(this);
    declareInterface<IEndEventActionTool>(this);
  }

  //---------------------------------------------------------------------------
  // Initialize - temporarily here for debugging
  //---------------------------------------------------------------------------
  StatusCode G4SimTimerTool::initialize()
  {
    ATH_MSG_DEBUG("initialize");
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Merge results from all threads
  //---------------------------------------------------------------------------
  StatusCode G4SimTimerTool::finalize()
  {
    ATH_MSG_DEBUG("finalize");

    mergeReports();

    // Report the results
    auto meanSigma = m_report.meanAndSigma();
    ATH_MSG_INFO("Finalized timing results for " << m_report.nEvent <<
                 " events (will be less than total)");
    ATH_MSG_INFO("Average time per event was " <<
                 std::setprecision(4) << meanSigma.first << " +- " <<
                 std::setprecision(4) << meanSigma.second);
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the action on request
  //---------------------------------------------------------------------------
  std::unique_ptr<G4SimTimer>
  G4SimTimerTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<G4SimTimer>();
    return std::move(action);
  }

}
