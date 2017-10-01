/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA_G4SIMTIMER_H
#define G4USERACTIONS_G4UA_G4SIMTIMER_H

// Infrastructure includes
#include "AthenaKernel/MsgStreamMember.h"

// Geant4 includes
#include "G4Timer.hh"
#include "G4UserEventAction.hh"

// Forward declarations
class G4Event;
class G4Run;

namespace G4UA
{

  /// @class G4SimTimer
  /// @brief A user action for monitoring G4 runtime at event and run level.
  ///
  /// This class implements the event and run action interfaces.
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
  class G4SimTimer : public G4UserEventAction
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
	  nEvent += rep.nEvent;
	  eventTime += rep.eventTime;
	  eventTimeSquared += rep.eventTimeSquared;
	}
      };

      /// Constructor
      G4SimTimer();

      /// Start timing this Geant4 event.
      virtual void BeginOfEventAction(const G4Event* event) override final;

      /// Finish timing this Geant4 event.
      virtual void EndOfEventAction(const G4Event* event) override final;

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
