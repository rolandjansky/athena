/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonPatternCombination.h
//   Header file for class MuonPatternCombination
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUON_MUONPATTERNSTATIONINTERSECT_H
#define MUON_MUONPATTERNSTATIONINTERSECT_H

#include "TrkExUtils/TrackSurfaceIntersection.h"
//#include "TrkEventPrimitives/GlobalPosition.h"
//#include "TrkEventPrimitives/GlobalDirection.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonPrepRawData/SortMuonPrepData.h"

namespace Trk {
    class PrepRawData;
}

#include <vector>

namespace Muon {

    /**
        @brief This class holds information needed for the Moore and MoMu pattern recognition for a muon chamber.
        It is designed to pass information of a global road search on PRDs to a segment maker
        The information includes:
        - the predicted intersection + direction of the global road with the chamber reference plane
        - a list of PrepRawData objects in the given chamber
    */
    class MuonPatternChamberIntersect {
    public:
        /** @brief constructor taking a Trk::TrackSurfaceIntersection
            @param intersect the intersect of the pattern with the chamber
            @param rios a vector of Trk::PrepRawData objects in the chamber that were associated with the pattern
        */
        MuonPatternChamberIntersect(const Trk::TrackSurfaceIntersection& intersect, const std::vector<const Trk::PrepRawData*>& rios);

        /** @brief constructor taking the global intersect position and direction as input
            @param intersect the intersect of the pattern with the chamber
            @param rios a vector of Trk::PrepRawData objects in the chamber that were associated with the pattern
        */
        MuonPatternChamberIntersect(const Amg::Vector3D& pos, const Amg::Vector3D& dir, const std::vector<const Trk::PrepRawData*>& rios);

        /** @brief intersect position in chamber */
        const Amg::Vector3D& intersectPosition() const;

        /** @brief intersect position in chamber */
        const Amg::Vector3D& intersectDirection() const;

        /** @brief reference to the Trk::TrackSurfaceIntersection */
        const Trk::TrackSurfaceIntersection& stationIntersect() const;

        /** @brief Access to the vector of associated Trk::PrepRawData */
        const std::vector<const Trk::PrepRawData*>& prepRawDataVec() const;

    private:
        // Prediction of the extrapolated position and direction of the road in the chamber
        Trk::TrackSurfaceIntersection m_intersect;

        // vector of PRDs in the chamber
        std::vector<const Trk::PrepRawData*> m_rios;
    };

    inline const Amg::Vector3D& MuonPatternChamberIntersect::intersectPosition() const { return stationIntersect().position(); }

    inline const Amg::Vector3D& MuonPatternChamberIntersect::intersectDirection() const { return stationIntersect().direction(); }

    inline const Trk::TrackSurfaceIntersection& MuonPatternChamberIntersect::stationIntersect() const { return m_intersect; }

    inline const std::vector<const Trk::PrepRawData*>& MuonPatternChamberIntersect::prepRawDataVec() const { return m_rios; }

}  // namespace Muon

#endif
