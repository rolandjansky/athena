/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValHistUtils/DefParamPullPlots.h"
#include "xAODTruth/TruthVertex.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

namespace Trk{

void DefParamPullPlots::init() {
    Pull_d0=NULL;
    Pull_z0=NULL;
    Pull_theta=NULL;
    Pull_phi0=NULL;
    Pull_qOverP=NULL;
    Pull_P=NULL;
}

void DefParamPullPlots::initializePlots() {
    Pull_d0 = Book1D("Pull_d0", "d0 Pull;d0 (track - truth) / error;Entries", 100, -5, 5);
    Pull_z0 = Book1D("Pull_z0", "z0 Pull;z0 (track - truth) / error;Entries", 100, -5, 5);
    Pull_phi0 = Book1D("Pull_phi0", "phi0 Pull;phi (track - truth) / error;Entries", 100, -5, 5);
    Pull_theta = Book1D("Pull_theta", "theta Pull;theta (track - truth) / error;Entries", 100, -5, 5);
    Pull_qOverP = Book1D("Pull_qOverP", "qOverP Pull;Q/P (track - truth) / error;Entries", 100, -5, 5);
    Pull_P = Book1D("Pull_P", "Momentum Pull;P (track-truth) / error; Entries", 100, -5, 5);
}

void DefParamPullPlots::fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt) {
    const Perigee& trackPer = trkprt.perigeeParameters();
    const AmgSymMatrix(5) * locError = trackPer.covariance();

    const float phi0_trk   = trkprt.phi0();
    const float theta_trk  = trkprt.theta();
    const float qoverp_trk = trkprt.qOverP()*1000;
    const float z0_trk     = trkprt.z0() + trkprt.vz();
	const float d0_trk	   = trkprt.d0();

    const float err_d0      = Amg::error(*locError,d0);
    const float err_z0      = Amg::error(*locError,z0);
    const float err_phi0    = Amg::error(*locError,phi0);
    const float err_theta   = Amg::error(*locError,theta);
    const float err_qoverp  = Amg::error(*locError,qOverP)*1000;

    const float charge = (truthprt.pdgId() > 0) ? -1.0 : 1.0;
    const float p_truth = truthprt.pt()*cosh(truthprt.eta())/1000.;
    const float theta_truth = 2*atan(exp(-truthprt.eta()));

//bug in MS track has some cov matrix = identity matrix -> errors all equal 1
    Pull_phi0->Fill( ( phi0_trk - truthprt.phi() ) / err_phi0 );
    Pull_theta->Fill( ( theta_trk - theta_truth ) / err_theta );
    Pull_qOverP->Fill( ( qoverp_trk - charge/p_truth ) / err_qoverp );
    if (qoverp_trk != 0) {
        const float err_p = err_qoverp / qoverp_trk / qoverp_trk;
        Pull_P->Fill( ( 1./fabs(qoverp_trk) - p_truth ) / err_p );
    }
    //d0 & z0 require truth vertex - not available in xAOD truth muons yet
    if (truthprt.hasProdVtx()) {
        Pull_d0->Fill( ( d0_trk - truthprt.prodVtx()->perp() ) / err_d0 );
        Pull_z0->Fill( ( z0_trk - truthprt.prodVtx()->z() ) / err_z0 );
    }
}

}
