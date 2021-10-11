/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_MDTCHAMBERGEOMETRY_H
#define DCMATH_MDTCHAMBERGEOMETRY_H

#include <cassert>
#include <vector>

#include "TrkDriftCircleMath/ChamberGeometry.h"
#include "TrkDriftCircleMath/DriftCircle.h"
#include "TrkDriftCircleMath/Line.h"
#include "TrkDriftCircleMath/MdtStationId.h"
#include "TrkDriftCircleMath/ResidualWithLine.h"

namespace TrkDriftCircleMath {

    class MdtChamberGeometry : public ChamberGeometry {
    public:
        MdtChamberGeometry();

        MdtChamberGeometry(MdtStationId id, unsigned int nml, unsigned int nlay, unsigned int ntubesml0, unsigned int ntubesml1,
                           const LocVec2D& tube0ml0, const LocVec2D& tube0ml1, double tubeDist, double tubeStage, double layDist,
                           double stationTheta);

        virtual ~MdtChamberGeometry() = default;

        void init();
        void setGeometry(unsigned int nml, unsigned int nlay, unsigned int ntubesml0, unsigned int ntubesml1, const LocVec2D& tube0ml0,
                         const LocVec2D& tube0ml1, double tubeDist, double tubeStage, double layDist, double stationTheta);

        unsigned int nml() const { return m_nml; }
        unsigned int nlay() const override { return m_nlay; }
        unsigned int ntubesml0() const { return m_ntubesml[0]; }
        unsigned int ntubesml1() const { return m_ntubesml[1]; }

        const MdtStationId& stationId() const override { return m_id; }

        const LocVec2D& firstTubeMl0() const { return m_firstTube[0]; }
        const LocVec2D& firstTubeMl1() const { return m_firstTube[1]; }

        double tubeDist() const { return m_tubeDist; }
        double tubeStage() const { return m_tubeStage; }
        double layerDist() const { return m_layDist; }

        double stationTheta() const override { return m_stationTheta; }

        double tubeRadius() const override { return m_tubeRad; }

        const std::vector<LocVec2D>& allTubes() const override;

        LocVec2D tubePosition(unsigned int ml, unsigned int lay, unsigned int tube) const override;

        DCVec tubesPassedByLine(const Line& line, int ml) const;
        DCVec tubesPassedByLine(const Line& line) const override { return tubesPassedByLine(line, -1); }

        /** methods taking reference to output vector to reduce memory usage */
        void tubesPassedByLine(const Line& line, int ml, DCVec& crossedTubes) const;
        void tubesPassedByLine(const Line& line, DCVec& crossedTubes) const { return tubesPassedByLine(line, -1, crossedTubes); }

        /** set that this is the second multi layer instead of the first,
            used in case the first ML is dead */
        void isSecondMultiLayer(bool isSecond) { m_isSecondMultiLayer = isSecond; }

        bool validGeometry() const override { return m_validGeometry; }

        void print() const override;

        bool validId(unsigned int ml, unsigned int lay, unsigned int tube) const;

    private:
        double xPosTube(unsigned int ml, unsigned int lay, unsigned int tube) const;
        double yPosTube(unsigned int ml, unsigned int lay) const;

        MdtStationId m_id{};
        unsigned int m_nml{0};
        unsigned int m_nlay{0};
        std::vector<unsigned int> m_ntubesml{};
        double m_tubeDist{0};
        double m_tubeRad{0};
        double m_tubeStage{0};
        double m_layDist{0};
        double m_stationTheta{0};

        std::vector<bool> m_wasInit{};
        std::vector<LocVec2D> m_firstTube{};
        std::vector<LocVec2D> m_allTubes{};
        bool m_validGeometry{false};
        bool m_isSecondMultiLayer{false};
    };

}  // namespace TrkDriftCircleMath

#endif
