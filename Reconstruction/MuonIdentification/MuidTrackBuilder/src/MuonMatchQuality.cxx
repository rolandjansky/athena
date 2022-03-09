/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   get quality of combined track match
///////////////////////////////////////////////////////////////////

#include "MuonMatchQuality.h"

#include <cmath>
#include <iomanip>
#include <memory>

#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "FourMomUtils/xAODP4Helpers.h"
#include "TrkTrack/Track.h"
namespace Rec {

    MuonMatchQuality::MuonMatchQuality(const std::string& type, const std::string& name, const IInterface* parent) :
        AthAlgTool(type, name, parent), m_alignmentUncertainty(nullptr) {
        declareInterface<IMuonMatchQuality>(this);
    }

    StatusCode MuonMatchQuality::initialize() {
        ATH_MSG_DEBUG("MuonMatchQuality::initialize().");

        // get the tools
        if (!m_tagTool.name().empty()) { ATH_CHECK(m_tagTool.retrieve()); }
        ATH_CHECK(m_trackQuery.retrieve());

        // set up alignment uncertainty between ID and MS tracking systems
        m_alignmentUncertainty = std::make_unique<AmgSymMatrix(5)>();
        m_alignmentUncertainty->setZero();
        (*m_alignmentUncertainty)(0, 0) = m_positionUncertainty * m_positionUncertainty;
        (*m_alignmentUncertainty)(1, 1) = m_positionUncertainty * m_positionUncertainty;
        (*m_alignmentUncertainty)(2, 2) = m_directionUncertainty * m_directionUncertainty;
        (*m_alignmentUncertainty)(3, 3) = m_directionUncertainty * m_directionUncertainty;

        return StatusCode::SUCCESS;
    }
    /** IMuonMatchQuality interface:
        match chiSquared between two tracks expressed at same inner (IP) surface,
        expected to handle indet with extrapolated spectrometer track or combined with constituent track */
    double MuonMatchQuality::innerMatchChi2(const Trk::Track& track1, const Trk::Track& track2, const EventContext& ctx) const {
        MuonMatchQuality::CacheAll CA = setCache(track1, track2, ctx);
        return CA.innerMatchChi2;
    }

    /** IMuonMatchQuality interface: degrees of freedom for chi2 match at IP */
    int MuonMatchQuality::innerMatchDOF(const Trk::Track& track1, const Trk::Track& track2) const {
        int matchDOF = 0;

        const Trk::Perigee* perigee1 = track1.perigeeParameters();
        const Trk::Perigee* perigee2 = track2.perigeeParameters();

        if (perigee1 && perigee2 && (*perigee1).associatedSurface().center() == (*perigee2).associatedSurface().center()) {
            if (m_trackQuery->isLineFit(track1) || m_trackQuery->isLineFit(track2)) {
                matchDOF = 4;
            } else {
                matchDOF = 5;
            }
        }

        return matchDOF;
    }
    double MuonMatchQuality::innerMatchProbability(const Trk::Track& track1, const Trk::Track& track2, const EventContext& ctx) const {
        MuonMatchQuality::CacheAll CA = setCache(track1, track2, ctx);
        return CA.innerMatchProbability;
    }

    std::pair<int, std::pair<double, double> > MuonMatchQuality::innerMatchAll(const Trk::Track& track1, const Trk::Track& track2,
                                                                               const EventContext& ctx) const {
        MuonMatchQuality::CacheAll CA = setCache(track1, track2, ctx);

        std::pair<int, std::pair<double, double> > aTriad =
            std::make_pair(CA.innerMatchDOF, std::make_pair(CA.innerMatchChi2, CA.innerMatchProbability));

        return aTriad;
    }
    double MuonMatchQuality::outerMatchChi2(const Trk::Track& track1, const Trk::Track& track2, const EventContext& ctx) const {
        // caching needs some development...
        // setCache(track1,track2);
        double outerMatchChi2 = 999999.;
        if (!m_tagTool.empty()) { outerMatchChi2 = m_tagTool->chi2(track1, track2, ctx); }

        return outerMatchChi2;
    }

    /** IMuonMatchQuality interface: degrees of freedom for chi2 match at first MS hit */
    int MuonMatchQuality::outerMatchDOF(const Trk::Track& /*track1*/, const Trk::Track& /*track2*/) const { return 4; }
    double MuonMatchQuality::outerMatchProbability(const Trk::Track& track1, const Trk::Track& track2, const EventContext& ctx) const {
        double outerMatchProbability = 0.;
        if (!m_tagTool.empty()) {
            double outer_chi2 = outerMatchChi2(track1, track2, ctx);

            // probability of MS chi2
            if (outer_chi2 > 0. && outer_chi2 < 1000.) {
                Genfun::CumulativeChiSquare func(4);
                outerMatchProbability = 1. - func(outer_chi2);
            }
        }
        return outerMatchProbability;
    }

    /** IMuonMatchQuality interface: check the track perigee parameters are expressed at the same surface */
    bool MuonMatchQuality::shareOrigin(const Trk::Track& track1, const Trk::Track& track2) const {
        const Trk::Perigee* perigee1 = track1.perigeeParameters();
        const Trk::Perigee* perigee2 = track2.perigeeParameters();

        return perigee1 && perigee2 && (*perigee1).associatedSurface().center() == (*perigee2).associatedSurface().center();
    }
    double MuonMatchQuality::simpleChi2(const Trk::Track& track1, const Trk::Track& track2, const EventContext& ctx) const {
        MuonMatchQuality::CacheAll CA = setCache(track1, track2, ctx);
        return CA.simpleChi2;
    }

    /* private method to fill cache */
    MuonMatchQuality::CacheAll MuonMatchQuality::setCache(const Trk::Track& track1, const Trk::Track& track2,
                                                          const EventContext& ctx) const {
        MuonMatchQuality::CacheAll CA;
        // set match degrees of freedom
        CA.innerMatchDOF = innerMatchDOF(track1, track2);
        if (!CA.innerMatchDOF) return CA;

        // parameter difference
        const Trk::Perigee* perigee1 = track1.perigeeParameters();
        const Trk::Perigee* perigee2 = track2.perigeeParameters();

        // coverity false positive (as null pointers cause innerMatchDOF failure)
        if (!perigee1 || !perigee2) return CA;

        double deltaD0 = perigee1->parameters()[Trk::d0] - perigee2->parameters()[Trk::d0];
        double deltaZ0 = perigee1->parameters()[Trk::z0] - perigee2->parameters()[Trk::z0];
        double deltaPhi0 = xAOD::P4Helpers::deltaPhi(perigee1->parameters()[Trk::phi0], perigee2->parameters()[Trk::phi0]);
        double deltaTheta = perigee1->parameters()[Trk::theta] - perigee2->parameters()[Trk::theta];
        double deltaQoverP = perigee1->parameters()[Trk::qOverP] - perigee2->parameters()[Trk::qOverP];

        // weight matrix for differences
        const AmgSymMatrix(5)* cov1 = perigee1->covariance();
        const AmgSymMatrix(5)* cov2 = perigee2->covariance();

        AmgSymMatrix(5) covariance;
        covariance.setZero();

        if (m_trackQuery->isCombined(track1, ctx)) {
            // FIXME: should take weighted difference etc -
            //        but this is anyway unreliable due to rounding errors
            ATH_MSG_WARNING("track1 isCombined: results unreliable ");
        } else if (m_trackQuery->isCombined(track2, ctx)) {
            // FIXME: weighted difference etc
            ATH_MSG_WARNING("track2 isCombined: results unreliable ");
        }

        covariance = *cov1 + *cov2 + *m_alignmentUncertainty;
        if (CA.innerMatchDOF < 5) {
            deltaQoverP = 0.;
            for (int i = 0; i != Trk::qOverP; ++i) { covariance(i, Trk::qOverP) = 0.; }
            for (int i = 0; i != Trk::qOverP; ++i) { covariance(Trk::qOverP, i) = 0.; }
            covariance(Trk::qOverP, Trk::qOverP) = 1.;
        } else {
            deltaQoverP /= Units::TeV;
            for (int i = 0; i != Trk::qOverP; ++i) { covariance(i, Trk::qOverP) /= Units::TeV; }
            for (int i = 0; i != Trk::qOverP; ++i) { covariance(Trk::qOverP, i) /= Units::TeV; }
            covariance(Trk::qOverP, Trk::qOverP) /= (Units::TeV * Units::TeV);
        }

        // invert summed covariance
        AmgMatrix(5, 5) weight = covariance.inverse().eval();

        AmgMatrix(5, 1) paramDifference;
        paramDifference.setZero();
        paramDifference(Trk::d0, 0) = deltaD0;
        paramDifference(Trk::z0, 0) = deltaZ0;
        paramDifference(Trk::phi0, 0) = deltaPhi0;
        paramDifference(Trk::theta, 0) = deltaTheta;
        paramDifference(Trk::qOverP, 0) = deltaQoverP;

        double matchChi2 = paramDifference.dot(weight * paramDifference);
        CA.innerMatchChi2 = matchChi2;

        // probability of chi2
        if (CA.innerMatchChi2 >= 0. && CA.innerMatchChi2 < 1000.) {
            CA.innerMatchProbability = 1. - Genfun::CumulativeChiSquare(CA.innerMatchDOF)(CA.innerMatchChi2);
        }

        // Protect against zero division
        bool nonzerocov = true;
        for (int k = 0; k < covariance.cols(); k++) {
            if (covariance(k, k) == 0) nonzerocov = false;
        }

        if (nonzerocov) {
            // simple chi2 as sometimes correlations overestimated
            CA.simpleChi2 = deltaD0 * deltaD0 / covariance(Trk::d0, Trk::d0) + deltaZ0 * deltaZ0 / covariance(Trk::z0, Trk::z0) +
                            deltaPhi0 * deltaPhi0 / covariance(Trk::phi0, Trk::phi0) +
                            deltaTheta * deltaTheta / covariance(Trk::theta, Trk::theta) +
                            deltaQoverP * deltaQoverP / covariance(Trk::qOverP, Trk::qOverP);

            if (CA.simpleChi2 + 5. < CA.innerMatchChi2) {
                ATH_MSG_DEBUG("problem with matchChi2 " << CA.innerMatchChi2 << ", simpleChi2 " << CA.simpleChi2);
            }
        } else {
            ATH_MSG_WARNING("Could not compute simpleChi2. Covariance matrix contains zeros");
        }

        ATH_MSG_VERBOSE(std::setiosflags(std::ios::fixed)
                        << "matchDOF" << std::setw(2) << CA.innerMatchDOF << "  matchChi2:" << std::setw(6) << std::setprecision(1)
                        << CA.innerMatchChi2 << "  simpleChi2:" << std::setw(6) << std::setprecision(1) << CA.simpleChi2 << "     pullD0 "
                        << std::setw(5) << std::setprecision(1) << deltaD0 / std::sqrt(covariance(0, 0)) << "  pullZ0 " << std::setw(5)
                        << std::setprecision(1) << deltaZ0 / std::sqrt(covariance(1, 1)) << "  pullPhi0 " << std::setw(5)
                        << std::setprecision(1) << deltaPhi0 / std::sqrt(covariance(2, 2)) << "  pullTheta " << std::setw(5)
                        << std::setprecision(1) << deltaTheta / std::sqrt(covariance(3, 3)) << "  pullQoverP " << std::setw(5)
                        << std::setprecision(1) << deltaQoverP / std::sqrt(covariance(4, 4)));

        return CA;
    }

}  // namespace Rec
