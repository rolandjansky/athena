/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak <tadej@cern.ch>

#include <StoreGate/ReadCondHandle.h>
#include <StoreGate/ReadHandle.h>
#include <StoreGate/WriteHandle.h>
#include <xAODEventInfo/EventAuxInfo.h>

#include "BeamSpotFixerAlg.h"

namespace Simulation
{

  BeamSpotFixerAlg::BeamSpotFixerAlg( const std::string& name, ISvcLocator* pSvcLocator )
    : AthReentrantAlgorithm( name, pSvcLocator )
  {
  }

  /** Athena algorithm's interface method initialize() */
  StatusCode BeamSpotFixerAlg::initialize()
  {
    ATH_CHECK(m_beamSpotKey.initialize());

    ATH_CHECK( m_inputKey.initialize() );
    ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_inputKey);
    ATH_CHECK( m_outputKey.initialize() );
    ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey);

    return StatusCode::SUCCESS;
  }

  StatusCode BeamSpotFixerAlg::execute(const EventContext& ctx) const
  {
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
    if (!beamSpotHandle.isValid()) {
      ATH_MSG_ERROR("Beam spot information not valid");
      return StatusCode::FAILURE;
    }

    SG::ReadHandle<xAOD::EventInfo> inputEventInfo(m_inputKey, ctx);
    if (!inputEventInfo.isValid()) {
      ATH_MSG_ERROR("Could not get input xAOD::EventInfo " << inputEventInfo.name() << " from store " << inputEventInfo.store());
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Found input xAOD::EventInfo " << inputEventInfo.name() << " in store " << inputEventInfo.store());

    SG::WriteHandle<xAOD::EventInfo> eventInfo(m_outputKey, ctx);
    ATH_CHECK(eventInfo.record(std::make_unique<xAOD::EventInfo>(), std::make_unique<xAOD::EventAuxInfo>()));
    if (!eventInfo.isValid()) {
      ATH_MSG_ERROR("Could not record output xAOD::EventInfo " << eventInfo.name() << " to store " << eventInfo.store());
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Recorded output xAOD::EventInfo " << eventInfo.name() << " in store " << eventInfo.store());

    // Copy the eventInfo data
    *eventInfo = *inputEventInfo;

    // Copy the beamspot info
    eventInfo->setBeamPos( beamSpotHandle->beamPos()[ Amg::x ],
                           beamSpotHandle->beamPos()[ Amg::y ],
                           beamSpotHandle->beamPos()[ Amg::z ] );
    eventInfo->setBeamPosSigma( beamSpotHandle->beamSigma( 0 ),
                                beamSpotHandle->beamSigma( 1 ),
                                beamSpotHandle->beamSigma( 2 ) );
    eventInfo->setBeamPosSigmaXY( beamSpotHandle->beamSigmaXY() );
    eventInfo->setBeamTiltXZ( beamSpotHandle->beamTilt( 0 ) );
    eventInfo->setBeamTiltYZ( beamSpotHandle->beamTilt( 1 ) );
    eventInfo->setBeamStatus( beamSpotHandle->beamStatus() );

    return StatusCode::SUCCESS;
  }

} // namespace Simulation
