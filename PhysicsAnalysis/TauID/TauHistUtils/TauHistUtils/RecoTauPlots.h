/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUHISTUTILS_RECOTAUPLOTS_H
#define TAUHISTUTILS_RECOTAUPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TauHistUtils/ParamPlots.h"
#include "xAODTau/TauDefs.h" 
#include "xAODTau/TauJet.h"
#include "xAODPFlow/versions/PFO_v1.h"

namespace Tau{

class RecoTauPlots: public PlotBase {
  public:
    RecoTauPlots(PlotBase *pParent, std::string sDir, std::string sTauJetContainerName);
    virtual ~RecoTauPlots();

    void fill(const xAOD::TauJet& tau);

    Tau::ParamPlots m_oParamPlots;

    //Here I try to include CellBased substructure variables, from the Particle FLow Object (PFO), begining with basic charged and neutral PFO 4-vector.
    TH1* m_cellBased_charPFO_Pt;
    TH1* m_cellBased_charPFO_Eta;
    TH1* m_cellBased_charPFO_Phi;
    TH1* m_cellBased_charPFO_E;

    TH1* m_cellBased_neuPFO_Pt;
    TH1* m_cellBased_neuPFO_Eta;
    TH1* m_cellBased_neuPFO_Phi;
    TH1* m_cellBased_neuPFO_E;
    TH1* m_cellBased_neuPFO_bdtPi0Score;

    //PanTau Variables
    TH1* m_pantau_CellBasedInput_isPanTauCandidate;
    TH1* m_pantau_CellBasedInput_DecayMode;
    TH1* m_pantau_CellBasedInput_BDTValue_1p0n_vs_1p1n;
    TH1* m_pantau_CellBasedInput_BDTValue_1p1n_vs_1pXn;
    TH1* m_pantau_CellBasedInput_BDTValue_3p0n_vs_3pXn;
    TH1* m_pantau_CellBasedInput_BDTVar_Basic_NNeutralConsts;
    TH1* m_pantau_CellBasedInput_BDTVar_Charged_JetMoment_EtDRxTotalEt;
    TH1* m_pantau_CellBasedInput_BDTVar_Charged_StdDev_Et_WrtEtAllConsts;
    TH1* m_pantau_CellBasedInput_BDTVar_Neutral_HLV_SumM;
    TH1* m_pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_1;
    TH1* m_pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2;
    TH1* m_pantau_CellBasedInput_BDTVar_Neutral_Ratio_1stBDTEtOverEtAllConsts;
    TH1* m_pantau_CellBasedInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts;
    TH1* m_pantau_CellBasedInput_BDTVar_Neutral_Shots_NPhotonsInSeed;
    TH1* m_pantau_CellBasedInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged;

    // BDT output

    TH1* m_id_BDTJetScore;
    TH1* m_id_BDTEleScore;

  private:
    void initializePlots();
    std::string m_sTauJetContainerName;
};

}

#endif
