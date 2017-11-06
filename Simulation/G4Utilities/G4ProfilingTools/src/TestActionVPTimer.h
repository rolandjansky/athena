/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TestActionVPTimer_H
#define TestActionVPTimer_H

#include "VolumeTreeNavigator.h"


#include "G4Timer.hh"
#include "G4VPhysicalVolume.hh"

#include <string>
#include <map>
#include <utility>
#include <vector>
#include <ostream>
#include <sstream>

class G4Run;
class G4Event;
class G4Step;

class Algorithm;


#include "G4UserEventAction.hh"
#include "G4UserRunAction.hh"
#include "G4UserSteppingAction.hh"
namespace G4UA{

  /*! @class TestActionVPTimer
      @brief Code for text output (into log file) of information about
the time spent simulating various pieces of the detector and
particles.
  */

  class TestActionVPTimer:
  public G4UserEventAction, public G4UserRunAction,  public G4UserSteppingAction
  {
    
  public:
    
    struct Config
    {
      int dCALO=2;
      int dBeam=2;
      int dIDET=2;
      int dMUON=2;	//!< Used for setting depths in jobOptions file
      std::string dDetail="";	//!< Path to set detailed depth in jobOptions file
    };
    
    TestActionVPTimer(const Config& config);
    
    
    struct volumeData {			//!< Structure of data for given volume
    public:
      volumeData operator+= (const volumeData& acc){
	this->tTotal += acc.tTotal;
	this->tElectron += acc.tElectron;
	this->tPhoton += acc.tPhoton;
	this->tNeutron += acc.tNeutron;
	this->tPion += acc.tPion;
	this->tBaryon += acc.tBaryon;
	this->tLepton += acc.tLepton;
	this->tMeson += acc.tMeson;
	this->tOther += acc.tOther;
	return *this;
      };	//!< Overloaded += operator

      double tTotal=0;			//!< Total time spent in volume
      double tElectron=0;			//!< Time spent on e objects in volume
      double tPhoton=0;			//!< Time spent on photons in volume
      double tNeutron=0;			//!< Time spent on neutrons in volume
      double tPion=0;				//!< Time spent on pions in volume
      double tBaryon=0;			//!< Time spent on other baryons in volume
      double tLepton=0;			//!< Time spent on other leptons in volume
      double tMeson=0;			//!< Time spent on all mesons in volume
      double tOther=0;			//!< Time spent on all other particles in volume (mostly nuclei)
      
    };

    typedef std::map<VolTree, TestActionVPTimer::volumeData> VolMap;
    typedef VolMap::const_iterator VolIt;
    

    struct Report
    {
      // time_index is map<VolTree, TestActionVPTimer::volumeData>
      // VolTree is  vector< pair<physvol*,int> >

      VolMap time_index;
      int nev=0;	//!< number of processed events
      double runTime=0;	//!< Double for storing this run time

      void merge(const Report& rep){

	nev+=rep.nev;
	runTime+=rep.runTime;

	// copy first map
	if(time_index.empty()){
	  time_index=rep.time_index;
	  return;
	}

	// must merge timers per particle and per tree	
	// loop on new report
	for(auto& element:rep.time_index){

	  // check if key exists
	  auto existing=time_index.find(element.first);
	  if(existing!=time_index.end()){
	    // sum timings (using volData)
	    existing->second+=element.second;

	  }
	  else
	    // add new entry
	    time_index.insert(element);
	}

      };
    };
    
    const Report& getReport() const
    { return m_report; }
    
    virtual void BeginOfEventAction(const G4Event*) override;
    virtual void EndOfEventAction(const G4Event*) override;
    virtual void BeginOfRunAction(const G4Run*) override;
    virtual void EndOfRunAction(const G4Run*) override;
    virtual void UserSteppingAction(const G4Step*) override;

  private:
    Config m_config;
    Report m_report;
    
    G4Timer* m_runTimer;			//!< Timer for the entire run
    G4Timer* m_eventTimer;		//!< Timer for this event
    G4Timer* v_timer;			//!< Timer activated for each volume
    double m_eventTime;	//!< Double for storing this event time
    
    VolTree v_history;	//!< Vector of the current volume history, used to assign times to each element
 
    
    double TimerSum(G4Timer* timer) const;    		//!< Gets the time from the timer for summation

  }; // class TestActionVPTimer
  
  
// Stops a timer and return the elapsed time

  inline double TestActionVPTimer::TimerSum(G4Timer* timer) const
  {
    if (timer == 0) return -999.;
    timer->Stop();
    return (timer->GetUserElapsed() + timer->GetSystemElapsed());
  }
  
  

} // namespace G4UA 



#endif // #define TestActionVPTimer_H
