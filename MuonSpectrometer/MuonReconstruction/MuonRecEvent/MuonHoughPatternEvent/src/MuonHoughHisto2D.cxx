/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHoughPatternEvent/MuonHoughHisto2D.h"

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "TFile.h"
#include "TH2F.h"

MuonHoughHisto2D::~MuonHoughHisto2D() { delete[] m_histBuffer; }

MuonHoughHisto2D::MuonHoughHisto2D(int nbinsx, double xmin, double xmax, int nbinsy, double ymin, double ymax, int number_of_maxima) :
    m_nbinsx(nbinsx),
    m_xmin(xmin),
    m_xmax(xmax),
    m_nbinsy(nbinsy),
    m_ymin(ymin),
    m_ymax(ymax),
    m_number_of_maxima(number_of_maxima),
    m_scale(10000),
    m_threshold(2.1 * m_scale),
    m_distance_to_next_maximum(100) {
    m_nbinsx_plus2 = nbinsx + 2;
    m_nbinsy_plus2 = nbinsy + 2;
    m_size = m_nbinsx_plus2 * m_nbinsy_plus2;

    m_binwidthx = (m_xmax - m_xmin) / (m_nbinsx + 0.);
    m_binwidthy = (m_ymax - m_ymin) / (m_nbinsy + 0.);

    m_invbinwidthx = 1. / m_binwidthx;
    m_invbinwidthy = 1. / m_binwidthy;

    m_maxima_found = 0;
    m_maximumBin = -1;
    m_maximum = 0;
    m_maximumIsValid = true;
    init();
}

void MuonHoughHisto2D::init() { m_histBuffer = new unsigned int[m_size]; }

void MuonHoughHisto2D::reset() {
    resetHisto();

    m_bins_above_threshold.clear();
    m_maximumbins.clear();  // obsolete?
    m_maxima_found = 0;
    m_maximumBin = -1;
    m_maximum = 0;
    m_maximumIsValid = true;
}

void MuonHoughHisto2D::findMaxima(int printlevel, bool which_segment) {
    MsgStream log(Athena::getMessageSvc(), "MuonHoughHisto2D::findMaxima");
    m_maximumbins.clear();  // clear old m_maxima_vector and start searching again

    int maximum_number = 0;

    while (maximum_number < m_number_of_maxima) {
        double maximum = -1.;
        int maxbin = -1;

        if (printlevel >= 3 || log.level() <= MSG::DEBUG) {
            log << MSG::DEBUG << "MuonHoughHisto2D::size bins above threshold: " << m_bins_above_threshold.size() << endmsg;
        }

        std::set<int>::iterator it = m_bins_above_threshold.begin();
        std::set<int>::iterator it_end = m_bins_above_threshold.end();
        for (; it != it_end; ++it) {
            if (!checkIfMaximumAlreadyUsed(*it)) {
                checkIfMaximum(*it, maximum, maxbin, which_segment, printlevel);  // call by ref
            }
        }

        if (maximum < m_threshold) {
            if (printlevel >= 2 || log.level() <= MSG::INFO) { log << MSG::INFO << "MuonHoughHisto2D:: no maximum found" << endmsg; }
            break;
        } else {
            if (printlevel >= 2 || log.level() <= MSG::INFO) {
                std::pair<double, double> coords;
                coords = binnumberToCoords(maxbin);
                log << MSG::INFO << "MuonHoughHisto2D:: Maximum found: " << maximum << " binnumber: " << maxbin << " R (z) " << coords.first
                    << " angle " << coords.second << endmsg;
            }
            m_maximumbins.push_back(maxbin);
        }  // maxbin <> m_threshold

        if (printlevel >= 10 || log.level() <= MSG::VERBOSE) {
            if (maximum == -1.) {
                log << MSG::VERBOSE << "MuonHoughHisto2D::No Bins Above Threshold" << endmsg;
            } else {
                std::pair<double, double> coords;
                coords = binnumberToCoords(maxbin);
                log << MSG::VERBOSE << "MuonHoughHisto2D::Maximum Number: " << maximum_number << " Maximum: " << maximum
                    << " binnumber: " << maxbin << " x: " << coords.first << " y: " << coords.second << endmsg;
            }
        }  // printlevel

        maximum_number++;
    }  // number_of_maxima

    m_maxima_found = 1;
}

std::pair<int, double> MuonHoughHisto2D::getMax() const {
    std::pair<int, double> maxpair;
    int maxbin = -1;  // convention! for no bin above threshold
    unsigned int maximum = m_threshold;

    if (m_maximumIsValid) {
        if (m_maximum > m_threshold) {
            maxbin = m_maximumBin;
            maximum = m_maximum;
        }
    } else {
        for (unsigned int i = 0; i < m_size; i++) {
            if (m_histBuffer[i] > maximum) {
                maximum = m_histBuffer[i];
                maxbin = i;
            }
        }
    }
    maxpair.first = maxbin;
    maxpair.second = maximum;

    return maxpair;
}

std::pair<int, double> MuonHoughHisto2D::getMaximumBin(unsigned int maximum_number, bool which_segment, int printlevel) {
    MsgStream log(Athena::getMessageSvc(), "MuonHoughHisto2D::getMaximumBin");
    if (m_maxima_found == 0) {
        findMaxima(printlevel, which_segment);  // fills m_maximumbins
    }

    int maxbin = -1;
    double maximum = -1.;

    if (printlevel >= 3 || log.level() <= MSG::DEBUG) {
        log << MSG::VERBOSE << "MuonHoughHisto2D:: m_maximumbins.size: " << m_maximumbins.size() << endmsg;
    }

    if (m_maximumbins.size() > maximum_number) {
        maxbin = m_maximumbins[maximum_number];
        maximum = content_Bin_Area(maxbin);
    }

    std::pair<int, double> maximumbin(maxbin, maximum);

    return maximumbin;

}  // getMaximumBin

std::pair<double, double> MuonHoughHisto2D::getCoordsMaximum(unsigned int maximum_number, bool which_segment, int printlevel) {
    MsgStream log(Athena::getMessageSvc(), "MuonHoughHisto2D::getCoordsMaximum");
    std::pair<double, double> coordsmaximum;
    int binnumber = getMaxBin(maximum_number, which_segment, printlevel);

    if (binnumber != -1) {
        coordsmaximum = binnumberToCoords(binnumber);
    } else {
        if (log.level() <= MSG::WARNING) log << MSG::WARNING << "HoughTransform::No Maximum Found" << endmsg;
        coordsmaximum.first = 99999.;
        coordsmaximum.second = 99999.;
    }
    return coordsmaximum;
}

bool MuonHoughHisto2D::checkIfMaximumAlreadyUsed(int binnumber) const {
    bool check = false;

    for (unsigned int i = 0; i < m_maximumbins.size(); i++) {
        if (distanceBins(binnumber, m_maximumbins[i]) < m_distance_to_next_maximum) {
            check = true;
            return check;
        }
    }

    return check;
}

bool MuonHoughHisto2D::checkIfMaximum(int binnumber, double& maximum, int& maxbin, bool /*which_segment*/, int /*printlevel*/) const {
    bool check = false;

    double content_bin_area = content_Bin_Area(binnumber);  // now no area anymore == getBinContent

    // when using negative weights the following can happen:
    if (content_bin_area < m_threshold) return check;

    if (content_bin_area == maximum) {
        if (getBinContent(maxbin) > getBinContent(binnumber))  // give preference to maximum with peak ( _U_ )
        {
            check = true;
            maximum = content_bin_area;
            maxbin = binnumber;
        }
    } else if (content_bin_area > maximum) {
        check = true;
        maximum = content_bin_area;
        maxbin = binnumber;
    }
    return check;
}  // checkIfMaximum

int MuonHoughHisto2D::distanceBins(int binnumber1, int binnumber2) const {
    int binnumberdistance = std::abs(binnumber1 - binnumber2);

    // Manhattan metric:
    int distance = (binnumberdistance % m_nbinsx_plus2) + (binnumberdistance / m_nbinsx_plus2);

    return distance;
}

std::pair<double, double> MuonHoughHisto2D::binnumberToCoords(int binnumber, int printlevel) const {
    MsgStream log(Athena::getMessageSvc(), "MuonHoughHisto2D::binnumberToCoords");
    std::pair<double, double> coordsbin;
    if (binnumber < 0) {
        if (log.level() <= MSG::WARNING) log << MSG::WARNING << "MuonHoughHisto2D::ERROR: negativebinnumber: " << binnumber << endmsg;
        coordsbin.first = 99999.;
        coordsbin.second = 99999.;
        return coordsbin;
    }

    double xcoord = binnumberToXCoord(binnumber);
    double ycoord = binnumberToYCoord(binnumber);

    if (printlevel >= 4 || log.level() <= MSG::VERBOSE) {
        log << MSG::VERBOSE << "MuonHoughHisto2D::Maximum: " << getBinContent(binnumber) << " binnumber: " << binnumber << " x: " << xcoord
            << " y: " << ycoord << endmsg;
    }

    coordsbin.first = xcoord;
    coordsbin.second = ycoord;
    return coordsbin;
}

int MuonHoughHisto2D::binInHistogram(unsigned int binnumber) const {
    int bininhisto = 0;

    if ((binnumber) % m_nbinsx_plus2 == 0) {
        bininhisto = 1;
    } else if ((binnumber + 1) % m_nbinsx_plus2 == 0) {
        bininhisto = 2;
    } else if (binnumber <= m_nbinsx_plus2) {
        bininhisto = 3;
    } else if (binnumber >= m_nbinsx_plus2 * (getNbinsY() + 1)) {
        bininhisto = 4;
    }

    return bininhisto;
}

TH2F* MuonHoughHisto2D::bookAndFillRootHistogram(const std::string& hname) const {
    TH2F* histogram = new TH2F(hname.c_str(), hname.c_str(), m_nbinsx, m_xmin, m_xmax, m_nbinsy, m_ymin, m_ymax);
    for (unsigned int i = 0; i < m_size; i++) {
        int ix = i % m_nbinsx_plus2;
        int iy = i / m_nbinsx_plus2;
        histogram->SetBinContent(ix, iy, m_histBuffer[i] / (double)m_scale);
    }
    return histogram;
}
