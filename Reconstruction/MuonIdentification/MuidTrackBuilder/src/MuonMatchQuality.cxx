/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonMatchQuality.h, (c) ATLAS Combined Muon software
//   get quality of combined track match
///////////////////////////////////////////////////////////////////

#include "MuidTrackBuilder/MuonMatchQuality.h"

#include <cmath>
#include <iomanip>

#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "TrkTrack/Track.h"


namespace Rec {


MuonMatchQuality::MuonMatchQuality(const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent), m_alignmentUncertainty(nullptr)
{
    declareInterface<IMuonMatchQuality>(this);
}


StatusCode
MuonMatchQuality::initialize()
{
    ATH_MSG_INFO("MuonMatchQuality::initialize() - package version " << PACKAGE_VERSION);

    // get the tools
    if (m_tagTool.name() != "") {
        if (m_tagTool.retrieve().isFailure()) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_tagTool);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Retrieved tool " << m_tagTool);
        }
    }

    if (m_trackQuery.retrieve().isFailure()) {
        ATH_MSG_FATAL("Failed to retrieve tool " << m_trackQuery);
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_INFO("Retrieved tool " << m_trackQuery);
    }

    // set up alignment uncertainty between ID and MS tracking systems
    m_alignmentUncertainty = new AmgSymMatrix(5);
    m_alignmentUncertainty->setZero();
    (*m_alignmentUncertainty)(0, 0) = m_positionUncertainty * m_positionUncertainty;
    (*m_alignmentUncertainty)(1, 1) = m_positionUncertainty * m_positionUncertainty;
    (*m_alignmentUncertainty)(2, 2) = m_directionUncertainty * m_directionUncertainty;
    (*m_alignmentUncertainty)(3, 3) = m_directionUncertainty * m_directionUncertainty;

    return StatusCode::SUCCESS;
}


StatusCode
MuonMatchQuality::finalize()
{
    delete m_alignmentUncertainty;
    ATH_MSG_DEBUG("finalized");

    return StatusCode::SUCCESS;
}


/** IMuonMatchQuality interface:
    match chiSquared between two tracks expressed at same inner (IP) surface,
    expected to handle indet with extrapolated spectrometer track or combined with constituent track */
double
MuonMatchQuality::innerMatchChi2(const Trk::Track& track1, const Trk::Track& track2) const
{
    MuonMatchQuality::CacheAll CA = setCache(track1, track2);
    return CA.innerMatchChi2;
}


/** IMuonMatchQuality interface: degrees of freedom for chi2 match at IP */
int
MuonMatchQuality::innerMatchDOF(const Trk::Track& track1, const Trk::Track& track2) const
{
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


/** IMuonMatchQuality interface: match probability for chi2 match at IP */
double
MuonMatchQuality::innerMatchProbability(const Trk::Track& track1, const Trk::Track& track2) const
{
    MuonMatchQuality::CacheAll CA = setCache(track1, track2);
    return CA.innerMatchProbability;
}


/** IMuonMatchQuality interface: match  DOF, Chi2 , probability for chi2 match at IP */
std::pair<int, std::pair<double, double> >
MuonMatchQuality::innerMatchAll(const Trk::Track& track1, const Trk::Track& track2) const
{
    MuonMatchQuality::CacheAll CA = setCache(track1, track2);

    std::pair<int, std::pair<double, double> > aTriad =
        std::make_pair(CA.innerMatchDOF, std::make_pair(CA.innerMatchChi2, CA.innerMatchProbability));

    return aTriad;
}


/** IMuonMatchQuality interface:
    match chiSquared between two tracks expressed at first muon spectrometer hit,
    extrapolates indet to first hit of spectrometer track */
double
MuonMatchQuality::outerMatchChi2(const Trk::Track& track1, const Trk::Track& track2) const
{
    // caching needs some development...
    // setCache(track1,track2);
    double outerMatchChi2 = 999999.;
    if (!m_tagTool.empty()) {
        outerMatchChi2 = m_tagTool->chi2(track1, track2);
    }

    return outerMatchChi2;
}


/** IMuonMatchQuality interface: degrees of freedom for chi2 match at first MS hit */
int
MuonMatchQuality::outerMatchDOF(const Trk::Track& /*track1*/, const Trk::Track& /*track2*/) const
{
    return 4;
}

/** IMuonMatchQuality interface: match probability for chi2 match at first MS hit */
double
MuonMatchQuality::outerMatchProbability(const Trk::Track& track1, const Trk::Track& track2) const
{
    // this needs work
    // setCache(track1,track2);

    double outerMatchProbability = 0.;
    if (!m_tagTool.empty()) {
        double outerMatchChi2 = m_tagTool->chi2(track1, track2);

        // probability of MS chi2
        if (outerMatchChi2 > 0. && outerMatchChi2 < 1000.) {
            Genfun::CumulativeChiSquare func(4);
            outerMatchProbability = 1. - func(outerMatchChi2);
        }
    }
    return outerMatchProbability;
}


/** IMuonMatchQuality interface: check the track perigee parameters are expressed at the same surface */
bool
MuonMatchQuality::shareOrigin(const Trk::Track& track1, const Trk::Track& track2) const
{
    const Trk::Perigee* perigee1 = track1.perigeeParameters();
    const Trk::Perigee* perigee2 = track2.perigeeParameters();

    if (perigee1 && perigee2 && (*perigee1).associatedSurface().center() == (*perigee2).associatedSurface().center()) {
        return true;
    }

    return false;
}

/** IMuonMatchQuality interface:
    as inner match chiSquared but simplified to just use diagonal errors */
double
MuonMatchQuality::simpleChi2(const Trk::Track& track1, const Trk::Track& track2) const
{
    MuonMatchQuality::CacheAll CA = setCache(track1, track2);
    return CA.simpleChi2;
}


/* private method to fill cache */
MuonMatchQuality::CacheAll
MuonMatchQuality::setCache(const Trk::Track& track1, const Trk::Track& track2) const
{
    MuonMatchQuality::CacheAll CA;
    // set chi2 to failure settings
    CA.innerMatchChi2        = 999999.;
    CA.simpleChi2            = 999999.;
    CA.innerMatchProbability = 0.;

    // set match degrees of freedom
    CA.innerMatchDOF = innerMatchDOF(track1, track2);
    if (!CA.innerMatchDOF) return CA;

    // parameter difference
    const Trk::Perigee* perigee1 = track1.perigeeParameters();
    const Trk::Perigee* perigee2 = track2.perigeeParameters();

    // coverity false positive (as null pointers cause innerMatchDOF failure)
    if (!perigee1 || !perigee2) return CA;

    double deltaD0     = perigee1->parameters()[Trk::d0] - perigee2->parameters()[Trk::d0];
    double deltaZ0     = perigee1->parameters()[Trk::z0] - perigee2->parameters()[Trk::z0];
    double deltaPhi0   = perigee1->parameters()[Trk::phi0] - perigee2->parameters()[Trk::phi0];
    double deltaTheta  = perigee1->parameters()[Trk::theta] - perigee2->parameters()[Trk::theta];
    double deltaQoverP = perigee1->parameters()[Trk::qOverP] - perigee2->parameters()[Trk::qOverP];

    if (deltaPhi0 < -M_PI) {
        deltaPhi0 += 2. * M_PI;
    } else if (deltaPhi0 > M_PI) {
        deltaPhi0 -= 2. * M_PI;
    }

    // weight matrix for differences
    const AmgSymMatrix(5)* cov1 = perigee1->covariance();
    const AmgSymMatrix(5)* cov2 = perigee2->covariance();

    AmgSymMatrix(5) covariance;
    covariance.setZero();

    if (m_trackQuery->isCombined(track1)) {
        // FIXME: should take weighted difference etc -
        //        but this is anyway unreliable due to rounding errors
        ATH_MSG_WARNING("track1 isCombined: results unreliable ");
    } else if (m_trackQuery->isCombined(track2)) {
        // FIXME: weighted difference etc
        ATH_MSG_WARNING("track2 isCombined: results unreliable ");
    }

    covariance = *cov1 + *cov2 + *m_alignmentUncertainty;
    if (CA.innerMatchDOF < 5) {
        deltaQoverP = 0.;
        for (int i = 0; i != Trk::qOverP; ++i) {
            covariance(i, Trk::qOverP) = 0.;
        }
        for (int i = 0; i != Trk::qOverP; ++i) {
            covariance(Trk::qOverP, i) = 0.;
        }
        covariance(Trk::qOverP, Trk::qOverP) = 1.;
    } else {
        deltaQoverP /= Units::TeV;
        for (int i = 0; i != Trk::qOverP; ++i) {
            covariance(i, Trk::qOverP) /= Units::TeV;
        }
        for (int i = 0; i != Trk::qOverP; ++i) {
            covariance(Trk::qOverP, i) /= Units::TeV;
        }
        covariance(Trk::qOverP, Trk::qOverP) /= (Units::TeV * Units::TeV);
    }

    // invert summed covariance
    AmgMatrix(5, 5) weight = covariance.inverse().eval();

    AmgMatrix(5, 1) paramDifference;
    paramDifference.setZero();
    paramDifference(Trk::d0, 0)     = deltaD0;
    paramDifference(Trk::z0, 0)     = deltaZ0;
    paramDifference(Trk::phi0, 0)   = deltaPhi0;
    paramDifference(Trk::theta, 0)  = deltaTheta;
    paramDifference(Trk::qOverP, 0) = deltaQoverP;

    double matchChi2  = paramDifference.dot(weight * paramDifference);
    CA.innerMatchChi2 = matchChi2;

    // probability of chi2
    if (CA.innerMatchChi2 >= 0. && CA.innerMatchChi2 < 1000.) {
        CA.innerMatchProbability = 1. - Genfun::CumulativeChiSquare(CA.innerMatchDOF)(CA.innerMatchChi2);
    }

    // Protect against zero division
    bool nonzerocov = true;
    for (int k = 0; k < 5; k++) {
        if (covariance(k, k) == 0) nonzerocov = false;
    }

    if (nonzerocov) {
        // simple chi2 as sometimes correlations overestimated
        CA.simpleChi2 = deltaD0 * deltaD0 / covariance(Trk::d0, Trk::d0)
                        + deltaZ0 * deltaZ0 / covariance(Trk::z0, Trk::z0)
                        + deltaPhi0 * deltaPhi0 / covariance(Trk::phi0, Trk::phi0)
                        + deltaTheta * deltaTheta / covariance(Trk::theta, Trk::theta)
                        + deltaQoverP * deltaQoverP / covariance(Trk::qOverP, Trk::qOverP);

        if (CA.simpleChi2 + 5. < CA.innerMatchChi2) {
            ATH_MSG_DEBUG("problem with matchChi2 " << CA.innerMatchChi2 << ", simpleChi2 " << CA.simpleChi2);
        }
    } else {
        ATH_MSG_WARNING("Could not compute simpleChi2. Covariance matrix contains zeros");
    }


    ATH_MSG_VERBOSE(std::setiosflags(std::ios::fixed)
                    << "matchDOF" << std::setw(2) << CA.innerMatchDOF << "  matchChi2:" << std::setw(6)
                    << std::setprecision(1) << CA.innerMatchChi2 << "  simpleChi2:" << std::setw(6)
                    << std::setprecision(1) << CA.simpleChi2 << "     pullD0 " << std::setw(5) << std::setprecision(1)
                    << deltaD0 / sqrt(covariance(0, 0)) << "  pullZ0 " << std::setw(5) << std::setprecision(1)
                    << deltaZ0 / sqrt(covariance(1, 1)) << "  pullPhi0 " << std::setw(5) << std::setprecision(1)
                    << deltaPhi0 / sqrt(covariance(2, 2)) << "  pullTheta " << std::setw(5) << std::setprecision(1)
                    << deltaTheta / sqrt(covariance(3, 3)) << "  pullQoverP " << std::setw(5) << std::setprecision(1)
                    << deltaQoverP / sqrt(covariance(4, 4)));

    return CA;
}


}  // namespace Rec
