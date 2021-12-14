/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUDQA_GENERALTAUPLOTS_H
#define TAUDQA_GENERALTAUPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "ParamPlots.h"
#include "xAODTau/TauJet.h"

namespace Tau{

class GeneralTauPlots: public PlotBase {
   public:
      GeneralTauPlots(PlotBase *pParent, const std::string& sDir, const std::string& sTauJetContainerName);
      virtual ~GeneralTauPlots();
      
      void fill(const xAOD::TauJet& tau, float weight);

      Tau::ParamPlots m_oParamPlots;
      TH1* m_tauCharge;
      TH1* m_tauNCoreTracks;
      TH1* m_tauNWideTracks;
      TH1* m_ptHighPt;

      // RNN
      TH1* m_RNNScore;
      TH1* m_RNNScoreSigTrans;
      TH1* m_ptRNNVeryLoose;
      TH1* m_ptRNNLoose;
      TH1* m_ptRNNMedium;
      TH1* m_ptRNNTight;
      TH1* m_ptRNNVeryLooseHighPt;
      TH1* m_ptRNNLooseHighPt;
      TH1* m_ptRNNMediumHighPt;
      TH1* m_ptRNNTightHighPt;

   private:
      void initializePlots();
      std::string m_sTauJetContainerName;
};

}

#endif
