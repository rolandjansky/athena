/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_IUSERACTIONSVC_H
#define G4ATLASINTERFACES_IUSERACTIONSVC_H

#include "GaudiKernel/IService.h"

/** @class IUserActionSvc IUserActionSvc.h "G4AtlasInterfaces/IUserActionSvc.h"
 *  
 *  Abstract interface to manage simulation user actions
 * 
 *  @author Andrea Di Simone
 *  @date   2015-01-13
 */

class G4UserRunAction;
class G4UserEventAction;
class G4UserTrackingAction;
class G4UserStackingAction;
class G4UserSteppingAction;
class G4VTrajectory;
class G4TrackingManager;
class G4StackManager;

class IUserActionSvc : virtual public IService {
 public:
  IUserActionSvc() {}
  virtual ~IUserActionSvc() {}
  static const InterfaceID& interfaceID() {
    static const InterfaceID IID_IUserActionSvc( "IUserActionSvc" , 1 , 0 ) ;
    return IID_IUserActionSvc ;
  }
  
  // methods to allow access at G4 kernel managers, which were accessed before through the FADS singletons
  virtual void SetTrajectory(G4VTrajectory * aTraj) = 0;
  virtual G4TrackingManager* TrackingManager() const = 0;
  virtual G4StackManager* StackManager() const = 0;
  virtual void ResetTrajectory() = 0;
  
  virtual void SetLegacyRA(G4UserRunAction* anAction) = 0;
  virtual void SetLegacyEA(G4UserEventAction* anAction) = 0;
  virtual void SetLegacySA(G4UserSteppingAction* anAction) = 0;
  virtual void SetLegacyTA(G4UserTrackingAction* anAction) = 0;
  virtual void SetLegacyStaA(G4UserStackingAction* anAction)= 0;
  
};

#endif
