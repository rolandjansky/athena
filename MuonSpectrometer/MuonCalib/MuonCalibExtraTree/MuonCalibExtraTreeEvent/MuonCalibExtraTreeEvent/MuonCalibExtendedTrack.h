/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBEXTENDEDTRACK_H
#define MUONCALIBEXTENDEDTRACK_H

#include <algorithm>
#include <string>
#include <vector>

#include "AthenaKernel/getMessageSvc.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonCalibExtraTreeEvent/MuonCalibExtendedTrackOverlap.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibLayerMapping.h"
#include "MuonCalibExtraTreeEvent/MuonCalibRawHitAssociationMap.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTrackSummary.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTrack_E.h"

namespace MuonCalib {

    class MuonCalibExtendedSegment;

    /**
       @class MuonCalibExtendedTrack

       A track plus everything one can dream of knowing about a track.
  */
    class MuonCalibExtendedTrack : public MuonCalibTrack_E {
    public:
        /**  Constructor taking input track. */
        MuonCalibExtendedTrack(const MuonCalibTrack_E& track, int pdgCode = 0, int barCode = 0);
        virtual ~MuonCalibExtendedTrack();

        /** returns trackparameter d0 at IP */
        // float d0ip() const;

        /** returns trackparameter z0 at IP */
        // float z0ip() const;

        /** returns momentum at IP */
        // float pip() const;

        /** returns pdgCode */
        int pdgCode() const;

        /** returns barCode */
        int barCode() const;

        /** access to hit counts */
        const MuonCalibTrackSummary& summary() const { return m_summary; }

        /** access to hits per station layer index (mdt) */
        const StationIndexHitsMap& mdtStationIndexHitsMap() const { return m_mdtHitsPerStationIndex; }

        /** access to hits per station layer index (rpc) */
        const StationIndexHitsMap& rpcStationIndexHitsMap() const { return m_rpcHitsPerStationIndex; }

        /** access to hits per station layer index (tgc) */
        const StationIndexHitsMap& tgcStationIndexHitsMap() const { return m_tgcHitsPerStationIndex; }

        /** access to hits per station layer index (csc) */
        const StationIndexHitsMap& cscStationIndexHitsMap() const { return m_cscHitsPerStationIndex; }

        /** dump all information to string */
        std::string dump() const;

        /** dump track parameters to string */
        std::string dumpPars() const;

        /** dump track summary to string */
        std::string dumpSummary() const;

        /** dump track intersects to string */
        std::string dumpIntersects() const;

        /** access to the list of intersected station layers */
        const StationIntersectedLayerMap& intersectedStationLayer() const;

        /** access station layers intersected by track without hits */
        const StationIndexSet& intersectedLayersWithoutHits() const { return m_intersectedLayersWithoutHits; }

        /** access rpc chambers intersected by track without hits */
        const StationIndexSet& intersectedRpcLayersWithoutHits() const { return m_intersectedRpcLayerWithoutHits; }

        /** access tgc chamber layers intersected by track without hits */
        const StationIndexSet& intersectedTgcLayersWithoutHits() const { return m_intersectedTgcLayerWithoutHits; }

        /** access hit identifier map (hits give the MuonFixedId */
        const IdHitMap& idHitMap() const { return m_hitIdMap; }

        /** hit information per station */
        const IdHitsMap& hitsPerChamberMap() const { return m_hitsPerChamber; }

        /** access to list of the segment that are associated to this track */
        const std::vector<std::shared_ptr<MuonCalibExtendedSegment>>& associatedSegments() const;
        ///** access to list of the tracks that are associated to this track */
        // const std::vector<std::shared_ptr<MuonCalibExtendedTrack>>& associatedTracks() const { return m_associatedTracks; }

        /** add associated segment */
        void addSegment(MuonCalibExtendedSegment* seg);
        /** add associated track */
        // void addAssociatedTrack(MuonCalibExtendedTrack* track) { m_associatedTracks.emplace_back(track); }

        /** check whether the given segment is associated to this one (pointer based) */
        bool isAssociated(const MuonCalibExtendedSegment* segment) const;

        /** check whether the given track is associated to this one (pointer based) */
        // bool isAssociated(const MuonCalibExtendedTrack* track) const;

        /** access raw hit assocation map */
        const MuonCalibRawHitAssociationMap& rawHitAssociationMap() const { return m_rawHitAssociationMap; }
        MuonCalibRawHitAssociationMap& rawHitAssociationMap() { return m_rawHitAssociationMap; }

        /** calculate hit overlap between two tracks */
        MuonCalibExtendedTrackOverlap calculateHitOverlap(const MuonCalibExtendedTrack& track) const;

        /** get associated track for the give author, returns zero if not found */
        // std::shared_ptr<MuonCalibExtendedTrack> getAssociatedTrack(int author) const;

        /** check whether the track is confirmed by an ID track */
        bool isIDConfirmed() const;

    private:
        int m_pdgCode{0};
        int m_barCode{0};
        /** position of perigee parameters */
        Amg::Vector3D m_pos{0., 0., 0.};

        /** direction of perigee parameters */
        Amg::Vector3D m_dir{0., 0., 0.};

        /** track summary */
        MuonCalibTrackSummary m_summary{};

        /** hit information per station layer index */
        StationIndexHitsMap m_mdtHitsPerStationIndex{};
        StationIndexHitsMap m_rpcHitsPerStationIndex{};
        StationIndexHitsMap m_tgcHitsPerStationIndex{};
        StationIndexHitsMap m_cscHitsPerStationIndex{};

        /** hit information per station */
        IdHitsMap m_hitsPerChamber{};

        /** hit information per station */
        IdHitMap m_hitIdMap{};

        /** map with all station layers intersected by track */
        StationIntersectedLayerMap m_intersectedLayers{};

        /** set with all station layers intersected by the track without hits */
        StationIndexSet m_intersectedLayersWithoutHits{};

        /** set with all rpc chamber identifiers of layers intersected by the track without hits */
        StationIndexSet m_intersectedRpcLayerWithoutHits{};

        /** set with all tgc chamber identifiers of layers intersected by the track without hits */
        StationIndexSet m_intersectedTgcLayerWithoutHits{};

        /** list of segments associated with this track */
        std::vector<std::shared_ptr<MuonCalibExtendedSegment>> m_associatedSegments;

        /** list of tracks associated with this track */
        std::vector<std::shared_ptr<MuonCalibExtendedTrack>> m_associatedTracks;

        /** raw hit association map */
        MuonCalibRawHitAssociationMap m_rawHitAssociationMap;
    };
}  // namespace MuonCalib
#endif
