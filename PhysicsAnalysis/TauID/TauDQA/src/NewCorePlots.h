/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUDQA_NEWCOREPLOTS_H
#define TAUDQA_NEWCOREPLOTS_H

#include "GeneralTauPlots.h"
#include "xAODTau/TauJet.h"

namespace Tau{

class NewCorePlots: public PlotBase {
  public:
    NewCorePlots(PlotBase *pParent, const std::string& sDir, std::string sTauJetContainerName);
    virtual ~NewCorePlots();
    void fill(const xAOD::TauJet& tau, float weight);

    TH1*  m_ipZ0SinThetaSigLeadTrk;
    TH1*  m_etOverPtLeadTrk;
    TH1*  m_ipSigLeadTrk;
    TH1*  m_massTrkSys;
    TH1*  m_trkWidth2;
    TH1*  m_trFlightPathSig;
    TH1*  m_etEflow;
    TH1*  m_nPi0;
    TH1*  m_tauDRMax;
    TH1*  m_EMRadius;
    TH1*  m_hadRadius;
    TH1*  m_isolFrac;
    TH1*  m_centFrac;
    TH1*  m_stripWidth2;
    TH1*  m_nStrip;
    TH1*  m_trkAvgDist;
    TH1*  m_lead2ClusterEOverAllClusterE;
    TH1*  m_lead3ClusterEOverAllClusterE;
    TH1*  m_caloIso;
    TH1*  m_mEflowTopo;
    TH1*  m_ptRatioEflowTopo;
    TH1*  m_nPi0Topo;
    TH1*  m_PSSFraction;
    TH1*  m_ChPiEMEOverCaloEME;
    TH1*  m_EMPOverTrkSysP;
    TH1*  m_innerTrkAvgDist;
    TH1*  m_ptRatioEflowApprox;
    TH1*  m_mEflowApprox;

    TH1* m_logSeedJetPt = nullptr;
    TH1* m_track_logPt = nullptr;
    TH1* m_track_eta = nullptr;
    TH1* m_track_d0TJVA = nullptr;
    TH1* m_track_z0SinthetaTJVA = nullptr;
    TH1* m_track_d0SigTJVA = nullptr;
    TH1* m_track_z0sinthetaSigTJVA = nullptr;
    TH1* m_track_charge = nullptr;
    TH1* m_track_qOverP = nullptr;
    TH1* m_track_logRConv = nullptr;
    TH1* m_track_tanhRConvII = nullptr;
    TH1* m_track_ptRatioSeedJet = nullptr;
    TH1* m_track_dRJetSeedAxis = nullptr;
    TH1* m_track_nInnermostPixHits = nullptr;
    TH1* m_track_nPixHits = nullptr;
    TH1* m_track_nPixelSharedHits = nullptr;
    TH1* m_track_nSiHits = nullptr;
    TH1* m_track_nSCTSharedHits = nullptr;
    TH1* m_track_nTRTHits = nullptr;
    TH1* m_track_eProbabilityHT = nullptr;
    TH1* m_track_eProbabilityNN = nullptr;
    TH1* m_track_eProbabilityHTorNN = nullptr;
    TH1* m_track_idScoreCharged = nullptr;
    TH1* m_track_idScoreIso = nullptr;
    TH1* m_track_idScoreConv = nullptr;
    TH1* m_track_idScoreFake = nullptr;

    TH1* m_cluster_logEt = nullptr;
    TH1* m_cluster_eta = nullptr;
    TH1* m_cluster_phi = nullptr;
    TH1* m_cluster_secondR = nullptr;
    TH1* m_cluster_secondLambda = nullptr;
    TH1* m_cluster_centerLambda = nullptr;
    TH1* m_cluster_firstEngDens = nullptr;
    TH1* m_cluster_EMproba = nullptr;

    TH1* m_clustersMeanCenterLambda = nullptr;
    TH1* m_clustersMeanFirstEngDens = nullptr;
    TH1* m_clustersMeanEMProbability = nullptr;
    TH1* m_clustersMeanSecondLambda = nullptr;
    TH1* m_clustersMeanPresamplerFrac = nullptr;
    TH1* m_PFOEngRelDiff = nullptr;

    TH1* m_TVz = nullptr;

  private:
    void initializePlots();
    std::string m_sTauJetContainerName;
};

}

#endif
