/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackTagTestTool.h"

#include "GaudiKernel/MsgStream.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkTrack/Track.h"

#ifdef MUONCOMBDEBUG
#include "AtlasHepMC/GenParticle.h"
#include "TrkTruthData/TrackTruth.h"
#include "TrkTruthData/TrackTruthCollection.h"
#endif

using namespace MuonCombined;


MuonTrackTagTestTool::MuonTrackTagTestTool(const std::string &type, const std::string &name, const IInterface *parent) :
  AthAlgTool(type, name, parent) {
    declareInterface<IMuonTrackTagTool>(this);
    declareProperty("Chi2Cut", m_chi2cut = 50.);
#ifdef MUONCOMBDEBUG
    declareProperty("Truth", m_truth = false);
#endif
    m_msEntrance       = 0;
    m_trackingGeometry = 0;
}

StatusCode
MuonTrackTagTestTool::initialize()
{
    ATH_CHECK(m_extrapolator.retrieve());

    if (!m_trackingGeometrySvc.empty()) {
        ATH_CHECK(m_trackingGeometrySvc.retrieve());
        msg(MSG::INFO) << "  geometry Svc " << m_trackingGeometrySvc << " retrieved " << endmsg;
    }

    msg(MSG::INFO) << "Initialized successfully" << endmsg;

    return StatusCode::SUCCESS;
}

double
MuonTrackTagTestTool::chi2(const Trk::Track &idTrack, const Trk::Track &msTrack) const
{
    std::call_once(m_trackingOnceFlag, [&]() {
        m_trackingGeometry = m_trackingGeometrySvc->trackingGeometry();
        if (m_trackingGeometry) m_msEntrance = m_trackingGeometry->trackingVolume("MuonSpectrometerEntrance");
        if (!m_msEntrance) msg(MSG::ERROR) << "MS entrance not available" << endmsg;
    });

    if (idTrack.perigeeParameters() == 0) {
        msg(MSG::WARNING) << "Skipping track combination - no perigee parameters for ID track" << endmsg;
        return 1e15;
    }
    if (msTrack.perigeeParameters() == 0) {
        msg(MSG::WARNING) << "Skipping track combination - no perigee parameters for MS track" << endmsg;
        return 1e5;
    }
    // skip tracks from backtracking
    if (dynamic_cast<const Trk::StraightLineSurface *>(&(**idTrack.measurementsOnTrack()->begin()).associatedSurface()))
        return 0;
    if (idTrack.measurementsOnTrack()->size() < 7
        || dynamic_cast<const Trk::StraightLineSurface *>(&(*idTrack.measurementsOnTrack())[6]->associatedSurface())
        || !dynamic_cast<const InDet::PixelClusterOnTrack *>(*idTrack.measurementsOnTrack()->begin()))
        return 0;
    DataVector<const Trk::TrackStateOnSurface>::const_iterator itStates = idTrack.trackStateOnSurfaces()->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator endState = idTrack.trackStateOnSurfaces()->end();
    int                                                        noutl = 0, ntrt = 0;
    for (; itStates != endState; ++itStates) {
        if ((**itStates).measurementOnTrack()) {
            const InDet::TRT_DriftCircleOnTrack *trthit =
                dynamic_cast<const InDet::TRT_DriftCircleOnTrack *>((**itStates).measurementOnTrack());
            if (trthit) {
                if ((**itStates).type(Trk::TrackStateOnSurface::Outlier))
                    noutl++;
                else
                    ntrt++;
            }
        }
    }
    // std::cout << "noutl: " << noutl << std::endl;
    double eta = idTrack.perigeeParameters()->eta();
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ntrt: " << ntrt << " ntrtoutl: " << noutl << " eta: " << eta << endmsg;
    if (noutl >= 15 || (ntrt == 0 && std::abs(eta) > .1 && std::abs(eta) < 1.9)) return 0;

    // skip tracks below 2.5 GeV
    double idqoverp = idTrack.perigeeParameters()->parameters()[Trk::qOverP];
    if (idqoverp != 0 && fabs(1 / idqoverp) < 2500) return 0;

    const Trk::TrackParameters *muonpar = msTrack.trackParameters()->front();

    bool checkphiflip = false, muonisstraight = false;
    /* double p_ms= (!idqoverp) ? 1.e9 : fabs(1/idqoverp)-4*GeV;
    if (p_ms<.5*GeV) p_ms=.5*GeV;
    double deltatheta=.3e3*10/p_ms;
    if (idTrack.perigeeParameters()->parameters()[Trk::theta]+deltatheta>M_PI ||
    idTrack.perigeeParameters()->parameters()[Trk::theta]-deltatheta<0) checkphiflip=true;
    */
    if (std::abs(muonpar->parameters()[Trk::qOverP]) < 1.e-9) checkphiflip = muonisstraight = true;

    double phiID = (**idTrack.trackParameters()->rbegin()).parameters()[Trk::phi], phiMS = muonpar->position().phi();
    double thetaID = (**idTrack.trackParameters()->rbegin()).parameters()[Trk::theta],
           thetaMS = muonpar->parameters()[Trk::theta];
    if (msgLvl(MSG::DEBUG))
        msg(MSG::DEBUG) << "phi ID: " << phiID << " phi MS: " << phiMS << " diff: " << phiID - phiMS
                        << " pt ID: " << idTrack.perigeeParameters()->pT() << " pt ms: " << muonpar->pT() << endmsg;
    if (msgLvl(MSG::DEBUG))
        msg(MSG::DEBUG) << "theta ID: " << thetaID << " theta MS: " << thetaMS << " diff: " << thetaID - thetaMS
                        << endmsg;
    double phidiff = fabs(phiID - phiMS);
    if (fabs(phidiff - 2 * M_PI) < phidiff) phidiff = 2 * M_PI - phidiff;
    if (checkphiflip && fabs(phidiff - M_PI) < phidiff) phidiff = fabs(M_PI - phidiff);
    double thetalimit = .6, philimit = .8;
    if (muonisstraight) {
        thetalimit = 1.;
        philimit   = 2;
    }
    if (!(fabs(thetaID - thetaMS) < thetalimit && fabs(phidiff) < philimit)) return 0;

    const Trk::TrackParameters *lastmeasidpar = 0;
    int                         index         = (int)idTrack.trackParameters()->size();
    while (!lastmeasidpar && index > 0) {
        index--;
        lastmeasidpar = (*idTrack.trackParameters())[index]->covariance() ? (*idTrack.trackParameters())[index] : 0;
    }
    if (!lastmeasidpar) {
        msg(MSG::WARNING) << "ID track parameters don't have error matrix!" << endmsg;
        return 0;
    }

    const Trk::TrackParameters *                               mspar  = 0;
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosit = msTrack.trackStateOnSurfaces()->begin();

    while (tsosit != msTrack.trackStateOnSurfaces()->end() && !mspar) {
        if ((**tsosit).type(Trk::TrackStateOnSurface::Measurement)
            && !(**tsosit).type(Trk::TrackStateOnSurface::Outlier)) {
            mspar = (**tsosit).trackParameters();
        }
        tsosit++;
    }

    if (!mspar) {
        msg(MSG::WARNING) << "Could not find muon track parameters!" << endmsg;
        return 0;
    }

    std::unique_ptr<const Trk::TrackParameters> idextrapolatedpar = std::unique_ptr<const Trk::TrackParameters>(
        m_extrapolator->extrapolateToVolume(*lastmeasidpar, *m_msEntrance, Trk::alongMomentum, Trk::muon));

    if (!idextrapolatedpar && lastmeasidpar->parameters()[Trk::qOverP] != 0
        && std::abs(1. / lastmeasidpar->parameters()[Trk::qOverP]) < 5. * CLHEP::GeV)
    {
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Extrapolating with p=5 GeV" << endmsg;
        AmgVector(5) params = lastmeasidpar->parameters();
        double sign         = (params[Trk::qOverP] > 0) ? 1 : -1;
        double newqoverp    = sign / (5. * CLHEP::GeV);
        params[Trk::qOverP] = newqoverp;
        std::unique_ptr<const Trk::TrackParameters> newlastidpar =
            std::unique_ptr<const Trk::TrackParameters>(lastmeasidpar->associatedSurface().createTrackParameters(
                params[0], params[1], params[2], params[3], params[4],
                new AmgSymMatrix(5)(*lastmeasidpar->covariance())));
        if (newlastidpar) {
            idextrapolatedpar = std::unique_ptr<const Trk::TrackParameters>(
                m_extrapolator->extrapolateToVolume(*newlastidpar, *m_msEntrance, Trk::alongMomentum, Trk::muon));
        }
    }

    if (!idextrapolatedpar || !idextrapolatedpar->covariance()) {
        if (msgLvl(MSG::DEBUG))
            msg(MSG::DEBUG) << "ID extrapolated par null or missing error matrix, par: " << idextrapolatedpar.get()
                            << endmsg;
        return 0;
    }
    const Trk::TrackParameters *                msparforextrapolator = mspar;
    std::unique_ptr<const Trk::TrackParameters> created_mspar;
    if (muonisstraight) {
        const AmgSymMatrix(5) &idcovmat = *idextrapolatedpar->covariance();
        AmgVector(5) params             = mspar->parameters();
        params[Trk::qOverP]             = idextrapolatedpar->parameters()[Trk::qOverP];
        if (!mspar->covariance()) {
            ATH_MSG_DEBUG("Muons parameters missing Error matrix: " << mspar);
            return 1e5;  // Sometimes it's 0, sometimes 1e15. Maybe for comparison of chi2? Just in case, will copy this
                         // value from earlier check on ms track. EJWM.
        }
        AmgSymMatrix(5) *newcovmat = new AmgSymMatrix(5)(*mspar->covariance());
        for (int i = 0; i < 5; i++) (*newcovmat)(i, 4) = idcovmat(i, 4);
        created_mspar =
            std::unique_ptr<const Trk::TrackParameters>(msparforextrapolator->associatedSurface().createTrackParameters(
                params[0], params[1], params[2], params[3], params[4], newcovmat));
        msparforextrapolator = created_mspar.get();
    }
    Trk::PropDirection    propdir = Trk::oppositeMomentum;
    Trk::DistanceSolution distsol = idextrapolatedpar->associatedSurface().straightLineDistanceEstimate(
        msparforextrapolator->position(), msparforextrapolator->momentum().unit());
    double distance = 0;
    if (distsol.numberOfSolutions() == 1)
        distance = distsol.first();
    else if (distsol.numberOfSolutions() == 2) {
        distance = (std::abs(distsol.first()) < std::abs(distsol.second())) ? distsol.first() : distsol.second();
    }
    // std::cout << "distance: " << distance << std::endl;
    if (distance > 0 && distsol.numberOfSolutions() > 0) propdir = Trk::alongMomentum;

    std::unique_ptr<const Trk::TrackParameters> msextrapolatedpar =
        std::unique_ptr<const Trk::TrackParameters>(m_extrapolator->extrapolate(
            *msparforextrapolator, idextrapolatedpar->associatedSurface(), propdir, false, Trk::muon));

    if (muonisstraight) {
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Muon track is straight line" << endmsg;
    }
    // std::cout << "idpar: " << measidpar << " mspar: " << measmspar << std::endl;

    if ((!msextrapolatedpar && !muonisstraight)) {
        msg(MSG::DEBUG) << "extrapolation failed, id:" << idextrapolatedpar.get() << " ms: " << msextrapolatedpar.get()
                        << endmsg;

        return 0;
    }
    double mychi2 = 1e15;
    if (msextrapolatedpar) mychi2 = chi2(*idextrapolatedpar, *msextrapolatedpar);
    if (muonisstraight) {
        std::unique_ptr<const Trk::TrackParameters> idpar_firsthit =
            std::unique_ptr<const Trk::TrackParameters>(m_extrapolator->extrapolate(
                *idextrapolatedpar, mspar->associatedSurface(), Trk::alongMomentum, false, Trk::muon));
        if (idpar_firsthit) {
            double chi2_2 = chi2(*idpar_firsthit, *mspar);
            if (chi2_2 < mychi2) mychi2 = chi2_2;
        }
    }
    return mychi2;
}


double
MuonTrackTagTestTool::chi2(const Trk::TrackParameters &idextrapolatedpar,
                           const Trk::TrackParameters &msextrapolatedpar) const
{
    double loc1ID  = idextrapolatedpar.parameters()[Trk::loc1];
    double loc2ID  = idextrapolatedpar.parameters()[Trk::loc2];
    double phiID   = idextrapolatedpar.parameters()[Trk::phi];
    double thetaID = idextrapolatedpar.parameters()[Trk::theta];

    double loc1MS  = msextrapolatedpar.parameters()[Trk::loc1];
    double loc2MS  = msextrapolatedpar.parameters()[Trk::loc2];
    double phiMS   = msextrapolatedpar.parameters()[Trk::phi];
    double thetaMS = msextrapolatedpar.parameters()[Trk::theta];
    // std::cout << "idpar: " << *idpar << " mspar: " << *mspar << std::endl;
    if (!idextrapolatedpar.covariance() || !msextrapolatedpar.covariance()) {
        if (msgLvl(MSG::DEBUG))
            msg(MSG::DEBUG) << "track parameters don't have error matrix! id: " << idextrapolatedpar.covariance()
                            << " ms: " << msextrapolatedpar.covariance() << endmsg;
        return 1e15;
    }
    const AmgSymMatrix(5) &idcovmat = *idextrapolatedpar.covariance();
    const AmgSymMatrix(5) &mscovmat = *msextrapolatedpar.covariance();
    // std::cout << "idpar: " << *idextrapolatedpar << " pos: " << idextrapolatedpar.position() << " mspar: " <<
    // *msextrapolatedpar << " position: " << msextrapolatedpar.position() << std::endl;
    double                      loc1diff = fabs(loc1ID - loc1MS);
    double                      loc2diff = fabs(loc2ID - loc2MS);
    const Trk::CylinderSurface *cylsurf =
        dynamic_cast<const Trk::CylinderSurface *>(&idextrapolatedpar.associatedSurface());
    const Trk::DiscSurface *discsurf = dynamic_cast<const Trk::DiscSurface *>(&idextrapolatedpar.associatedSurface());

    if (cylsurf) {
        double length = 2 * M_PI * cylsurf->bounds().r();
        if (fabs(loc1diff - length) < loc1diff) loc1diff = length - loc1diff;
    }
    if (discsurf) {
        if (fabs(loc2diff - 2 * M_PI) < loc2diff) loc2diff = 2 * M_PI - loc2diff;
    }
    double phidiff = fabs(phiID - phiMS);
    if (fabs(phidiff - 2 * M_PI) < phidiff) phidiff = 2 * M_PI - phidiff;
    if (fabs(phidiff - M_PI) < phidiff) phidiff -= M_PI;  // catch singularity in phi near theta=0

    double thetadiff = thetaID - thetaMS;
    // std::cout << "loc1diff: " << loc1diff << " loc2diff: " << loc2diff << " phidiff: " << phidiff << " thetadiff: "
    // << thetadiff << " idcov: " << idcovmat << " mscov: " << mscovmat << " idcovmat+mscovmat: " << idcovmat+mscovmat
    // << std::endl;
    double chi2 = loc1diff * loc1diff / (idcovmat(0, 0) + mscovmat(0, 0))
                  + loc2diff * loc2diff / (idcovmat(1, 1) + mscovmat(1, 1))
                  + phidiff * phidiff / (idcovmat(2, 2) + mscovmat(2, 2))
                  + thetadiff * thetadiff / (idcovmat(3, 3) + mscovmat(3, 3));
    chi2 = std::abs(chi2);
    // if (goodmatch) std::cout << "chi2 " << chi2 << std::endl;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " chi2: " << chi2 << endmsg;
    return chi2;
}
