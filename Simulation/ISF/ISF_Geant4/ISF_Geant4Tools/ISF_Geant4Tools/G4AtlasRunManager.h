/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TransportTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_GEANT4TOOLS_G4ATLASRUNMANAGER_H
#define ISF_GEANT4TOOLS_G4ATLASRUNMANAGER_H

// Base class header
#include "G4RunManager.hh"

// Athena headers
#include "AthenaKernel/MsgStreamMember.h"
#include "G4AtlasInterfaces/ISensitiveDetectorMasterTool.h"
#include "G4AtlasInterfaces/IFastSimulationMasterTool.h"
#include "G4AtlasInterfaces/IPhysicsListTool.h"

// Gaudi headers
#include "GaudiKernel/ToolHandle.h"

// G4 headers
#include "G4UserSteppingAction.hh"

namespace ISF {
  class IParticleBroker;
}

namespace iGeant4
{
  /** @class G4AtlasRunManager

      @author Robert Harrington
  */

  class TrackProcessor;

  class G4AtlasRunManager : public G4RunManager {

    friend class G4TransportTool;

  public:
    virtual ~G4AtlasRunManager() {}

    static G4AtlasRunManager* GetG4AtlasRunManager();

    void SetUserSteppingAction(G4UserSteppingAction* p) { m_steppingActions.push_back(p); }

    bool ProcessEvent(G4Event* event);

    void RunTermination();
    void SetCurrentG4Event(int);

  protected:

    void InitializeGeometry();
    void InitializePhysics();

  private:
    G4AtlasRunManager();
    void EndEvent();

    void SetReleaseGeo(bool b) { m_releaseGeo = b; }
    void SetRecordFlux(bool b) { m_recordFlux = b; }

    /// Log a message using the Athena controlled logging system
    MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
    /// Check whether the logging system is active at the provided verbosity level
    bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }

    /// Private message stream member
    mutable Athena::MsgStreamMember m_msg;

    std::vector<G4UserSteppingAction*> m_steppingActions;
    bool m_releaseGeo;
    bool m_recordFlux;
    ToolHandle<ISensitiveDetectorMasterTool> m_senDetTool;
    ToolHandle<IFastSimulationMasterTool> m_fastSimTool;
    ToolHandle<IPhysicsListTool> m_physListTool;
  };

}


#endif // ISF_GEANT4TOOLS_G4ATLASRUNMANAGER_H
