/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4INTERFACES_IPHYSICSVALIDATIONUSERACTION_H
#define ISF_GEANT4INTERFACES_IPHYSICSVALIDATIONUSERACTION_H

#include "GaudiKernel/IAlgTool.h"

#include "FadsActions/UserAction.h"
#include "FadsActions/TrackingAction.h"

#include "FadsActions/FadsSteppingAction.h"

//#include "G4UserSteppingAction.hh"

namespace iGeant4 {

  static const InterfaceID IID_IPhysicsValidationUserAction("IPhysicsValidationUserAction",1,0);

  //class IPhysicsValidationUserAction : virtual public IAlgTool, public G4UserSteppingAction {
  class IPhysicsValidationUserAction : virtual public IAlgTool, public FADS::UserAction, public FADS::TrackingAction {
    
  public:

  IPhysicsValidationUserAction(const std::string& name) : FADS::UserAction(name) {}

    virtual ~IPhysicsValidationUserAction(){}
    
    static const InterfaceID& interfaceID();
    
    //FADS::FadsSteppingAction* steppingAction() { FADS::FadsSteppingAction::GetSteppingAction()->SetMCTSteppingAction(this);
  };

  inline const InterfaceID& IPhysicsValidationUserAction::interfaceID()  { return IID_IPhysicsValidationUserAction; }

}

#endif // ISF_GEANT4INTERFACES_IPHYSICSVALIDATIONUSERACTION_H
