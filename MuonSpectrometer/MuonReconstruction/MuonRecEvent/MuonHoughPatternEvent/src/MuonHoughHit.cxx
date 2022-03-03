/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHoughPatternEvent/MuonHoughHit.h"

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonHoughPatternEvent/MuonHoughMathUtils.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkSurfaces/Surface.h"

MuonHoughHit::MuonHoughHit(const Trk::PrepRawData* prd) {
    const Trk::TrkDetElementBase* element = prd->detectorElement();
    const Identifier identifier = prd->identify();

    m_pos = element->surface(identifier).Trk::Surface::localToGlobal(prd->localPosition());

    m_radius = m_pos.perp();
    m_abs = m_pos.mag();
    m_theta = std::atan2(m_radius, getHitz());
    m_phi = m_pos.phi();
    m_barrel = (std::abs(m_radius / getHitz()) < MuonHough::tan_barrel) ? 0 : 1;

    m_phi_sector = calcPhiSector();
    m_magnetic_trackratio = calcMagneticTrackRatio();
    m_prd = prd;
}

MuonHoughHit::MuonHoughHit(const Amg::Vector3D& pos_vec, bool measures_phi, MuonHough::DetectorTechnology detector_id, double prob,
                           double weight, const Trk::PrepRawData* prd, int id) :
    m_orig_weight(weight) {
    m_detector_id = detector_id;
    m_measures_phi = measures_phi;
    m_pos = pos_vec;
    m_probability = prob;
    m_weight = weight;
    m_prd = prd;
    m_id = id;

    m_radius = m_pos.perp();
    m_abs = m_pos.mag();
    m_theta = std::atan2(m_radius, getHitz());
    m_phi = m_pos.phi();
    m_barrel = (std::abs(m_radius / getHitz()) < MuonHough::tan_barrel) ? 0 : 1;
    m_phi_sector = calcPhiSector();
    m_magnetic_trackratio = calcMagneticTrackRatio();
}

std::string MuonHoughHit::getWhichDetector() const {
    MsgStream log(Athena::getMessageSvc(), "MuonHoughHit::getWhichDetector");
    std::string detector_name;
    switch (m_detector_id) {
        case MuonHough::MDT: detector_name = "MDT"; break;
        case MuonHough::CSC: detector_name = "CSC"; break;
        case MuonHough::RPC: detector_name = "RPC"; break;
        case MuonHough::TGC: detector_name = "TGC"; break;
        default:
            if (log.level() <= MSG::WARNING) log << MSG::WARNING << "MuonHoughHit:: no valid detector_id" << endmsg;
    }
    return detector_name;
}

int MuonHoughHit::calcPhiSector() const {
    double phi = m_phi;  // [-Pi , Pi]
    phi += MuonHough::half_phisector;

    if (phi < 0) phi += MuonHough::two_Pi;                                    // [ 0 , 2*Pi ]
    int sectorhit = static_cast<int>(phi / (2 * MuonHough::half_phisector));  // 0..15
    if (sectorhit == MuonHough::phisectors) sectorhit += -1;                  // could happen in rare cases
    return sectorhit;
}

double MuonHoughHit::calcMagneticTrackRatio() const {
    // for formulas see muonhoughmathutils, only here for cpu improvements
    double magnetic_trk_ratio = 0.;

    if (m_barrel) {
        // Barrel hypothesis

        if (m_radius >= MuonHough::radius_cylinder) {
            double diff_b = m_radius - MuonHough::radius_cylinder;
            magnetic_trk_ratio = diff_b * diff_b / m_abs;
        }
    } else {
        int sign = 1;
        if (getHitz() < 0) sign = -1;
        // Endcap hypothesis
        if (std::abs(getHitz()) < MuonHough::z_cylinder) {
            magnetic_trk_ratio = 0.;
        } else if (std::abs(getHitz()) < MuonHough::z_end) {
            // Inside Toroid
            double diff_e = getHitz() - sign * MuonHough::z_cylinder;
            magnetic_trk_ratio = diff_e * diff_e / m_abs;

        } else {
            // Outside Toroid
            magnetic_trk_ratio = (-MuonHough::z_magnetic_range_squared + sign * 2 * getHitz() * MuonHough::z_magnetic_range) / m_abs;
        }
    }

    return magnetic_trk_ratio;
}
