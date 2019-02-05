/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @file PanTauDetails_p1.cxx
/// 
/// Implementation of persistent class 
/// for PanTau::PanTauDetails
/// 
/// Package: Reconstruction/tauEventTPCnv
/// 
/// @author Christian Limbach

#include "tauEventTPCnv/PanTauDetails_p1.h"

PanTauDetails_p1::PanTauDetails_p1()
  : m_isPanTauCandidate(0), //!< tells you whether this tauRec seed is considered as candidate for PanTau
    m_Name_InputAlg("UnknownInputAlg"),
    m_RecoMode_SubAlg(0),
    m_RecoMode_PanTau(0),
    m_BDTValue_1p0n_vs_1p1n(-5),
    m_BDTValue_1p1n_vs_1pXn(-5),
    m_BDTValue_3p0n_vs_3pXn(-5),
    m_pantau_proto_pt(0),
    m_pantau_proto_eta(0),
    m_pantau_proto_phi(0),
    m_pantau_proto_m(0),
    m_pantau_final_pt(0),
    m_pantau_final_eta(0),
    m_pantau_final_phi(0),
    m_pantau_final_m(0),
    
    m_ChargedEFOs_pt(),
    m_ChargedEFOs_eta(),
    m_ChargedEFOs_phi(),
    m_ChargedEFOs_m(),
    
    m_NeutralEFOs_pt(),
    m_NeutralEFOs_eta(),
    m_NeutralEFOs_phi(),
    m_NeutralEFOs_m(),
    
    m_Pi0NeutEFOs_pt(),
    m_Pi0NeutEFOs_eta(),
    m_Pi0NeutEFOs_phi(),
    m_Pi0NeutEFOs_m(),
    
    m_eflowRec_Basic_NPi0NeutConsts(0),
    m_eflowRec_Basic_NNeutralConsts(0),
    m_eflowRec_Charged_HLV_SumPt(0),
    m_eflowRec_Charged_Ratio_EtOverEtAllConsts(0),
    m_eflowRec_Neutral_HLV_SumM(0),
    m_eflowRec_Neutral_PID_BDTValues_EtSort_1(0),
    m_eflowRec_Neutral_PID_BDTValues_BDTSort_2(0),
    m_eflowRec_Neutral_Ratio_EtOverEtAllConsts(0),
    m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts(0),
    m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged(0),
    
    m_CellBased_Basic_NNeutralConsts(0),
    m_CellBased_Charged_JetMoment_EtDRxTotalEt(0),
    m_CellBased_Charged_StdDev_Et_WrtEtAllConsts(0),
    m_CellBased_Neutral_HLV_SumM(0),
    m_CellBased_Neutral_PID_BDTValues_BDTSort_1(0),
    m_CellBased_Neutral_PID_BDTValues_BDTSort_2(0),
    m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts(0),
    m_CellBased_Neutral_Ratio_EtOverEtAllConsts(0),
    m_CellBased_Neutral_Shots_NPhotonsInSeed(0),
    m_CellBased_Combined_DeltaR1stNeutralTo1stCharged(0)
{}

