/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
//                                                              //
//$Id:TestActionVPTimer.h,v 1.0 2008/07/08 13:46:47 zmarshal Exp $//
//                                                              //
//                      TestActionVPTimer                        //
//      Code for text output (into log file)                    //
//      of information about the time spent simulating          //
//      various pieces of the detector and particles.           //
//                                                              //
//      Written by                                              //
//         Zachary Marshall, Caltech, USA                       //
//         Wolfgang Ehrenfeld, University of Hamburg, Germany   //
//      Revised by						//
//         Kevin Sapp, Pitt, USA				//
//                                                              //
//      @version $Revision: 660034 $ 				//
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef TestActionVPTimer_H
#define TestActionVPTimer_H

#include "VolumeTreeNavigator.h"

#include "G4AtlasTools/UserActionBase.h"

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

struct volData {			//!< Structure of data for given volume
 public:
  volData operator+= (const volData&);	//!< Overloaded += operator
  double tTotal;			//!< Total time spent in volume
  double tElectron;			//!< Time spent on e objects in volume
  double tPhoton;			//!< Time spent on photons in volume
  double tNeutron;			//!< Time spent on neutrons in volume
  double tPion;				//!< Time spent on pions in volume
  double tBaryon;			//!< Time spent on other baryons in volume
  double tLepton;			//!< Time spent on other leptons in volume
  double tMeson;			//!< Time spent on all mesons in volume
  double tOther;			//!< Time spent on all other particles in volume (mostly nuclei)

  volData(): tTotal(0.), tElectron(0.), tPhoton(0.),
	     tNeutron(0.), tPion(0.), tBaryon(0.),
	     tLepton(0.), tMeson(0.), tOther(0.) { ; }
};

volData volData::operator+= (const volData& acc)
{
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
}

typedef std::map<VolTree, volData> VolMap;
typedef VolMap::const_iterator VolIt;

class TestActionVPTimer final: public UserActionBase
{  
 public:

  TestActionVPTimer(const std::string& type, const std::string& name, const IInterface* parent);
  virtual void BeginOfEvent(const G4Event*) override;	//!< Action that starts the new event timers
  virtual void EndOfEvent(const G4Event*) override;	//!< Action that prints all available information at the end of each event
  virtual void EndOfRun(const G4Run*) override;		//!< Action that prints all information at the end of the run
  virtual void Step(const G4Step*) override;		//!< Stepping action that increments the appropriate timer
  StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
 
 private:

  //std::string fName;			//!< Name of output csv file
  //std::ofstream file;			//!< Output csv file of time data (easy import into calc, etc.)

  G4Timer* m_runTimer;			//!< Timer for the entire run
  G4Timer* m_eventTimer;		//!< Timer for this event
  G4Timer* v_timer;			//!< Timer activated for each volume
  double m_runTime, m_eventTime;	//!< Double for storing this event and run time
  int dCALO, dBeam, dIDET, dMUON;	//!< Used for setting depths in jobOptions file
  std::string dDetail;			//!< Path to set detailed depth in jobOptions file
  int m_nev;

  VolTree v_history;	//!< Vector of the current volume history, used to assign times to each element
  VolMap v_time_index;	//!< Map of times for each volume tree, by particle 

  double TimerSum(G4Timer* timer) const;    		//!< Gets the time from the timer for summation
  void TimerPrint(std::pair<VolTree, volData>, 
		  const double tTotal,
		  const int depth = 0) const;
//  void TimerPrint(std::ofstream&,
//		  std::pair<VolTree, volData>, 
//		  const int depth = 0) const;		//!< (Modifiable) print function for a map element
  void TreeOut(VolTree, const double tAtlas, int depth = 0);
//  void TreeOut(std::ofstream&, VolTree, int depth = 0);	//!< Accumulates a tree-structure listing of volume times
};


// Stops a timer and return the elapsed time

inline double TestActionVPTimer::TimerSum(G4Timer* timer) const
{
  if (timer == 0) return -999.;
  timer->Stop();
  return (timer->GetUserElapsed() + timer->GetSystemElapsed());
}


// Formats print string for timer output

inline std::string vPrFmt (double time, double nEv, double tRun, int depth, std::string id)
{
  std::string dprnt = stringify(depth);
  if (depth == -999) dprnt = "      ";
  std::ostringstream oss;
  oss << std::setw(12) << time << std::setw(12) << time/nEv << std::setw(12) << time/tRun
      << std::setw(6) << dprnt << "  " << id;
  return oss.str();
}


// Prints information about time by particle type and volume (modify to suit)

inline void TestActionVPTimer::TimerPrint(std::pair<VolTree, volData> vp, const double tTotal, const int depth) const
{
  G4VPhysicalVolume* pv = vp.first.back().first;
  std::string physical = stringify(pv->GetName());
  std::string copyNo = stringify(vp.first.back().second);
  std::string mother = "";
  if (vp.first.size() != 1) mother = stringify( (vp.first.end()-2)->first->GetName() );
  G4cout << vPrFmt(vp.second.tTotal, m_nev, tTotal, depth-1, physical+"_"+copyNo+"  "+mother) << G4endl;
  G4cout << vPrFmt(vp.second.tElectron, m_nev, tTotal, depth-1, " - electrons & positrons") << G4endl;
  G4cout << vPrFmt(vp.second.tPhoton, m_nev, tTotal, depth-1, " - photons") << G4endl;
  G4cout << vPrFmt(vp.second.tNeutron, m_nev, tTotal, depth-1, " - neutrons") << G4endl;
  G4cout << vPrFmt(vp.second.tPion, m_nev, tTotal, depth-1, " - pions") << G4endl;

  double tOther = vp.second.tTotal - vp.second.tElectron - vp.second.tPhoton - vp.second.tNeutron - vp.second.tPion;
  G4cout << vPrFmt(tOther, m_nev, m_runTime, depth-1, " - other particles") << G4endl;
  return;
}
#endif // #define TestActionVPTimer_H
