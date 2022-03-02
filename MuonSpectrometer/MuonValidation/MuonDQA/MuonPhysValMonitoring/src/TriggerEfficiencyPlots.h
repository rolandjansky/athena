/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHYSVALMONITORING_TRIGGEREFFICIENCYPLOTS_H
#define MUONPHYSVALMONITORING_TRIGGEREFFICIENCYPLOTS_H

#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/PlotBase.h"
#include "xAODMuon/Muon.h"

/// class holding Pt plots for Inner Detector RTT Validation and implementing fill methods
class TriggerEfficiencyPlots : public PlotBase {
public:
    TriggerEfficiencyPlots(PlotBase* pParent, const std::string& dirName);
    void fillNumerator(const xAOD::Muon& mu, float Treshold);
    void fillDenominator(const xAOD::Muon& mu, float Treshold);
    void fillFeatures(const xAOD::Muon& mu, float Treshold);

    Trk::ParamPlots m_pDenomPlots;
    Trk::ParamPlots m_pNumPlots;
    Trk::ParamPlots m_pFeatPlots;

    Trk::ParamPlots m_pDenomPlots_threshold;
    Trk::ParamPlots m_pNumPlots_threshold;
    Trk::ParamPlots m_pFeatPlots_threshold;
};

#endif
