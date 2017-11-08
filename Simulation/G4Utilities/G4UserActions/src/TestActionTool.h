/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_TESTACTIONTOOL_H
#define G4USERACTIONS_TESTACTIONTOOL_H

// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"

// Geant4 includes
#include "G4UserRunAction.hh"
#include "G4UserEventAction.hh"
#include "G4UserSteppingAction.hh"

// G4Atlas includes
#include "G4AtlasInterfaces/IUserActionTool.h"
#include "G4AtlasTools/UserActionToolBase.h"


namespace G4UA
{

  /// @class TestAction
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class TestAction : public G4UserRunAction,
                     public G4UserEventAction,
                     public G4UserSteppingAction
  {

    public:

      /// Results to be accumulated across threads at end of job
      struct Report
      {
        int numEvent = 0;
        int numStep = 0;
        void merge(const Report& other) {
          numEvent += other.numEvent;
          numStep += other.numStep;
        }
      };

      /// Constructor
      TestAction();

      /// Executes before the run
      virtual void BeginOfRunAction(const G4Run*) override final;

      /// Exeuctes after the run
      virtual void EndOfRunAction(const G4Run*) override final;

      /// Executes at beginning of G4 event
      virtual void BeginOfEventAction(const G4Event*) override final;

      /// Executes at end of G4 event
      virtual void EndOfEventAction(const G4Event*) override final;

      /// Stepping action method
      virtual void UserSteppingAction(const G4Step* step) override final;

      /// Get the results report
      inline const Report& getReport() const
      { return m_report; }

    private:

      /// Results report
      Report m_report;

  }; // class TestAction


  /// @class TestActionTool
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class TestActionTool
    : public extends<UserActionToolBase<TestAction>, IUserActionTool>
  {

    public:

      /// Standard constructor
      TestActionTool(const std::string& type, const std::string& name,
                     const IInterface* parent);

      /// Initialize the tool
      virtual StatusCode initialize() override final;

      /// Finalize the tool
      virtual StatusCode finalize() override final;

      /// Fill user action
      virtual StatusCode
      fillUserAction(G4AtlasUserActions* actions) override final;

  }; // class TestActionTool

} // namespace G4UA

#endif // G4USERACTIONS_TESTACTIONTOOL_H
