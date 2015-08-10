/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CheckActivation_H
#define CheckActivation_H

#include "FadsActions/ActionsBase.h"
#include "FadsActions/UserAction.h"
#include <string>

class CheckActivation: public FADS::ActionsBase , public FADS::UserAction {

  public:
   CheckActivation(std::string s): FADS::ActionsBase(s) , FADS::UserAction(s) {}
   void BeginOfEventAction(const G4Event*);

};

#endif
