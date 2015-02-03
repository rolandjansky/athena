/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValHistUtils/DefParamPullPlots.h"
#include "xAODTruth/TruthVertex.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

namespace Trk{

void DefParamPullPlots::init() {
    Pull_d0     = NULL;
    Pull_z0     = NULL;
    Pull_theta  = NULL;
    Pull_phi0   = NULL;
    Pull_qOverP = NULL;
    Pull_P      = NULL;

    Pull_d0_vs_pt     = NULL;
    Pull_z0_vs_pt     = NULL;
    Pull_theta_vs_pt  = NULL;
    Pull_phi0_vs_pt   = NULL;
    Pull_qOverP_vs_pt = NULL;
    Pull_P_vs_pt      = NULL;
}

void DefParamPullPlots::initializePlots() {
    Pull_d0     = Book1D( "Pull_d0",     "d0 Pull;d0 (track - truth) / error;Entries",       100, -5, 5 );
    Pull_z0     = Book1D( "Pull_z0",     "z0 Pull;z0 (track - truth) / error;Entries",       100, -5, 5 );
    Pull_phi0   = Book1D( "Pull_phi0",   "phi0 Pull;phi (track - truth) / error;Entries",    100, -5, 5 );
    Pull_theta  = Book1D( "Pull_theta",  "theta Pull;theta (track - truth) / error;Entries", 100, -5, 5 );
    Pull_qOverP = Book1D( "Pull_qOverP", "qOverP Pull;Q/P (track - truth) / error;Entries",  100, -5, 5 );
    Pull_P      = Book1D( "Pull_P",      "Momentum Pull;P (track-truth) / error; Entries",   100, -5, 5 );

    Pull_d0_vs_pt     = Book2D( "Pull_d0_vs_pt",     "pt; d0 Pull vs pt; d0 Pull; Entries",             200, 0, 200, 100, -5, 5 );
    Pull_z0_vs_pt     = Book2D( "Pull_z0_vs_pt",     "pt; z0 Pull vs pt; z0 Pull; Entries",             200, 0, 200, 100, -5, 5 );
    Pull_phi0_vs_pt   = Book2D( "Pull_phi0_vs_pt",   "pt; phi0 Pull vs pt; phi Pull; Entries",          200, 0, 200, 100, -5, 5 );
    Pull_theta_vs_pt  = Book2D( "Pull_theta_vs_pt",  "pt; theta Pull vs pt; theta Pull; Entries",       200, 0, 200, 100, -5, 5 );
    Pull_qOverP_vs_pt = Book2D( "Pull_qOverP_vs_pt", "pt; qOverP Pull vs pt; Q/P Pull; Entries",        200, 0, 200, 100, -5, 5 );
    Pull_P_vs_pt      = Book2D( "Pull_P_vs_pt",      "pt; Momentum Pull vs pt; Momentum Pull; Entries", 200, 0, 200, 100, -5, 5 );
}

void DefParamPullPlots::fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt) {
    const Perigee& trackPer = trkprt.perigeeParameters();
    const AmgSymMatrix(5) * locError = trackPer.covariance();

    const float d0_trk     = trkprt.d0();
    const float z0_trk     = trkprt.z0();
    const float phi_trk    = trkprt.phi0();
    const float theta_trk  = trkprt.theta();
    const float qoverp_trk = trkprt.qOverP();
    const float pt_trk     = trkprt.pt()/1000.;

    const float err_d0     = Amg::error( *locError, d0     );
    const float err_z0     = Amg::error( *locError, z0     );
    const float err_phi    = Amg::error( *locError, phi0   );
    const float err_theta  = Amg::error( *locError, theta  );
    const float err_qoverp = Amg::error( *locError, qOverP );

    const float phi_truth    = truthprt.phi();
    const float theta_truth  = 2*atan(exp(-truthprt.eta()));
    const float p_truth      = truthprt.pt()*cosh(truthprt.eta());
    const float qoverp_truth = truthprt.charge()/p_truth;

    Pull_d0->Fill( d0_trk / err_d0 ); // assumes d0_truth = 0
    Pull_phi0->Fill( ( phi_trk - phi_truth ) / err_phi );
    Pull_theta->Fill( ( theta_trk - theta_truth ) / err_theta );
    Pull_qOverP->Fill( ( qoverp_trk - qoverp_truth ) / err_qoverp );

    Pull_d0_vs_pt->Fill( pt_trk, d0_trk / err_d0 );
    Pull_phi0_vs_pt->Fill( pt_trk, ( phi_trk - phi_truth ) / err_phi );
    Pull_theta_vs_pt->Fill( pt_trk, ( theta_trk - theta_truth ) / err_theta );
    Pull_qOverP_vs_pt->Fill( pt_trk, ( qoverp_trk - qoverp_truth ) / err_qoverp );


    if (qoverp_trk != 0) {
        const float p_trk = 1./fabs(qoverp_trk);
        const float err_p = err_qoverp / qoverp_trk / qoverp_trk;
        Pull_P->Fill( ( p_trk - p_truth ) / err_p );
        Pull_P_vs_pt->Fill( pt_trk, ( p_trk - p_truth ) / err_p );
    }

    if (truthprt.hasProdVtx()) {
        const float z0_truth = truthprt.prodVtx()->z() - trkprt.vz();
        Pull_z0->Fill( ( z0_trk - z0_truth ) / err_z0 );
        Pull_z0_vs_pt->Fill( pt_trk, ( z0_trk - z0_truth ) / err_z0 );
    }
}

}