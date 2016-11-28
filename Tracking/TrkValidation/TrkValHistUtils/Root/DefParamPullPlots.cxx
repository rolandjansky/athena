/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValHistUtils/DefParamPullPlots.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

namespace Trk {
  void
  DefParamPullPlots::init() {
    Pull_d0 = NULL;
    Pull_z0 = NULL;
    Pull_theta = NULL;
    Pull_phi = NULL;
    Pull_qOverP = NULL;

    Pull_d0_vs_pt = NULL;
    Pull_z0_vs_pt = NULL;
    Pull_theta_vs_pt = NULL;
    Pull_phi_vs_pt = NULL;
    Pull_qOverP_vs_pt = NULL;
  }

  void
  DefParamPullPlots::initializePlots() {
    Pull_d0 =
      Book1D("Pull" + m_sType + "_d0", "d0 Pull " + m_sType + ";d0 (track - truth) / error;Entries", 100, -5, 5);
    Pull_z0 =
      Book1D("Pull" + m_sType + "_z0", "z0 Pull " + m_sType + ";z0 (track - truth) / error;Entries", 100, -5, 5);
    Pull_phi = Book1D("Pull" + m_sType + "_phi", "phi Pull " + m_sType + ";phi (track - truth) / error;Entries", 100,
                      -5, 5);
    Pull_theta = Book1D("Pull" + m_sType + "_theta", "theta Pull " + m_sType + ";theta (track - truth) / error;Entries",
                        100, -5, 5);
    Pull_qOverP = Book1D("Pull" + m_sType + "_qOverP",
                         "qOverP Pull " + m_sType + ";Q/P (track - truth) / error;Entries", 100, -5, 5);

    Double_t Pull_ptBins[8] = {
      5, 10, 25, 40, 60, 100, 200, 1000
    };
    std::pair<Int_t, Double_t *> ptBins(7, Pull_ptBins);
    Pull_d0_vs_pt = Book2D("Pull" + m_sType + "_d0_vs_pt", m_sType + " d0 Pull vs pt;pt;d0 Pull", ptBins.first,
                           ptBins.second, 160, -8, 8);
    Pull_z0_vs_pt = Book2D("Pull" + m_sType + "_z0_vs_pt", m_sType + " z0 Pull vs pt;pt;z0 Pull", ptBins.first,
                           ptBins.second, 160, -8, 8);
    Pull_phi_vs_pt = Book2D("Pull" + m_sType + "_phi_vs_pt", m_sType + " phi Pull vs pt;pt;phi Pull", ptBins.first,
                            ptBins.second, 160, -8, 8);
    Pull_theta_vs_pt = Book2D("Pull" + m_sType + "_theta_vs_pt", m_sType + " theta Pull vs pt;pt;theta Pull",
                              ptBins.first, ptBins.second, 160, -8, 8);
    Pull_qOverP_vs_pt = Book2D("Pull" + m_sType + "_qOverP_vs_pt", m_sType + " qOverP Pull vs pt;pt;Q/P Pull",
                               ptBins.first, ptBins.second, 160, -8, 8);
  }

  void
  DefParamPullPlots::fill(const xAOD::TrackParticle &trkprt, const xAOD::TruthParticle &truthprt) {
    const double d0_trk = trkprt.d0();
    const double z0_trk = trkprt.z0();
    const double phi_trk = trkprt.phi0();
    const double theta_trk = trkprt.theta();
    const double qoverp_trk = trkprt.qOverP();

#if !defined(ROOTCORE) && defined(STANDALONE)
    const Perigee &trackPer = trkprt.perigeeParameters();
    const AmgSymMatrix(5) * locError = trackPer.covariance();

    const double err_d0 = Amg::error(*locError, d0);
    const double err_z0 = Amg::error(*locError, z0);
    const double err_phi = Amg::error(*locError, phi0);
    const double err_theta = Amg::error(*locError, theta);
    const double err_qoverp = Amg::error(*locError, qOverP);
#else
    const AmgSymMatrix(5) locError = trkprt.definingParametersCovMatrix();

    const double err_d0 = Amg::error(locError, 0); // d0
    const double err_z0 = Amg::error(locError, 1);// z0
    const double err_phi = Amg::error(locError, 2); // phi
    const double err_theta = Amg::error(locError, 3); // theta
    const double err_qoverp = Amg::error(locError, 4); // qOverP
#endif

    const double d0_truth = 0;
    const double phi_truth = truthprt.phi();
    const double theta_truth = 2 * atan(exp(-truthprt.eta()));
    const double qoverp_truth = truthprt.charge() / (truthprt.pt() * cosh(truthprt.eta()));
    const double pt_truth = truthprt.pt() / 1000.;

    const double d0_pull = (d0_trk - d0_truth) / err_d0;
    const double phi_pull = (phi_trk - phi_truth) / err_phi;
    const double theta_pull = (theta_trk - theta_truth) / err_theta;
    const double qoverp_pull = (qoverp_trk - qoverp_truth) / err_qoverp;

    if (truthprt.hasProdVtx()) {
      const double z0_truth = truthprt.prodVtx()->z() - trkprt.vz();
      const double z0_pull = (z0_trk - z0_truth) / err_z0;
      Pull_z0->Fill(z0_pull);
      Pull_z0_vs_pt->Fill(pt_truth, z0_pull);
    }

    Pull_d0->Fill(d0_pull);
    Pull_phi->Fill(phi_pull);
    Pull_theta->Fill(theta_pull);
    Pull_qOverP->Fill(qoverp_pull);

    Pull_d0_vs_pt->Fill(pt_truth, d0_pull);
    Pull_phi_vs_pt->Fill(pt_truth, phi_pull);
    Pull_theta_vs_pt->Fill(pt_truth, theta_pull);
    Pull_qOverP_vs_pt->Fill(pt_truth, qoverp_pull);
  }
}
