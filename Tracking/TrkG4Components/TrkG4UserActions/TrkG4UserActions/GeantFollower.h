/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeantFollower.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef GeantFollower_H
#define GeantFollower_H

#include "FadsActions/UserAction.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>
#include <vector>

/** @class GeantFollower

    @author Andreas.Salzburger@cern.ch
*/

namespace Trk {
    class IGeantFollowerHelper;
}

class StoreGateSvc;

class GeantFollower: public FADS::UserAction {

  public:
    /** Standard FADS UsesAction */
    GeantFollower(std::string s);

    /** All G4 interface methods */
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);
    void SteppingAction(const G4Step*);
    
  private:
    std::string                                  m_name;
    ToolHandle<Trk::IGeantFollowerHelper>     m_helper;
    mutable const Trk::IGeantFollowerHelper*  m_helperPointer;
    

};

#endif

