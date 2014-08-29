/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LooperKiller_H
#define LooperKiller_H

#include "FadsActions/UserAction.h"
#include <string>

class LooperKiller: public FADS::UserAction {

public:
   LooperKiller(std::string s): UserAction(s), m_max_steps(1000000), m_print_steps(100), m_count_steps(0), 
                                m_killed_tracks(0), m_verbose_level(4), m_initted(false), m_abortEvent(true),
                                m_setError(false) {}
   void BeginOfEventAction(const G4Event*);
   void EndOfEventAction(const G4Event*);
   void BeginOfRunAction(const G4Run*);
   void EndOfRunAction(const G4Run*);
   void SteppingAction(const G4Step*);
private:
   long m_max_steps;
   int m_print_steps;
   int m_count_steps;
   long m_killed_tracks;
   int m_verbose_level;
   bool m_initted;
   void ParseProperties();
   bool m_abortEvent, m_setError;
};


#endif
