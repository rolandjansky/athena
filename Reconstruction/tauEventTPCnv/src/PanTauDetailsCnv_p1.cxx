/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// ////////////////////////////////////////////////////////////
/// 
/// Implementation of converter for PanTauDetails class
/// 
/// ////////////////////////////////////////////////////////////


#include "PanTauEvent/PanTauDetails.h"
#include "tauEventTPCnv/PanTauDetailsCnv_p1.h"


void PanTauDetailsCnv_p1::persToTrans( const PanTauDetails_p1 *persObj, PanTau::PanTauDetails *transObj, MsgStream& /*log*/ ) {

    //data
    transObj->set_isPanTauCandidate                              (persObj->m_isPanTauCandidate);
    transObj->set_InputAlgName                                   (persObj->m_Name_InputAlg);
    transObj->set_RecoModeSubAlg                                 (persObj->m_RecoMode_SubAlg);
    transObj->set_RecoModePanTau                                 (persObj->m_RecoMode_PanTau);
    transObj->set_BDTValue_1p0n_vs_1p1n                          (persObj->m_BDTValue_1p0n_vs_1p1n);
    transObj->set_BDTValue_1p1n_vs_1pXn                          (persObj->m_BDTValue_1p1n_vs_1pXn);
    transObj->set_BDTValue_3p0n_vs_3pXn                          (persObj->m_BDTValue_3p0n_vs_3pXn);
    
    transObj->set_pantau_proto_pt                                (persObj->m_pantau_proto_pt);
    transObj->set_pantau_proto_eta                               (persObj->m_pantau_proto_eta);
    transObj->set_pantau_proto_phi                               (persObj->m_pantau_proto_phi);
    transObj->set_pantau_proto_m                                 (persObj->m_pantau_proto_m);
    transObj->set_pantau_final_pt                                (persObj->m_pantau_final_pt);
    transObj->set_pantau_final_eta                               (persObj->m_pantau_final_eta);
    transObj->set_pantau_final_phi                               (persObj->m_pantau_final_phi);
    transObj->set_pantau_final_m                                 (persObj->m_pantau_final_m);
    
    transObj->set_ChargedEFOs_pt                                 (persObj->m_ChargedEFOs_pt);
    transObj->set_ChargedEFOs_eta                                (persObj->m_ChargedEFOs_eta);
    transObj->set_ChargedEFOs_phi                                (persObj->m_ChargedEFOs_phi);
    transObj->set_ChargedEFOs_m                                  (persObj->m_ChargedEFOs_m);
    
    transObj->set_NeutralEFOs_pt                                 (persObj->m_NeutralEFOs_pt);
    transObj->set_NeutralEFOs_eta                                (persObj->m_NeutralEFOs_eta);
    transObj->set_NeutralEFOs_phi                                (persObj->m_NeutralEFOs_phi);
    transObj->set_NeutralEFOs_m                                  (persObj->m_NeutralEFOs_m);
    
    transObj->set_Pi0NeutEFOs_pt                                 (persObj->m_Pi0NeutEFOs_pt);
    transObj->set_Pi0NeutEFOs_eta                                (persObj->m_Pi0NeutEFOs_eta);
    transObj->set_Pi0NeutEFOs_phi                                (persObj->m_Pi0NeutEFOs_phi);
    transObj->set_Pi0NeutEFOs_m                                  (persObj->m_Pi0NeutEFOs_m);
    
    
    transObj->set_eflowRec_Basic_NPi0NeutConsts                  (persObj->m_eflowRec_Basic_NPi0NeutConsts);
    transObj->set_eflowRec_Basic_NNeutralConsts                  (persObj->m_eflowRec_Basic_NNeutralConsts);
    transObj->set_eflowRec_Charged_HLV_SumPt                     (persObj->m_eflowRec_Charged_HLV_SumPt);
    transObj->set_eflowRec_Charged_Ratio_EtOverEtAllConsts       (persObj->m_eflowRec_Charged_Ratio_EtOverEtAllConsts);
    transObj->set_eflowRec_Neutral_HLV_SumM                      (persObj->m_eflowRec_Neutral_HLV_SumM);
    transObj->set_eflowRec_Neutral_PID_BDTValues_EtSort_1        (persObj->m_eflowRec_Neutral_PID_BDTValues_EtSort_1);
    transObj->set_eflowRec_Neutral_PID_BDTValues_BDTSort_2       (persObj->m_eflowRec_Neutral_PID_BDTValues_BDTSort_2);
    transObj->set_eflowRec_Neutral_Ratio_EtOverEtAllConsts       (persObj->m_eflowRec_Neutral_Ratio_EtOverEtAllConsts);
    transObj->set_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts (persObj->m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts);
    transObj->set_eflowRec_Combined_DeltaR1stNeutralTo1stCharged (persObj->m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged);
    
    transObj->set_CellBased_Basic_NNeutralConsts                 (persObj->m_CellBased_Basic_NNeutralConsts);
    transObj->set_CellBased_Charged_JetMoment_EtDRxTotalEt       (persObj->m_CellBased_Charged_JetMoment_EtDRxTotalEt);
    transObj->set_CellBased_Charged_StdDev_Et_WrtEtAllConsts     (persObj->m_CellBased_Charged_StdDev_Et_WrtEtAllConsts);
    transObj->set_CellBased_Neutral_HLV_SumM                     (persObj->m_CellBased_Neutral_HLV_SumM);
    transObj->set_CellBased_Neutral_PID_BDTValues_BDTSort_1      (persObj->m_CellBased_Neutral_PID_BDTValues_BDTSort_1);
    transObj->set_CellBased_Neutral_PID_BDTValues_BDTSort_2      (persObj->m_CellBased_Neutral_PID_BDTValues_BDTSort_2);
    transObj->set_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts(persObj->m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts);
    transObj->set_CellBased_Neutral_Ratio_EtOverEtAllConsts      (persObj->m_CellBased_Neutral_Ratio_EtOverEtAllConsts);
    transObj->set_CellBased_Neutral_Shots_NPhotonsInSeed         (persObj->m_CellBased_Neutral_Shots_NPhotonsInSeed);
    transObj->set_CellBased_Combined_DeltaR1stNeutralTo1stCharged(persObj->m_CellBased_Combined_DeltaR1stNeutralTo1stCharged);
    
    
}


void PanTauDetailsCnv_p1::transToPers( const PanTau::PanTauDetails *transObj, PanTauDetails_p1 *persObj, MsgStream& /*log*/ ) {

    //data
    
    persObj->m_isPanTauCandidate                                = transObj->isPanTauCandidate();
    persObj->m_Name_InputAlg                                    = transObj->inputAlgName();
    persObj->m_RecoMode_SubAlg                                  = transObj->RecoModeSubAlg();
    persObj->m_RecoMode_PanTau                                  = transObj->RecoModePanTau();
    
    persObj->m_BDTValue_1p0n_vs_1p1n                            = transObj->BDTValue_1p0n_vs_1p1n();
    persObj->m_BDTValue_1p1n_vs_1pXn                            = transObj->BDTValue_1p1n_vs_1pXn();
    persObj->m_BDTValue_3p0n_vs_3pXn                            = transObj->BDTValue_3p0n_vs_3pXn();
    
    persObj->m_pantau_proto_pt                                  = transObj->pantau_proto_pt();
    persObj->m_pantau_proto_eta                                 = transObj->pantau_proto_eta();
    persObj->m_pantau_proto_phi                                 = transObj->pantau_proto_phi();
    persObj->m_pantau_proto_m                                   = transObj->pantau_proto_m();
    persObj->m_pantau_final_pt                                  = transObj->pantau_final_pt();
    persObj->m_pantau_final_eta                                 = transObj->pantau_final_eta();
    persObj->m_pantau_final_phi                                 = transObj->pantau_final_phi();
    persObj->m_pantau_final_m                                   = transObj->pantau_final_m();
    
    persObj->m_ChargedEFOs_pt                                   = transObj->ChargedEFOs_pt();
    persObj->m_ChargedEFOs_eta                                  = transObj->ChargedEFOs_eta();
    persObj->m_ChargedEFOs_phi                                  = transObj->ChargedEFOs_phi();
    persObj->m_ChargedEFOs_m                                    = transObj->ChargedEFOs_m();
    
    persObj->m_NeutralEFOs_pt                                   = transObj->NeutralEFOs_pt();
    persObj->m_NeutralEFOs_eta                                  = transObj->NeutralEFOs_eta();
    persObj->m_NeutralEFOs_phi                                  = transObj->NeutralEFOs_phi();
    persObj->m_NeutralEFOs_m                                    = transObj->NeutralEFOs_m();
    
    persObj->m_Pi0NeutEFOs_pt                                   = transObj->Pi0NeutEFOs_pt();
    persObj->m_Pi0NeutEFOs_eta                                  = transObj->Pi0NeutEFOs_eta();
    persObj->m_Pi0NeutEFOs_phi                                  = transObj->Pi0NeutEFOs_phi();
    persObj->m_Pi0NeutEFOs_m                                    = transObj->Pi0NeutEFOs_m();
    
    
    persObj->m_eflowRec_Basic_NPi0NeutConsts                    = transObj->eflowRec_Basic_NPi0NeutConsts();
    persObj->m_eflowRec_Basic_NNeutralConsts                    = transObj->eflowRec_Basic_NNeutralConsts();
    persObj->m_eflowRec_Charged_HLV_SumPt                       = transObj->eflowRec_Charged_HLV_SumPt();
    persObj->m_eflowRec_Charged_Ratio_EtOverEtAllConsts         = transObj->eflowRec_Charged_Ratio_EtOverEtAllConsts();
    persObj->m_eflowRec_Neutral_HLV_SumM                        = transObj->eflowRec_Neutral_HLV_SumM();
    persObj->m_eflowRec_Neutral_PID_BDTValues_EtSort_1          = transObj->eflowRec_Neutral_PID_BDTValues_EtSort_1();
    persObj->m_eflowRec_Neutral_PID_BDTValues_BDTSort_2         = transObj->eflowRec_Neutral_PID_BDTValues_BDTSort_2();
    persObj->m_eflowRec_Neutral_Ratio_EtOverEtAllConsts         = transObj->eflowRec_Neutral_Ratio_EtOverEtAllConsts();
    persObj->m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts = transObj->eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts();
    persObj->m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged   = transObj->eflowRec_Combined_DeltaR1stNeutralTo1stCharged();
    
    persObj->m_CellBased_Basic_NNeutralConsts                   = transObj->CellBased_Basic_NNeutralConsts();
    persObj->m_CellBased_Charged_JetMoment_EtDRxTotalEt         = transObj->CellBased_Charged_JetMoment_EtDRxTotalEt();
    persObj->m_CellBased_Charged_StdDev_Et_WrtEtAllConsts       = transObj->CellBased_Charged_StdDev_Et_WrtEtAllConsts();
    persObj->m_CellBased_Neutral_HLV_SumM                       = transObj->CellBased_Neutral_HLV_SumM();
    persObj->m_CellBased_Neutral_PID_BDTValues_BDTSort_1        = transObj->CellBased_Neutral_PID_BDTValues_BDTSort_1();
    persObj->m_CellBased_Neutral_PID_BDTValues_BDTSort_2        = transObj->CellBased_Neutral_PID_BDTValues_BDTSort_2();
    persObj->m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts  = transObj->CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts();
    persObj->m_CellBased_Neutral_Ratio_EtOverEtAllConsts        = transObj->CellBased_Neutral_Ratio_EtOverEtAllConsts();
    persObj->m_CellBased_Neutral_Shots_NPhotonsInSeed           = transObj->CellBased_Neutral_Shots_NPhotonsInSeed();
    persObj->m_CellBased_Combined_DeltaR1stNeutralTo1stCharged  = transObj->CellBased_Combined_DeltaR1stNeutralTo1stCharged();
    
}
