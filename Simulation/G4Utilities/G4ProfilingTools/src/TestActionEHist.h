/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
//								//
//			TestActionEHist.h			//
//	Code for ROOT output (filename selected in code)	//
//	of histograms of the initial kinetic energy		//
//	(from truth) of specified particles, by volume.		//
//								//
//	Written by 	Kevin Sapp				//
//			University of Pittsburgh		//
//			kcs34@pitt.edu				//
//	Last update	01.26.10				//
//                                                              //
//                                                              //
//////////////////////////////////////////////////////////////////

#ifndef TestActionEHist_H
#define TestActionEHist_H

#include "VolumeTreeNavigator.h"


#include <string>
#include <vector>

class G4Run;
class G4Event;
class G4Step;
class G4String;
class G4VPhysicalVolume;
class G4TouchableHistory;
class Algorithm;
class IMessageSvc;
class TFile;


#include "G4UserTrackingAction.hh"
#include "G4UserRunAction.hh"
#include "G4UserSteppingAction.hh"
namespace G4UA{

  class TestActionEHist:
    public G4UserTrackingAction, public G4UserRunAction, public G4UserSteppingAction
  {
    
  public:
    
    struct Config
    {      
      int dCALO=2;
      int dBeam=2;
      int dIDET=2;
      int dMUON=2;       //!< Used for depth limits applied in jobOptions
      int maxhists=1000;
      int maxdirs=1000;            //!< Used to set maximum number of histograms/directories from jobOptions
      std::string dDetail="";  //!< Path used to set detailed depth cuts
      std::string name="default.root";	//!< Name of root output file
    };
    
    TestActionEHist(const Config& config);
    virtual void PreUserTrackingAction(const G4Track*) override;
    virtual void PostUserTrackingAction(const G4Track*) override;
    virtual void BeginOfRunAction(const G4Run*) override;
    virtual void EndOfRunAction(const G4Run*) override;
    virtual void UserSteppingAction(const G4Step*) override;
  private:
    Config m_config;	//!< holds the python configuration
    
    TFile* m_world;				//!< File in which to store neutron & electron info
    
    bool m_firstStep;			//!< Flag indicating whether step is first in current volume
    
    std::string m_p_tag;			//!< Used to specify current particle in tracking

    std::vector<std::string> m_trajectory;	//!< Used to store volume names which the current track has entered
    
    void BuildHists (		//!< Function to generate TH1-type histograms
		     std::string vol_tag,		//!< Tag to identify object by volume
		     std::string part_tag,		//!< Tag to identify object by particle
		     int& hLeft,			//!< Amount of histogram space remaining, set by maxhists
		     double xfill = -1,		//!< Parameter to fill histogram along x
		     double yfill = -1,		//!< Paramteter to fill histogram along y
		     const int nbins = 3000,	//!< Number of bins in histogram
		     const int binsize = 1);	//!< Size of bins in histogram, in MeV
    
    bool BuildDirs (		//!< Function to generate TDirectory objects
		    std::string vol_tag,		//!< Tag to identify object by volume
		    std::string dirTitle,		//!< Directory title
		    int& dLeft);			//!< Remaining directories to create
    
    
    
  }; // class TestActionEHist
  
  
} // namespace G4UA 

#endif // #define TestActionEHist_H
