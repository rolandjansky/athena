/*
  Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration
*/

#include "UserHooksUtils.h"
#include "UserSetting.h"
#include "Pythia8_i/UserHooksFactory.h"
#include "boost/lexical_cast.hpp"
#include "boost/bind.hpp"
#include <boost/math/tools/roots.hpp>
#include <math.h>
#include <stdexcept>
#include <iostream>

#include "suep_shower.h"

namespace Pythia8{
  class DecayToSUEP;
}

Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::DecayToSUEP> DecayToSUEPCreator("DecayToSUEP");

// Uncomment the following line to enable debug messages to be printed to std::cout
//#define SUEP_DEBUG 1

namespace Pythia8{

  //******************************
  //** Main UserHook derived class
  //******************************

  /** Pythia8 UserHook to decay a given scalar particle to SUEP. 
   *
   * Details on models available on arXiv:1612.00850.
   * This is an adaption from the available public code at: 
   * https://gitlab.com/simonknapen/suep_generator 
   * by Simon Knapen.
   *
   */
  class DecayToSUEP : public UserHooks{
    
  public:
    
    DecayToSUEP(): 
      m_pdgId("DecayToSUEP:PDGId", 25), 
      m_mass("DecayToSUEP:Mass", 125.0),
      m_darkMesonMass("DecayToSUEP:DarkMesonMass", 1.), 
      m_darkTemperature("DecayToSUEP:DarkTemperature", 1.) {
      
      std::cout<<"**********************************************************"<<std::endl;
      std::cout<<"*                                                        *"<<std::endl;
      std::cout<<"*             Enabled SUEP decay UserHook!               *"<<std::endl;
      std::cout<<"*                                                        *"<<std::endl;
      std::cout<<"**********************************************************"<<std::endl;
      
    }
    
    ~DecayToSUEP(){}

    // Enable the call to the user-hook
    virtual bool canVetoProcessLevel() {
      return true;
    }

    /* Actually implement the user hook.
     *
     * We slightly abuse this function in the sense that no veto is performed but instead
     * the hook is used to modify the event record in between the process-level and parton-level steps.
     * This modification is allowed in the Pythia8 manual, with warning of event consistency that
     * should pose no harm in this context.
     */
    virtual bool doVetoProcessLevel(Event& process);


  private:

    /** PDG Id of particle to be decayed to SUEP */
    Pythia8_UserHooks::UserSetting<int> m_pdgId;

    /** Mass of system decaying [GeV] */
    Pythia8_UserHooks::UserSetting<double> m_mass;

    /** Dark-meson mass parameter [GeV] */
    Pythia8_UserHooks::UserSetting<double> m_darkMesonMass;

    /** Temperature parameter [GeV] */
    Pythia8_UserHooks::UserSetting<double> m_darkTemperature;

  };


  //******************************
  //** Implementation of DecayToSUEP UserHook
  //******************************
  bool DecayToSUEP::doVetoProcessLevel(Event& process)
  {
#ifdef SUEP_DEBUG
    std::cout << "[SUEP_DEBUG] " << "Start of user hook for this event." << std::endl;
#endif

    //First, find the particle to decay
    bool particleFound=false;

#ifdef SUEP_DEBUG
    for (int ii=0; ii < process.size(); ++ii) {
      std::cout << "[SUEP_DEBUG] " << ii << ": id=" << process[ii].id() << ", Final=" << process[ii].isFinal() << ", Status=" << process[ii].status() << ", daughter1=" << process[ii].daughter1() << ", daughter2=" << process[ii].daughter2() << std::endl;
    }
#endif

    for (int ii=0; ii < process.size(); ++ii) {
      if ( (process[ii].id() == m_pdgId(settingsPtr)) and (process[ii].daughter1()!=process[ii].daughter2() && process[ii].daughter1()>0 && process[ii].daughter2()>0) ) {

	Vec4 higgs4mom, mesonmom;
	vector< Vec4 > suep_shower4momenta;	
	particleFound=true;

	//setup SUEP shower
	static Suep_shower suep_shower(m_darkMesonMass(settingsPtr), m_darkTemperature(settingsPtr), m_mass(settingsPtr), rndmPtr);

#ifdef SUEP_DEBUG
	std::cout << "[SUEP_DEBUG] " << "Particle (pdgId=" << m_pdgId(settingsPtr) << ", isFinal=True) found. Decaying to SUEP now." << std::endl;
#endif

	// First undo decay
	process[ii].undoDecay();

	int originalEventSize = process.size();

	// Generate the shower, output are 4 vectors in the rest frame of the shower
	higgs4mom=process[ii].p();
	int nShowerAttempts=0;
	do {
	  try {
	    nShowerAttempts++;
	    suep_shower4momenta=suep_shower.generate_shower();
	    if( suep_shower4momenta.size()<3){
	      //Failed to balance energy or less than 3 particles in the shower
	      //Try again until nShowerAttempts >= 3
	    } else {
	      //All ok!
	      nShowerAttempts = -1; //exit condition
	    }
	  } catch (std::exception &e) {
	    //Failed to generate the shower!
	    //Can happen in some rare circumstances, 
	    //Try again until nShowerAttempts >= 3
	  }
	} while ((nShowerAttempts > 0) && (nShowerAttempts < 3));
	if (nShowerAttempts >= 3) {
	  //Something is seriously wrong then, print warning and skip to next event
	  std::cout << "[SUEP] WARNING: Something went wrong in generating the shower. Skipping the event." << std::endl;
	  return true; //veto the event!
	}

	// Loop over hidden sector mesons and append to the event	
	for (unsigned j = 0; j < suep_shower4momenta.size(); ++j){
	  //construct pythia 4vector
	  mesonmom = suep_shower4momenta[j];
            
	  // boost to the lab frame
	  mesonmom.bst(higgs4mom.px()/higgs4mom.e(),higgs4mom.py()/higgs4mom.e(), higgs4mom.pz()/higgs4mom.e());
            
	  //append particle to the event. Hidden/dark meson pdg code is 999999.
	  process.append(999999, 91, ii, 0, 0, 0, 0, 0, mesonmom.px(), mesonmom.py(), mesonmom.pz(), mesonmom.e(), m_darkMesonMass(settingsPtr)); 

#ifdef SUEP_DEBUG
	  std::cout << "[SUEP_DEBUG] " << "Adding dark meson with px=" << mesonmom.px() << ", py=" << mesonmom.py() << ", pz=" << mesonmom.pz() << ", m=" << m_darkMesonMass(settingsPtr) << std::endl;
#endif
	}

	// Just to be sure, only modify Higgs status and daughters if a valid decay did happen
	if ( suep_shower4momenta.size() > 0 ) {
#ifdef SUEP_DEBUG
	  std::cout << "[SUEP_DEBUG] " << "Setting original particle status-code as non-Final particle. Adding daughters with indices: " << originalEventSize << " - " << process.size()-1 << std::endl;
#endif
	  // Change the status code of the Higgs to reflect that it has decayed.
	  process[ii].statusNeg();
          
	  //set daughters of the Higgs. Take advantage that we just appended them
	  process[ii].daughters(originalEventSize, process.size()-1); 
	}
	
	//no need to continue the loop
	break;

      } // if particle to decay found

    } // loop over particles in the event

    if (not particleFound) {
      std::cout << "[DecayToSUEP] " << "Particle " << m_pdgId(settingsPtr) << " not found. Nothing to decay to SUEP for this event." << std::endl;
    } else {
#ifdef SUEP_DEBUG      
      std::cout << "[SEUP_DEBUG] " << "All Done for this event." << std::endl;
#endif
    }
#ifdef SUEP_DEBUG      
    std::cout << "[SUEP_DEBUG] Printing event after adding SUEP:" << std::endl;
    for (int ii=0; ii < process.size(); ++ii) {
      std::cout << "[SUEP_DEBUG] " << ii << ": id=" << process[ii].id() << ", Final=" << process[ii].isFinal() << ", mayDecay=" << process[ii].mayDecay() << ", Status=" << process[ii].status() << ", daughter1=" << process[ii].daughter1() << ", daughter2=" << process[ii].daughter2() << std::endl;
    }
#endif

    //return false: let the event continue
    return false;
  }


} // namespace Pythia8
