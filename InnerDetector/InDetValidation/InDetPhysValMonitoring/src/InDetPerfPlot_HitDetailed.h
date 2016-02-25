/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPERFPLOT_HITDETAILED_H
#define INDETPERFPLOT_HITDETAILED_H

#include "InDetPlotBase.h"
//#include "TProfile.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTruth/TruthParticle.h"



//#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/EfficiencyPlots.h"
#include "TrkValHistUtils/EfficiencyPurityCalculator.h"

class InDetPerfPlot_HitDetailed: public InDetPlotBase {
  public:
  InDetPerfPlot_HitDetailed(InDetPlotBase *pParent, std::string sDir);
    void fill(const xAOD::TrackParticle& trk);

    //void fillDenom(const xAOD::TruthParticle& truth);

    ///@name Hit content of xAOD::TrackParticle as function of eta
    ///@{
    void setEtaBinning(int nbins, float eta_min, float eta_max);

    TProfile* n_vs_eta_BLayerHits;
    TProfile* n_vs_eta_BLayerOutliers;
    TProfile* n_vs_eta_BLayerSharedHits;
    TProfile* n_vs_eta_BLayerSplitHits;
    TProfile* n_vs_eta_DBMHitsNeg;
    TProfile* n_vs_eta_DBMHitsPos;
    TProfile* n_vs_eta_PixelHits;
    TProfile* n_vs_eta_PixelHoles;
    TProfile* n_vs_eta_PixelOutliers;
    TProfile* n_vs_eta_PixelContribLayers;
    TProfile* n_vs_eta_PixelSharedHits;
    TProfile* n_vs_eta_PixelSplitHits;
    TProfile* n_vs_eta_PixelGangedHits;
    TProfile* n_vs_eta_SCTHits;
    TProfile* n_vs_eta_SCTHoles;
    TProfile* n_vs_eta_SCTOutliers;
    TProfile* n_vs_eta_SCTDoubleHoles;
    TProfile* n_vs_eta_SCTSharedHits;
    TProfile* n_vs_eta_TRTHits;
    TProfile* n_vs_eta_TRTHighThresholdHits;
    TProfile* n_vs_eta_TRTOutliers;
    TProfile* n_vs_eta_TRTHighThresholdOutliers;

    TH1* tru_vs_eta_BLayer;
    TH1* BLayer_efficiency;

    ///@}

  private:
    void initializePlots();

    //void finalizePlots();

    int m_etaNBins;
    float m_etaMin, m_etaMax, m_etaMinDBM, m_etaMaxDBM;
};

#endif

