/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUHISTUTILS_NEWCOREPLOTS_H
#define TAUHISTUTILS_NEWCOREPLOTS_H

#include "GeneralTauPlots.h"
#include "xAODTau/TauJet.h"

namespace Tau{
   
   class NewCorePlots: public PlotBase {
   public:
      NewCorePlots(PlotBase *pParent, std::string sDir, std::string sTauJetContainerName);
      virtual ~NewCorePlots();
      void fill(const xAOD::TauJet& tau);
      
      TH1*  m_ipZ0SinThetaSigLeadTrk;
      TH1*  m_etOverPtLeadTrk;
      //    TH1*  m_leadTrkPt;
      TH1*  m_ipSigLeadTrk;
      TH1*  m_massTrkSys;
      TH1*  m_trkWidth2;
      TH1*  m_trFlightPathSig;
      TH1*  m_etEflow;
      //    TH1*  m_mEflow;
      TH1*  m_nPi0;
      TH1*  m_tauDRMax;
      TH1*  m_tauSflight;
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
      TH1*  m_caloIsoCorrected;
      TH1*  m_mEflowTopo;
      TH1*  m_ptRatioEflowTopo;
      TH1*  m_nPi0Topo;
      TH1*  m_PSSFraction;
      TH1*  m_ChPiEMEOverCaloEME;
      TH1*  m_EMPOverTrkSysP;
      TH1*  m_innerTrkAvgDist;
      TH1*  m_ptRatioEflowApprox;
      TH1*  m_mEflowApprox;
      
      
   private:
      void initializePlots();
      std::string m_sTauJetContainerName;
   };
   
}

#endif
