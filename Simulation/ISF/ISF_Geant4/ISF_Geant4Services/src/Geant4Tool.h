// -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOL_h
#define ISF_GEANT4TOOL_h

#include <unordered_map>

//Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

//Athena
#include "AthenaBaseComps/AthAlgTool.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"

// G4Atlas
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"
#include "G4AtlasInterfaces/ISensitiveDetectorMasterTool.h"
#include "G4AtlasInterfaces/IFastSimulationMasterTool.h"

//ISF
#include "ISF_Interfaces/BaseSimulatorTool.h"
#include "ISF_Interfaces/ISimulationSelector.h"
#include "ISF_Geant4Interfaces/ITransportTool.h"

class G4Timer;

namespace ISF {

  class Geant4Tool : public BaseSimulatorTool  {
  public:
    Geant4Tool( const std::string& type, const std::string& name,  const IInterface* parent);

    ~Geant4Tool();

    virtual StatusCode initialize() override;

    virtual StatusCode finalize() override;

    virtual StatusCode simulate( const ISFParticle& isp, ISFParticleContainer& secondaries ) override;

    virtual StatusCode simulateVector( const ConstISFParticleVector& particles, ISFParticleContainer& secondaries ) override;

    virtual StatusCode setupEvent() override;

    virtual StatusCode setupEventST() override;

    virtual StatusCode releaseEvent() override;

    virtual StatusCode releaseEventST() override;

    virtual ISF::SimulationFlavor simFlavor() const override { return ISF::Geant4; };

    virtual void push( ISF::ISFParticle *particle, const ISF::ISFParticle *parent ) override;

  private:

    ServiceHandle<IDetectorGeometrySvc> m_detGeoSvc{this, "DetectorGeometrySvc", "DetectorGeometrySvc", ""};

    /** Track Creation & transport */
    PublicToolHandle<iGeant4::ITransportTool>  m_g4TransportTool{this, "SimulationTool", "ISFG4TransportTool", ""}; //!< Pointer to the TrackCreator AlgTool

    /** Geant4 Python Configuration Tool */
    PublicToolHandle<IAlgTool>                 m_configTool{this, "G4ConfigTool", "", ""}; //!< Pointer to the G4 Python Config Tool

    PublicToolHandle<ISensitiveDetectorMasterTool> m_senDetTool{this, "SensitiveDetectorMasterTool", "SensitiveDetectorMasterTool", ""};
    PublicToolHandle<IFastSimulationMasterTool> m_fastSimTool{this, "FastSimulationMasterTool", "FastSimulationMasterTool", ""};

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
  };

}

#endif
