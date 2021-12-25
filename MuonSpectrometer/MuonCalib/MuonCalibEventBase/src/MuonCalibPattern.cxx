/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibPattern.h"

#include <algorithm>
#include <limits>

#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibStl/DeleteObject.h"

namespace MuonCalib {

    unsigned int MuonCalibPattern::muonSegments() const { return m_muonSegments.size(); }
    const MuonCalibPattern::MuonSegmentVec& MuonCalibPattern::muonSegs() const { return m_muonSegments; }
    MuonCalibPattern::MuonSegmentVec& MuonCalibPattern::muonSegs() { return m_muonSegments; }

    // track parameters
    double MuonCalibPattern::chi2() const { return m_pars.chi2; }
    double MuonCalibPattern::z0() const { return m_pars.z0; }
    double MuonCalibPattern::r0() const { return m_pars.dist0; }
    double MuonCalibPattern::invP() const { return m_pars.invP; }
    double MuonCalibPattern::phi() const { return m_pars.phi; }
    double MuonCalibPattern::theta() const { return m_pars.theta; }

    unsigned int MuonCalibPattern::nmdtHits() const { return m_pars.nmdt; }
    unsigned int MuonCalibPattern::nrpcHits() const { return m_pars.nrpc; }
    unsigned int MuonCalibPattern::ntgcHits() const { return m_pars.ntgc; }
    unsigned int MuonCalibPattern::ncscHits() const { return m_pars.ncsc; }

    // methodes for adding segments
    void MuonCalibPattern::addMuonSegment(MuonCalibSegPtr seg) { m_muonSegments.emplace_back(std::move(seg)); }

    MuonCalibPattern::MuonCalibPattern(MuonCalibPattern::defineParams pars) : m_pars{pars} {
        std::cout << "Consider to add NSW information here " << __FILE__ << ":" << __LINE__ << std::endl;
    }

    void MuonCalibPattern::copy(const MuonCalibPattern& other) {
        m_pars = other.m_pars;
        /// Need to find out whether the segment should be copied or not
        for (const MuonCalibSegPtr& other_seg : other.muonSegs()) {
            MuonCalibSegPtr seg{new MuonCalibSegment(*other_seg)};
            addMuonSegment(seg);
        }
    }
    MuonCalibPattern::MuonCalibPattern(const MuonCalibPattern& other) { copy(other); }
    MuonCalibPattern& MuonCalibPattern::operator=(const MuonCalibPattern& pat) {
        if (this != &pat) { copy(pat); }
        return *this;
    }

    std::ostream& MuonCalibPattern::dump(std::ostream& os) const {
        os << "MuonCalibPattern: chi2 " << chi2() << " invP " << invP() << " nsegments " << muonSegments() << std::endl
           << " (r0,z0) = (" << r0() << "," << z0() << ")  (phi,theta) = (" << phi() << "," << theta() << ")";
        return os;
    }

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibPattern& pat) { return pat.dump(stream); }
