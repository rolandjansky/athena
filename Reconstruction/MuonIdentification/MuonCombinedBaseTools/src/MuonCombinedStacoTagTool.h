/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONCOMBINEDSTACOTAGTOOL_H
#define MUONCOMBINEDBASETOOLS_MUONCOMBINEDSTACOTAGTOOL_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedTagTool.h"
#include "MuonCombinedToolInterfaces/IMuonTrackTagTool.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSegment/SegmentCollection.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include <vector>

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

namespace Muon
{
  class MuonEDMPrinterTool;
}
namespace Trk {
  class IExtrapolator;
}

namespace MuonCombined {
  class InDetCandidate;
  class MuonCandidate;
  class InDetCandidateToTagMap;
  class IMuonTrackTagTool;

  class MuonCombinedStacoTagTool: public AthAlgTool, virtual public IMuonCombinedTagTool
  {

  public:
    MuonCombinedStacoTagTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonCombinedStacoTagTool(void); // destructor
  
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    /**IMuonCombinedTagTool interface: build combined  muons from a muon and a vector of indet candidates */    
    virtual
      void combine( const MuonCandidate& muonCandidate, const std::vector<const InDetCandidate*>& indetCandidates, InDetCandidateToTagMap& tagMap,
		    TrackCollection* combTracks, TrackCollection* METracks) const override;

  private:

    std::unique_ptr<const Trk::Perigee> theCombIdMu( const Trk::Perigee& indetPerigee, const Trk::Perigee& extrPerigee, double& chi2 ) const;

    // helpers, managers, tools
    ToolHandle<Muon::MuonEDMPrinterTool>        m_printer;
    ToolHandle<MuonCombined::IMuonTrackTagTool> m_tagTool;
    ToolHandle<Trk::IExtrapolator>              m_extrapolator;
    ToolHandle<Trk::IParticleCaloExtensionTool> m_caloExtTool;

  };

}	// end of namespace

#endif // MUIDCOMBINEDTOOLS_MUIDMUONRECOVERY_H


