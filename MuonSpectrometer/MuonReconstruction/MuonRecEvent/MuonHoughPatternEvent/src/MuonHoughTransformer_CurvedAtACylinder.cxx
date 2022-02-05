/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHoughPatternEvent/MuonHoughTransformer_CurvedAtACylinder.h"

#include "AthenaKernel/getMessageSvc.h"
#include "CxxUtils/sincos.h"
#include "GaudiKernel/MsgStream.h"

MuonHoughTransformer_CurvedAtACylinder::MuonHoughTransformer_CurvedAtACylinder(int nbins, int nbins_angle, double detectorsize,
                                                                               double detectorsize_angle, double threshold_histo,
                                                                               int number_of_sectors) :
    MuonHoughTransformer(nbins, nbins_angle, detectorsize, detectorsize_angle, threshold_histo, number_of_sectors) {
    m_add_weight_radius = false;
    m_weight_constant_radius = 0.;
    m_add_weight_angle = false;

    // fill array with scanned curvatures

    m_invcurvature = new double[m_nbins / 2];
    m_weightcurvature = new double[m_nbins / 2];

    for (int i = 0; i < m_nbins / 2; i++) {
        double x0 = -i + 24;                      // 24 ... -55
        if (x0 > 15.) x0 = 15 + (x0 - 15) * 0.5;  // 19.5,19,18.5, .., 15.5,15,14,13,.. 2,1,0,-1...-55  // 80 bins

        double curvature = -3. * 3500. * 20.25 / (x0 - 19.75);  // >0
        m_invcurvature[i] = 1. / curvature;
        if (i <= 10) {
            m_weightcurvature[i] = 1.;
        } else if (i <= 20) {
            m_weightcurvature[i] = 1 - 0.05 * (i - 10);
        } else {
            m_weightcurvature[i] = 0.5;
        }
    }
}

MuonHoughTransformer_CurvedAtACylinder::~MuonHoughTransformer_CurvedAtACylinder() {
    delete[] m_invcurvature;
    delete[] m_weightcurvature;
}

void MuonHoughTransformer_CurvedAtACylinder::fillHit(MuonHoughHit* hit, double weight) {
    // MuonHough transform studies

    // cylinder for extrapolation planes in muon system endcap/barrel transition

    int sectorhit = sector(hit);

    bool isbarrel = hit->isBarrel();

    for (int i = 0; i < m_nbins / 2; i++) {
        // not a too large curve for endcap hits (generates peaks in houghtransform)
        const double ratio = hit->getMagneticTrackRatio() * m_invcurvature[i];
        if (!isbarrel && std::abs(ratio) > 0.5) break;

        // positive curvature (for positive i):
        double thetas[2];
        MuonHoughMathUtils::thetasForCurvedHit(ratio, hit, thetas[0], thetas[1]);

        const double weight_curvature =
            weight * 1. /
            (1. +
             m_eventsize_weightfactor *
                 (thetas[0] -
                  hit->getTheta()));  //* m_weightcurvature[i]; // m_eventsize_weightfactor is defined in MuonHoughTransformer::fill(event)

        // Remove theta solutions outside 0 - 180 degree range
        if (thetas[0] > 0. && thetas[0] < M_PI) {
            double theta_in_grad = thetas[0] * MuonHough::rad_degree_conversion_factor;

            const double weight_curvature_theta = weight_curvature * (0.5 + 0.5 * std::sin(thetas[0]));

            fillHisto(i + 0.5, theta_in_grad, weight_curvature_theta, 2 * sectorhit);  // overlap and single sector filling
        }

        // negative curvature (for negative i)

        // Remove theta solutions outside 0 - 180 degree range
        if (thetas[1] > 0. && thetas[1] < M_PI) {
            double theta_in_grad = thetas[1] * MuonHough::rad_degree_conversion_factor;

            const double weight_curvature_theta = weight_curvature * (0.5 + 0.5 * std::sin(thetas[1]));
            fillHisto(-i - 0.5, theta_in_grad, weight_curvature_theta, 2 * sectorhit);  // overlap and single sector filling
        }
    }
}

int MuonHoughTransformer_CurvedAtACylinder::fillHisto(double xbin, double theta_in_grad, double weight, int sector) {
    MuonHoughHisto2D* histo = m_histos.getHisto(sector);

    const int filled_binnumber = histo->fill(xbin, theta_in_grad, weight);

    // overlap filling:
    // overlap and single sector filling:
    // nearby sectors:
    if (m_number_of_sectors >= 3) {
        const double reduced_weight = 0.8 * weight;  // arbitrary should be between 0.5 and 1
        if (sector != 0 && sector != m_number_of_sectors - 1) {
            m_histos.getHisto(sector + 1)->fill(filled_binnumber, reduced_weight);
            m_histos.getHisto(sector - 1)->fill(filled_binnumber, reduced_weight);
        } else if (sector == 0) {
            m_histos.getHisto(sector + 1)->fill(filled_binnumber, reduced_weight);
            m_histos.getHisto(m_number_of_sectors - 1)->fill(filled_binnumber, reduced_weight);
        } else {
            m_histos.getHisto(sector - 1)->fill(filled_binnumber, reduced_weight);
            m_histos.getHisto(0)->fill(filled_binnumber, reduced_weight);
        }
    }

    // smearing effect:

    const double fifth_weight = 0.2 * weight;
    const int upperright = filled_binnumber + m_nbins_plus3;
    const int lowerleft = filled_binnumber - m_nbins_plus3;

    if (theta_in_grad - m_binwidthy < 0) {
        histo->fill(upperright, fifth_weight);

        if (m_use_negative_weights) { histo->fill(upperright - 2, -fifth_weight); }
    } else if (theta_in_grad + m_binwidthy > 180.) {
        histo->fill(lowerleft, fifth_weight);
        if (m_use_negative_weights) { histo->fill(xbin + m_binwidthx, theta_in_grad - m_binwidthy, -fifth_weight); }
    } else {
        histo->fill(upperright, fifth_weight);
        histo->fill(lowerleft, fifth_weight);
        if (m_use_negative_weights) {
            histo->fill(upperright - 2, -fifth_weight);
            histo->fill(lowerleft + 2, -fifth_weight);
        }
    }
    return filled_binnumber;
}

MuonHoughPattern* MuonHoughTransformer_CurvedAtACylinder::hookAssociateHitsToMaximum(const MuonHoughHitContainer* event,
                                                                                     std::pair<double, double> coordsmaximum,
                                                                                     double max_residu_mm, double /*max_residu_grad */,
                                                                                     int maxsector, bool /*which_segment */,
                                                                                     int printlevel) const {
    MsgStream log(Athena::getMessageSvc(), "MuonHoughTransformer_CurvedAtACylinder::hookAssociateHitsToMaximum");
    MuonHoughPattern* houghpattern = new MuonHoughPattern(MuonHough::hough_curved_at_a_cylinder);

    // double ecurvature=0. // todo: recalculate estimated curvature for hits associated to maximum
    double etheta = 0., sin_phi = 0., cos_phi = 0., sin_theta = 0., cos_theta = 0., ephi = 0.;
    const double theta = m_muonhoughmathutils.angleFromGradToRadial(coordsmaximum.second);

    double invcurvature = 0.;
    double curvature = 0.;
    if (m_nbins < 40) {
        curvature = MuonHoughMathUtils::sgn(coordsmaximum.first) / (coordsmaximum.first * coordsmaximum.first);
        invcurvature = 1. / curvature;
    } else {
        // coordsmaximum.first = -80.5 .. 80.5 // 160 bins
        int index = static_cast<int>(std::floor(std::abs(coordsmaximum.first)));
        if (index >= m_nbins / 2) {
            index = m_nbins / 2 - 1;
            if (printlevel >= 4 || log.level() <= MSG::VERBOSE) log << MSG::VERBOSE << "warning overflow maximum found" << endmsg;
        }
        invcurvature = MuonHoughMathUtils::sgn(coordsmaximum.first) * m_invcurvature[index];
        curvature = 1. / invcurvature;
    }

    // allowed sectors:
    int sector_1 = maxsector / 2;  // primary sector
    int sector_2 = sector_1 + 1;
    int sector_3 = sector_1 - 1;
    if (sector_2 > m_number_of_sectors / 2 - 1) { sector_2 = 0; }  // if sector_2 larger than 15 -> 0
    if (maxsector % 2 == 1) {                                      // if overlap maximum then only association to 2 sectors
        sector_3 = sector_1;
    } else if (sector_3 < 0) {
        sector_3 = m_number_of_sectors / 2 - 1;
    }  // if sector_3 smaller than 0 -> 15

    if (printlevel >= 4 || log.level() <= MSG::VERBOSE) {
        log << MSG::VERBOSE << "sector: " << maxsector << endmsg;
        log << MSG::VERBOSE << "coordsmaximumfirst: " << coordsmaximum.first << " curvature: " << curvature << endmsg;
        log << MSG::VERBOSE << "coordsmaximumsecond: " << coordsmaximum.second << " coordsmaximumsecondinrad: " << theta << endmsg;
        log << MSG::VERBOSE << "MuonHoughTransformer_CurvedAtACylinder::size of event: " << event->size() << endmsg;
        log << MSG::VERBOSE << "allowed sectors: " << sector_1 << " , " << sector_2 << " & " << sector_3 << endmsg;
    }

    for (unsigned int i = 0; i < event->size(); i++) {
        MuonHoughHit* hit = event->getHit(i);
        int sectorhit = sector(hit);
        if (sectorhit == sector_1 || sectorhit == sector_2 || sectorhit == sector_3) {
            double z0 = 0.;  // offset from IP on z-axis
            const double sdis = MuonHoughMathUtils::signedDistanceCurvedToHit(z0, theta, invcurvature, hit->getPosition());

            double radius3d = hit->getAbs();
            if (radius3d < 5000.)
                radius3d = 5000.;
            else if (radius3d > 15000.)
                radius3d = 15000.;
            double scale = radius3d / 5000.;

            double residu_distance_mm = std::abs(sdis);

            if (printlevel >= 4 || log.level() <= MSG::VERBOSE) {
                log << MSG::VERBOSE << "MuonHoughTransformer_CurvedAtACylinder:: " << hit->getPosition() << endmsg;
                log << MSG::VERBOSE << "MuonHoughTransformer_CurvedAtACylinder::residu_distance: " << sdis
                    << " max_residu_mm*scale: " << max_residu_mm * scale << endmsg;
            }

            if (std::abs(residu_distance_mm) < max_residu_mm * scale)  // here no circular effect
            {
                double phi = hit->getPhi();
                CxxUtils::sincos scphi(phi);
                sin_phi += scphi.sn;
                cos_phi += scphi.cs;

                const double theta = MuonHoughMathUtils::thetaForCurvedHit(invcurvature, event->getHit(i));
                if (theta > 0 && theta < M_PI) {
                    if (printlevel >= 4 || log.level() <= MSG::VERBOSE) {
                        log << MSG::VERBOSE
                            << "MuonHoughTransformer_CurvedAtACylinder::hit added to houghpattern! Sector number: " << sectorhit << endmsg;
                        if (event->getHit(i)->getAssociated())
                            log << MSG::VERBOSE << " hit already earlier associated to pattern!" << endmsg;
                    }
                    houghpattern->addHit(event->getHit(i));
                    event->getHit(i)->setAssociated(true);
                    CxxUtils::sincos sctheta(theta);
                    sin_theta += sctheta.sn;
                    cos_theta += sctheta.cs;
                }
            }  // residu
        }      // sector constraint
    }          // hitno

    etheta = std::atan2(sin_theta, cos_theta);
    //  etheta = theta;
    ephi = std::atan2(sin_phi, cos_phi);
    houghpattern->setETheta(etheta);
    houghpattern->setERTheta(0.);
    houghpattern->setEPhi(ephi);
    houghpattern->setECurvature(curvature);

    if (printlevel >= 4 || log.level() <= MSG::VERBOSE) {
        log << MSG::VERBOSE << " number of hits added to pattern: " << houghpattern->size() << endmsg;
    }
    if (houghpattern->empty() && (printlevel >= 1 || log.level() <= MSG::WARNING)) {
        log << MSG::WARNING << "MuonHoughTransformer_CurvedAtACylinder::WARNING : no hits found on pattern" << endmsg;
    }

    return houghpattern;
}

float MuonHoughTransformer_CurvedAtACylinder::weightHoughTransform(double /*r0*/) const {
    // not in use for this transform
    return 1.;
}
