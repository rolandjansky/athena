/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4INTERFACES_ISD_ACTIVATEUSERACTION_H
#define ISF_GEANT4INTERFACES_ISD_ACTIVATEUSERACTION_H

#include "GaudiKernel/IAlgTool.h"

namespace iGeant4 {

  static const InterfaceID IID_ISDActivateUserAction("ISDActivateUserAction",1,0);
  
  class ISDActivateUserAction : virtual public IAlgTool, public FADS::UserAction, public FADS::TrackingAction {
    
  public:    

  ISDActivateUserAction(const std::string& name) : FADS::UserAction(name), FADS::TrackingAction() {}
    
    virtual ~ISDActivateUserAction() {}

    static const InterfaceID& interfaceID();
  };
  
  inline const InterfaceID& ISDActivateUserAction::interfaceID()  { return IID_ISDActivateUserAction; }
  
}

#endif // ISF_GEANT4INTERFACES_ISD_ACTIVATEUSERACTION_H
