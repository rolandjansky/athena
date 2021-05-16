/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUPATSEGMENT_H
#define MUPATSEGMENT_H

#include <mutex>
#include <set>

#include "Identifier/Identifier.h"
#include "MuPatPrimitives/MuPatCandidateBase.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentQuality.h"
#include "MuonStationIndex/MuonStationIndex.h"
#include "TrkParameters/TrackParameters.h"

namespace Muon {

    class MuPatTrack;

    /**
        @brief segment candidate object.
        The purpose of the segment candidate is three folded:
        - provide the generic MuPatCandidateBase interface for tracks
        - keep track of tracks the segment is accociated to
        - cache additional information that cannot be stored on the track

        The following information is cached:
        - the segment quality integer calculated by the MuonSegmentSelectionTool
        - the MuonSegmentQuality
        - a AtaPlane that represents the segment and allows extrapolation
        - the index of the segment in the order list of segments in the track steering cache
        - an integer (usedInFit) showing on how many track candidates the segment is included
        - the identifier of the chamber with most hits on the segment
        - a string summarizing the segment that can be used for dumping it to screen
        - a chamber index
        - a station index
        - a flag indicating whether the segment is in the barrel or endcap
        - a flag indicating whether the segment contains MDT hits
     */

    class MuPatSegment : public MuPatCandidateBase {
    public:
        MuPatSegment();

        ~MuPatSegment();

        MuPatSegment(const MuPatSegment& info);

        MuPatSegment& operator=(const MuPatSegment& info);

        int quality{0};
        const MuonSegmentQuality* segQuality{nullptr};
        const MuonSegment* segment{nullptr};
        std::shared_ptr<const Trk::AtaPlane> segPars{nullptr};
        int segmentIndex{-1};  //!< index of segment within station
        int usedInFit{0};
        Identifier chid{0};
        std::string name{};
        MuonStationIndex::ChIndex chIndex{MuonStationIndex::ChUnknown};
        MuonStationIndex::StIndex stIndex{MuonStationIndex::StUnknown};
        bool isEndcap{false};
        bool isMdt{false};  //!< true for MDT, false for CSC

        /** @brief returns first track parameters */
        const Trk::TrackParameters& entryPars() const;

        /** @brief reset the maximum number of objects counter */
        static void resetMaxNumberOfInstantiations();

        /** @brief maximum number of objects of this type in memory */
        static unsigned int maxNumberOfInstantiations();

        /** current number of objects of this type in memory */
        static unsigned int numberOfInstantiations();

        /** @brief reset the copy constructor counter */
        static void resetNumberOfCopies();

        /** @brief number of times the copy constructor was called since last reset */
        static unsigned int numberOfCopies();

        /** @brief add a new track to the segment */
        void addTrack(MuPatTrack*);

        /** @brief remove a track from the segment */
        void removeTrack(MuPatTrack*);

        /** access to the tracks the segment is associated with */
        const std::set<MuPatTrack*>& tracks() const { return m_associatedTracks; }

    private:
        /** @brief Keeping track of number of object instances */
        static void addInstance();

        /** @brief Keeping track of number of object instances */
        static void removeInstance();

        static unsigned int s_numberOfInstantiations ATLAS_THREAD_SAFE;     //<! current number of objects of this type in memory
        static unsigned int s_maxNumberOfInstantiations ATLAS_THREAD_SAFE;  //<! maximum number of objects of this type in memory
        static unsigned int s_numberOfCopies ATLAS_THREAD_SAFE;  //<! number of times the copy constructor was called since last reset
        static std::mutex s_mutex;                               //<! to remove race condition in addInstance()

        std::set<MuPatTrack*> m_associatedTracks;  //<! list of associated tracks

    };  // class MuPatSegment

    //
    // static inline functions implementations
    //
    inline unsigned int MuPatSegment::maxNumberOfInstantiations() {
        const std::lock_guard<std::mutex> lock(s_mutex);

        return s_maxNumberOfInstantiations;
    }

    inline unsigned int MuPatSegment::numberOfCopies() {
        const std::lock_guard<std::mutex> lock(s_mutex);

        return s_numberOfCopies;
    }

    inline void MuPatSegment::addInstance() {
        const std::lock_guard<std::mutex> lock(s_mutex);

        ++s_numberOfInstantiations;
        if (s_numberOfInstantiations > s_maxNumberOfInstantiations) { s_maxNumberOfInstantiations = s_numberOfInstantiations; }
    }

    inline void MuPatSegment::removeInstance() {
        const std::lock_guard<std::mutex> lock(s_mutex);

        if (s_numberOfInstantiations > 0) --s_numberOfInstantiations;
    }

    inline void MuPatSegment::resetMaxNumberOfInstantiations() {
        const std::lock_guard<std::mutex> lock(s_mutex);

        s_maxNumberOfInstantiations = 0;
    }

    inline void MuPatSegment::resetNumberOfCopies() {
        const std::lock_guard<std::mutex> lock(s_mutex);

        s_numberOfCopies = 0;
    }

    //
    // inline member functions implementations
    //
    inline const Trk::TrackParameters& MuPatSegment::entryPars() const { return *segPars; }

    struct SortSegInfoByR {
        bool operator()(const MuPatSegment* c1, const MuPatSegment* c2) { return operator()(*c1, *c2); }
        bool operator()(const MuPatSegment& c1, const MuPatSegment& c2) {
            return c1.segment->globalPosition().perp() < c2.segment->globalPosition().perp();
        }
    };
    struct SortSegInfoByZ {
        bool operator()(const MuPatSegment* c1, const MuPatSegment* c2) { return operator()(*c1, *c2); }
        bool operator()(const MuPatSegment& c1, const MuPatSegment& c2) {
            return fabs(c1.segment->globalPosition().z()) < fabs(c2.segment->globalPosition().z());
        }
    };
    struct SortSegInfoByQuality {
        bool operator()(const MuPatSegment* c1, const MuPatSegment* c2) { return operator()(*c1, *c2); }
        bool operator()(const MuPatSegment& c1, const MuPatSegment& c2) { return c1.quality > c2.quality; }
    };

}  // namespace Muon

#endif
