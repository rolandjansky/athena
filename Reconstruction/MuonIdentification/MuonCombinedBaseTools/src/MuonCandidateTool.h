/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONCANDIDATETOOL_H
#define MUONCOMBINEDBASETOOLS_MUONCANDIDATETOOL_H

#include "MuonCombinedToolInterfaces/IMuonCandidateTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMuonTrackExtrapolationTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"

namespace MuonCombined {

  class MuonCandidateTool: public AthAlgTool, virtual public IMuonCandidateTool
  {

  public:
    MuonCandidateTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonCandidateTool()=default;
  
    virtual StatusCode initialize() override;

    /**IMuonCandidateTool interface: build a MuonCandidateCollection from a TrackCollection of spectrometer tracks */
    virtual
      void create( const xAOD::TrackParticleContainer& tracks, MuonCandidateCollection& outputCollection, TrackCollection& outputTracks ) override;

  private:
    ToolHandle<Muon::MuonEDMPrinterTool> m_printer;
    ToolHandle<Rec::ICombinedMuonTrackBuilder> m_trackBuilder;
    ToolHandle<Muon::IMuonTrackExtrapolationTool> m_trackExtrapolationTool;
    ToolHandle<Trk::ITrackAmbiguityProcessorTool> m_ambiguityProcessor;
    ToolHandle<Trk::IExtendedTrackSummaryTool> m_trackSummaryTool {this, "TrackSummaryTool", "MuonTrackSummaryTool"};
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

    unsigned int m_extrapolationStrategy;
  };

}	// end of namespace

#endif 


