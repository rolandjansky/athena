/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/MdtChamberGeometry.h"

#include <MuonIdHelpers/MdtIdHelper.h>  /// maxNTubesPerLayer
#include <TString.h>                    // for Form

#include <algorithm>
#include <iostream>

#include "TrkDriftCircleMath/SortDcsByY.h"

namespace {
    constexpr int maxNTubesPerLayer = MdtIdHelper::maxNTubesPerLayer;
}
namespace TrkDriftCircleMath {

    MdtChamberGeometry::MdtChamberGeometry() { init(); }

    MdtChamberGeometry::MdtChamberGeometry(MdtStationId id, unsigned int nml, unsigned int nlay, unsigned int ntubesml0,
                                           unsigned int ntubesml1, const LocVec2D& tube0ml0, const LocVec2D& tube0ml1, double tubeDist,
                                           double tubeStage, double layDist, double stationTheta) :
        m_id{id} {
        init();
        setGeometry(nml, nlay, ntubesml0, ntubesml1, tube0ml0, tube0ml1, tubeDist, tubeStage, layDist, stationTheta);
    }

    void MdtChamberGeometry::setGeometry(unsigned int nml, unsigned int nlay, unsigned int ntubesml0, unsigned int ntubesml1,
                                         const LocVec2D& tube0ml0, const LocVec2D& tube0ml1, double tubeDist, double tubeStage,
                                         double layDist, double stationTheta) {
        m_nml = nml;
        m_nlay = nlay;
        m_ntubesml.clear();
        m_ntubesml.push_back(ntubesml0);
        m_ntubesml.push_back(ntubesml1);
        m_tubeDist = tubeDist;
        m_tubeStage = tubeStage;
        m_layDist = layDist;

        m_firstTube[0] = tube0ml0;
        m_firstTube[1] = tube0ml1;

        m_allTubes.clear();
        if (m_nml < 1 || m_nml > 2)
            throw std::runtime_error(
                Form("File: %s, Line: %d\nMdtChamberGeometry::setGeometry() - got called with nml=%d which is definitely out of range",
                     __FILE__, __LINE__, m_nml));
        if (m_nlay < 1 || m_nlay > 4)
            throw std::runtime_error(
                Form("File: %s, Line: %d\nMdtChamberGeometry::setGeometry() - got called with nlay=%d which is definitely out of range",
                     __FILE__, __LINE__, m_nlay));
        if (ntubesml0 < 1 || ntubesml0 > maxNTubesPerLayer)
            throw std::runtime_error(Form(
                "File: %s, Line: %d\nMdtChamberGeometry::setGeometry() - got called with ntubesml0=%d which is definitely out of range",
                __FILE__, __LINE__, ntubesml0));
        // there can be chambers with only 1 multilayer. Then, the second multilayer will have ntubesml1=0
        if (ntubesml1 > maxNTubesPerLayer)
            throw std::runtime_error(Form(
                "File: %s, Line: %d\nMdtChamberGeometry::setGeometry() - got called with ntubesml1=%d which is definitely out of range",
                __FILE__, __LINE__, ntubesml1));

        // create vector with all tubes in chamber
        for (unsigned int ml = 0; ml < m_nml; ++ml) {
            if (!m_wasInit[ml]) continue;
            for (unsigned int lay = 0; lay < m_nlay; ++lay) {
                for (unsigned int tube = 0; tube < m_ntubesml[ml]; ++tube) { m_allTubes.emplace_back(tubePosition(ml, lay, tube)); }
            }
        }

        m_stationTheta = stationTheta;
    }

    void MdtChamberGeometry::init() {
        m_validGeometry = true;
        if (m_id.isSmallMdt()) {
            m_tubeDist = 15.10;
            m_tubeRad = 7.1;
            m_layDist = 13.085;
        } else {
            m_tubeDist = 30.035;
            m_tubeRad = 14.6;
            m_layDist = 26.011;
        }
        // initialize first tubes to zero
        m_ntubesml.push_back(0);
        m_ntubesml.push_back(0);
        m_firstTube.emplace_back(0., 0.);
        m_firstTube.emplace_back(0., 0.);
        m_wasInit.push_back(true);
        m_wasInit.push_back(true);
    }

    bool MdtChamberGeometry::validId(unsigned int ml, unsigned int lay, unsigned int tube) const {
        if (!m_validGeometry) return false;
        if (ml > 1) {
            std::cout << " ERROR wrong index: ml " << ml << " max " << m_nml << std::endl;
            print();
            return false;
        } else if (lay > m_nlay) {
            std::cout << " ERROR wrong index: lay " << lay << " max " << m_nlay << std::endl;
            print();
            return false;
        } else if (tube > m_ntubesml[ml]) {
            std::cout << " ERROR wrong index: tube " << tube << " max " << m_ntubesml[ml] << std::endl;
            print();
            return false;
        }
        return true;
    }

    const std::vector<LocVec2D>& MdtChamberGeometry::allTubes() const { return m_allTubes; }

    void MdtChamberGeometry::tubesPassedByLine(const Line& line, int inMultilayer, DCVec& crossedTubes) const {
        crossedTubes.reserve(50);
        ResidualWithLine resLine{line};
        const LocVec2D& linepos = line.position();
        const LocVec2D& linedir = line.direction();
        double dxdy = std::abs(linedir.y()) > 0.0001 ? linedir.x() / linedir.y() : linedir.x() / 0.0001;
        for (unsigned int ml = 0; ml < m_nml; ++ml) {
            // check whether geometry was initialized for given multilayer
            if (!m_wasInit[ml]) continue;

            // if indicated only scan single multilayer
            if (inMultilayer != -1 && inMultilayer != (int)ml) { continue; }
            for (unsigned int lay = 0; lay < m_nlay; ++lay) {
                double ylay = yPosTube(ml, lay);
                double xfirsttube = xPosTube(ml, lay, 0);
                double xintersect = dxdy * (ylay - linepos.y()) + linepos.x();
                double relpos = (xintersect - xfirsttube) / m_tubeDist;
                int ctube = (int)relpos;
                if (ctube < 0) ctube = 0;
                if (ctube >= (int)m_ntubesml[ml]) ctube = m_ntubesml[ml] - 1;

                if (inMultilayer != -1)

                    for (int i = ctube - 1; i >= 0; --i) {
                        const LocVec2D& lp = tubePosition(ml, lay, i);
                        double res = resLine.residual(lp);
                        if (std::abs(res) > m_tubeRad) {
                            if (m_tubeDist > 0) {
                                if (res > m_tubeRad) break;
                            } else {
                                if (res < -m_tubeRad) break;
                            }
                        } else {
                            // if this is a chamber with only the second ml, set the ml index accordingly
                            unsigned int actualMl = m_isSecondMultiLayer ? 1 : ml;
                            crossedTubes.emplace_back(lp, m_tubeRad, res, DriftCircle::EmptyTube, MdtId(m_id.isBarrel(), actualMl, lay, i),
                                                      0);
                        }
                    }
                for (int i = ctube; i < (int)m_ntubesml[ml]; ++i) {
                    const LocVec2D& lp = tubePosition(ml, lay, i);
                    double res = resLine.residual(lp);
                    if (std::abs(res) > m_tubeRad) {
                        if (m_tubeDist > 0) {
                            if (res < -m_tubeRad) break;
                        } else {
                            if (res > m_tubeRad) break;
                        }
                    } else {
                        unsigned int actualMl = m_isSecondMultiLayer ? 1 : ml;
                        crossedTubes.emplace_back(lp, m_tubeRad, res, DriftCircle::EmptyTube, MdtId(m_id.isBarrel(), actualMl, lay, i), 0);
                    }
                }
            }
        }
    }

    DCVec MdtChamberGeometry::tubesPassedByLine(const Line& line, int inMultilayer) const {
        DCVec crossedTubes;

        if (!m_validGeometry) return crossedTubes;

        tubesPassedByLine(line, inMultilayer, crossedTubes);

        std::stable_sort(crossedTubes.begin(), crossedTubes.end(), SortDcsByY());
        return crossedTubes;
    }
    void MdtChamberGeometry::print() const {
        std::cout << " MdtChamberGeometry " << m_id << std::endl;
        std::cout << "  nml " << m_nml << " nlay " << m_nlay << " ntube1 " << m_ntubesml[0] << " ntube2 " << m_ntubesml[1] << std::endl;
        std::cout << " pos ml1 " << m_firstTube[0] << "  ml2 " << m_firstTube[1] << std::endl;
        std::cout << " tubeDist " << m_tubeDist << " tubeStage " << m_tubeStage << " layDist " << m_layDist << " tubeRad " << m_tubeRad
                  << std::endl;
    }

    LocVec2D MdtChamberGeometry::tubePosition(unsigned int ml, unsigned int lay, unsigned int tube) const {
        if (!validId(ml, lay, tube)) {
            throw std::runtime_error(Form("%s:%d -- Invalid combination of multilayer ml: %u, layer: %u and tube: %d given: ", __FILE__,
                                          __LINE__, ml, lay, tube));
        }
        LocVec2D tube_vec{xPosTube(ml, lay, tube), yPosTube(ml, lay)};
        return tube_vec;
    }

    inline double MdtChamberGeometry::xPosTube(unsigned int ml, unsigned int lay, unsigned int tube) const {
        double xpos = tube * m_tubeDist + m_firstTube[ml].x();

        if (lay % 2 == 1) {
            if (m_nlay == 4 && ml == 1)
                xpos -= m_tubeStage;
            else
                xpos += m_tubeStage;
        }

        return xpos;
    }

    double MdtChamberGeometry::yPosTube(unsigned int ml, unsigned int lay) const { return lay * m_layDist + m_firstTube[ml].y(); }

}  // namespace TrkDriftCircleMath
