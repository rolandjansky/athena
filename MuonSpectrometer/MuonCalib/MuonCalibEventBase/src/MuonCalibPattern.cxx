/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibPattern.h"

#include <algorithm>
#include <limits>

#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibStl/DeleteObject.h"

namespace MuonCalib {

    MuonCalibPattern::MuonCalibPattern() :
        m_chi2(std::numeric_limits<double>::max()),
        m_z0(std::numeric_limits<double>::max()),
        m_dist0(std::numeric_limits<double>::max()),
        m_invP(std::numeric_limits<double>::max()),
        m_phi(std::numeric_limits<double>::max()),
        m_theta(std::numeric_limits<double>::max()),
        m_nmdt(0),
        m_nrpc(0),
        m_ntgc(0),
        m_ncsc(0) {}

    MuonCalibPattern::MuonCalibPattern(double chi2, double z0, double r0, double invP, double phi, double theta) :
        m_chi2(chi2), m_z0(z0), m_dist0(r0), m_invP(invP), m_phi(phi), m_theta(theta), m_nmdt(0), m_nrpc(0), m_ntgc(0), m_ncsc(0) {}

    MuonCalibPattern::MuonCalibPattern(double chi2, double z0, double r0, double invP, double phi, double theta, unsigned int nmdt,
                                       unsigned int nrpc, unsigned int ntgc, unsigned int ncsc) :
        m_chi2(chi2),
        m_z0(z0),
        m_dist0(r0),
        m_invP(invP),
        m_phi(phi),
        m_theta(theta),
        m_nmdt(nmdt),
        m_nrpc(nrpc),
        m_ntgc(ntgc),
        m_ncsc(ncsc) {}

    MuonCalibPattern::MuonCalibPattern(const MuonCalibPattern& pat) {
        m_chi2 = pat.chi2();
        m_z0 = pat.z0();
        m_dist0 = pat.r0();
        m_invP = pat.invP();
        m_phi = pat.phi();
        m_theta = pat.theta();
        m_nmdt = pat.nmdtHits();
        m_nrpc = pat.nrpcHits();
        m_ntgc = pat.ntgcHits();
        m_ncsc = pat.ncscHits();

        MuonSegCit pat_it = pat.muonSegBegin();
        MuonSegCit pat_it_end = pat.muonSegEnd();
        for (; pat_it != pat_it_end; ++pat_it) {
            MuonCalibSegment* seg = new MuonCalibSegment(**pat_it);
            addMuonSegment(seg);
        }
    }

    MuonCalibPattern& MuonCalibPattern::operator=(const MuonCalibPattern& pat) {
        if (this != &pat) {
            m_chi2 = pat.chi2();
            m_z0 = pat.z0();
            m_dist0 = pat.r0();
            m_invP = pat.invP();
            m_phi = pat.phi();
            m_theta = pat.theta();
            m_nmdt = pat.nmdtHits();
            m_nrpc = pat.nrpcHits();
            m_ntgc = pat.ntgcHits();
            m_ncsc = pat.ncscHits();

            std::for_each(muonSegBegin(), muonSegEnd(), DeleteObject());
            m_muonSegments.clear();
            MuonSegCit pat_it = pat.muonSegBegin();
            MuonSegCit pat_it_end = pat.muonSegEnd();
            for (; pat_it != pat_it_end; ++pat_it) {
                MuonCalibSegment* seg = new MuonCalibSegment(**pat_it);
                addMuonSegment(seg);
            }
        }
        return *this;
    }

    MuonCalibPattern::~MuonCalibPattern() {
        // MuonCalibPattern owns the segments
        std::for_each(muonSegBegin(), muonSegEnd(), DeleteObject());
        m_muonSegments.clear();
    }

    std::ostream& MuonCalibPattern::dump(std::ostream& os) const {
        os << "MuonCalibPattern: chi2 " << chi2() << " invP " << invP() << " nsegments " << muonSegments() << std::endl
           << " (r0,z0) = (" << r0() << "," << z0() << ")  (phi,theta) = (" << phi() << "," << theta() << ")";
        return os;
    }

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibPattern& pat) { return pat.dump(stream); }
