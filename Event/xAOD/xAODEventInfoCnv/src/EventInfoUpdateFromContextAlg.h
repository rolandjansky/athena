/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file
 *
 * An algorithm to produce an updated final xAOD::EventInfo
 * with possible run number,... and beam spot property overrides
 *
 * @author Michael Duehrssen <michael.duehrssen@cern.ch>
 *
 */

#ifndef XAODEVENTINFOCNV_EVENTINFOUPDATEFROMCONTEXTALG_H
#define XAODEVENTINFOCNV_EVENTINFOUPDATEFROMCONTEXTALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

#if !defined(XAOD_ANALYSIS) && !defined(GENERATIONBASE)
#include "StoreGate/ReadCondHandleKey.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#endif

namespace xAODMaker
{

  class EventInfoUpdateFromContextAlg : public AthReentrantAlgorithm
  {
  public:

    EventInfoUpdateFromContextAlg(const std::string &name, ISvcLocator *pSvcLocator);
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
#if !defined(XAOD_ANALYSIS) && !defined(GENERATIONBASE)
    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
#endif

    SG::ReadHandleKey<xAOD::EventInfo> m_signalInputKey{ this, "SignalInputKey", "Input_EventInfo", "ReadHandleKey for input xAOD::EventInfo" };
    SG::WriteHandleKey<xAOD::EventInfo> m_outputKey{ this, "OutputKey", "EventInfo", "WriteHandleKey for Output xAOD::EventInfo" };

    Gaudi::Property<uint32_t> m_mcChannelNumber{ this, "MCChannelNumber", 0, "sample MC channel number" };
  };

} // namespace xAODMaker

#endif
