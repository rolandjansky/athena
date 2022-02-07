/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// PowhegHooksBB4L.h 
// Copyright (C) 2017 Tomas Jezo, Markus Seidel, Ben Nachman 

// Author: Tomas Jezo, Markus Seidel and Ben Nachman based on 
// PowhegHooks.h by Richard Corke

#include "Pythia8_i/UserHooksFactory.h"
#include "Pythia8Plugins/PowhegHooks.h"
#include "UserSetting.h"

namespace Pythia8 {

  // Create the user hook
  class TopRecoilHook;
  Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::TopRecoilHook> TopRecoilHookCreator("TopRecoilHook");
  
  // Use userhooks to veto PYTHIA emissions above the POWHEG scale
  class TopRecoilHook : public UserHooks {//class PowhegHooksBB4L : public PowhegHooks {

  public:

    // Constructor.
    //  m_doTopRecoil : eikonal correction in GW dipole on/off when no MEC applied.
    //  m_useOldDipole  : in GW dipole, use partons before or after branching.
    //  m_doList        : diagnostic output; set false for production runs.
    TopRecoilHook(bool doTopRecoilIn=true, bool useOldDipoleIn=false, bool doListIn = false) {
      m_doTopRecoil = doTopRecoilIn;
      m_useOldDipole = useOldDipoleIn;
      m_doList = doListIn; 
      // Constructor also creates some histograms for analysis inside User Hook.
      m_wtCorr = new Hist("corrective weight", 100, 0., 2.);
      std::cout << " enabling TopRecoilHook"<< std::endl;

    }

    // Destructor prints histogram.
    ~TopRecoilHook() override {
      if (m_doTopRecoil) cout << *m_wtCorr;
      delete m_wtCorr;
    }


    // Initialise. Only use hook for simple showers with recoilToColoured = off.
    virtual bool initAfterBeams() override {

      // Switch off if not using simple showers or if recoilToColoured = on.
      bool recoilToColoured = settingsPtr->flag("TimeShower:recoilToColoured");
      if (recoilToColoured){
	m_doTopRecoil=false;
	std::cout << "  TopRecoilHook should not be used with RecoilToColoured=on; disabling"<< std::endl;
      }
      // Flag if W mass term is already accounted for (true) or not (false).
      m_recoilDeadCone        = settingsPtr->flag("TimeShower:recoilDeadCone");
      // All ok.
      return true;
    }

    // Allow a veto after an FSR emission
    virtual bool canVetoFSREmission() override {return m_doTopRecoil;}

    // Access the event after an FSR emission, specifically inside top decay.
    virtual bool doVetoFSREmission( int sizeOld, const Event& event, int iSys,
				    bool inResonance) override {
    
      // Check that we are inside a resonance decay.
      if (!inResonance) return false;

      // Check that it is a top decay. 
      int iTop = partonSystemsPtr->getInRes(iSys);
      if (iTop == 0 || event[iTop].idAbs() != 6) return false;
    
      // Skip first emission, where ME corrections are already made.
      int sizeOut = partonSystemsPtr->sizeOut(iSys);
      if (sizeOut == 2) return false;
    
      // Location of trial new particles: radiator, emitted, recoiler.
      int iRad = sizeOld;
      int iEmt = sizeOld + 1;
      int iRec = sizeOld + 2;
    
      // The above partons are after emission;
      // alternatively use the ones before.
      if (m_useOldDipole) {
	iRad = event[iRad].mother1();
	iRec = event[iRec].mother1();
      }
    
      // Check if newly emitted gluon matches (anti)top colour line.
      if (event[iEmt].id() != 21) return false;
      if (event[iTop].id() == 6) {
	if (event[iEmt].col() != event[iTop].col()) return false;
      } else {
	if (event[iEmt].acol() != event[iTop].acol()) return false;
      }
    
      // Recoiler should now be a W, else something is wrong.
      if (event[iRec].idAbs() != 24) {
	cout << " ERROR: recoiler is " << event[iRec].id() << endl;
	return false;
      }
    
      // Denominator: eikonal weight with W as recoiler.
      double pRadRec = event[iRad].p() * event[iRec].p();
      double pRadEmt = event[iRad].p() * event[iEmt].p();
      double pRecEmt = event[iRec].p() * event[iEmt].p();
      double wtW = 2. * pRadRec / (pRadEmt * pRecEmt)
	- pow2(event[iRad].m() / pRadEmt);
      // If m_recoilDeadCone = on, include W mass term in denominator.
      if (m_recoilDeadCone) wtW -= pow2(event[iRec].m() / pRecEmt);

      // Numerator: eikonal weight with top as recoiler.
      double pRadTop = event[iRad].p() * event[iTop].p();
      double pTopEmt = event[iTop].p() * event[iEmt].p();
      double wtT = 2. * pRadTop / (pRadEmt * pTopEmt)
	- pow2(event[iRad].m() / pRadEmt) - pow2(event[iTop].m() / pTopEmt);
    
      // Histogram weight ratio.
      m_wtCorr->fill( wtT / wtW );
    
      // List relevant properties.
      if (m_doList) {
	std::cout << "\n now event with sizeOld = " << sizeOld << ", iSys = "
		  << iSys << ", sizeOut = " << sizeOut << scientific
		  << setprecision(3)
		  << ", weight with W = " << wtW << " and with t = " << wtT << std::endl;
	partonSystemsPtr->list();
	event.list();
      }
    
      // Accept/reject emission. Smooth suppression or step function.
      return (wtT < wtW * rndmPtr->flat());
    }


private:
  
    // Options and Histograms.
    bool  m_doTopRecoil, m_useOldDipole, m_doList, m_recoilDeadCone;
    Hist *m_wtCorr;

  };

//==========================================================================

} // end namespace Pythia8
