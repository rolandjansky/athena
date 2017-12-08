/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsActionBase_H
#define FadsActionBase_H

#include "G4UserEventAction.hh"
#include "G4UserRunAction.hh"
#include "G4UserSteppingAction.hh"
#include "G4UserTrackingAction.hh"
#include "G4UserStackingAction.hh"

namespace FADS {

class FadsActionBase: public G4UserEventAction,public G4UserRunAction,
					  public G4UserSteppingAction, public G4UserTrackingAction,
					  public G4UserStackingAction { 
public:
	FadsActionBase() {std::cout<<"FadsActionBase"<<std::endl;}
	virtual ~FadsActionBase() {;}
};

}
#endif
