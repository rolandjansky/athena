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


#include "G4String.hh"

#include <string>
#include <vector>

class G4Run;
class G4Event;
class G4Step;
class G4Timer;
class ITHistSvc;


#include "G4AtlasInterfaces/IBeginEventAction.h"
#include "G4AtlasInterfaces/IEndEventAction.h"
#include "G4AtlasInterfaces/IBeginRunAction.h"
#include "G4AtlasInterfaces/IEndRunAction.h"
#include "G4AtlasInterfaces/ISteppingAction.h"
namespace G4UA{
  
  /// @class TestActionTimer
  /// @brief User action to measure time spent in subdetectors
  ///
  /// 
  ///         @author Zachary Marshall, Caltech, USA                       
  ///         @author Wolfgang Ehrenfeld, University of Hamburg, Germany

  class TestActionTimer:
  public IBeginEventAction,  public IEndEventAction,  public IBeginRunAction,  public IEndRunAction,  public ISteppingAction
  {
    
  public:
    /// constructor
    TestActionTimer();
    
    /// this holds all the data from individual threads that needs to be merged at EoR
    struct Report
    {
      int nev=0;
      std::vector<double> time; //!< Vector of timers for each of the enum
      std::vector<std::string> timeName;  //!< Vector of names for each of the timers
      double runTime=0;
      void merge(const Report& rep){
	nev+=rep.nev;
	runTime+=rep.runTime;
	// copy first report
	if(time.empty()){
	  time=rep.time;
	  timeName=rep.timeName;
	  return;
	}
	// sum the following ones
	for(unsigned int i=0;i<time.size();++i)
	  time[i]+=rep.time[i];
      }
    };
    
    const Report& getReport() const
    { return m_report; }
    
    virtual void beginOfEvent(const G4Event*) override;
    virtual void endOfEvent(const G4Event*) override;
    virtual void beginOfRun(const G4Run*) override;
    virtual void endOfRun(const G4Run*) override;
    virtual void processStep(const G4Step*) override;

    /* Enumeration for timers to be used
       First timers are by subdetector, second few are by particle
       These are not straightforward for the non-expert to interpret*/
    enum { eEMB, eEMEC, eFC1, eFC23, eFCO, eHEC, eCry, eLAr, eHCB, 
	   ePre, eMu, ePx, eSct, eSev, eTrt, eOther, 
	   eElec, ePos, eGam, eNeut, eMax };

  private:
    Report m_report;

    
    G4Timer* m_runTimer;                     //!< Timer for the entire run
    G4Timer* m_eventTimer;                   //!< Timer for this event
    double  m_eventTime;           //!< Double for storing this event
    
    std::vector<G4Timer*> m_timer;           //!< Vector of timers for each of the enum
    
    double TimerSum(G4Timer* timer) const;   //!< Gets the appropriate time from the timer for adding to the sum
    
    void PPanic();                           //!< Method to shut down all particle timers
    void VPanic();                           //!< Method to shut down all volume timers
    int ClassifyVolume( G4String& ) const; //!< Method to sort out which volume we are in
    
  }; // class TestActionTimer
  
  
} // namespace G4UA 

#endif // #define TestActionTimer_H
