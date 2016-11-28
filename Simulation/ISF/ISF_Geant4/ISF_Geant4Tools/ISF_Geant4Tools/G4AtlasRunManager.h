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

// Gaudi headers
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// Athena headers
#include "AthenaKernel/MsgStreamMember.h"
#include "G4AtlasInterfaces/ISensitiveDetectorMasterTool.h"
#include "G4AtlasInterfaces/IFastSimulationMasterTool.h"
#include "G4AtlasInterfaces/IPhysicsListTool.h"
#include "G4AtlasInterfaces/IUserActionSvc.h"

namespace iGeant4
{
  /** @class G4AtlasRunManager

      @author Robert Harrington
  */
  /// @TODO Add Documentation

  class TrackProcessor; // FIXME - pretty sure that this is obsolete.

  class G4AtlasRunManager : public G4RunManager {

    friend class G4TransportTool; // Does this class really need friends?

  public:
    virtual ~G4AtlasRunManager() {}

    static G4AtlasRunManager* GetG4AtlasRunManager();

    /// Configure the user action service handle
    void SetUserActionSvc(const std::string& typeAndName);

    bool ProcessEvent(G4Event* event);

    void RunTermination() override final;
    void SetCurrentG4Event(int);

  protected:
    void Initialize() override final;
    void InitializeGeometry() override final;
    void InitializePhysics() override final;

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

    bool m_releaseGeo;
    bool m_recordFlux;
    ToolHandle<ISensitiveDetectorMasterTool> m_senDetTool;
    ToolHandle<IFastSimulationMasterTool> m_fastSimTool;
    ToolHandle<IPhysicsListTool> m_physListTool;
    /// Handle to the user action service
    ServiceHandle<G4UA::IUserActionSvc> m_userActionSvc;
  };

}


#endif // ISF_GEANT4TOOLS_G4ATLASRUNMANAGER_H
