/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Michael Duehrssen <michael.duehrssen@cern.ch>

#include "EventInfoUpdateFromContextAlg.h"

#include <xAODEventInfo/EventAuxInfo.h>


namespace xAODMaker
{

  EventInfoUpdateFromContextAlg::EventInfoUpdateFromContextAlg(const std::string &name, ISvcLocator *pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator) {}

  StatusCode EventInfoUpdateFromContextAlg::initialize()
  {
    ATH_MSG_DEBUG("Initializing...");

    // Check and initialize keys
#if !defined(XAOD_ANALYSIS) && !defined(GENERATIONBASE)
    ATH_CHECK(m_beamSpotKey.initialize());
#endif

    ATH_CHECK( m_signalInputKey.initialize() );
    ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_signalInputKey);
    ATH_CHECK( m_outputKey.initialize() );
    ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey);

    return StatusCode::SUCCESS;
  }


  StatusCode EventInfoUpdateFromContextAlg::execute(const EventContext& ctx) const
  {
    ATH_MSG_DEBUG("execute() begin");

    SG::ReadHandle<xAOD::EventInfo> signalEvent(m_signalInputKey, ctx);
    if (!signalEvent.isValid()) {
      ATH_MSG_ERROR("Could not get signal xAOD::EventInfo " << signalEvent.name() << " from store " << signalEvent.store());
      return StatusCode::FAILURE;
    }
    float sig_beamPosSigmaZ=-1;
    try {
      sig_beamPosSigmaZ=signalEvent->beamPosSigmaZ();
    }
    catch(const SG::ExcBadAuxVar&) {
    }

    ATH_MSG_DEBUG("Found signal xAOD::EventInfo " << signalEvent.name() << " in store " << signalEvent.store()<<" : "<<*signalEvent<<" beamspot size z="<<sig_beamPosSigmaZ);

    // Creating output timings container
    SG::WriteHandle<xAOD::EventInfo> outputEvent(m_outputKey, ctx);
    ATH_CHECK(outputEvent.record(std::make_unique<xAOD::EventInfo>(), std::make_unique<xAOD::EventAuxInfo>()));
    if (!outputEvent.isValid()) {
      ATH_MSG_ERROR("Could not record output xAOD::EventInfo " << outputEvent.name() << " to store " << outputEvent.store());
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Recorded output xAOD::EventInfo " << outputEvent.name() << " in store " << outputEvent.store());

    // Copy the eventInfo data from background event
    *outputEvent = *signalEvent;

    //Update run number, event number, lumi block and timestamp from EventContext
    outputEvent->setRunNumber( ctx.eventID().run_number() );
    outputEvent->setEventNumber( ctx.eventID().event_number() );
    outputEvent->setLumiBlock( ctx.eventID().lumi_block() );
    outputEvent->setTimeStamp( ctx.eventID().time_stamp() );

    // Propagate MC metadata
    if (outputEvent->mcChannelNumber() == 0) {
      if (m_mcChannelNumber.value() != 0) {
        ATH_MSG_WARNING("Input mcChannelNumber is 0, setting it to " << m_mcChannelNumber.value());
        outputEvent->setMCChannelNumber(m_mcChannelNumber.value());
      } else {
        ATH_MSG_WARNING("Input mcChannelNumber is 0");
      }
    } else {
      if (m_mcChannelNumber.value() != 0 && signalEvent->mcChannelNumber() != m_mcChannelNumber.value()) {
        ATH_MSG_WARNING("Input mcChannelNumber (" << signalEvent->mcChannelNumber()
                        << ") and provided mcChannelNumber (" << m_mcChannelNumber.value() << ") do not match.");
      }
    }

    if (signalEvent->mcEventNumber() == 0) {
      if (signalEvent->eventNumber() != 0) {
        ATH_MSG_WARNING("Input mcEventNumber is 0, setting it to match the eventNumber (" << signalEvent->eventNumber() << ")");
        outputEvent->setMCEventNumber(signalEvent->eventNumber());
      } else {
        ATH_MSG_ERROR("Input eventNumber and mcEventNumber are 0");
        return StatusCode::FAILURE;
      }
    }

    if (signalEvent->mcEventWeights().empty()) {
      ATH_MSG_ERROR("Input mcEventWeights are empty. This should not happen.");
      return StatusCode::FAILURE;
    }

    // Ensure correct beam spot info
#if !defined(XAOD_ANALYSIS) && !defined(GENERATIONBASE)
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

    ATH_MSG_DEBUG("Output xAOD::EventInfo " << outputEvent.name() << " : " << *outputEvent<<" beamspot size z="<<outputEvent->beamPosSigmaZ());
    ATH_MSG_DEBUG("execute() end");
    return StatusCode::SUCCESS;
  }


} // namespace xAODMaker
