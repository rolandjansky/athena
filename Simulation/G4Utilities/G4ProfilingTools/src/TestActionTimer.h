/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
//                                                              //
//     $Id: TestActionTimer.h 660034 2015-04-10 17:20:49Z jchapman $ //
//                                                              //
//                      TestActionTimer                         //
//      Code for text output (into log file)                    //
//      of information about the time spent simulating          //
//      various pieces of the detector and particles.           //
//                                                              //
//      Written by                                              //
//         Zachary Marshall, Caltech, USA                       //
//         Wolfgang Ehrenfeld, University of Hamburg, Germany   //
//                                                              //
//      @version $Revision: 660034 $ //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef TestActionTimer_H
#define TestActionTimer_H

#include "G4AtlasTools/UserActionBase.h"

#include "G4String.hh"

#include <string>
#include <vector>

class G4Run;
class G4Event;
class G4Step;
class G4Timer;
class ITHistSvc;

class TestActionTimer final: public UserActionBase 
{  
 public:

  TestActionTimer(const std::string& type, const std::string& name, const IInterface* parent);
  virtual void BeginOfEvent(const G4Event*) override; //!< Action that starts the new event timers
  virtual void EndOfEvent(const G4Event*) override;   //!< Action that prints all available information at the end of each event
  virtual void BeginOfRun(const G4Run*) override;     //!< Action that starts the timers at the beginning of the run
  virtual void EndOfRun(const G4Run*) override;       //!< Action that prints all information at the end of the run
  virtual void Step(const G4Step*) override;      //!< Stepping action that increments the appropriate timer
  virtual StatusCode queryInterface(const InterfaceID&, void**);

private:
  /* Enumeration for timers to be used
  First timers are by subdetector, second few are by particle
  These are not straightforward for the non-expert to interpret*/
  enum { eEMB, eEMEC, eFC1, eFC23, eFCO, eHEC, eCry, eLAr, eHCB, 
	 ePre, eMu, ePx, eSct, eSev, eTrt, eOther, 
	 eElec, ePos, eGam, eNeut, eMax };

  G4Timer* m_runTimer;                     //!< Timer for the entire run
  G4Timer* m_eventTimer;                   //!< Timer for this event
  double m_runTime, m_eventTime;           //!< Double for storing this event and run time
    
  std::vector<G4Timer*> m_timer;           //!< Vector of timers for each of the enum
  std::vector<double> m_time;              //!< Vector of times for each of the enum
  std::vector<std::string> m_timeName;     //!< Vector of names for each of the timers
  
  double TimerSum(G4Timer* timer) const;   //!< Gets the appropriate time from the timer for adding to the sum
  int m_nev;

  ServiceHandle<ITHistSvc> m_histSvc;

  void PPanic();                           //!< Method to shut down all particle timers
  void VPanic();                           //!< Method to shut down all volume timers
  int ClassifyVolume( G4String& ) const; //!< Method to sort out which volume we are in
};

#endif // #define TestActionTimer_H
