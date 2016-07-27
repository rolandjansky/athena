/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_G4SimTimer_H
#define G4UserActions_G4SimTimer_H

#include "G4AtlasTools/UserActionBase.h"

#include <string>

#include "G4Timer.hh"

/// Current version of the G4SimTimer user action
class G4SimTimer final: public UserActionBase {

 private:
  double runTime;
  double eventTime;
  double accumulatedEventTime;
  double accumulatedEventTimeSquared;
  unsigned int nrOfEntries;
  G4Timer* runTimer;
  G4Timer* eventTimer;
  double averageTimePerEvent();
  double Sigma();

 public:

  G4SimTimer(const std::string& type, const std::string& name, const IInterface* parent)
    : UserActionBase(type, name, parent), runTime(0), eventTime(0),
      accumulatedEventTime(0), accumulatedEventTimeSquared(0),
      nrOfEntries(0)
  {
    runTimer = new G4Timer();
    eventTimer = new G4Timer();
  }

  ~G4SimTimer() {delete runTimer; delete eventTimer;}

  virtual StatusCode initialize() override;

  virtual void BeginOfEvent(const G4Event*) override;
  virtual void EndOfEvent(const G4Event*) override;
  virtual void BeginOfRun(const G4Run*) override;
  virtual void EndOfRun(const G4Run*) override;

  virtual StatusCode queryInterface(const InterfaceID&, void**) override;

};

#endif // G4SimTimer_H


//=============================================================================
// New design below for multithreading
//=============================================================================


#ifndef G4USERACTIONS_G4UA_G4SIMTIMER_H
#define G4USERACTIONS_G4UA_G4SIMTIMER_H

// Infrastructure includes
#include "AthenaKernel/MsgStreamMember.h"
#include "G4AtlasInterfaces/IBeginEventAction.h"
#include "G4AtlasInterfaces/IEndEventAction.h"

// Geant4 includes
#include "G4Timer.hh"

// Forward declarations
class G4Event;
class G4Run;

namespace G4UA
{

  /// @class G4SimTimer
  /// @brief A user action for monitoring G4 runtime at event and run level.
  ///
  /// This class implements the BeginEvent and EndEvent, and BeginRun actions.
  /// The implementation was mostly taken from the previous G4SimTimer design.
  /// Results across worker threads are merged in finalize method of the
  /// G4SimTimerTool.
  ///
  /// IMPORTANT NOTE: The G4SimTimer results will seem nonsensical in MT
  /// because the USER time is reported which sums over all threads in the
  /// process. The per-event timing can roughly be determined by dividing
  /// by the number of threads, but this is only precise when averaged over
  /// all events.
  ///
  /// @todo TODO: Enable the run timer using a run-action (now possible).
  ///
  /// @author Steve Farrell <Steven.Farrell>
  /// @author ???
  ///
  class G4SimTimer : public IBeginEventAction, public IEndEventAction
  {

    public:

      /// Simple struct which holds timing results for an action instance.
      struct Report
      {
        /// Initializes the variables
        Report();
        /// Number of timed G4 events (we skip the first).
        /// Might want to use a larger int type.
        unsigned int nEvent;
        /// Accumulated event time
        double eventTime;
        /// Accumulated squared event time
        double eventTimeSquared;
        /// Calculate the mean and sample std dev
        std::pair<double, double> meanAndSigma();

	void merge(const Report& rep){
	  nEvent+=rep.nEvent;
	  eventTime+=rep.eventTime;
	  eventTimeSquared+=rep.eventTimeSquared;
	}
      };

      /// Constructor
      G4SimTimer();

      /// Start timing this Geant4 event.
      virtual void beginOfEvent(const G4Event* event) override;

      /// Finish timing this Geant4 event.
      virtual void endOfEvent(const G4Event* event) override;

      /// Retrieve my timing results
      const Report& getReport() const
      { return m_results; }

    private:

      /// Log a message using the Athena controlled logging system
      MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }

      /// Private message stream member
      mutable Athena::MsgStreamMember m_msg;

      /// My private instance of an event timer.
      G4Timer m_eventTimer;

      /// My timing results
      Report m_results;

      /// Used to skip the first event
      bool m_firstEvent;

  }; // class G4SimTimer

} // namespace G4UA

#endif // G4USERACTIONS_G4UA_G4SIMTIMER_H
