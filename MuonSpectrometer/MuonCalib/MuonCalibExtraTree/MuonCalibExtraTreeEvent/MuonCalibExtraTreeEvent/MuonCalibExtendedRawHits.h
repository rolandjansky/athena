/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBEXTENDEDRAWHITS_H
#define MUONCALIBEXTENDEDRAWHITS_H

#include "CxxUtils/checker_macros.h"
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibExtraTreeEvent/MuonCalibLayerMapping.h"
#include "MuonCalibExtraUtils/MuonFixedIdManipulator.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {

    class MuonCalibExtendedSegment;
    class MuonCalibPattern;
    class MuonCalibExtendedTrack;
    class MuonCalibRawHitCollection;

    /**
       @class MuonCalibExtendedRawHits

       Access to all raw hits with layer/station granularity.
       Also information on whether the hits are associated with segments or tracks

    */
    class MuonCalibExtendedRawHits {
    public:
        using MdtCalibRawHitPtr = MuonCalibRawHitCollection::MdtCalibRawHitPtr;
        using CscCalibRawHitPtr = MuonCalibRawHitCollection::CscCalibRawHitPtr;

        using RpcCalibRawHitPtr = MuonCalibRawHitCollection::RpcCalibRawHitPtr;
        using TgcCalibRawHitPtr = MuonCalibRawHitCollection::TgcCalibRawHitPtr;

        using ExtendedSegPtr = std::shared_ptr<MuonCalibExtendedSegment>;
        using ExtendedTrkPtr = std::shared_ptr<MuonCalibExtendedTrack>;
        struct ExtendedChannelInfo {
            std::vector<MdtCalibRawHitPtr> hits;
            std::vector<ExtendedSegPtr> associatedSegments;
            std::vector<ExtendedTrkPtr> associatedTracks;
        };

        typedef std::map<MuonFixedId, ExtendedChannelInfo> MdtHitInfoMap;
        typedef MdtHitInfoMap::iterator MdtHitInfoIt;
        typedef MdtHitInfoMap::const_iterator MdtHitInfoCit;

        struct MdtRegionInfo {
            unsigned int nhitsAboveAdcThreshold{0};
            unsigned int nhitsOnSegment{0};
            unsigned int nhitsOnTrack{0};
            MdtHitInfoMap hits{};
        };

        typedef std::map<MuonFixedId, MdtRegionInfo> MdtHitChamberIdMap;
        typedef std::map<MuonFixedIdManipulator::StationIndex, MdtRegionInfo> MdtHitStationIndexMap;

        struct ExtendedInfo {
            std::vector<ExtendedSegPtr> associatedSegments;
            std::vector<ExtendedTrkPtr> associatedTracks;
        };

        typedef std::pair<RpcCalibRawHitPtr, ExtendedInfo> RpcHitInfo;
        typedef std::vector<RpcHitInfo> RpcHitInfoVec;
        typedef std::map<MuonFixedId, RpcHitInfo> RpcHitChamberIdMap;
        typedef std::map<MuonFixedIdManipulator::StationIndex, RpcHitInfoVec> RpcHitStationIndexMap;

        typedef std::pair<TgcCalibRawHitPtr, ExtendedInfo> TgcHitInfo;
        typedef std::vector<TgcHitInfo> TgcHitInfoVec;
        typedef std::map<MuonFixedId, TgcHitInfo> TgcHitChamberIdMap;
        typedef std::map<MuonFixedIdManipulator::StationIndex, TgcHitInfoVec> TgcHitStationIndexMap;

        typedef std::pair<CscCalibRawHitPtr, ExtendedInfo> CscHitInfo;

        typedef std::vector<CscHitInfo> CscHitInfoVec;
        typedef std::map<MuonFixedId, CscHitInfo> CscHitChamberIdMap;
        typedef std::map<MuonFixedIdManipulator::StationIndex, CscHitInfoVec> CscHitStationIndexMap;

    public:
        /**  Default constructor . */
        MuonCalibExtendedRawHits();

        MuonCalibExtendedRawHits(const MuonCalibRawHitCollection& rawHits, const std::vector<ExtendedSegPtr>& segments,
                                 const std::vector<ExtendedTrkPtr>& tracks);

        std::string dumpMdtHits() const;

        /** access to the mdt hits per chamber and station */
        const MdtHitChamberIdMap& mdtHitChamberIdMap() const { return m_mdtChamberIdMap; }
        const MdtHitStationIndexMap& mdtChamberIndexMap() const { return m_mdtChamberIndexMap; }

        /** access to the rpc hits per chamber and station */
        const RpcHitChamberIdMap& rpcHitChamberIdMap() const { return m_rpcChamberIdMap; }
        const RpcHitStationIndexMap& rpcChamberIndexMap() const { return m_rpcChamberIndexMap; }

        /** access to the tgc hits per chamber and station */
        const TgcHitChamberIdMap& tgcHitChamberIdMap() const { return m_tgcChamberIdMap; }
        const TgcHitStationIndexMap& tgcChamberIndexMap() const { return m_tgcChamberIndexMap; }

        /** access to the csc hits per chamber and station */
        const CscHitChamberIdMap& cscHitChamberIdMap() const { return m_cscChamberIdMap; }
        const CscHitStationIndexMap& cscChamberIndexMap() const { return m_cscChamberIndexMap; }

    private:
        double m_adcThreshold{50.};

        MdtHitChamberIdMap m_mdtChamberIdMap;
        MdtHitStationIndexMap m_mdtChamberIndexMap;

        RpcHitChamberIdMap m_rpcChamberIdMap;
        RpcHitStationIndexMap m_rpcChamberIndexMap;

        TgcHitChamberIdMap m_tgcChamberIdMap;
        TgcHitStationIndexMap m_tgcChamberIndexMap;

        CscHitChamberIdMap m_cscChamberIdMap;
        CscHitStationIndexMap m_cscChamberIndexMap;
    };

}  // namespace MuonCalib

#endif
