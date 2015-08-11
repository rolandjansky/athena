/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4INTERFACES_ITRACKPROCESSORUSERACTION_H
#define ISF_GEANT4INTERFACES_ITRACKPROCESSORUSERACTION_H

#include "GaudiKernel/IAlgTool.h"

#include "FadsActions/UserAction.h"
#include "FadsActions/TrackingAction.h"

#include "FadsActions/FadsSteppingAction.h"

//#include "G4UserSteppingAction.hh"

namespace iGeant4 {

  static const InterfaceID IID_ITrackProcessorUserAction("ITrackProcessorUserAction",1,0);

  //class ITrackProcessorUserAction : virtual public IAlgTool, public G4UserSteppingAction {
  class ITrackProcessorUserAction : virtual public IAlgTool, public FADS::UserAction, public FADS::TrackingAction {
    
  public:

  ITrackProcessorUserAction(const std::string& name) : FADS::UserAction(name) {}

    virtual ~ITrackProcessorUserAction(){}
    
    static const InterfaceID& interfaceID();
    
    //FADS::FadsSteppingAction* steppingAction() { FADS::FadsSteppingAction::GetSteppingAction()->SetMCTSteppingAction(this);
  };

  inline const InterfaceID& ITrackProcessorUserAction::interfaceID()  { return IID_ITrackProcessorUserAction; }

}

#endif // ISF_GEANT4INTERFACES_ITRACKPROCESSORUSERACTION_H
