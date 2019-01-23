// -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOTOOL_h
#define ISF_FASTCALOTOOL_h

// STL includes
#include <string>

//Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

//Athena
#include "AthenaBaseComps/AthAlgTool.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"

//ISF
#include "ISF_Interfaces/BaseSimulatorTool.h"
#include "ISF_Event/ISFParticleContainer.h"
#include "ISF_FastCaloSimInterfaces/IPunchThroughTool.h"

// Tracking includes
#include "TrkExInterfaces/ITimedExtrapolator.h"
#include "TrkEventPrimitives/PdgToParticleHypothesis.h"

#include "TRandom3.h"

// forward declarations
class ITrackingGeometrySvc;
class CaloCellContainer;

namespace Trk {
  class TrackingVolume;
  class TrackingGeometry;
}


//Calo
#include "CaloInterface/ICaloCellMakerTool.h"
#include "CaloEvent/CaloCellContainer.h"

namespace ISF {

class FastCaloTool : public BaseSimulatorTool  {
public:
  FastCaloTool( const std::string& type, const std::string& name,  const IInterface* parent);

  ~FastCaloTool();

  StatusCode initialize() override;

  virtual StatusCode simulate( const ISFParticle& isp, ISFParticleContainer&, McEventCollection* );

  virtual StatusCode setupEventST() override;

  virtual StatusCode setupEvent() override;

  virtual StatusCode releaseEventST() override;

  virtual StatusCode releaseEvent() override;

  virtual SimulationFlavor simFlavor() const override { return ISF::FastCaloSim; };
private:
    StatusCode commonInitialize(); // TEMP

    /** Called by setupEvent() and setupEventMT() */
    StatusCode commonSetup();

    /** process the given particle */
    StatusCode processOneParticle( const ISF::ISFParticle &isfp);

    /** extrapolation through Calo */
    std::vector<Trk::HitInfo>* caloHits(const ISF::ISFParticle &isfp) const;

    /** The Extrapolator setup */
    PublicToolHandle<Trk::ITimedExtrapolator>      m_extrapolator{this, "Extrapolator", "", ""};

    /** whether CellContainer to be created will own (default) its cells or not */
    int m_ownPolicy;

    // particle processing mode
    bool                                m_batchProcessMcTruth{false};       //!< process particles from McTruth at end of event

    bool                                m_simulateUndefinedBCs{false};      //!< do/don't simulate undefined barcode particles

    std::string  m_caloCellsOutputName{"AllCalo"};

    // authorise input to be the same as output (to be done with care)
    bool m_caloCellHack{false};
    //check if punch through simulation is used
    bool m_doPunchThrough{false};

    Trk::PdgToParticleHypothesis        m_pdgToParticleHypothesis;

    // list of tools to be used
    PublicToolHandleArray<ICaloCellMakerTool> m_caloCellMakerTools_setup{this, "CaloCellMakerTools_setup", {}, ""};
    PublicToolHandleArray<ICaloCellMakerTool> m_caloCellMakerTools_simulate{this, "CaloCellMakerTools_simulate", {}, ""};
    PublicToolHandleArray<ICaloCellMakerTool> m_caloCellMakerTools_release{this, "CaloCellMakerTools_release", {}, ""};
    PublicToolHandle< IPunchThroughTool > m_punchThroughTool{this, "PunchThroughTool", "", ""};
    CaloCellContainer *                 m_theContainer{};

  SG::WriteHandleKey< CaloCellContainer > m_caloCellKey{ this, "CaloCells", "DefaultCaloCellContainer", "The name of the output CaloCellContainer" };

  TRandom3 m_rndm;
};

}

#endif
