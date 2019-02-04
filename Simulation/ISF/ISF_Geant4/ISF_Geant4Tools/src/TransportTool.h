/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_TRANSPORTTOOL_H
#define ISF_GEANT4TOOLS_TRANSPORTTOOL_H

// STL headers
#include <string>
#include <unordered_map>

// Gaudi headers
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"

// Athena headers
#include "AthenaKernel/IAthRNGSvc.h"
#include "G4AtlasInterfaces/IG4AtlasSvc.h"
#include "G4AtlasInterfaces/IUserActionSvc.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"
#include "G4AtlasInterfaces/ISensitiveDetectorMasterTool.h"
#include "G4AtlasInterfaces/IFastSimulationMasterTool.h"
#include "G4AtlasInterfaces/IPhysicsListTool.h"

// ISF includes
#include "ISF_Interfaces/BaseSimulatorTool.h"
#include "ISF_Interfaces/ISimulationSelector.h"
#include "ISF_Interfaces/IInputConverter.h"
#include "ISF_Geant4Tools/IG4RunManagerHelper.h"

class G4Event;

class G4AtlasRunManager;

namespace ISF {
  class ISFParticle;
}

namespace HepMC {
  class GenEvent;
}

namespace iGeant4
{

  /** @class G4TransportTool

      Geant4 AlgTool to create a ISFParticle at a volume entry/exit
      - universal transport tool

      @author Robert Harrington
  */

  class G4TransportTool : public ISF::BaseSimulatorTool {

  public:
    /** Constructor */
    G4TransportTool(const std::string&,const std::string&,const IInterface*);

    /** Destructor */
    virtual ~G4TransportTool ();

    /** AlgTool initialize method */
    virtual StatusCode initialize() override final;

    /// G4 initialization called only by the first tool instance.
    /// This is done (for now) because we get multiple tool instances in hive.
    void initializeOnce();

    /** AlgTool finalize method */
    virtual StatusCode finalize() override final;

    /// G4 finalization called only by the first tool instance.
    /// This is done (for now) because we get multiple tool instances in hive.
    void finalizeOnce();

    virtual StatusCode simulate( const ISF::ISFParticle& isp, ISF::ISFParticleContainer& secondaries, McEventCollection* mcEventCollection ) override;

    virtual StatusCode simulateVector( const ISF::ConstISFParticleVector& particles, ISF::ISFParticleContainer& secondaries, McEventCollection* mcEventCollection ) override;

    virtual StatusCode setupEvent() override;

    virtual StatusCode setupEventST() override;

    virtual StatusCode releaseEvent() override;

    virtual StatusCode releaseEventST() override;

    virtual ISF::SimulationFlavor simFlavor() const override { return ISF::Geant4; };

    virtual void push( ISF::ISFParticle *particle, const ISF::ISFParticle *parent ) override;

  private:

    /// This command prints a message about a G4Command depending on its returnCode
    void commandLog(int returnCode, const std::string& commandString) const;

    HepMC::GenEvent* genEvent(McEventCollection* mcEventCollection) const;

    /// @name Configurable Properties
    /// @{

    // timing checks
    bool  m_doTiming{true};
    //float m_runTime;
    float m_accumulatedEventTime{0.};
    float m_accumulatedEventTimeSq{0.};
    unsigned int m_nrOfEntries{0};

    G4Timer* m_runTimer{nullptr};
    G4Timer* m_eventTimer{nullptr};

    // store secondary particles that have been pushed back
    std::unordered_map< ISF::ISFParticle const*, ISF::ISFParticleContainer > m_secondariesMap;
    std::string m_mcEventCollectionName{"TruthEvent"};
    /// Helper Tool to provide G4RunManager
    PublicToolHandle<ISF::IG4RunManagerHelper>  m_g4RunManagerHelper{this, "G4RunManagerHelper", "iGeant4::G4RunManagerHelper/G4RunManagerHelper", ""};
    G4AtlasRunManager    *m_pRunMgr{};

    std::string m_libList{""};
    std::string m_physList{""};
    std::string m_fieldMap{""};
    bool   m_releaseGeoModel{true};
    bool   m_recordFlux{false};
    /// Commands to send to the G4 UI
    std::vector<std::string> m_g4commands;
    /// Activate multi-threading configuration
    bool m_useMT{false};
    // Random number service
    ServiceHandle<IAthRNGSvc> m_rndmGenSvc{this, "RandomNumberService", "AthRNGSvc", ""};
    /// G4AtlasSvc
    ServiceHandle<IG4AtlasSvc> m_g4atlasSvc{this, "G4AtlasSvc", "G4AtlasSvc", ""};
    /// user action service
    ServiceHandle<G4UA::IUserActionSvc> m_userActionSvc{this, "UserActionSvc", "", ""};
    /// Detector Geometry Service (builds G4 Geometry)
    ServiceHandle<IDetectorGeometrySvc> m_detGeoSvc{this, "DetGeoSvc", "DetectorGeometrySvc", ""};
    /// Service to convert ISF_Particles into a G4Event
    ServiceHandle<ISF::IInputConverter> m_inputConverter{this, "InputConverter", "ISF_InputConverter", ""};
    /// Physics List Tool
    PublicToolHandle<IPhysicsListTool> m_physListTool{this, "PhysicsListTool", "PhysicsListToolBase", ""};
    /// Sensitive Detector Master Tool
    PublicToolHandle<ISensitiveDetectorMasterTool> m_senDetTool{this, "SenDetMasterTool", "SensitiveDetectorMasterTool", ""};
    /// Fast Simulation Master Tool
    PublicToolHandle<IFastSimulationMasterTool> m_fastSimTool{this, "FastSimMasterTool", "FastSimulationMasterTool", ""};
    /// @}

  };


}


#endif // ISF_GEANT4TOOLS_TRANSPORTTOOL_H
