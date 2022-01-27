/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMuonCombinedTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IRECMUONCREATORTOOL_H
#define IRECMUONCREATORTOOL_H

#include <string>
#include <vector>

#include "GaudiKernel/IAlgTool.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/InDetCandidateToTagMap.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/SlowMuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace MuonCombined {

 
 
    /** @class IMuonCreatorTool
        @brief interface for tools building combined muons from ID and Muon candidates

        The tool is capable of writing a large number of different output formats. Whether or not certain outputs
        are written is controled using the OutputData struct defined below.
        If the pointer to a certain type is non-zero, the container is filled.
        The MuonContainer is mandatory, all other output types are optional.

     */

    class IMuonCreatorTool : virtual public IAlgTool {
    public:
      
        using InDetCandidateTags = std::pair<const InDetCandidate*, std::vector<const TagBase*> > ;

        struct OutputData {
            OutputData(xAOD::MuonContainer& container) :
                muonContainer(&container){}
            /** MuonContainer to be filled with the Muon objects */
            xAOD::MuonContainer* muonContainer{nullptr};

            /** container for the combined track particles */
            xAOD::TrackParticleContainer* combinedTrackParticleContainer{nullptr};

            /** container for the extrapolated track particles */
            xAOD::TrackParticleContainer* extrapolatedTrackParticleContainer{nullptr};

            /** container for the extrapolated track particles */
            xAOD::TrackParticleContainer* msOnlyExtrapolatedTrackParticleContainer{nullptr};

            /** container for the combined tracks */
            TrackCollection* combinedTrackCollection{nullptr};

            /** container for the extrapolated tracks */
            TrackCollection* extrapolatedTrackCollection{nullptr};

            /** container for the extrapolated tracks */
            TrackCollection* msOnlyExtrapolatedTrackCollection{nullptr};

            /** container for the xAOD segments from MuGirl */
            xAOD::MuonSegmentContainer* xaodSegmentContainer{nullptr};

            /** collection for the segments from MuGirl */
            Trk::SegmentCollection* muonSegmentCollection{nullptr};

            /** container for the Slow muon content */
            xAOD::SlowMuonContainer* slowMuonContainer{nullptr};

            /** container for the clusters associated with muons */
            xAOD::CaloClusterContainer* clusterContainer{nullptr};
        };

        static const InterfaceID& interfaceID(){
               static const InterfaceID IID_IMuonCreatorTool("MuonCombined::IMuonCreatorTool", 1, 0);
                return IID_IMuonCreatorTool;
        }


        /**IMuonCreatorTool interface: build muons from ID and MS candidates */

        virtual void create(const EventContext& ctx, const MuonCandidateCollection* muonCandidates, 
                            const std::vector<const InDetCandidateToTagMap*>& tagMaps, OutputData& outputData) const = 0;

        /** create a muon from a muon candidate */
        virtual xAOD::Muon* create(const EventContext& ctx, const MuonCandidate& candidate, OutputData& outputData) const = 0;

        /** create a muon from an ID candidate */
        virtual xAOD::Muon* create(const EventContext& ctx, InDetCandidateTags& candidate, OutputData& outputData) const = 0;
        /** default virtual destructor */
        virtual ~IMuonCreatorTool() = default;
    };

 
}  // namespace MuonCombined

#endif
