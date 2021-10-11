/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MSSurfaces.h"

#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"

namespace {
    static constexpr double rMinEI = 1700., rMinEM = 1500., rMinEO = 2200., rMinEE = 5000.;
    static constexpr double rMaxEI = 6200., rMaxEM = 12000., rMaxEO = 12000., rMaxEE = 9500.;
    static constexpr double rBI = 5000., rBM = 7000., rBO = 9500., rBE = 5000.;
    static constexpr double lenBI = 12000., lenBM = 10000., lenBO = 13000., lenBE = 12000.;

}  // namespace
MSSurfaces::MSSurfaces() : m_vec(12), m_station(12) {
    const Amg::Vector3D posEI(0., 0., 7500.), posEM(0., 0., 13500.), posEO(0., 0., 21000.), posEE(0., 0., 10000.), zero(0., 0., 0.);
    Amg::Transform3D transEIA = Amg::Transform3D(Amg::Translation3D(posEI));
    Amg::Transform3D transEMA = Amg::Transform3D(Amg::Translation3D(posEM));
    Amg::Transform3D transEOA = Amg::Transform3D(Amg::Translation3D(posEO));
    Amg::Transform3D transEEA = Amg::Transform3D(Amg::Translation3D(posEE));
    Amg::Transform3D transEIC = Amg::Transform3D(Amg::Translation3D(-posEI));
    Amg::Transform3D transEMC = Amg::Transform3D(Amg::Translation3D(-posEM));
    Amg::Transform3D transEOC = Amg::Transform3D(Amg::Translation3D(-posEO));
    Amg::Transform3D transEEC = Amg::Transform3D(Amg::Translation3D(-posEE));

    m_vec[BI] = std::make_unique<Trk::CylinderSurface>(rBI, lenBI);
    m_station[BI] = "BI";

    m_vec[BM] = std::make_unique<Trk::CylinderSurface>(rBM, lenBM);
    m_station[BM] = "BM";

    m_vec[BO] = std::make_unique<Trk::CylinderSurface>(rBO, lenBO);
    m_station[BO] = "BO";

    m_vec[BE] = std::make_unique<Trk::CylinderSurface>(rBE, lenBE);
    m_station[BE] = "BE";

    m_vec[EIA] = std::make_unique<Trk::DiscSurface>(transEIA, rMinEI, rMaxEI);
    m_station[EIA] = "EIA";

    m_vec[EMA] = std::make_unique<Trk::DiscSurface>(transEMA, rMinEM, rMaxEM);
    m_station[EMA] = "EMA";

    m_vec[EOA] = std::make_unique<Trk::DiscSurface>(transEOA, rMinEO, rMaxEO);
    m_station[EOA] = "EOA";

    m_vec[EEA] = std::make_unique<Trk::DiscSurface>(transEEA, rMinEE, rMaxEE);
    m_station[EEA] = "EEA";

    m_vec[EIC] = std::make_unique<Trk::DiscSurface>(transEIC, rMinEI, rMaxEI);
    m_station[EIC] = "EIC";

    m_vec[EMC] = std::make_unique<Trk::DiscSurface>(transEMC, rMinEM, rMaxEM);
    m_station[EMC] = "EMC";

    m_vec[EOC] = std::make_unique<Trk::DiscSurface>(transEOC, rMinEO, rMaxEO);
    m_station[EOC] = "EOC";

    m_vec[EEC] = std::make_unique<Trk::DiscSurface>(transEEC, rMinEE, rMaxEE);
    m_station[EEC] = "EEC";
}