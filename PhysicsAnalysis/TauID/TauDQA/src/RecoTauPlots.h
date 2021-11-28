/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUDQA_RECOTAUPLOTS_H
#define TAUDQA_RECOTAUPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "ParamPlots.h"
#include "xAODTau/TauJet.h"
#include "xAODPFlow/PFO.h"

namespace Tau{

class RecoTauPlots: public PlotBase {
  public:
    RecoTauPlots(PlotBase *pParent, const std::string& sDir, const std::string& sTauJetContainerName);
    virtual ~RecoTauPlots();

    void fill(const xAOD::TauJet& tau, float weight);

    Tau::ParamPlots m_oParamPlots;

    //Here I try to include CellBased substructure variables, from the Particle Flow Object (PFO), begining with basic charged and neutral PFO 4-vector.
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
    TH1* m_pantau_CellBasedInput_BDTVar_Charged_HLV_SumM;
    TH1* m_pantau_CellBasedInput_BDTVar_Neutral_HLV_SumM;
    TH1* m_pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_1;
    TH1* m_pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2;
    TH1* m_pantau_CellBasedInput_BDTVar_Neutral_Ratio_1stBDTEtOverEtAllConsts;
    TH1* m_pantau_CellBasedInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts;
    TH1* m_pantau_CellBasedInput_BDTVar_Neutral_Shots_NPhotonsInSeed;
    TH1* m_pantau_CellBasedInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged;


  private:
    void initializePlots();
    std::string m_sTauJetContainerName;
};

}

#endif
