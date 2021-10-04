/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_MDTMULTICHAMBERGEOMETRY_H
#define DCMATH_MDTMULTICHAMBERGEOMETRY_H

#include <ostream>
#include <vector>

#include "TrkDriftCircleMath/ChamberGeometry.h"
#include "TrkDriftCircleMath/DriftCircle.h"
#include "TrkDriftCircleMath/Line.h"
#include "TrkDriftCircleMath/MdtChamberGeometry.h"

namespace TrkDriftCircleMath {

    class MdtMultiChamberGeometry : public ChamberGeometry {
    public:
        MdtMultiChamberGeometry();

        MdtMultiChamberGeometry(const std::vector<MdtChamberGeometry>& chambers);

        virtual ~MdtMultiChamberGeometry();

        const std::vector<LocVec2D>& allTubes() const override;

        DCVec tubesPassedByLine(const Line& line, int ml) const;
        DCVec tubesPassedByLine(const Line& line) const override { return tubesPassedByLine(line, -1); }

        unsigned int nlay() const override;

        LocVec2D tubePosition(unsigned int ml, unsigned int lay, unsigned int tube) const override;

        bool validGeometry() const override { return m_validGeometry; }

        virtual void print() const override;

        virtual double tubeRadius() const override;

        double stationTheta() const override;

        const MdtStationId& stationId() const override { return m_chambers[0].stationId(); }

        unsigned int nChambers() const { return m_chambers.size(); }

    private:
        std::vector<MdtChamberGeometry> m_chambers;
        std::vector<LocVec2D> m_allTubes;
        bool m_validGeometry{false};
    };

    inline double MdtMultiChamberGeometry::stationTheta() const {
        double theta = 0.;
        if (m_chambers.empty() || !m_validGeometry) return 0.;

        for (const MdtChamberGeometry& chit : m_chambers) theta += chit.stationTheta();
        return theta / m_chambers.size();
    }

    inline double MdtMultiChamberGeometry::tubeRadius() const {
        if (m_chambers.empty()) return 14.6;
        return m_chambers.front().tubeRadius();
    }

}  // namespace TrkDriftCircleMath

#endif
