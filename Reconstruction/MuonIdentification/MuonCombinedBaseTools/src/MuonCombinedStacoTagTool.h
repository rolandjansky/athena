/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONCOMBINEDSTACOTAGTOOL_H
#define MUONCOMBINEDBASETOOLS_MUONCOMBINEDSTACOTAGTOOL_H

#include "MuonCombinedToolInterfaces/IMuonCombinedTagTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonCombinedToolInterfaces/IMuonTrackTagTool.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSegment/SegmentCollection.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "TrkExInterfaces/IExtrapolator.h"

#include <string>
#include <vector>

namespace MuonCombined {
  class InDetCandidate;
  class MuonCandidate;
  class InDetCandidateToTagMap;

  class MuonCombinedStacoTagTool: public AthAlgTool, virtual public IMuonCombinedTagTool
  {

  public:
    MuonCombinedStacoTagTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonCombinedStacoTagTool()=default;
  
    virtual StatusCode initialize() override;

    /**IMuonCombinedTagTool interface: build combined  muons from a muon and a vector of indet candidates */    
    virtual
      void combine( const MuonCandidate& muonCandidate, const std::vector<const InDetCandidate*>& indetCandidates, InDetCandidateToTagMap& tagMap,
		    TrackCollection* combTracks, TrackCollection* METracks) const override;

  private:

    std::unique_ptr<const Trk::Perigee> theCombIdMu( const Trk::Perigee& indetPerigee, const Trk::Perigee& extrPerigee, double& chi2 ) const;

    ToolHandle<Muon::MuonEDMPrinterTool>        m_printer {this, "Printer", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};
    ToolHandle<MuonCombined::IMuonTrackTagTool> m_tagTool {this, "TagTool", "MuonCombined::MuonTrackTagTestTool/MuonTrackTagTestTool"};
    ToolHandle<Trk::IExtrapolator>              m_extrapolator {this, "Extrapolator", "Trk::Extrapolator/AtlasExtrapolator"};
    ToolHandle<Trk::IParticleCaloExtensionTool> m_caloExtTool {this, "ParticleCaloExtensionTool", "Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"};
  };

}	// end of namespace

#endif // MUIDCOMBINEDTOOLS_MUIDMUONRECOVERY_H


