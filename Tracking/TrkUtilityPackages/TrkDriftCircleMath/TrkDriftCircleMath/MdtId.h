/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_MDTID_H
#define DCMATH_MDTID_H

#include <MuonIdHelpers/MdtIdHelper.h>

#include <ostream>

namespace TrkDriftCircleMath {

    class MdtId {
    public:
        /// the tube number of a tube in a tubeLayer in encoded in the GeoSerialIdentifier (modulo maxNTubesPerLayer)
        static constexpr int maxNTubesPerLayer = MdtIdHelper::maxNTubesPerLayer;
        static constexpr int multiLayerShift = 10000;
        static constexpr int stationShift = 100000;

        MdtId() = default;
        /// Parameters:
        ///     - isBarrel: Boolean flag whether the chamber is built into the barrel or endcap
        ///     - ml: Number of the multi layer in the chamber
        ///      -lay: Layer number inside the multilayer
        ///      -tube: Tube number in the layer
        MdtId(bool isBarrel, int ml, int lay, int tube) {
            m_id = stationShift * isBarrel + multiLayerShift * ml + maxNTubesPerLayer * lay + tube;
        }
        bool isBarrel() const { return m_id / stationShift; }
        int ml() const { return (m_id % stationShift) / multiLayerShift; }
        int lay() const { return (m_id % multiLayerShift) / maxNTubesPerLayer; }
        int tube() const { return (m_id % multiLayerShift) % maxNTubesPerLayer; }
        int value() const { return m_id; }

        bool operator==(const MdtId& other) const { return m_id == other.m_id; }
        bool operator!=(const MdtId& other) const { return !(*this == other); }

    private:
        int m_id{0};
    };

    // DR (clang32) operator<< to be defined inside the namespace
    std::ostream& operator<<(std::ostream& os, const MdtId& id);

}  // namespace TrkDriftCircleMath

#endif
