/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValHistUtils/ParamPlots.h"

namespace Trk {
  ParamPlots::ParamPlots(PlotBase *pParent, std::string sDir, std::string sParticleType) : PlotBase(pParent, sDir),
    m_sParticleType(sParticleType) {
    eta = NULL;
    phi = NULL;
    pt = NULL;

    eta_phi = NULL;
    eta_pt = NULL;
  }

  void
  ParamPlots::initializePlots() {
    pt = Book1D("pt", m_sParticleType + " pt;" + m_sParticleType + " Transverse Momentum [GeV];Entries / 1 GeV", 200,
                0., 200);
    eta =
      Book1D("eta", m_sParticleType + " eta;" + m_sParticleType + " Pseudo-Rapidity;Entries / 0.05", 128, -3.2, 3.2);
    phi =
      Book1D("phi", m_sParticleType + " phi;" + m_sParticleType + " Azimuthal Angle;Entries / 0.05", 128, -3.2, 3.2);

    eta_pt = Book2D("eta_pt",
                    m_sParticleType + " eta vs pt;" + m_sParticleType + " eta;" + m_sParticleType + " pt;Entries.0.05/1 GeV", 128, -3.2, 3.2, 200, 0.,
                    200);
    eta_phi = Book2D("eta_phi",
                     m_sParticleType + " eta vs phi;" + m_sParticleType + " eta;" + m_sParticleType + " phi;Entries.0.05/0.5", 128, -3.2, 3.2, 128, -3.2,
                     3.2);
  }

  void
  ParamPlots::fill(const xAOD::IParticle &prt) {
    pt->Fill(prt.pt() * 0.001);
    eta->Fill(prt.eta());
    phi->Fill(prt.phi());

    eta_pt->Fill(prt.eta(), prt.pt() * 0.001);
    eta_phi->Fill(prt.eta(), prt.phi());
  }
}
