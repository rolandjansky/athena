/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEGMENTKEY_H
#define MUON_MUONSEGMENTKEY_H

#include <set>
#include <vector>

namespace Trk {
    class MeasurementBase;
}

namespace Muon {

    class MuonSegment;
    /**
        Class to cache the identifiers on a segment in sets that can later be used to perform an
        overlap removel between two segments.

        @author Ed Moyse, Niels van Eldik
    */
    class MuonSegmentKey {
    public:
        /** The first int is the Identifier, whilst the second is the sign of the 1st measurement (i.e. drift radius)*/
        typedef std::set<std::pair<unsigned int, int> > MeasurementKeys;

        /** default constructor */
        MuonSegmentKey();

        /** constructor taking a MuonSegment */
        MuonSegmentKey(const MuonSegment& segment);

        /** constructor taking a vector of measurements */
        MuonSegmentKey(const std::vector<const Trk::MeasurementBase*>& measurements);

        /** function to calculate keys from a vector of measurements, the signCor is can be
            use to flip the signs of the MDT hits if the two segments that are compared have opposite directions */
        void calculateKeys(const std::vector<const Trk::MeasurementBase*>& measurements, int signCor = 1);

        virtual ~MuonSegmentKey();

        /** access to precision hit keys */
        const MeasurementKeys& keys() const;

        /** access to trigger hit keys */
        const MeasurementKeys& keysTrigger() const;

    private:
        MeasurementKeys m_measKeys;
        MeasurementKeys m_measKeysTrigger;
    };

}  // namespace Muon

inline const Muon::MuonSegmentKey::MeasurementKeys& Muon::MuonSegmentKey::keysTrigger() const { return m_measKeysTrigger; }

inline const Muon::MuonSegmentKey::MeasurementKeys& Muon::MuonSegmentKey::keys() const { return m_measKeys; }

#endif
