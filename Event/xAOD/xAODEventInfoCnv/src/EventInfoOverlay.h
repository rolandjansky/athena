/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file
 *
 * An algorithm to produce a proper final xAOD::EventInfo from
 * signal and background ones.
 *
 * @author Tadej Novak <tadej@cern.ch>
 *
 */

#ifndef OVERLAYCOMMONALGS_EVENTINFOOVERLAY_H
#define OVERLAYCOMMONALGS_EVENTINFOOVERLAY_H

#include <AthenaBaseComps/AthReentrantAlgorithm.h>
#include <xAODEventInfo/EventInfo.h>

#if !defined(XAOD_ANALYSIS) && !defined(SIMULATIONBASE) && !defined(GENERATIONBASE)
#include <StoreGate/ReadCondHandleKey.h>
#include <BeamSpotConditionsData/BeamSpotData.h>
#endif

namespace xAODMaker
{

class EventInfoOverlay : public AthReentrantAlgorithm
{
public:
  EventInfoOverlay(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;

private:
#if !defined(XAOD_ANALYSIS) && !defined(SIMULATIONBASE) && !defined(GENERATIONBASE)
  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
#endif

  SG::ReadHandleKey<xAOD::EventInfo> m_bkgInputKey{ this, "BkgInputKey", "Bkg_EventInfo", "ReadHandleKey for Background xAOD::EventInfo" };
  SG::ReadHandleKey<xAOD::EventInfo> m_signalInputKey{ this, "SignalInputKey", "Sig_EventInfo", "ReadHandleKey for Signal xAOD::EventInfo" };
  SG::WriteHandleKey<xAOD::EventInfo> m_outputKey{ this, "OutputKey", "EventInfo", "WriteHandleKey for Output xAOD::EventInfo" };

  Gaudi::Property<bool> m_dataOverlay{ this, "DataOverlay", false, "True if running data overlay" };
};

} // namespace xAODMaker

#endif
