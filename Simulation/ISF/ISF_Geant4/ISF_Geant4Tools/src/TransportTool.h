/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// G4TransportTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_GEANT4TOOLS_TRANSPORTTOOL_H
#define ISF_GEANT4TOOLS_TRANSPORTTOOL_H

// Base class headers
#include "AthenaBaseComps/AthAlgTool.h"
#include "ISF_Geant4Interfaces/ITransportTool.h"

// STL headers
#include <string>

// Gaudi headers
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Athena headers
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "G4AtlasInterfaces/IG4AtlasSvc.h"
#include "G4AtlasInterfaces/IUserActionSvc.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"
#include "G4AtlasInterfaces/ISensitiveDetectorMasterTool.h"
#include "G4AtlasInterfaces/IFastSimulationMasterTool.h"
#include "G4AtlasInterfaces/IPhysicsListTool.h"

// ISF includes
#include "ISF_Interfaces/IInputConverter.h"
#include "ISF_Geant4Tools/IG4RunManagerHelper.h"

class G4Event;

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

  class G4AtlasRunManager;

  class G4TransportTool : public extends<AthAlgTool, ITransportTool>
  {

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

    /** Creates a new ParticleState from a given ParticleState, universal transport tool */
    virtual StatusCode process(const ISF::ISFParticle& isp) override final;

    /** Creates a new ParticleState from a given ParticleState, universal transport tool */
    virtual StatusCode processVector(const std::vector<const ISF::ISFParticle*>& particles) override final;

  private:

    /// This command prints a message about a G4Command depending on its returnCode
    void commandLog(int returnCode, const std::string& commandString) const;

    HepMC::GenEvent* genEvent() const;

    /// @name Configurable Properties
    /// @{
    std::string m_mcEventCollectionName{"TruthEvent"};
    /// Helper Tool to provide G4RunManager
    PublicToolHandle<ISF::IG4RunManagerHelper>  m_g4RunManagerHelper{this, "G4RunManagerHelper", "iGeant4::G4RunManagerHelper/G4RunManagerHelper", ""};
    G4AtlasRunManager    *m_pRunMgr{};

    std::string m_libList{""};
    std::string m_physList{""};
    std::string m_fieldMap{""};
    std::string m_rndmGen{"athena"};
    bool   m_releaseGeoModel{true};
    bool   m_recordFlux{false};
    /// Commands to send to the G4 UI
    std::vector<std::string> m_g4commands;
    /// Activate multi-threading configuration
    bool m_useMT{false};
    // Random number service
    ServiceHandle<IAtRndmGenSvc> m_rndmGenSvc{this, "RandomNumberService", "AtDSFMTGenSvc", ""};
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
    ToolHandle<ISensitiveDetectorMasterTool> m_senDetTool{this, "SenDetMasterTool", "SensitiveDetectorMasterTool", ""};
    /// Fast Simulation Master Tool
    ToolHandle<IFastSimulationMasterTool> m_fastSimTool{this, "FastSimMasterTool", "FastSimulationMasterTool", ""};
    /// @}

  };


}


#endif // ISF_GEANT4TOOLS_TRANSPORTTOOL_H
