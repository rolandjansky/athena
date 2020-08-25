/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

  // Check and initialize keys
#if !defined(XAOD_ANALYSIS) && !defined(SIMULATIONBASE) && !defined(GENERATIONBASE)
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

  // Creating output timings container
  SG::WriteHandle<xAOD::EventInfo> outputEvent(m_outputKey, ctx);
  ATH_CHECK(outputEvent.record(std::make_unique<xAOD::EventInfo>(), std::make_unique<xAOD::EventAuxInfo>()));
  if (!outputEvent.isValid()) {
    ATH_MSG_ERROR("Could not record output xAOD::EventInfo " << outputEvent.name() << " to store " << outputEvent.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output xAOD::EventInfo " << outputEvent.name() << " in store " << outputEvent.store());


  // Copy the eventInfo data from background event
  *outputEvent = *bkgEvent;
  // Remove all subevents for now
  outputEvent->clearSubEvents();

  if (!m_dataOverlay.value()) {
    outputEvent->setEventNumber(signalEvent->eventNumber());
  }

  // Propagate MC metadata
  outputEvent->setMCChannelNumber(signalEvent->mcChannelNumber());
  outputEvent->setMCEventNumber(signalEvent->mcEventNumber());
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
#if !defined(XAOD_ANALYSIS) && !defined(SIMULATIONBASE) && !defined(GENERATIONBASE)
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
#endif

  ATH_MSG_DEBUG("execute() end");
  return StatusCode::SUCCESS;
}


} // namespace xAODMaker
