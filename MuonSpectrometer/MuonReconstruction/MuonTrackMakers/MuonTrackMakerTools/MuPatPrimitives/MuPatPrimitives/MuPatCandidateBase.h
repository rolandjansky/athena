/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUPATCANDIDATEBASE_H
#define MUPATCANDIDATEBASE_H

#include <set>
#include <vector>

#include "Identifier/Identifier.h"
#include "MuPatPrimitives/MuPatHit.h"
#include "MuonStationIndex/MuonStationIndex.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {
    class MeasurementBase;
}

namespace Muon {

    /**
        @brief track candidate entry object.

        The purpose of the class is to provide a common interface for tracks and segments.
        It give access to:
        - the eta hits
        - the phi hits
        - the fake phi hits (PseudoMeasurements)
        - the all hits
        - the MuPatHitList (order listed used to sort the hits)
        - a set with hamber Identifier on the track
        - a set of chamber layers on the track
        - a set of station layers on the track
        - a flag whether the track/segment has a measured momentum
        - number of mdt hits in the first multi layer
        - number of mdt hits in the second multi layer
        - number of csc eta hits
        - number of csc phi hits
        - number of rpc eta hits
        - number of rpc phi hits
        - number of tgc eta hits
        - number of tgc phi hits
        - a flag whether the candidate has endcap hits
        - a flag whether the candidate has small chambers
        - a flag whether the candidate has large chambers
        - a flag whether the candidate has a small/large overlap

     */
    class MuPatCandidateBase {
        friend class MuPatCandidateTool;

    public:
        typedef std::vector<const Trk::MeasurementBase*> MeasVec;

        typedef std::set<MuonStationIndex::ChIndex>::iterator chamber_iterator;
        typedef std::set<MuonStationIndex::ChIndex>::const_iterator const_chamber_iterator;

        typedef std::set<MuonStationIndex::StIndex>::iterator station_iterator;
        typedef std::set<MuonStationIndex::StIndex>::const_iterator const_station_iterator;

    public:
        /** @brief constructor. */
        MuPatCandidateBase() = default;

        /** @brief constructor. The constructor takes ownership of the entryPars but not of the hits. */
        MuPatCandidateBase(const MeasVec& etaHits, const MeasVec& phiHits, const MeasVec& fakePhiHits, const MeasVec& allHits);

        /** @brief destructor. */
        virtual ~MuPatCandidateBase() = default;

        /** @brief return all eta hits on the entry */
        const MeasVec& etaHits() const;

        /** @brief return all phi hits on the entry */
        const MeasVec& phiHits() const;

        /** @brief return all fake phi hits on the entry */
        const MeasVec& fakePhiHits() const;

        /** @brief return all hits on the entry. (Does not including the fake phi hits) */
        const MeasVec& hits() const;

        /** @brief returns whether entry contains a small chamber */
        bool hasSmallChamber() const;

        /** @brief returns whether entry contains a small chamber */
        bool hasLargeChamber() const;

        /** @brief returns whether there is at least one small/large overlap in the same station layer */
        bool hasSLOverlap() const;

        /** @brief return track parameters representing the entry */
        virtual const Trk::TrackParameters& entryPars() const = 0;

        /** @brief returns whether the entry contains endcap hits */
        bool hasEndcap() const;

        /** @brief set whether entry has endcap hits */
        void hasEndcap(bool hasEC);

        /** @brief returns whether entry has a momentum measurement */
        bool hasMomentum() const;

        /** @brief returns set with contained stationIndices */
        const std::set<MuonStationIndex::StIndex>& stations() const;

        /** @brief returns set with contained chamberIndices */
        const std::set<MuonStationIndex::ChIndex>& chambers() const;

        /** @brief returns whether the ChamberIndex is already contained in candidate */
        bool containsChamber(MuonStationIndex::ChIndex chIndex) const;

        /** @brief returns whether the StationIndex is already contained in candidate */
        bool containsStation(MuonStationIndex::StIndex chIndex) const;

        /** @brief returns a reference to the hit list */
        const MuPatHitList& hitList() const { return m_hitList; }

        /** @brief returns set with contained chamber ids */
        const std::set<Identifier>& chamberIds() const;

        /** @brief checks whether the two entries contain the same chamber */
        bool shareChambers(const MuPatCandidateBase& entry) const;

    protected:
        // all non-const functions protected, only accessible by derived classes and MCTBEntryHandler and MuPatHitHandler

        /** @brief set whether entry has at least one small/large overlap in the same station layer */
        void hasSLOverlap(bool hasSL);

        /** @brief set whether entry has small chamber */
        void hasSmallChamber(bool hasSmall);

        /** @brief set whether entry has large chamber */
        void hasLargeChamber(bool hasLarge);

        /** @brief set eta hits on the entry */
        void setEtaHits(const MeasVec& hits);

        /** @brief set phi hits on the entry */
        void setPhiHits(const MeasVec& hits);

        /** @brief set fake phi hits on the entry */
        void setFakePhiHits(const MeasVec& hits);

        /** @brief set all hits on the entry */
        void setAllHits(const MeasVec& hits);

        /** @brief returns a reference to the hit list */
        MuPatHitList& hitList() { return m_hitList; }

        /** @brief returns set with contained chamber ids */
        std::set<Identifier>& chamberIds();

        /** @brief clear the list of contained chambers. Will also clear the list of contained stations and list of chamber Ids. */
        void clearChambers();

        /** @brief add the chamber to the list of contained chambers. Will also add the corresponding station. */
        void addChamber(MuonStationIndex::ChIndex chIndex);

        /** Set the list of chambers and update list of stations */
        void setChambers(const std::set<MuonStationIndex::ChIndex>& chambers);

    private:
        MeasVec m_etaHits;      //<! vector containing all eta measurements
        MeasVec m_phiHits;      //<! vector containing all phi measurements
        MeasVec m_fakePhiHits;  //<! vector containing all fake phi measurements
        MeasVec m_allHits;      //<! vector containing all measurements (except fake phi measurements)

        MuPatHitList m_hitList;

        std::set<Identifier> m_chamberIds;  //<! set to store chamber Ids of hits associated with candidate

    protected:
        std::set<MuonStationIndex::ChIndex> m_chambers;  //<! set to store chamber indices of segments associated with candidate
        std::set<MuonStationIndex::StIndex> m_stations;  //<! set to store station indices of segments associated with candidate
        bool m_hasMomentum{false};

    public:
        /** public hit counters */
        unsigned int nmdtHitsMl1{0};
        unsigned int nmdtHitsMl2{0};
        unsigned int ncscHitsEta{0};
        unsigned int ncscHitsPhi{0};
        unsigned int nrpcHitsEta{0};
        unsigned int nrpcHitsPhi{0};
        unsigned int ntgcHitsEta{0};
        unsigned int ntgcHitsPhi{0};

    private:
        bool m_hasEndcap{false};
        bool m_hasSmallChamber{false};
        bool m_hasLargeChamber{false};
        bool m_hasSLOverlap{false};
    };

}  // namespace Muon

#endif
