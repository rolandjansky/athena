/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONCANDIDATETOOL_H
#define MUONCOMBINEDBASETOOLS_MUONCANDIDATETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedToolInterfaces/IMuonCandidateTool.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"

class IBeamCondSvc;

namespace Trk {
  class ITrackAmbiguityProcessorTool;
}

namespace Rec {
  class ICombinedMuonTrackBuilder;
}
namespace Muon {
  class MuonEDMPrinterTool;
  class IMuonTrackExtrapolationTool;
}

namespace MuonCombined {

  class MuonCandidateTool: public AthAlgTool, virtual public IMuonCandidateTool
  {

  public:
    MuonCandidateTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~MuonCandidateTool(void); // destructor
  
    StatusCode initialize();
    StatusCode finalize();

    /**IMuonCandidateTool interface: build a MuonCandidateCollection from a TrackCollection of spectrometer tracks */
    void create( const xAOD::TrackParticleContainer& tracks, MuonCandidateCollection& outputCollection ) const;

  private:
    ToolHandle<Muon::MuonEDMPrinterTool> m_printer;
    ToolHandle<Rec::ICombinedMuonTrackBuilder> m_trackBuilder;
    ToolHandle<Muon::IMuonTrackExtrapolationTool> m_trackExtrapolationTool;
    ToolHandle<Trk::ITrackAmbiguityProcessorTool> m_ambiguityProcessor;
    ServiceHandle< IBeamCondSvc > m_beamService;

    unsigned int m_extrapolationStrategy;
  };

}	// end of namespace

#endif 


