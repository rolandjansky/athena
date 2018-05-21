/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUHISTUTILS_GENERALTAUPLOTS_H
#define TAUHISTUTILS_GENERALTAUPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TauHistUtils/ParamPlots.h"
#include "xAODTau/TauJet.h"
#include "xAODTau/TauDefs.h"

namespace Tau{

class GeneralTauPlots: public PlotBase {
   public:
      GeneralTauPlots(PlotBase *pParent, std::string sDir, std::string sTauJetContainerName);
      virtual ~GeneralTauPlots();
      
      void fill(const xAOD::TauJet& tau);

      Tau::ParamPlots m_oParamPlots;
      TH1* m_tauCharge;
      TH1* m_tauNCoreTracks;
      TH1* m_tauNWideTracks;

      // BDT
      TH1* m_id_BDTJetScore;
      TH1* m_id_BDTJetScoreSigTrans;
      TH1* m_pt_jetBDTloose;
      TH1* m_pt_jetBDTmed;
      TH1* m_pt_jetBDTtight;
      TH1* m_pt_jetBDTlooseHighPt;
      TH1* m_pt_jetBDTmedHighPt;
      TH1* m_pt_jetBDTtightHighPt;

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
