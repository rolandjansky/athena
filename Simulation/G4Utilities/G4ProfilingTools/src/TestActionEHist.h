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

#include "G4AtlasTools/UserActionBase.h"

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

class TestActionEHist final: public UserActionBase
{  
 public:

  TestActionEHist(const std::string& type, const std::string& name, const IInterface* parent);
  virtual void BeginOfRun(const G4Run*) override;		//!< Action that starts the timers at the beginning of the run
  virtual void EndOfRun(const G4Run*) override;		//!< Action that prints all information at the end of the run
  virtual void PreTracking(const G4Track*) override;	//!< Tracking action called at beginning of track, stores trajectory
  virtual void PostTracking(const G4Track*) override;	//!< Tracking action called at end of track, stores trajectory
  virtual void Step(const G4Step*) override;		//!< Stepping action that increments the appropriate timer

  virtual StatusCode queryInterface(const InterfaceID&, void**);
  

 private:

  //const G4VProcess* _pds;		//!< Pointer to current process defining the step at postStepPoint
  TFile* m_world;				//!< File in which to store neutron & electron info

  bool m_firstStep;			//!< Flag indicating whether step is first in current volume

  std::string m_name;			//!< Name of root output file
  int m_dCALO, m_dBeam, m_dIDET, m_dMUON;	//!< Used for depth limits applied in jobOptions
  std::string m_dDetail;			//!< Path used to set detailed depth cuts
  int m_maxhists, m_maxdirs;		//!< Used to set maximum number of histograms/directories from jobOptions
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
};


#include "G4AtlasInterfaces/IPreTrackingAction.h"
#include "G4AtlasInterfaces/IPostTrackingAction.h"
#include "G4AtlasInterfaces/IBeginRunAction.h"
#include "G4AtlasInterfaces/IEndRunAction.h"
#include "G4AtlasInterfaces/ISteppingAction.h"
namespace G4UA{

  class TestActionEHist:
  public IPreTrackingAction,  public IPostTrackingAction,  public IBeginRunAction,  public IEndRunAction,  public ISteppingAction
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
    virtual void preTracking(const G4Track*) override;
    virtual void postTracking(const G4Track*) override;
    virtual void beginOfRun(const G4Run*) override;
    virtual void endOfRun(const G4Run*) override;
    virtual void processStep(const G4Step*) override;
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
