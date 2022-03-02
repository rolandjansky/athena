/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerEfficiencyPlots.h"

TriggerEfficiencyPlots::TriggerEfficiencyPlots(PlotBase* pParent, const std::string& sDir) :
    PlotBase(pParent, sDir),
    m_pDenomPlots(this, "Denom/", "Muons"),
    m_pNumPlots(this, "Numer/", "Muons"),
    m_pFeatPlots(this, "Features/", "Muons"),
    m_pDenomPlots_threshold(this, "DenomT/", "Muons"),
    m_pNumPlots_threshold(this, "NumerT/", "Muons"),
    m_pFeatPlots_threshold(this, "FeaturesT/", "Muons") {}

void TriggerEfficiencyPlots::fillNumerator(const xAOD::Muon& mu, float Treshold) {
    m_pNumPlots.fill(mu);
    if (mu.pt() > Treshold) m_pNumPlots_threshold.fill(mu);
}

void TriggerEfficiencyPlots::fillDenominator(const xAOD::Muon& mu, float Treshold) {
    m_pDenomPlots.fill(mu);
    if (mu.pt() > Treshold) m_pDenomPlots_threshold.fill(mu);
}

void TriggerEfficiencyPlots::fillFeatures(const xAOD::Muon& mu, float Treshold) {
    m_pFeatPlots.fill(mu);
    if (mu.pt() > Treshold) m_pFeatPlots_threshold.fill(mu);
}
