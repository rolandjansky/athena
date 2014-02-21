/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
//                                                              //
//     $Id: TestActionTimer.h 448395 2011-07-12 18:28:47Z gsedov $ //
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
//      @version $Revision: 448395 $ //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef TestActionTimer_H
#define TestActionTimer_H

#include "FadsActions/UserAction.h"

#include <string>
#include <vector>

class G4Run;
class G4Event;
class G4Step;
class G4Timer;

class TestActionTimer: public FADS::UserAction 
{  
 public:

  TestActionTimer(std::string s);
  void BeginOfEventAction(const G4Event*); //!< Action that starts the new event timers
  void EndOfEventAction(const G4Event*);   //!< Action that prints all available information at the end of each event
  void BeginOfRunAction(const G4Run*);     //!< Action that starts the timers at the beginning of the run
  void EndOfRunAction(const G4Run*);       //!< Action that prints all information at the end of the run
  void SteppingAction(const G4Step*);      //!< Stepping action that increments the appropriate timer

private:

  /* Enumeration for timers to be used
  First timers are by subdetector, second few are by particle
  These are not straightforward for the non-expert to interpret*/
  enum { eEMB, eEMEC, eFC1, eFC23, eFCO, eHEC, eCry, eLAr, eHCB, 
	 ePre, eMu, ePx, eSct, eSev, eTrt, eOther, 
	 eElec, eGam, eMax };

  G4Timer* m_runTimer;                     //!< Timer for the entire run
  G4Timer* m_eventTimer;                   //!< Timer for this event
  double m_runTime, m_eventTime;           //!< Double for storing this event and run time
    
  std::vector<G4Timer*> m_timer;           //!< Vector of timers for each of the enum
  std::vector<double> m_time;              //!< Vector of times for each of the enum
  std::vector<std::string> m_timeName;     //!< Vector of names for each of the timers
  
  double TimerSum(G4Timer* timer) const;   //!< Gets the appropriate time from the timer for adding to the sum
  int m_nev;

  void PPanic();                           //!< Method to shut down all particle timers
  void VPanic();                           //!< Method to shut down all volume timers
};

#endif // #define TestActionTimer_H
