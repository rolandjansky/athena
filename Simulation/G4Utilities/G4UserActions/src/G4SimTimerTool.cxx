/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/G4SimTimerTool.h"
#include "CxxUtils/make_unique.h"

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
    declareInterface<IG4EventActionTool>(this);
  }

  //---------------------------------------------------------------------------
  // Initialize - temporarily here for debugging
  //---------------------------------------------------------------------------
  StatusCode G4SimTimerTool::initialize()
  {
    ATH_MSG_DEBUG( "Initializing " << name() );
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Merge results from all threads
  //---------------------------------------------------------------------------
  StatusCode G4SimTimerTool::finalize()
  {
    ATH_MSG_DEBUG( "Finalizing " << name() );

    mergeReports();

    // Report the results
    auto meanSigma = m_report.meanAndSigma();
    ATH_MSG_INFO("Finalized timing results for " << m_report.nEvent <<
                 " events (not all events used)");
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
    ATH_MSG_DEBUG("Making a G4SimTimer action");
    return CxxUtils::make_unique<G4SimTimer>();
  }

}
