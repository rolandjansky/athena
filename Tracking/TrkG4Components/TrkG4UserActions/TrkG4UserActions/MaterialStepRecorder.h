/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialStepRecorder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MaterialStepRecorder_H
#define MaterialStepRecorder_H

/** @class MaterialStepRecorder

    @author Andreas.Salzburger@cern.ch
    @author Wolfgang.Lukas@cern.ch
*/

namespace Trk {
  class IPositionMomentumWriter;
}

#include <vector>

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaBaseComps/AthMessaging.h"

#include "G4UserEventAction.hh"
#include "G4UserRunAction.hh"
#include "G4UserSteppingAction.hh"

#include "TrkGeometry/MaterialStepCollection.h"
#include "TrkGeometry/ElementTable.h"
#include "TrkGeometry/Material.h"

namespace G4UA{


  class MaterialStepRecorder: public AthMessaging, public G4UserEventAction,  public G4UserRunAction,  public G4UserSteppingAction
  {

  public:
    MaterialStepRecorder();
    virtual void BeginOfEventAction(const G4Event*) override;
    virtual void EndOfEventAction(const G4Event*) override;
    virtual void BeginOfRunAction(const G4Run*) override;
    virtual void UserSteppingAction(const G4Step*) override;
  private:

    typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
    /// Pointer to StoreGate (event store by default)
    mutable StoreGateSvc_t m_evtStore;
    /// Pointer to StoreGate (detector store by default)
    mutable StoreGateSvc_t m_detStore;

    Trk::MaterialStepCollection*    m_matStepCollection; //FIXME convert to a WriteHandle
    std::string                     m_matStepCollectionName; //FIXME should be passed in via a Config struct rather than hardcoded.

    bool                            m_recordComposition; //FIXME should be passed in via a Config struct rather than hardcoded.

    double                          m_totalNbOfAtoms;
    size_t                          m_totalSteps;
    size_t                          m_eventID;

    Trk::ElementTable*              m_elementTable;  //FIXME convert to a WriteHandle
    std::string                     m_elementTableName;

    Trk::ElementTable*              m_runElementTable;

  }; // class MaterialStepRecorder


} // namespace G4UA

#endif
