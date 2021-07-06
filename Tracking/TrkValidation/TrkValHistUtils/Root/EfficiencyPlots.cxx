/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValHistUtils/EfficiencyPlots.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/EfficiencyPurityCalculator.h"

namespace Trk {
  EfficiencyPlots::EfficiencyPlots(PlotBase *pParent, const std::string& sDir, ParamPlots *pNumPlots,
                                   ParamPlots *pDenomPlots) : PlotBase(pParent, sDir) {
    m_pNumPlots = pNumPlots;
    m_pDenomPlots = pDenomPlots;

    eff_eta = nullptr;
    eff_phi = nullptr;
    eff_pt = nullptr;
    eff_pti = nullptr;
  }

  void
  EfficiencyPlots::initializePlots() {
    eff_eta = Book1D("Eff_eta", m_pNumPlots->eta, "Eta Efficiency; eta; efficiency");
    eff_phi = Book1D("Eff_phi", m_pNumPlots->phi, "Phi Efficiency;Azimuthal Angle; efficiency");
    eff_pt = Book1D("Eff_pt", m_pNumPlots->pt, "Pt Efficiency; Transverse Momentum [GeV]; efficiency");
    eff_pti = Book1D("Eff_pti", m_pNumPlots->pt,
                     "Pt integrated Efficiency; Integrated Transverse Momentum [GeV]; efficiency");
  }

  void
  EfficiencyPlots::finalizePlots() {
    EfficiencyPurityCalculator calc;

    calc.calculateEfficiency(m_pDenomPlots->eta, m_pNumPlots->eta, eff_eta);
    calc.calculateEfficiency(m_pDenomPlots->phi, m_pNumPlots->phi, eff_phi);
    calc.calculateEfficiency(m_pDenomPlots->pt, m_pNumPlots->pt, eff_pt);
    calc.calculateIntegrated(m_pDenomPlots->pt, m_pNumPlots->pt, eff_pti, 1, EfficiencyPurityCalculator::kX);
  }
}
