/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FastIDKiller_H
#define FastIDKiller_H

#include "FadsActions/ActionsBase.h"
#include "FadsActions/UserAction.h"
#include <string>

class FastIDKiller: public FADS::ActionsBase , public FADS::UserAction {

  public:
   FastIDKiller(std::string s): FADS::ActionsBase(s),FADS::UserAction(s),m_energyCut(100000000.),m_killCount(0),m_init(false),m_idR(0.),m_idZ(0.) {}
   void BeginOfEventAction(const G4Event*);
   void EndOfEventAction(const G4Event*);
   void BeginOfRunAction(const G4Run*);
   void EndOfRunAction(const G4Run*);
   void SteppingAction(const G4Step*);
   void doInit();					//!< Initialization; zeroes variables and outputs a message
   void KillParticle(const G4Step* aStep);		//!< Routine for killing single particles
   double m_energyCut;				//!< Cut off for electron energy, below which they will be killed
   unsigned long m_killCount;			//!< Counts the number of particles killed
   bool m_init;					//!< Initialization boolean (actions don't always see 'begin of run')
   double m_idR, m_idZ;                            //!< Edge of the inner detector envelope in R and Z

};

#endif

