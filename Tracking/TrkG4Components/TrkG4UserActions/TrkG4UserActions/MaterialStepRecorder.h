/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialStepRecorder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MaterialStepRecorder_H
#define MaterialStepRecorder_H

#include "G4AtlasTools/UserActionBase.h"
#include <string>
#include <vector>

#include "TrkGeometry/MaterialStepCollection.h"
#include "TrkGeometry/ElementTable.h"
#include "TrkGeometry/Material.h"


/** @class MaterialStepRecorder

    @author Andreas.Salzburger@cern.ch
    @author Wolfgang.Lukas@cern.ch
*/

namespace Trk {
  class IPositionMomentumWriter;
}

class MaterialStepRecorder final: public UserActionBase {

 public:
  /** Standard UserAction Constructor */
  MaterialStepRecorder(const std::string& type, const std::string& name, const IInterface* parent);

  /** All G4 interface methods */
  virtual void BeginOfEvent(const G4Event*) override;
  virtual void EndOfEvent(const G4Event*) override;
  virtual void BeginOfRun(const G4Run*) override;
  virtual void EndOfRun(const G4Run*) override;
  virtual void Step(const G4Step*) override;


  virtual StatusCode queryInterface(const InterfaceID&, void**) override;

 private:
  Trk::MaterialStepCollection*    m_matStepCollection; //FIXME convert to a WriteHandle
  std::string                     m_matStepCollectionName;

  bool                            m_recordComposition;

  double                          m_totalNbOfAtoms;
  size_t                          m_totalSteps;
  size_t                          m_eventID;

  Trk::ElementTable*              m_elementTable;  //FIXME convert to a WriteHandle
  std::string                     m_elementTableName;

  Trk::ElementTable*              m_runElementTable;

};

#include "G4AtlasInterfaces/IBeginEventAction.h"
#include "G4AtlasInterfaces/IEndEventAction.h"
#include "G4AtlasInterfaces/IBeginRunAction.h"
#include "G4AtlasInterfaces/ISteppingAction.h"
#include "AthenaBaseComps/AthMessaging.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"
namespace G4UA{


  class MaterialStepRecorder:
    public AthMessaging, public IBeginEventAction,  public IEndEventAction,  public IBeginRunAction,  public ISteppingAction
  {

  public:
    MaterialStepRecorder();
    virtual void beginOfEvent(const G4Event*) override;
    virtual void endOfEvent(const G4Event*) override;
    virtual void beginOfRun(const G4Run*) override;
    virtual void processStep(const G4Step*) override;
  private:

    typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
    /// Pointer to StoreGate (event store by default)
    mutable StoreGateSvc_t m_evtStore;
    /// Pointer to StoreGate (detector store by default)
    mutable StoreGateSvc_t m_detStore;

    Trk::MaterialStepCollection*    m_matStepCollection; //FIXME convert to a WriteHandle
    std::string                     m_matStepCollectionName;

    bool                            m_recordComposition;

    double                          m_totalNbOfAtoms;
    size_t                          m_totalSteps;
    size_t                          m_eventID;

    Trk::ElementTable*              m_elementTable;  //FIXME convert to a WriteHandle
    std::string                     m_elementTableName;

    Trk::ElementTable*              m_runElementTable;

  }; // class MaterialStepRecorder


} // namespace G4UA

#endif
