/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HitWrapper_H
#define HitWrapper_H

#include "FadsActions/ActionsBase.h"
#include "FadsActions/UserAction.h"
#include <string>

class HitWrapper: public FADS::ActionsBase , public FADS::UserAction {

  public:
   HitWrapper(std::string s): FADS::ActionsBase(s),FADS::UserAction(s),m_init(false),m_time(25.) { }
   void BeginOfEventAction(const G4Event*);
   void EndOfEventAction(const G4Event*);
   void BeginOfRunAction(const G4Run*);
   void EndOfRunAction(const G4Run*);
   void SteppingAction(const G4Step*);

  private:
   bool m_init; //!< Have the properties been parsed?
   double m_time; //!< Time over which to wrap the hits
   void parseProperties();

};

#endif
