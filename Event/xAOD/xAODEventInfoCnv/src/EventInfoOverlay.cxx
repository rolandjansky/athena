/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak <tadej@cern.ch>

#include "EventInfoOverlay.h"

#include <xAODEventInfo/EventAuxInfo.h>


namespace xAODMaker
{


EventInfoOverlay::EventInfoOverlay(const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) {}


StatusCode EventInfoOverlay::initialize()
{
  ATH_MSG_DEBUG("Initializing...");

  ATH_MSG_INFO("Data overlay: " << m_dataOverlay.value());
  if (m_validateBeamSpot.value()) {
    ATH_MSG_INFO("Will validate beam spot size");
  }

  // Check and initialize keys
#if !defined(XAOD_ANALYSIS) && !defined(GENERATIONBASE)
  ATH_CHECK(m_beamSpotKey.initialize());
#endif

  ATH_CHECK( m_bkgInputKey.initialize(!m_bkgInputKey.key().empty()) );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_bkgInputKey);
  ATH_CHECK( m_signalInputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_signalInputKey);
  ATH_CHECK( m_outputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey);

  return StatusCode::SUCCESS;
}


StatusCode EventInfoOverlay::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("execute() begin");

  SG::ReadHandle<xAOD::EventInfo> bkgEvent(m_bkgInputKey, ctx);
  if (!bkgEvent.isValid()) {
    ATH_MSG_ERROR("Could not get background xAOD::EventInfo " << bkgEvent.name() << " from store " << bkgEvent.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found background xAOD::EventInfo " << bkgEvent.name() << " in store " << bkgEvent.store());

  SG::ReadHandle<xAOD::EventInfo> signalEvent(m_signalInputKey, ctx);
  if (!signalEvent.isValid()) {
    ATH_MSG_ERROR("Could not get signal xAOD::EventInfo " << signalEvent.name() << " from store " << signalEvent.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found signal xAOD::EventInfo " << signalEvent.name() << " in store " << signalEvent.store());

  auto outputEvent = std::make_unique<xAOD::EventInfo>();
  auto outputEventAux = std::make_unique<xAOD::EventAuxInfo>();
  outputEvent->setStore (outputEventAux.get());

  // Copy the eventInfo data from background event
  *outputEvent = *bkgEvent;
  // Remove all subevents for now
  outputEvent->clearSubEvents();

  if (!m_dataOverlay.value()) {
    outputEvent->setEventNumber(signalEvent->eventNumber());
  }

  // Propagate MC metadata
  if (signalEvent->mcChannelNumber() == 0) {
    if (m_mcChannelNumber.value() != 0) {
      ATH_MSG_WARNING("Signal mcChannelNumber is 0, setting it to " << m_mcChannelNumber.value());
      outputEvent->setMCChannelNumber(m_mcChannelNumber.value());
    } else {
      ATH_MSG_WARNING("Signal mcChannelNumber is 0");
      outputEvent->setMCChannelNumber(signalEvent->mcChannelNumber());
    }
  } else {
    if (m_mcChannelNumber.value() != 0 && signalEvent->mcChannelNumber() != m_mcChannelNumber.value()) {
      ATH_MSG_WARNING("Signal mcChannelNumber (" << signalEvent->mcChannelNumber()
                      << ") and provided mcChannelNumber (" << m_mcChannelNumber.value() << ") do not match.");
    }
    outputEvent->setMCChannelNumber(signalEvent->mcChannelNumber());
  }

  if (signalEvent->mcEventNumber() == 0) {
    if (signalEvent->eventNumber() != 0) {
      ATH_MSG_WARNING("Signal mcEventNumber is 0, setting it to match the eventNumber (" << signalEvent->eventNumber() << ")");
      outputEvent->setMCEventNumber(signalEvent->eventNumber());
    } else {
      ATH_MSG_ERROR("Signal eventNumber and mcEventNumber are 0");
      return StatusCode::FAILURE;
    }
  } else {
    outputEvent->setMCEventNumber(signalEvent->mcEventNumber());
  }

  if (signalEvent->mcEventWeights().empty()) {
    ATH_MSG_ERROR("Signal mcEventWeights are empty. This should not happen.");
    return StatusCode::FAILURE;
  }
  outputEvent->setMCEventWeights(signalEvent->mcEventWeights());

  // MC+MC overlay should always be marked as simulation
  if (!m_dataOverlay.value()) {
    outputEvent->setEventTypeBitmask(xAOD::EventInfo::IS_SIMULATION);
  }

  // Propagate core event flags
  outputEvent->setEventFlags(xAOD::EventInfo::Core,
                             signalEvent->eventFlags(xAOD::EventInfo::Core)
                             | bkgEvent->eventFlags(xAOD::EventInfo::Core) );
  outputEvent->setErrorState(xAOD::EventInfo::Core,
                             std::max(signalEvent->errorState(xAOD::EventInfo::Core),
                                      bkgEvent->errorState(xAOD::EventInfo::Core)));

  // Ensure correct beam spot info
#if !defined(XAOD_ANALYSIS) && !defined(GENERATIONBASE)
  if (m_validateBeamSpot.value()) {
    if (std::abs(signalEvent->beamPosSigmaZ() - bkgEvent->beamPosSigmaZ()) > 1e-5f) {
      ATH_MSG_ERROR("Beam spot size does not match between signal and background events");
      return StatusCode::FAILURE;
    }
  } else {
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
    if (!beamSpotHandle.isValid()) {
      ATH_MSG_ERROR("Beam spot information not valid");
      return StatusCode::FAILURE;
    }
    outputEvent->setBeamPos( beamSpotHandle->beamPos()[ Amg::x ],
                            beamSpotHandle->beamPos()[ Amg::y ],
                            beamSpotHandle->beamPos()[ Amg::z ] );
    outputEvent->setBeamPosSigma( beamSpotHandle->beamSigma( 0 ),
                                  beamSpotHandle->beamSigma( 1 ),
                                  beamSpotHandle->beamSigma( 2 ) );
    outputEvent->setBeamPosSigmaXY( beamSpotHandle->beamSigmaXY() );
    outputEvent->setBeamTiltXZ( beamSpotHandle->beamTilt( 0 ) );
    outputEvent->setBeamTiltYZ( beamSpotHandle->beamTilt( 1 ) );
    outputEvent->setBeamStatus( beamSpotHandle->beamStatus() );
  }
#endif

  // Creating output timings container
  SG::WriteHandle<xAOD::EventInfo> outputEventH(m_outputKey, ctx);
  ATH_CHECK(outputEventH.record(std::move(outputEvent), std::move(outputEventAux)));
  ATH_MSG_DEBUG("Recorded output xAOD::EventInfo " << outputEventH.name() << " in store " << outputEventH.store());

  ATH_MSG_DEBUG("execute() end");
  return StatusCode::SUCCESS;
}


} // namespace xAODMaker
