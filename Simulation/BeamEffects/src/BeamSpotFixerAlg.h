// Dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak <tadej@cern.ch>

#ifndef BEAMEFFECTS_BEAMSPOTFIXERALG_H
#define BEAMEFFECTS_BEAMSPOTFIXERALG_H

#include <AthenaBaseComps/AthReentrantAlgorithm.h>
#include <StoreGate/ReadCondHandleKey.h>
#include <StoreGate/ReadHandleKey.h>
#include <StoreGate/WriteHandleKey.h>

#include <BeamSpotConditionsData/BeamSpotData.h>
#include <xAODEventInfo/EventInfo.h>

namespace Simulation
{

  /** @class BeamSpotFixerAlg
    An algorithm to fix beam spot information in xAOD::EventInfo
  */
  class BeamSpotFixerAlg : public AthReentrantAlgorithm
  {
  public:
    //** Constructor with parameters */
    BeamSpotFixerAlg( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~BeamSpotFixerAlg() = default;

    /** Athena algorithm's interface method initialize() */
    virtual StatusCode initialize() override final;

    /** Athena algorithm's interface method execute() */
    virtual StatusCode execute(const EventContext& ctx) const override final;

  private:
    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

    SG::ReadHandleKey<xAOD::EventInfo> m_inputKey{ this, "InputKey", "Input_EventInfo", "ReadHandleKey for Input xAOD::EventInfo" };
    SG::WriteHandleKey<xAOD::EventInfo> m_outputKey{ this, "OutputKey", "EventInfo", "WriteHandleKey for Output xAOD::EventInfo" };
  };

}

#endif // BEAMEFFECTS_BEAMSPOTFIXERALG_H
