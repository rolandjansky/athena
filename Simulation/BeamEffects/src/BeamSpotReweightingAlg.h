// Dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BEAMEFFECTS_BeamSpotReweightingAlg_H
#define BEAMEFFECTS_BeamSpotReweightingAlg_H

// base class header
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// Athena includes
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "GeneratorObjects/McEventCollection.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/PhysicalConstants.h"

// Forward declarations
#include "AtlasHepMC/GenEvent_fwd.h"
#include "AtlasHepMC/GenVertex_fwd.h"

namespace Simulation
{

  /** @class BeamSpotReweightingAlg

      This AthReentrantAlgorithm calculates an event weight based such that 
      the distribution of z-positions of events matches a new beamspot size
     */
  class BeamSpotReweightingAlg : public AthReentrantAlgorithm
  {
  public:

    //** Constructor with parameters */
    BeamSpotReweightingAlg( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~BeamSpotReweightingAlg() = default;

    /** Athena algorithm's interface method initialize() */
    virtual StatusCode initialize() override final;

    /** Athena algorithm's interface method execute() */
    virtual StatusCode execute(const EventContext& ctx) const override final;

  private:

    /** Ensure that the GenEvent::signal_process_vertex has been set */
    HepMC::GenVertex* GetSignalProcessVertex(const HepMC::GenEvent& ge) const;
    
    SG::ReadHandleKey<McEventCollection> m_inputMcEventCollection { this, "InputMcEventCollection", "TruthEvent", "The name of the input McEventCollection" };
    
    SG::WriteDecorHandleKey<xAOD::EventInfo> m_beamSpotWeight {this, "BeamSpotWeight", "EventInfo.BeamSpotWeight", "Decoration for a beam spot weight when reweighting the beam spot size" };
      
    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };  
    
    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo { this, "EventInfo", "EventInfo", "EventInfo object in SG" };
      
    float m_input_beam_sigma_z = 42*Gaudi::Units::mm;
  };

}

#endif // BEAMEFFECTS_BeamSpotReweightingAlg_H
