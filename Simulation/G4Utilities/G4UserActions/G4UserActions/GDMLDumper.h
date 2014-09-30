/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GDMLDumper_H
#define GDMLDumper_H

#include "FadsActions/ActionsBase.h"
#include "FadsActions/UserAction.h"
#include <string>

class G4LogicalVolume;

class GDMLDumper: public FADS::ActionsBase , public FADS::UserAction {

  public:
   GDMLDumper(std::string s): FADS::ActionsBase(s),FADS::UserAction(s),m_init(false) {}
   void EndOfEventAction(const G4Event*);

  private:
   bool m_init;
   void PullVolumes( G4LogicalVolume* v ) const;

};

#endif
