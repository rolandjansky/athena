/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHoughPatternEvent/MuonHoughTransformer_rzcosmics.h"

#include "AthenaKernel/getMessageSvc.h"
#include "CxxUtils/sincos.h"
#include "GaudiKernel/MsgStream.h"

MuonHoughTransformer_rzcosmics::MuonHoughTransformer_rzcosmics(int nbins, int nbins_angle, double detectorsize, double detectorsize_angle,
                                                               double threshold_histo, int number_of_sectors) :
    MuonHoughTransformer(nbins, nbins_angle, detectorsize, detectorsize_angle, threshold_histo, number_of_sectors) {
    m_add_weight_radius = true;
    m_weight_constant_radius = 0.3;  // 1./(1 + m_weight_constant_radius*std::abs(r0)/m_detectorsize) = 1/(1+10^-5*r)
    m_add_weight_angle = true;

    m_phisec = new double[m_number_of_sectors];
    m_sinphisec = new double[m_number_of_sectors];
    m_cosphisec = new double[m_number_of_sectors];

    for (int phisector = 0; phisector < m_number_of_sectors; phisector++) {
        m_phisec[phisector] = (phisector + 0.5) * M_PI / (m_number_of_sectors + 0.) - M_PI;  // phi [-Pi,0]
        CxxUtils::sincos sc(m_phisec[phisector]);
        m_sinphisec[phisector] = sc.sn;
        m_cosphisec[phisector] = sc.cs;
    }

    m_theta_in_grad = new double[m_nbins_angle];
    m_sintheta = new double[m_nbins_angle];
    m_costheta = new double[m_nbins_angle];

    for (int i = 0; i < m_nbins_angle; i++) {
        m_theta_in_grad[i] = (i + 0.5) * m_stepsize_per_angle;
        const double theta_in_rad = MuonHough::degree_rad_conversion_factor * m_theta_in_grad[i];
        CxxUtils::sincos sc(theta_in_rad);
        m_sintheta[i] = sc.sn;
        m_costheta[i] = sc.cs;
    }
}

MuonHoughTransformer_rzcosmics::~MuonHoughTransformer_rzcosmics() {
    delete[] m_phisec;
    delete[] m_sinphisec;
    delete[] m_cosphisec;
    delete[] m_theta_in_grad;
    delete[] m_sintheta;
    delete[] m_costheta;
}

void MuonHoughTransformer_rzcosmics::fillHit(MuonHoughHit* hit, double weight) {
    const double invradius = 1. / hit->getRadius();
    const double hitx = hit->getHitx();
    const double hity = hit->getHity();
    const double hitz = hit->getHitz();

    for (int phisector = 0; phisector < m_number_of_sectors; phisector++) {
        const double rphi = hitx * m_cosphisec[phisector] + hity * m_sinphisec[phisector];
        const double dotprod = rphi * invradius;

        //      for (double theta_in_grad=m_stepsize_per_angle/2.; theta_in_grad<m_detectorsize_angle; theta_in_grad+=m_stepsize_per_angle)
        //      {
        for (int i = 0; i < m_nbins_angle; i++) {
            const double rz0 = -m_costheta[i] * rphi + m_sintheta[i] * hitz;
            const double weighthough = weight * weightHoughTransform(rz0, m_sintheta[i], m_sinphisec[phisector], dotprod);
            fillHisto(rz0, m_theta_in_grad[i], weighthough, phisector);
        }
    }
}

int MuonHoughTransformer_rzcosmics::fillHisto(double rz0, double theta_in_grad, double weight, int sector) {
    MuonHoughHisto2D* histo = m_histos.getHisto(sector);

    int filled_binnumber = histo->fill(rz0, theta_in_grad, weight);

    // this houghtransform has a full butterfly pattern:
    double half_weight = 0.5 * weight;

    // should be filled using filled_binnumber!

    if (theta_in_grad - m_binwidthy < 0) {
        histo->fill(rz0 + m_binwidthx, theta_in_grad + m_binwidthy, half_weight);
        // histo->fill(rz0-m_binwidthx,theta_in_grad-m_binwidthy + 180.,half_weight); // no cyclic angle here
        histo->fill(rz0 - m_binwidthx, theta_in_grad + m_binwidthy, half_weight);
    } else if (theta_in_grad + m_binwidthy > 180.) {
        // histo->fill(rz0+m_binwidthx,theta_in_grad+m_binwidthy - 180.,half_weight);
        histo->fill(rz0 - m_binwidthx, theta_in_grad - m_binwidthy, half_weight);
        histo->fill(rz0 + m_binwidthx, theta_in_grad - m_binwidthy, half_weight);
    } else {
        histo->fill(rz0 + m_binwidthx, theta_in_grad + m_binwidthy, half_weight);
        histo->fill(rz0 - m_binwidthx, theta_in_grad - m_binwidthy, half_weight);
        histo->fill(rz0 - m_binwidthx, theta_in_grad + m_binwidthy, half_weight);
        histo->fill(rz0 + m_binwidthx, theta_in_grad - m_binwidthy, half_weight);
    }
    return filled_binnumber;
}

MuonHoughPattern* MuonHoughTransformer_rzcosmics::hookAssociateHitsToMaximum(const MuonHoughHitContainer* event,
                                                                             std::pair<double, double> coordsmaximum,
                                                                             double maximum_residu_mm, double /*maximum_residu_angle*/,
                                                                             int maxsector, bool /*which_segment*/, int printlevel) const {
    MsgStream log(Athena::getMessageSvc(), "MuonHoughTransformer_rzcosmics::hookAssociateHitsToMaximum");
    MuonHoughPattern* houghpattern = new MuonHoughPattern(MuonHough::hough_rzcosmics);

    double eradius = 0., er0 = 0.;
    const double theta = m_muonhoughmathutils.angleFromGradToRadial(coordsmaximum.second);
    const double rz0 = coordsmaximum.first;

    const double phimax = m_phisec[maxsector];

    if (printlevel >= 4 || log.level() <= MSG::VERBOSE) {
        log << MSG::VERBOSE << "sector: " << maxsector << " phimax: " << phimax << endmsg;
        log << MSG::VERBOSE << "coordsmaximumfirst: " << rz0 << endmsg;
        log << MSG::VERBOSE << "coordsmaximumsecond: " << coordsmaximum.second << " coordsmaximumsecondinrad: " << theta << endmsg;
        log << MSG::VERBOSE << "MuonHoughTransformer_rzcosmics::size of event: " << event->size() << endmsg;
    }

    for (unsigned int i = 0; i < event->size(); i++) {
        const double hitx = event->getHitx(i);
        const double hity = event->getHity(i);

        // select which hits could be in maximum:
        const double hitz = event->getHitz(i);
        const double perp = hitx * m_cosphisec[maxsector] + hity * m_sinphisec[maxsector];

        double residu_distance = MuonHoughMathUtils::signedDistanceToLine(hitz, perp, rz0, theta);

        if (printlevel >= 4 || log.level() <= MSG::VERBOSE) {
            log << MSG::VERBOSE << "MuonHoughTransformer_rzcosmics::hitx: " << hitx << " hity: " << hity << " hitz: " << hitz
                << " perp: " << perp << endmsg;
            log << MSG::VERBOSE << "MuonHoughTransformer_rzcosmics::residu_distance: " << residu_distance << endmsg;
        }

        if (std::abs(residu_distance) < maximum_residu_mm)  // here no circular effect
        {
            if (printlevel >= 4 || log.level() <= MSG::VERBOSE) {
                log << MSG::VERBOSE
                    << "MuonHoughTransformer_rzcosmics::hit added to houghpattern! detector: " << event->getHit(i)->getWhichDetector()
                    << endmsg;
                if (event->getHit(i)->getAssociated()) log << MSG::VERBOSE << " hit already earlier associated to pattern!" << endmsg;
            }
            houghpattern->addHit(event->getHit(i));
            event->getHit(i)->setAssociated(true);

            double rz0hit = residu_distance + rz0;
            eradius += rz0hit;

            double r0hit = hitx * m_sinphisec[maxsector] - hity * m_cosphisec[maxsector];
            er0 += r0hit;

        }  // hit in distance
    }      // hitno

    eradius = eradius / (houghpattern->size() + 1e-7);
    er0 = er0 / (houghpattern->size() + 1e-7);

    houghpattern->setEPhi(phimax);
    houghpattern->setERPhi(er0);
    houghpattern->setETheta(theta);
    houghpattern->setERTheta(eradius);
    houghpattern->setECurvature(1.);

    if (houghpattern->empty()) {
        if (printlevel >= 4 || log.level() <= MSG::VERBOSE) {
            log << MSG::VERBOSE << "MuonHoughTransformer_rzcosmics::WARNING : no hits found on pattern" << endmsg;
        }
    }

    else if (std::abs(eradius - rz0) > 500.) {
        if (printlevel >= 4 || log.level() <= MSG::VERBOSE) {
            log << MSG::VERBOSE << "MuonHoughTransformer_rzcosmics::WARNING Eradius or Etheta calc. WRONG" << endmsg;
            log << MSG::VERBOSE << "MuonHoughTransformer_rzcosmics::eradius: " << rz0 << " etheta: " << theta << endmsg;
            log << MSG::VERBOSE << "MuonHoughTransformer_rzcosmics::eradius: " << eradius << " etheta: " << theta << endmsg;
        }
        houghpattern->setERTheta(rz0);
    }

    updateParameters(houghpattern);  // not possible when phi direction not known!

    if (printlevel >= 4 || log.level() <= MSG::VERBOSE) {
        log << MSG::VERBOSE << "MuonHoughTransformer_rzcosmics::updateParameterstheta new phi: " << houghpattern->getEPhi()
            << " old phi: " << phimax << endmsg;
        log << MSG::VERBOSE << "MuonHoughTransformer_rzcosmics::updateParameterstheta new r0: " << houghpattern->getERPhi()
            << " old r0: " << er0 << endmsg;
        log << MSG::VERBOSE << "MuonHoughTransformer_rzcosmics::updateParameterstheta new theta: " << houghpattern->getETheta()
            << " old theta: " << theta << endmsg;
        log << MSG::VERBOSE << "MuonHoughTransformer_rzcosmics::updateParameterstheta new z0: " << houghpattern->getERTheta()
            << " old z0: " << eradius << endmsg;
    }

    return houghpattern;
}

float MuonHoughTransformer_rzcosmics::weightHoughTransform(double r0) const {
    if (m_add_weight_radius) {
        return m_detectorsize / (m_detectorsize + m_weight_constant_radius * std::abs(r0));
    } else {
        return 1;
    }  // weight function, to give more importance to patterns close to origin
}

float MuonHoughTransformer_rzcosmics::weightHoughTransform(double r0, double sintheta, double sinphi,
                                                           double dotprod) const  // theta in grad
{
    if (!m_add_weight_angle) {
        return weightHoughTransform(r0);
    } else {
        double dotprod_part = 0.5 + 0.5 * dotprod * dotprod;     // preference for angles that are normal to the chamber
        double sintheta_part = 0.9 + 0.1 * sintheta * sintheta;  // preference for patterns from above
        double sinphi_part = 0.75 + 0.25 * sinphi * sinphi;      // preference for patterns from above
        float r_theta_weight = dotprod_part * sintheta_part * sinphi_part;

        return r_theta_weight * weightHoughTransform(r0);  // preference for patterns with low impact parameter
    }
}

int MuonHoughTransformer_rzcosmics::sector(MuonHoughHit* /*hit*/) const {
    // function not implemented for this transform
    return 0;
}

void MuonHoughTransformer_rzcosmics::updateParameters(MuonHoughPattern* houghpattern) {
    const unsigned int size = houghpattern->size();

    if (size <= 1) return;

    const double phi = houghpattern->getEPhi();
    const double cosphi = std::cos(phi);
    const double sinphi = std::sin(phi);

    double sum_radii = 0.;
    double sum_z = 0.;

    for (unsigned int i = 0; i < size; i++) {
        sum_radii += houghpattern->getHitx(i) * cosphi + houghpattern->getHity(i) * sinphi;
        sum_z += houghpattern->getHitz(i);
    }

    const double av_radii = sum_radii / (size + 0.);
    const double av_z = sum_z / (size + 0.);

    double sumr = 0.;
    double sumz = 0.;
    for (unsigned int i = 0; i < size; i++) {
        double radius = houghpattern->getHitx(i) * cosphi + houghpattern->getHity(i) * sinphi;
        double hitz = houghpattern->getHitz(i);
        double r_offset = radius - av_radii;
        double z_offset = hitz - av_z;
        double weight = r_offset * r_offset + z_offset * z_offset;
        int sign = 1;
        if (r_offset * radius + z_offset * hitz < 0) { sign = -1; }
        sumr += weight * sign * r_offset;
        sumz += weight * sign * z_offset;
    }

    if (std::abs(sumr) < 0.000001 || std::abs(sumz) < 0.000001) { return; }

    double theta = std::atan2(sumr, sumz);

    if (theta < 0) theta += M_PI;

    // if theta almost straight rely on hit for prediction (transform has difficulties prediction direction in this case):
    double offset = 0.02;
    if (theta < offset) {
        if (houghpattern->getHitz(0) < 0) { theta = M_PI - theta; }
    }

    else if (theta > M_PI - offset) {
        if (houghpattern->getHitz(0) > 0) { theta = M_PI - theta; }
    }

    const double rz0 = av_z * std::sin(theta) - av_radii * std::cos(theta);

    houghpattern->setETheta(theta);
    houghpattern->setERTheta(rz0);
}
