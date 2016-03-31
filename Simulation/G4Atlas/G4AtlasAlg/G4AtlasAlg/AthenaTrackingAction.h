/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaTrackingAction_H
#define AthenaTrackingAction_H

//#include "FadsActions/ApplicationTrackingAction.h"
#include "G4AtlasTools/UserActionBase.h"
#include "G4AtlasInterfaces/IUserActionSvc.h"


class AthenaTrackingAction:  public UserActionBase {
public:
 AthenaTrackingAction(const std::string& type, const std::string& name, const IInterface* parent);

  virtual void PreTracking(const G4Track*) override;
  virtual void PostTracking(const G4Track*) override;

  virtual StatusCode queryInterface(const InterfaceID&, void**) override;

  virtual  StatusCode initialize() override;

private:

  // needed for trajectory manipulation
  ServiceHandle<IUserActionSvc>    m_UASvc;
  IUserActionSvc* m_UASvcQuick;


};

#endif
