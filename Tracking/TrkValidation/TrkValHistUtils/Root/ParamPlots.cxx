/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>



#include "TrkValHistUtils/ParamPlots.h"

namespace Trk {
  ParamPlots::ParamPlots(PlotBase *pParent, const std::string& sDir, std::string sParticleType) : PlotBase(pParent, sDir),
    m_sParticleType(std::move(sParticleType)) {
    eta = nullptr;
    phi = nullptr;
    pt = nullptr;

    eta_phi = nullptr;
    eta_pt = nullptr;
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
  ParamPlots::fill(const xAOD::IParticle &prt, float weight) {
    pt->Fill(prt.pt() * 0.001, weight); 
    eta->Fill(prt.eta(),weight);
    phi->Fill(prt.phi(),weight);
    eta_pt->Fill(prt.eta(), prt.pt() * 0.001, weight);
    eta_phi->Fill(prt.eta(), prt.phi(),weight);
  }
}
