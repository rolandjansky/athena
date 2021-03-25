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
    NewCorePlots(PlotBase *pParent, std::string sDir, std::string sTauJetContainerName);
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

    TH1* m_logSeedJetPt;
    TH1* m_track_logPt;
    TH1* m_track_eta;
    TH1* m_track_d0TJVA;
    TH1* m_track_z0SinthetaTJVA;
    TH1* m_track_d0SigTJVA;
    TH1* m_track_z0sinthetaSigTJVA;
    TH1* m_track_charge;
    TH1* m_track_qOverP;
    TH1* m_track_logRConv;
    TH1* m_track_tanhRConvII;
    TH1* m_track_ptRatioSeedJet;
    TH1* m_track_dRJetSeedAxis;
    TH1* m_track_nInnermostPixHits;
    TH1* m_track_nPixHits;
    TH1* m_track_nPixelSharedHits;
    TH1* m_track_nSiHits;
    TH1* m_track_nSCTSharedHits;
    TH1* m_track_nTRTHits;
    TH1* m_track_eProbabilityHT;
    TH1* m_track_eProbabilityNN;
    TH1* m_track_eProbabilityHTorNN;
    TH1* m_track_idScoreCharged;
    TH1* m_track_idScoreIso;
    TH1* m_track_idScoreConv;
    TH1* m_track_idScoreFake;

    TH1* m_cluster_logEt;
    TH1* m_cluster_eta;
    TH1* m_cluster_phi;
    TH1* m_cluster_secondR;
    TH1* m_cluster_secondLambda;
    TH1* m_cluster_centerLambda;
    TH1* m_cluster_firstEngDens;
    TH1* m_cluster_EMproba;

    TH1* m_clustersMeanCenterLambda;
    TH1* m_clustersMeanFirstEngDens;
    TH1* m_clustersMeanEMProbability;
    TH1* m_clustersMeanSecondLambda;
    TH1* m_clustersMeanPresamplerFrac;
    TH1* m_PFOEngRelDiff;

    TH1* m_TVz;

  private:
    void initializePlots();
    std::string m_sTauJetContainerName;
};

}

#endif
