/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// ////////////////////////////////////////////////////////////
/// 
/// Implementation of converter for PanTauDetails class
/// 
/// ////////////////////////////////////////////////////////////


#define private public
#define protected public
#include "PanTauEvent/PanTauDetails.h"
#undef private
#undef protected

#include "tauEventTPCnv/PanTauDetailsCnv_p1.h"


void PanTauDetailsCnv_p1::persToTrans( const PanTauDetails_p1 *persObj, PanTau::PanTauDetails *transObj, MsgStream& /*log*/ ) {

    //data
    transObj->m_isPanTauCandidate                                = persObj->m_isPanTauCandidate;
    transObj->m_Name_InputAlg                                    = persObj->m_Name_InputAlg;
    transObj->m_RecoMode_SubAlg                                  = persObj->m_RecoMode_SubAlg;
    transObj->m_RecoMode_PanTau                                  = persObj->m_RecoMode_PanTau;
    transObj->m_pantau_proto_pt                                  = persObj->m_pantau_proto_pt;
    transObj->m_pantau_proto_eta                                 = persObj->m_pantau_proto_eta;
    transObj->m_pantau_proto_phi                                 = persObj->m_pantau_proto_phi;
    transObj->m_pantau_proto_m                                   = persObj->m_pantau_proto_m;
    transObj->m_pantau_final_pt                                  = persObj->m_pantau_final_pt;
    transObj->m_pantau_final_eta                                 = persObj->m_pantau_final_eta;
    transObj->m_pantau_final_phi                                 = persObj->m_pantau_final_phi;
    transObj->m_pantau_final_m                                   = persObj->m_pantau_final_m;
    
    transObj->m_ChargedEFOs_pt                                   = persObj->m_ChargedEFOs_pt;
    transObj->m_ChargedEFOs_eta                                  = persObj->m_ChargedEFOs_eta;
    transObj->m_ChargedEFOs_phi                                  = persObj->m_ChargedEFOs_phi;
    transObj->m_ChargedEFOs_m                                    = persObj->m_ChargedEFOs_m;
    
    transObj->m_NeutralEFOs_pt                                   = persObj->m_NeutralEFOs_pt;
    transObj->m_NeutralEFOs_eta                                  = persObj->m_NeutralEFOs_eta;
    transObj->m_NeutralEFOs_phi                                  = persObj->m_NeutralEFOs_phi;
    transObj->m_NeutralEFOs_m                                    = persObj->m_NeutralEFOs_m;
    
    transObj->m_Pi0NeutEFOs_pt                                   = persObj->m_Pi0NeutEFOs_pt;
    transObj->m_Pi0NeutEFOs_eta                                  = persObj->m_Pi0NeutEFOs_eta;
    transObj->m_Pi0NeutEFOs_phi                                  = persObj->m_Pi0NeutEFOs_phi;
    transObj->m_Pi0NeutEFOs_m                                    = persObj->m_Pi0NeutEFOs_m;
    
    
    transObj->m_eflowRec_Basic_NPi0NeutConsts                    = persObj->m_eflowRec_Basic_NPi0NeutConsts;
    transObj->m_eflowRec_Basic_NNeutralConsts                    = persObj->m_eflowRec_Basic_NNeutralConsts;
    transObj->m_eflowRec_Charged_HLV_SumPt                       = persObj->m_eflowRec_Charged_HLV_SumPt;
    transObj->m_eflowRec_Charged_Ratio_EtOverEtAllConsts         = persObj->m_eflowRec_Charged_Ratio_EtOverEtAllConsts;
    transObj->m_eflowRec_Neutral_HLV_SumM                        = persObj->m_eflowRec_Neutral_HLV_SumM;
    transObj->m_eflowRec_Neutral_PID_BDTValues_EtSort_1          = persObj->m_eflowRec_Neutral_PID_BDTValues_EtSort_1;
    transObj->m_eflowRec_Neutral_PID_BDTValues_BDTSort_2         = persObj->m_eflowRec_Neutral_PID_BDTValues_BDTSort_2;
    transObj->m_eflowRec_Neutral_Ratio_EtOverEtAllConsts         = persObj->m_eflowRec_Neutral_Ratio_EtOverEtAllConsts;
    transObj->m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts = persObj->m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts;
    transObj->m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged   = persObj->m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged;
    
    transObj->m_CellBased_Basic_NNeutralConsts                   = persObj->m_CellBased_Basic_NNeutralConsts;
    transObj->m_CellBased_Charged_JetMoment_EtDRxTotalEt         = persObj->m_CellBased_Charged_JetMoment_EtDRxTotalEt;
    transObj->m_CellBased_Charged_StdDev_Et_WrtEtAllConsts       = persObj->m_CellBased_Charged_StdDev_Et_WrtEtAllConsts;
    transObj->m_CellBased_Neutral_HLV_SumM                       = persObj->m_CellBased_Neutral_HLV_SumM;
    transObj->m_CellBased_Neutral_PID_BDTValues_BDTSort_1        = persObj->m_CellBased_Neutral_PID_BDTValues_BDTSort_1;
    transObj->m_CellBased_Neutral_PID_BDTValues_BDTSort_2        = persObj->m_CellBased_Neutral_PID_BDTValues_BDTSort_2;
    transObj->m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts  = persObj->m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts;
    transObj->m_CellBased_Neutral_Ratio_EtOverEtAllConsts        = persObj->m_CellBased_Neutral_Ratio_EtOverEtAllConsts;
    transObj->m_CellBased_Neutral_Shots_NPhotonsInSeed           = persObj->m_CellBased_Neutral_Shots_NPhotonsInSeed;
    transObj->m_CellBased_Combined_DeltaR1stNeutralTo1stCharged  = persObj->m_CellBased_Combined_DeltaR1stNeutralTo1stCharged;
    
    
}


void PanTauDetailsCnv_p1::transToPers( const PanTau::PanTauDetails *transObj, PanTauDetails_p1 *persObj, MsgStream& /*log*/ ) {

    //data
    
    persObj->m_isPanTauCandidate                                = transObj->m_isPanTauCandidate;
    persObj->m_Name_InputAlg                                    = transObj->m_Name_InputAlg;
    persObj->m_RecoMode_SubAlg                                  = transObj->m_RecoMode_SubAlg;
    persObj->m_RecoMode_PanTau                                  = transObj->m_RecoMode_PanTau;
    persObj->m_pantau_proto_pt                                  = transObj->m_pantau_proto_pt;
    persObj->m_pantau_proto_eta                                 = transObj->m_pantau_proto_eta;
    persObj->m_pantau_proto_phi                                 = transObj->m_pantau_proto_phi;
    persObj->m_pantau_proto_m                                   = transObj->m_pantau_proto_m;
    persObj->m_pantau_final_pt                                  = transObj->m_pantau_final_pt;
    persObj->m_pantau_final_eta                                 = transObj->m_pantau_final_eta;
    persObj->m_pantau_final_phi                                 = transObj->m_pantau_final_phi;
    persObj->m_pantau_final_m                                   = transObj->m_pantau_final_m;
    
    persObj->m_ChargedEFOs_pt                                   = transObj->m_ChargedEFOs_pt;
    persObj->m_ChargedEFOs_eta                                  = transObj->m_ChargedEFOs_eta;
    persObj->m_ChargedEFOs_phi                                  = transObj->m_ChargedEFOs_phi;
    persObj->m_ChargedEFOs_m                                    = transObj->m_ChargedEFOs_m;
    
    persObj->m_NeutralEFOs_pt                                   = transObj->m_NeutralEFOs_pt;
    persObj->m_NeutralEFOs_eta                                  = transObj->m_NeutralEFOs_eta;
    persObj->m_NeutralEFOs_phi                                  = transObj->m_NeutralEFOs_phi;
    persObj->m_NeutralEFOs_m                                    = transObj->m_NeutralEFOs_m;
    
    persObj->m_Pi0NeutEFOs_pt                                   = transObj->m_Pi0NeutEFOs_pt;
    persObj->m_Pi0NeutEFOs_eta                                  = transObj->m_Pi0NeutEFOs_eta;
    persObj->m_Pi0NeutEFOs_phi                                  = transObj->m_Pi0NeutEFOs_phi;
    persObj->m_Pi0NeutEFOs_m                                    = transObj->m_Pi0NeutEFOs_m;
    
    
    persObj->m_eflowRec_Basic_NPi0NeutConsts                    = transObj->m_eflowRec_Basic_NPi0NeutConsts;
    persObj->m_eflowRec_Basic_NNeutralConsts                    = transObj->m_eflowRec_Basic_NNeutralConsts;
    persObj->m_eflowRec_Charged_HLV_SumPt                       = transObj->m_eflowRec_Charged_HLV_SumPt;
    persObj->m_eflowRec_Charged_Ratio_EtOverEtAllConsts         = transObj->m_eflowRec_Charged_Ratio_EtOverEtAllConsts;
    persObj->m_eflowRec_Neutral_HLV_SumM                        = transObj->m_eflowRec_Neutral_HLV_SumM;
    persObj->m_eflowRec_Neutral_PID_BDTValues_EtSort_1          = transObj->m_eflowRec_Neutral_PID_BDTValues_EtSort_1;
    persObj->m_eflowRec_Neutral_PID_BDTValues_BDTSort_2         = transObj->m_eflowRec_Neutral_PID_BDTValues_BDTSort_2;
    persObj->m_eflowRec_Neutral_Ratio_EtOverEtAllConsts         = transObj->m_eflowRec_Neutral_Ratio_EtOverEtAllConsts;
    persObj->m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts = transObj->m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts;
    persObj->m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged   = transObj->m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged;
    
    persObj->m_CellBased_Basic_NNeutralConsts                   = transObj->m_CellBased_Basic_NNeutralConsts;
    persObj->m_CellBased_Charged_JetMoment_EtDRxTotalEt         = transObj->m_CellBased_Charged_JetMoment_EtDRxTotalEt;
    persObj->m_CellBased_Charged_StdDev_Et_WrtEtAllConsts       = transObj->m_CellBased_Charged_StdDev_Et_WrtEtAllConsts;
    persObj->m_CellBased_Neutral_HLV_SumM                       = transObj->m_CellBased_Neutral_HLV_SumM;
    persObj->m_CellBased_Neutral_PID_BDTValues_BDTSort_1        = transObj->m_CellBased_Neutral_PID_BDTValues_BDTSort_1;
    persObj->m_CellBased_Neutral_PID_BDTValues_BDTSort_2        = transObj->m_CellBased_Neutral_PID_BDTValues_BDTSort_2;
    persObj->m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts  = transObj->m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts;
    persObj->m_CellBased_Neutral_Ratio_EtOverEtAllConsts        = transObj->m_CellBased_Neutral_Ratio_EtOverEtAllConsts;
    persObj->m_CellBased_Neutral_Shots_NPhotonsInSeed           = transObj->m_CellBased_Neutral_Shots_NPhotonsInSeed;
    persObj->m_CellBased_Combined_DeltaR1stNeutralTo1stCharged  = transObj->m_CellBased_Combined_DeltaR1stNeutralTo1stCharged;
    
}
