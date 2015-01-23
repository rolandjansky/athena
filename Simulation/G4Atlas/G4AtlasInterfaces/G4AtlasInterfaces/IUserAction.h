/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_IUSERACTION_H
#define G4ATLASINTERFACES_IUSERACTION_H

#include "GaudiKernel/IAlgTool.h"
//#include "G4AtlasInterfaces/UserActionRole.h"

/** @class IUserAction IUserAction.h "G4AtlasInterfaces/IUserAction.h"
 *  
 *  Abstract interface to Geant4 Physics list classes
 * 
 *  @author Andrea Di Simone
 *  @date   2015-01-12
 */


class G4Run;
class G4Event;
class G4Step;
class G4Track;


namespace G4AtlasUA{

  enum Role{BeginOfRun, BeginOfEvent, PreTracking, Step, PostTracking, EndOfEvent,EndOfRun};  

}

class IUserAction : virtual public IAlgTool {
 public:
  IUserAction() {}
  virtual ~IUserAction() {}
  static const InterfaceID& interfaceID() {
    static const InterfaceID IID_IUserAction( "IUserAction" , 1 , 0 ) ;
    return IID_IUserAction ;
  }
  
  /** interface for the BeginOfRun */
  virtual void BeginOfRun (const G4Run*) = 0 ;

  /** interface for the EndOfRun */
  virtual void EndOfRun (const G4Run*) = 0 ;
  
  /** interface for the BeginOfEvent */
  virtual void BeginOfEvent (const G4Event*) = 0 ;
  /** interface for the BeginOfEvent */
  virtual void EndOfEvent (const G4Event*) = 0 ;
  
  /** interface for stepping actions */
  virtual void Step (const G4Step*) = 0 ;
  
  /** interface for PreUserTrackingAction */
  virtual void PreTracking (const G4Track*) = 0 ;
  /** interface for PostUserTrackingAction */
  virtual void PostTracking (const G4Track*) = 0 ;
  
  /** to ask an action what are its roles */
  virtual std::vector<G4AtlasUA::Role> Roles() = 0;
  
  //  /** to ask an action what is its global priority */
  //virtual G4AtlasUA::Priority Priority() = 0;
  ///** to set the global priority */
  //virtual void SetPriority(G4AtlasUA::Priority) = 0;
  
  /** to ask an action what is its priority for a given role */
  //virtual  G4AtlasUA::Priority Priority(G4AtlasUA::Role) = 0;
  ///** to set a priority for a given role */
  //virtual void SetPriority( G4AtlasUA::Priority, G4AtlasUA::Role) = 0;
  
  //** to add a role for the action */
  virtual void AddRole(G4AtlasUA::Role) = 0;
  
};
#endif
