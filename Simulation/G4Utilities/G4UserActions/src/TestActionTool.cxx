/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Main header include
#include "TestActionTool.h"

// Geant4 includes
#include "G4ios.hh"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // TestAction constructor
  //---------------------------------------------------------------------------
  TestAction::TestAction()
  {}

  //---------------------------------------------------------------------------
  // Begin of G4 run method
  //---------------------------------------------------------------------------
  void TestAction::BeginOfRunAction(const G4Run*)
  {
    G4cout << "TestAction::BeginOfRunAction" << G4endl;
  }

  //---------------------------------------------------------------------------
  // End of G4 run method
  //---------------------------------------------------------------------------
  void TestAction::EndOfRunAction(const G4Run*)
  {
    G4cout << "TestAction::EndOfRunAction - "
           << m_report.numEvent << " events processed, "
           << m_report.numStep << " steps processed"
           << G4endl;
  }

  //---------------------------------------------------------------------------
  // Begin of G4 event method
  //---------------------------------------------------------------------------
  void TestAction::BeginOfEventAction(const G4Event*)
  {
    G4cout << "TestAction::BeginOfEventAction" << G4endl;
    m_report.numEvent++;
  }

  //---------------------------------------------------------------------------
  // End of G4 event method
  //---------------------------------------------------------------------------
  void TestAction::EndOfEventAction(const G4Event*)
  {
    G4cout << "TestAction::EndOfEventAction" << G4endl;
  }

  //---------------------------------------------------------------------------
  // G4 stepping method
  //---------------------------------------------------------------------------
  void TestAction::UserSteppingAction(const G4Step*)
  {
    m_report.numStep++;
  }

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  TestActionTool::TestActionTool(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent)
    : UserActionToolBase<TestAction>(type, name, parent)
  {
  }

  //---------------------------------------------------------------------------
  // Initialize the tool
  //---------------------------------------------------------------------------
  StatusCode TestActionTool::initialize()
  {
    ATH_MSG_INFO( "Initializing " << name() );
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Finalize the tool
  //---------------------------------------------------------------------------
  StatusCode TestActionTool::finalize()
  {
    ATH_MSG_INFO( "Finalizing " << name() );

    // Accumulate results across threads
    TestAction::Report results;
    m_actions.accumulate(results, &TestAction::getReport, &TestAction::Report::merge);
    ATH_MSG_INFO("Final accumulated results: " <<
                 results.numEvent << " events processed, " <<
                 results.numStep << " steps processed");

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Construct and fill the user action
  //---------------------------------------------------------------------------
  StatusCode TestActionTool::fillUserAction(G4AtlasUserActions& actions)
  {
    ATH_MSG_INFO("Constructing and filling a TestAction");

    auto action = std::make_unique<TestAction>();
    actions.runActions.push_back( action.get() );
    actions.eventActions.push_back( action.get() );
    actions.steppingActions.push_back( action.get() );
    m_actions.set( std::move(action) );

    return StatusCode::SUCCESS;
  }

} // namespace G4UA
