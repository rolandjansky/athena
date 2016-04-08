/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PanTauEvent/PanTauDetails.h"
#include "PanTauEvent/TauFeature.h"
#include "PanTauEvent/TauSeed.h"

//#include "GaudiKernel/MsgStream.h"
#include <string>

std::string PanTau::PanTauDetails::s_className = "PanTauDetails";

/** Default constructor */
PanTau::PanTauDetails::PanTauDetails()
    :
    Analysis::TauDetails(),
    m_features(nullptr),
    m_seed(nullptr),
    m_Name_InputAlg("InvalidInputAlg"),
    
    m_RecoMode_SubAlg(-5),
    m_RecoMode_PanTau(-5),
    m_BDTValue_1p0n_vs_1p1n(-5),
    m_BDTValue_1p1n_vs_1pXn(-5),
    m_BDTValue_3p0n_vs_3pXn(-5),
    m_pantau_proto_pt(-1111),
    m_pantau_proto_eta(-11),
    m_pantau_proto_phi(-11),
    m_pantau_proto_m(-1111),
    m_pantau_final_pt(-1111),
    m_pantau_final_eta(-11),
    m_pantau_final_phi(-11),
    m_pantau_final_m(-1111),
    
    m_eflowRec_Basic_NPi0NeutConsts(-5),
    m_eflowRec_Basic_NNeutralConsts(-5),
    m_eflowRec_Charged_HLV_SumPt(-1111),
    m_eflowRec_Charged_Ratio_EtOverEtAllConsts(-5),
    m_eflowRec_Neutral_HLV_SumM(-1111),
    m_eflowRec_Neutral_PID_BDTValues_EtSort_1(-5),
    m_eflowRec_Neutral_PID_BDTValues_BDTSort_2(-5),
    m_eflowRec_Neutral_Ratio_EtOverEtAllConsts(-5),
    m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts(-5),
    m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged(-5),

    m_CellBased_Basic_NNeutralConsts(-5),
    m_CellBased_Charged_JetMoment_EtDRxTotalEt(-1111),
    m_CellBased_Charged_StdDev_Et_WrtEtAllConsts(-1111),
    m_CellBased_Neutral_HLV_SumM(-1111),
    m_CellBased_Neutral_PID_BDTValues_BDTSort_1(-5),
    m_CellBased_Neutral_PID_BDTValues_BDTSort_2(-5),
    m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts(-5),
    m_CellBased_Neutral_Ratio_EtOverEtAllConsts(-5),
    m_CellBased_Neutral_Shots_NPhotonsInSeed(-5),
    m_CellBased_Combined_DeltaR1stNeutralTo1stCharged(-5)
{
}



PanTau::PanTauDetails::PanTauDetails(PanTau::PanTauSeed* inputSeed) 
    :
    Analysis::TauDetails(),
    m_PanTauSeed(inputSeed),
    m_features(nullptr),
    m_seed(nullptr)
{
    if(inputSeed->getIsValidSeed() == true) {
        bool        isValid         = false;
        TauFeature* curFeatures     = inputSeed->getFeatures();
        m_Name_InputAlg             = inputSeed->getNameInputAlgorithm();
        
        m_eflowRec_Basic_NPi0NeutConsts                     = (int)curFeatures->value(m_Name_InputAlg + "_Basic_NPi0NeutConsts", isValid);
        m_eflowRec_Basic_NNeutralConsts                     = (int)curFeatures->value(m_Name_InputAlg + "_Basic_NNeutralConsts", isValid);
        m_eflowRec_Charged_HLV_SumPt                        = curFeatures->value(m_Name_InputAlg + "_Charged_HLV_SumPt", isValid);
        m_eflowRec_Charged_Ratio_EtOverEtAllConsts          = curFeatures->value(m_Name_InputAlg + "_Charged_Ratio_EtOverEtAllConsts", isValid);
        m_eflowRec_Neutral_HLV_SumM                         = curFeatures->value(m_Name_InputAlg + "_Neutral_HLV_SumM", isValid);
        m_eflowRec_Neutral_PID_BDTValues_EtSort_1           = curFeatures->value(m_Name_InputAlg + "_Neutral_PID_BDTValues_EtSort_1", isValid);
        m_eflowRec_Neutral_PID_BDTValues_BDTSort_2          = curFeatures->value(m_Name_InputAlg + "_Neutral_PID_BDTValues_BDTSort_2", isValid);
        m_eflowRec_Neutral_Ratio_EtOverEtAllConsts          = curFeatures->value(m_Name_InputAlg + "_Neutral_Ratio_EtOverEtAllConsts", isValid);
        m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts  = curFeatures->value(m_Name_InputAlg + "_Neutral_Mean_DRToLeading_WrtEtAllConsts", isValid);
        m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged    = curFeatures->value(m_Name_InputAlg + "_Combined_DeltaR1stNeutralTo1stCharged", isValid);

        m_CellBased_Basic_NNeutralConsts                    = (int)curFeatures->value(m_Name_InputAlg + "_Basic_NNeutralConsts", isValid);
        m_CellBased_Charged_JetMoment_EtDRxTotalEt          = curFeatures->value(m_Name_InputAlg + "_Charged_JetMoment_EtDRxTotalEt", isValid);
        m_CellBased_Charged_StdDev_Et_WrtEtAllConsts        = curFeatures->value(m_Name_InputAlg + "_Charged_StdDev_Et_WrtEtAllConsts", isValid);
        m_CellBased_Neutral_HLV_SumM                        = curFeatures->value(m_Name_InputAlg + "_Neutral_HLV_SumM", isValid);
        m_CellBased_Neutral_PID_BDTValues_BDTSort_1         = curFeatures->value(m_Name_InputAlg + "_Neutral_PID_BDTValues_BDTSort_1", isValid);
        m_CellBased_Neutral_PID_BDTValues_BDTSort_2         = curFeatures->value(m_Name_InputAlg + "_Neutral_PID_BDTValues_BDTSort_2", isValid);
        m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts   = curFeatures->value(m_Name_InputAlg + "_Neutral_Ratio_1stBDTEtOverEtAllConsts", isValid);
        m_CellBased_Neutral_Ratio_EtOverEtAllConsts         = curFeatures->value(m_Name_InputAlg + "_Neutral_Ratio_EtOverEtAllConsts", isValid);
        m_CellBased_Neutral_Shots_NPhotonsInSeed            = curFeatures->value(m_Name_InputAlg + "_Neutral_Shots_NPhotonsInSeed", isValid);
        m_CellBased_Combined_DeltaR1stNeutralTo1stCharged   = curFeatures->value(m_Name_InputAlg + "Combined_DeltaR1stNeutralTo1stCharged", isValid);
        
    }
}

    
/*
PanTau::PanTauDetails::PanTauDetails(
        const PanTau::TauSeed       *seed,
        const PanTau::TauFeature    *features
        )
    :
    Analysis::TauDetails(),
    m_PanTauSeed(0),
    m_features(features),
    m_seed(seed),
    m_RecoMode_SubAlg(-5),
    m_RecoMode_PanTau(-5),
    m_BDTValue_1p0n_vs_1p1n(-5),
    m_BDTValue_1p1n_vs_1pXn(-5),
    m_BDTValue_3p0n_vs_3pXn(-5),
    m_pantau_proto_pt(-1111),
    m_pantau_proto_eta(-11),
    m_pantau_proto_phi(-11),
    m_pantau_proto_m(-1111),
    m_pantau_final_pt(-1111),
    m_pantau_final_eta(-11),
    m_pantau_final_phi(-11),
    m_pantau_final_m(-1111),
    
    m_eflowRec_Basic_NPi0NeutConsts(-5),
    m_eflowRec_Basic_NNeutralConsts(-5),
    m_eflowRec_Charged_HLV_SumPt(-1111),
    m_eflowRec_Charged_Ratio_EtOverEtAllConsts(-5),
    m_eflowRec_Neutral_HLV_SumM(-1111),
    m_eflowRec_Neutral_PID_BDTValues_EtSort_1(-5),
    m_eflowRec_Neutral_PID_BDTValues_BDTSort_2(-5),
    m_eflowRec_Neutral_Ratio_EtOverEtAllConsts(-5),
    m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts(-5),
    m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged(-5),

    m_CellBased_Basic_NNeutralConsts(-5),
    m_CellBased_Charged_JetMoment_EtDRxTotalEt(-1111),
    m_CellBased_Charged_StdDev_Et_WrtEtAllConsts(-1111),
    m_CellBased_Neutral_HLV_SumM(-1111),
    m_CellBased_Neutral_PID_BDTValues_BDTSort_1(-5),
    m_CellBased_Neutral_PID_BDTValues_BDTSort_2(-5),
    m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts(-5),
    m_CellBased_Neutral_Ratio_EtOverEtAllConsts(-5),
    m_CellBased_Neutral_Shots_NPhotonsInSeed(-5),
    m_CellBased_Combined_DeltaR1stNeutralTo1stCharged(-5)
{
}
*/


/** Copy constructor */
PanTau::PanTauDetails::PanTauDetails(
    const PanTau::PanTauDetails& rhs
    ):
    Analysis::TauDetails(),
    m_PanTauSeed(rhs.m_PanTauSeed),
    m_features(rhs.m_features ? new PanTau::TauFeature(*rhs.m_features) : 0),
    m_seed(rhs.m_seed),
    m_RecoMode_SubAlg(rhs.m_RecoMode_SubAlg),
    m_RecoMode_PanTau(rhs.m_RecoMode_PanTau),
    m_BDTValue_1p0n_vs_1p1n(rhs.m_BDTValue_1p0n_vs_1p1n),
    m_BDTValue_1p1n_vs_1pXn(rhs.m_BDTValue_1p1n_vs_1pXn),
    m_BDTValue_3p0n_vs_3pXn(rhs.m_BDTValue_3p0n_vs_3pXn),
    m_pantau_proto_pt(rhs.m_pantau_proto_pt),
    m_pantau_proto_eta(rhs.m_pantau_proto_eta),
    m_pantau_proto_phi(rhs.m_pantau_proto_phi),
    m_pantau_proto_m(rhs.m_pantau_proto_m),
    m_pantau_final_pt(rhs.m_pantau_final_pt),
    m_pantau_final_eta(rhs.m_pantau_final_eta),
    m_pantau_final_phi(rhs.m_pantau_final_phi),
    m_pantau_final_m(rhs.m_pantau_final_m),
    
    m_eflowRec_Basic_NPi0NeutConsts(rhs.m_eflowRec_Basic_NPi0NeutConsts),
    m_eflowRec_Basic_NNeutralConsts(rhs.m_eflowRec_Basic_NNeutralConsts),
    m_eflowRec_Charged_HLV_SumPt(rhs.m_eflowRec_Charged_HLV_SumPt),
    m_eflowRec_Charged_Ratio_EtOverEtAllConsts(rhs.m_eflowRec_Charged_Ratio_EtOverEtAllConsts),
    m_eflowRec_Neutral_HLV_SumM(rhs.m_eflowRec_Neutral_HLV_SumM),
    m_eflowRec_Neutral_PID_BDTValues_EtSort_1(rhs.m_eflowRec_Neutral_PID_BDTValues_EtSort_1),
    m_eflowRec_Neutral_PID_BDTValues_BDTSort_2(rhs.m_eflowRec_Neutral_PID_BDTValues_BDTSort_2),
    m_eflowRec_Neutral_Ratio_EtOverEtAllConsts(rhs.m_eflowRec_Neutral_Ratio_EtOverEtAllConsts),
    m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts(rhs.m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts),
    m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged(rhs.m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged),

    m_CellBased_Basic_NNeutralConsts(rhs.m_CellBased_Basic_NNeutralConsts),
    m_CellBased_Charged_JetMoment_EtDRxTotalEt(rhs.m_CellBased_Charged_JetMoment_EtDRxTotalEt),
    m_CellBased_Charged_StdDev_Et_WrtEtAllConsts(rhs.m_CellBased_Charged_StdDev_Et_WrtEtAllConsts),
    m_CellBased_Neutral_HLV_SumM(rhs.m_CellBased_Neutral_HLV_SumM),
    m_CellBased_Neutral_PID_BDTValues_BDTSort_1(rhs.m_CellBased_Neutral_PID_BDTValues_BDTSort_1),
    m_CellBased_Neutral_PID_BDTValues_BDTSort_2(rhs.m_CellBased_Neutral_PID_BDTValues_BDTSort_2),
    m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts(rhs.m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts),
    m_CellBased_Neutral_Ratio_EtOverEtAllConsts(rhs.m_CellBased_Neutral_Ratio_EtOverEtAllConsts),
    m_CellBased_Neutral_Shots_NPhotonsInSeed(rhs.m_CellBased_Neutral_Shots_NPhotonsInSeed),
    m_CellBased_Combined_DeltaR1stNeutralTo1stCharged(rhs.m_CellBased_Combined_DeltaR1stNeutralTo1stCharged)
{
}

PanTau::PanTauDetails::~PanTauDetails()
{
  if(m_features != 0) delete m_features;
}

/** Assignment operator */
PanTau::PanTauDetails& PanTau::PanTauDetails::operator=(const PanTau::PanTauDetails& rhs)
{
    if (this!=&rhs){
       if(m_features != 0) delete m_features;
        m_features = (rhs.m_features ? new PanTau::TauFeature(*rhs.m_features) : 0);
        m_seed = rhs.m_seed;
        m_PanTauSeed = rhs.m_PanTauSeed;

        m_RecoMode_SubAlg = rhs.m_RecoMode_SubAlg;
        m_RecoMode_PanTau = rhs.m_RecoMode_PanTau;
        m_BDTValue_1p0n_vs_1p1n = rhs.m_BDTValue_1p0n_vs_1p1n;
        m_BDTValue_1p1n_vs_1pXn = rhs.m_BDTValue_1p1n_vs_1pXn;
        m_BDTValue_3p0n_vs_3pXn = rhs.m_BDTValue_3p0n_vs_3pXn;
        m_pantau_proto_pt = rhs.m_pantau_proto_pt;
        m_pantau_proto_eta = rhs.m_pantau_proto_eta;
        m_pantau_proto_phi = rhs.m_pantau_proto_phi;
        m_pantau_proto_m = rhs.m_pantau_proto_m;
        m_pantau_final_pt = rhs.m_pantau_final_pt;
        m_pantau_final_eta = rhs.m_pantau_final_eta;
        m_pantau_final_phi = rhs.m_pantau_final_phi;
        m_pantau_final_m = rhs.m_pantau_final_m;
    
        m_eflowRec_Basic_NPi0NeutConsts = rhs.m_eflowRec_Basic_NPi0NeutConsts;
        m_eflowRec_Basic_NNeutralConsts = rhs.m_eflowRec_Basic_NNeutralConsts;
        m_eflowRec_Charged_HLV_SumPt = rhs.m_eflowRec_Charged_HLV_SumPt;
        m_eflowRec_Charged_Ratio_EtOverEtAllConsts = rhs.m_eflowRec_Charged_Ratio_EtOverEtAllConsts;
        m_eflowRec_Neutral_HLV_SumM = rhs.m_eflowRec_Neutral_HLV_SumM;
        m_eflowRec_Neutral_PID_BDTValues_EtSort_1 = rhs.m_eflowRec_Neutral_PID_BDTValues_EtSort_1;
        m_eflowRec_Neutral_PID_BDTValues_BDTSort_2 = rhs.m_eflowRec_Neutral_PID_BDTValues_BDTSort_2;
        m_eflowRec_Neutral_Ratio_EtOverEtAllConsts = rhs.m_eflowRec_Neutral_Ratio_EtOverEtAllConsts;
        m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts = rhs.m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts;
        m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged = rhs.m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged;
        
        m_CellBased_Basic_NNeutralConsts = rhs.m_CellBased_Basic_NNeutralConsts;
        m_CellBased_Charged_JetMoment_EtDRxTotalEt = rhs.m_CellBased_Charged_JetMoment_EtDRxTotalEt;
        m_CellBased_Charged_StdDev_Et_WrtEtAllConsts = rhs.m_CellBased_Charged_StdDev_Et_WrtEtAllConsts;
        m_CellBased_Neutral_HLV_SumM = rhs.m_CellBased_Neutral_HLV_SumM;
        m_CellBased_Neutral_PID_BDTValues_BDTSort_1 = rhs.m_CellBased_Neutral_PID_BDTValues_BDTSort_1;
        m_CellBased_Neutral_PID_BDTValues_BDTSort_2 = rhs.m_CellBased_Neutral_PID_BDTValues_BDTSort_2;
        m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts = rhs.m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts;
        m_CellBased_Neutral_Ratio_EtOverEtAllConsts = rhs.m_CellBased_Neutral_Ratio_EtOverEtAllConsts;
        m_CellBased_Neutral_Shots_NPhotonsInSeed = rhs.m_CellBased_Neutral_Shots_NPhotonsInSeed;
        m_CellBased_Combined_DeltaR1stNeutralTo1stCharged = rhs.m_CellBased_Combined_DeltaR1stNeutralTo1stCharged;
    }
    return *this;
}


const std::string& PanTau::PanTauDetails::className() const {
    return s_className;
}





//////////////////////////////////////////////////////////////////
///             Implementations of Set & Get Functions
//////////////////////////////////////////////////////////////////



bool PanTau::PanTauDetails::setScalarDetailByEnum(double newValue, int detEnum_index) {
    PanTau::PanTauDetails::DetailsEnumeration detail = (PanTau::PanTauDetails::DetailsEnumeration)detEnum_index;
    
    switch(detail) {
        
        case PanTau::PanTauDetails::t_isPanTauCandidate:          set_isPanTauCandidate((int)newValue); break;
        case PanTau::PanTauDetails::t_RecoMode_SubAlg:            set_RecoModeSubAlg((int)newValue); break;
        case PanTau::PanTauDetails::t_RecoMode_PanTau:            set_RecoModePanTau((int)newValue); break;
        
        case PanTau::PanTauDetails::t_BDTValue_1p0n_vs_1p1n:      set_BDTValue_1p0n_vs_1p1n(newValue); break;
        case PanTau::PanTauDetails::t_BDTValue_1p1n_vs_1pXn:      set_BDTValue_1p1n_vs_1pXn(newValue); break;
        case PanTau::PanTauDetails::t_BDTValue_3p0n_vs_3pXn:      set_BDTValue_3p0n_vs_3pXn(newValue); break;
        
        case PanTau::PanTauDetails::t_pantau_proto_pt:            set_pantau_proto_pt(newValue); break;
        case PanTau::PanTauDetails::t_pantau_proto_eta:           set_pantau_proto_eta(newValue); break;
        case PanTau::PanTauDetails::t_pantau_proto_phi:           set_pantau_proto_phi(newValue); break;
        case PanTau::PanTauDetails::t_pantau_proto_m:             set_pantau_proto_m(newValue); break;
        
        case PanTau::PanTauDetails::t_pantau_final_pt:            set_pantau_final_pt(newValue); break;
        case PanTau::PanTauDetails::t_pantau_final_eta:           set_pantau_final_eta(newValue); break;
        case PanTau::PanTauDetails::t_pantau_final_phi:           set_pantau_final_phi(newValue); break;
        case PanTau::PanTauDetails::t_pantau_final_m:             set_pantau_final_m(newValue); break;
        
//         case PanTau::PanTauDetails::t_eflowRec_Basic_NPi0NeutConsts:                    set_eflowRec_Basic_NPi0NeutConsts(newValue); break;
//         case PanTau::PanTauDetails::t_eflowRec_Basic_NNeutralConsts:                    set_eflowRec_Basic_NNeutralConsts(newValue); break;
//         case PanTau::PanTauDetails::t_eflowRec_Charged_HLV_SumPt:                       set_eflowRec_Charged_HLV_SumPt(newValue); break;
//         case PanTau::PanTauDetails::t_eflowRec_Charged_Ratio_EtOverEtAllConsts:         set_eflowRec_Charged_Ratio_EtOverEtAllConsts(newValue); break;
//         case PanTau::PanTauDetails::t_eflowRec_Neutral_HLV_SumM:                        set_eflowRec_Neutral_HLV_SumM(newValue); break;
//         case PanTau::PanTauDetails::t_eflowRec_Neutral_PID_BDTValues_EtSort_1:          set_eflowRec_Neutral_PID_BDTValues_EtSort_1(newValue); break;
//         case PanTau::PanTauDetails::t_eflowRec_Neutral_PID_BDTValues_BDTSort_2:         set_eflowRec_Neutral_PID_BDTValues_BDTSort_2(newValue); break;
//         case PanTau::PanTauDetails::t_eflowRec_Neutral_Ratio_EtOverEtAllConsts:         set_eflowRec_Neutral_Ratio_EtOverEtAllConsts(newValue); break;
//         case PanTau::PanTauDetails::t_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts: set_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts(newValue); break;
//         case PanTau::PanTauDetails::t_eflowRec_Combined_DeltaR1stNeutralTo1stCharged:   set_eflowRec_Combined_DeltaR1stNeutralTo1stCharged(newValue); break;
// 
//         case PanTau::PanTauDetails::t_CellBased_Basic_NNeutralConsts:                   set_CellBased_Basic_NNeutralConsts(newValue); break;
//         case PanTau::PanTauDetails::t_CellBased_Charged_JetMoment_EtDRxTotalEt:         set_CellBased_Charged_JetMoment_EtDRxTotalEt(newValue); break;
//         case PanTau::PanTauDetails::t_CellBased_Charged_StdDev_Et_WrtEtAllConsts:       set_CellBased_Charged_StdDev_Et_WrtEtAllConsts(newValue); break;
//         case PanTau::PanTauDetails::t_CellBased_Neutral_HLV_SumM:                       set_CellBased_Neutral_HLV_SumM(newValue); break;
//         case PanTau::PanTauDetails::t_CellBased_Neutral_PID_BDTValues_BDTSort_1:        set_CellBased_Neutral_PID_BDTValues_BDTSort_1(newValue); break;
//         case PanTau::PanTauDetails::t_CellBased_Neutral_PID_BDTValues_BDTSort_2:        set_CellBased_Neutral_PID_BDTValues_BDTSort_2(newValue); break;
//         case PanTau::PanTauDetails::t_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts:  set_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts(newValue); break;
//         case PanTau::PanTauDetails::t_CellBased_Neutral_Ratio_EtOverEtAllConsts:        set_CellBased_Neutral_Ratio_EtOverEtAllConsts(newValue); break;
//         case PanTau::PanTauDetails::t_CellBased_Neutral_Shots_NPhotonsInSeed:           set_CellBased_Neutral_Shots_NPhotonsInSeed(newValue); break;
//         case PanTau::PanTauDetails::t_CellBased_Combined_DeltaR1stNeutralTo1stCharged:  set_CellBased_Combined_DeltaR1stNeutralTo1stCharged(newValue); break;
        
        default:
            std::cout << "PanTauDetails - detail not found" << std::endl;
            return false;
    }
    
    return true;
}



bool PanTau::PanTauDetails::setVectorDetailByEnum(std::vector<double> newVec, int detEnum_index) {
    
    PanTau::PanTauDetails::DetailsEnumeration detail = (PanTau::PanTauDetails::DetailsEnumeration)detEnum_index;
    
    switch(detail) {
        
        case PanTau::PanTauDetails::t_ChargedEFOs_pt:  set_ChargedEFOs_pt(newVec); break;
        case PanTau::PanTauDetails::t_ChargedEFOs_eta: set_ChargedEFOs_eta(newVec); break;
        case PanTau::PanTauDetails::t_ChargedEFOs_phi: set_ChargedEFOs_phi(newVec); break;
        case PanTau::PanTauDetails::t_ChargedEFOs_m:   set_ChargedEFOs_m(newVec); break;
        
        case PanTau::PanTauDetails::t_NeutralEFOs_pt:  set_NeutralEFOs_pt(newVec); break;
        case PanTau::PanTauDetails::t_NeutralEFOs_eta: set_NeutralEFOs_eta(newVec); break;
        case PanTau::PanTauDetails::t_NeutralEFOs_phi: set_NeutralEFOs_phi(newVec); break;
        case PanTau::PanTauDetails::t_NeutralEFOs_m:   set_NeutralEFOs_m(newVec); break;
        
        case PanTau::PanTauDetails::t_Pi0NeutEFOs_pt:  set_Pi0NeutEFOs_pt(newVec); break;
        case PanTau::PanTauDetails::t_Pi0NeutEFOs_eta: set_Pi0NeutEFOs_eta(newVec); break;
        case PanTau::PanTauDetails::t_Pi0NeutEFOs_phi: set_Pi0NeutEFOs_phi(newVec); break;
        case PanTau::PanTauDetails::t_Pi0NeutEFOs_m:   set_Pi0NeutEFOs_m(newVec); break;
        
        default:
            return false;
    }
    
    return true;
}


//! =============================================================
//! Basic PanTau Variables (validity, reco mode & momenta)
//! =============================================================

int PanTau::PanTauDetails::isPanTauCandidate() const {
    return m_isPanTauCandidate;
}

void PanTau::PanTauDetails::set_isPanTauCandidate(
        int isPTC ) {
    
    if(isPTC == 0 || isPTC == 1) {
        m_isPanTauCandidate = isPTC;
        return;
    } else {
        m_isPanTauCandidate = 0;
        std::cout << "ERROR in PanTau::PanTauDetails::set_isPanTauCandidate  argument is neither 0 nor 1! (it is " << isPTC << ")! Will set isPTC to 0" << std::endl;
        return;
    }
}


std::string PanTau::PanTauDetails::inputAlgName() const {return m_Name_InputAlg;}
void        PanTau::PanTauDetails::set_InputAlgName(std::string inAlg) {m_Name_InputAlg = inAlg;}

int     PanTau::PanTauDetails::RecoModeSubAlg() const { return m_RecoMode_SubAlg; }
int     PanTau::PanTauDetails::RecoModePanTau() const { return m_RecoMode_PanTau; }

double  PanTau::PanTauDetails::BDTValue_1p0n_vs_1p1n() const { return m_BDTValue_1p0n_vs_1p1n; }
double  PanTau::PanTauDetails::BDTValue_1p1n_vs_1pXn() const { return m_BDTValue_1p1n_vs_1pXn; }
double  PanTau::PanTauDetails::BDTValue_3p0n_vs_3pXn() const { return m_BDTValue_3p0n_vs_3pXn; }
    
double  PanTau::PanTauDetails::pantau_proto_pt() const { return m_pantau_proto_pt; }
double  PanTau::PanTauDetails::pantau_proto_eta() const { return m_pantau_proto_eta; }
double  PanTau::PanTauDetails::pantau_proto_phi() const { return m_pantau_proto_phi; }
double  PanTau::PanTauDetails::pantau_proto_m() const { return m_pantau_proto_m; }

double  PanTau::PanTauDetails::pantau_final_pt() const { return m_pantau_final_pt; }
double  PanTau::PanTauDetails::pantau_final_eta() const { return m_pantau_final_eta; }
double  PanTau::PanTauDetails::pantau_final_phi() const { return m_pantau_final_phi; }
double  PanTau::PanTauDetails::pantau_final_m() const { return m_pantau_final_m; }

std::vector<double> PanTau::PanTauDetails::ChargedEFOs_pt() const { return m_ChargedEFOs_pt; }
std::vector<double> PanTau::PanTauDetails::ChargedEFOs_eta() const { return m_ChargedEFOs_eta; }
std::vector<double> PanTau::PanTauDetails::ChargedEFOs_phi() const { return m_ChargedEFOs_phi; }
std::vector<double> PanTau::PanTauDetails::ChargedEFOs_m() const { return m_ChargedEFOs_m; }
std::vector<double> PanTau::PanTauDetails::NeutralEFOs_pt() const { return m_NeutralEFOs_pt; }
std::vector<double> PanTau::PanTauDetails::NeutralEFOs_eta() const { return m_NeutralEFOs_eta; }
std::vector<double> PanTau::PanTauDetails::NeutralEFOs_phi() const { return m_NeutralEFOs_phi; }
std::vector<double> PanTau::PanTauDetails::NeutralEFOs_m() const { return m_NeutralEFOs_m; }
std::vector<double> PanTau::PanTauDetails::Pi0NeutEFOs_pt() const { return m_Pi0NeutEFOs_pt; }
std::vector<double> PanTau::PanTauDetails::Pi0NeutEFOs_eta() const { return m_Pi0NeutEFOs_eta; }
std::vector<double> PanTau::PanTauDetails::Pi0NeutEFOs_phi() const { return m_Pi0NeutEFOs_phi; }
std::vector<double> PanTau::PanTauDetails::Pi0NeutEFOs_m() const { return m_Pi0NeutEFOs_m; }
    
void    PanTau::PanTauDetails::set_RecoModeSubAlg( int newValue){ m_RecoMode_SubAlg = newValue; }
void    PanTau::PanTauDetails::set_RecoModePanTau( int newValue){ m_RecoMode_PanTau = newValue; }
void    PanTau::PanTauDetails::set_BDTValue_1p0n_vs_1p1n(double newValue){ m_BDTValue_1p0n_vs_1p1n = newValue; }
void    PanTau::PanTauDetails::set_BDTValue_1p1n_vs_1pXn(double newValue){ m_BDTValue_1p1n_vs_1pXn = newValue; }
void    PanTau::PanTauDetails::set_BDTValue_3p0n_vs_3pXn(double newValue){ m_BDTValue_3p0n_vs_3pXn = newValue; }
void    PanTau::PanTauDetails::set_pantau_proto_pt( double newValue){ m_pantau_proto_pt = newValue; }
void    PanTau::PanTauDetails::set_pantau_proto_eta( double newValue){ m_pantau_proto_eta = newValue; }
void    PanTau::PanTauDetails::set_pantau_proto_phi( double newValue){ m_pantau_proto_phi = newValue; }
void    PanTau::PanTauDetails::set_pantau_proto_m( double newValue){ m_pantau_proto_m = newValue; }
void    PanTau::PanTauDetails::set_pantau_final_pt( double newValue){ m_pantau_final_pt = newValue; }
void    PanTau::PanTauDetails::set_pantau_final_eta( double newValue){ m_pantau_final_eta = newValue; }
void    PanTau::PanTauDetails::set_pantau_final_phi( double newValue){ m_pantau_final_phi = newValue; }
void    PanTau::PanTauDetails::set_pantau_final_m( double newValue){ m_pantau_final_m = newValue; }
void    PanTau::PanTauDetails::set_ChargedEFOs_pt( std::vector<double> newValue) { m_ChargedEFOs_pt = newValue; }
void    PanTau::PanTauDetails::set_ChargedEFOs_eta( std::vector<double> newValue) { m_ChargedEFOs_eta = newValue; }
void    PanTau::PanTauDetails::set_ChargedEFOs_phi( std::vector<double> newValue) { m_ChargedEFOs_phi = newValue; }
void    PanTau::PanTauDetails::set_ChargedEFOs_m( std::vector<double> newValue) { m_ChargedEFOs_m = newValue; }
void    PanTau::PanTauDetails::set_NeutralEFOs_pt( std::vector<double> newValue) { m_NeutralEFOs_pt = newValue; }
void    PanTau::PanTauDetails::set_NeutralEFOs_eta( std::vector<double> newValue) { m_NeutralEFOs_eta = newValue; }
void    PanTau::PanTauDetails::set_NeutralEFOs_phi( std::vector<double> newValue) { m_NeutralEFOs_phi = newValue; }
void    PanTau::PanTauDetails::set_NeutralEFOs_m( std::vector<double> newValue) { m_NeutralEFOs_m = newValue; }
void    PanTau::PanTauDetails::set_Pi0NeutEFOs_pt( std::vector<double> newValue) { m_Pi0NeutEFOs_pt = newValue; }
void    PanTau::PanTauDetails::set_Pi0NeutEFOs_eta( std::vector<double> newValue) { m_Pi0NeutEFOs_eta = newValue; }
void    PanTau::PanTauDetails::set_Pi0NeutEFOs_phi( std::vector<double> newValue) { m_Pi0NeutEFOs_phi = newValue; }
void    PanTau::PanTauDetails::set_Pi0NeutEFOs_m( std::vector<double> newValue) { m_Pi0NeutEFOs_m = newValue; }

//! =============================================================
//! Corfu ID Variables
//! =============================================================

//! Get Methods
//! ===========

int    PanTau::PanTauDetails::eflowRec_Basic_NPi0NeutConsts() const {return m_eflowRec_Basic_NPi0NeutConsts;}
int    PanTau::PanTauDetails::eflowRec_Basic_NNeutralConsts() const {return m_eflowRec_Basic_NNeutralConsts;}
double PanTau::PanTauDetails::eflowRec_Charged_HLV_SumPt() const {return m_eflowRec_Charged_HLV_SumPt;}
double PanTau::PanTauDetails::eflowRec_Charged_Ratio_EtOverEtAllConsts() const {return m_eflowRec_Charged_Ratio_EtOverEtAllConsts;}
double PanTau::PanTauDetails::eflowRec_Neutral_HLV_SumM() const {return m_eflowRec_Neutral_HLV_SumM;}
double PanTau::PanTauDetails::eflowRec_Neutral_PID_BDTValues_EtSort_1() const {return m_eflowRec_Neutral_PID_BDTValues_EtSort_1;}
double PanTau::PanTauDetails::eflowRec_Neutral_PID_BDTValues_BDTSort_2() const {return m_eflowRec_Neutral_PID_BDTValues_BDTSort_2;}
double PanTau::PanTauDetails::eflowRec_Neutral_Ratio_EtOverEtAllConsts() const {return m_eflowRec_Neutral_Ratio_EtOverEtAllConsts;}
double PanTau::PanTauDetails::eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts() const {return m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts;}
double PanTau::PanTauDetails::eflowRec_Combined_DeltaR1stNeutralTo1stCharged() const {return m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged;}

int    PanTau::PanTauDetails::CellBased_Basic_NNeutralConsts() const {return m_CellBased_Basic_NNeutralConsts;}
double PanTau::PanTauDetails::CellBased_Charged_JetMoment_EtDRxTotalEt() const {return m_CellBased_Charged_JetMoment_EtDRxTotalEt;}
double PanTau::PanTauDetails::CellBased_Charged_StdDev_Et_WrtEtAllConsts() const {return m_CellBased_Charged_StdDev_Et_WrtEtAllConsts;}
double PanTau::PanTauDetails::CellBased_Neutral_HLV_SumM() const {return m_CellBased_Neutral_HLV_SumM;}
double PanTau::PanTauDetails::CellBased_Neutral_PID_BDTValues_BDTSort_1() const {return m_CellBased_Neutral_PID_BDTValues_BDTSort_1;}
double PanTau::PanTauDetails::CellBased_Neutral_PID_BDTValues_BDTSort_2() const {return m_CellBased_Neutral_PID_BDTValues_BDTSort_2;}
double PanTau::PanTauDetails::CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts() const {return m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts;}
double PanTau::PanTauDetails::CellBased_Neutral_Ratio_EtOverEtAllConsts() const {return m_CellBased_Neutral_Ratio_EtOverEtAllConsts;}
double PanTau::PanTauDetails::CellBased_Neutral_Shots_NPhotonsInSeed() const {return m_CellBased_Neutral_Shots_NPhotonsInSeed;}
double PanTau::PanTauDetails::CellBased_Combined_DeltaR1stNeutralTo1stCharged() const {return m_CellBased_Combined_DeltaR1stNeutralTo1stCharged;}


//! Set Methods
//! ===========

void PanTau::PanTauDetails::set_eflowRec_Basic_NPi0NeutConsts(double newValue) { m_eflowRec_Basic_NPi0NeutConsts= newValue;}
void PanTau::PanTauDetails::set_eflowRec_Basic_NNeutralConsts(double newValue) { m_eflowRec_Basic_NNeutralConsts= newValue;}
void PanTau::PanTauDetails::set_eflowRec_Charged_HLV_SumPt(double newValue) { m_eflowRec_Charged_HLV_SumPt= newValue;}
void PanTau::PanTauDetails::set_eflowRec_Charged_Ratio_EtOverEtAllConsts(double newValue) { m_eflowRec_Charged_Ratio_EtOverEtAllConsts= newValue;}
void PanTau::PanTauDetails::set_eflowRec_Neutral_HLV_SumM(double newValue) { m_eflowRec_Neutral_HLV_SumM= newValue;}
void PanTau::PanTauDetails::set_eflowRec_Neutral_PID_BDTValues_EtSort_1(double newValue) { m_eflowRec_Neutral_PID_BDTValues_EtSort_1= newValue;}
void PanTau::PanTauDetails::set_eflowRec_Neutral_PID_BDTValues_BDTSort_2(double newValue) { m_eflowRec_Neutral_PID_BDTValues_BDTSort_2= newValue;}
void PanTau::PanTauDetails::set_eflowRec_Neutral_Ratio_EtOverEtAllConsts(double newValue) { m_eflowRec_Neutral_Ratio_EtOverEtAllConsts= newValue;}
void PanTau::PanTauDetails::set_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts(double newValue) { m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts= newValue;}
void PanTau::PanTauDetails::set_eflowRec_Combined_DeltaR1stNeutralTo1stCharged(double newValue) { m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged= newValue;}

void PanTau::PanTauDetails::set_CellBased_Basic_NNeutralConsts(double newValue) { m_CellBased_Basic_NNeutralConsts= newValue;}
void PanTau::PanTauDetails::set_CellBased_Charged_JetMoment_EtDRxTotalEt(double newValue) { m_CellBased_Charged_JetMoment_EtDRxTotalEt= newValue;}
void PanTau::PanTauDetails::set_CellBased_Charged_StdDev_Et_WrtEtAllConsts(double newValue) { m_CellBased_Charged_StdDev_Et_WrtEtAllConsts= newValue;}
void PanTau::PanTauDetails::set_CellBased_Neutral_HLV_SumM(double newValue) { m_CellBased_Neutral_HLV_SumM= newValue;}
void PanTau::PanTauDetails::set_CellBased_Neutral_PID_BDTValues_BDTSort_1(double newValue) { m_CellBased_Neutral_PID_BDTValues_BDTSort_1= newValue;}
void PanTau::PanTauDetails::set_CellBased_Neutral_PID_BDTValues_BDTSort_2(double newValue) { m_CellBased_Neutral_PID_BDTValues_BDTSort_2= newValue;}
void PanTau::PanTauDetails::set_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts(double newValue) { m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts= newValue;}
void PanTau::PanTauDetails::set_CellBased_Neutral_Ratio_EtOverEtAllConsts(double newValue) { m_CellBased_Neutral_Ratio_EtOverEtAllConsts= newValue;}
void PanTau::PanTauDetails::set_CellBased_Neutral_Shots_NPhotonsInSeed(double newValue) { m_CellBased_Neutral_Shots_NPhotonsInSeed= newValue;}
void PanTau::PanTauDetails::set_CellBased_Combined_DeltaR1stNeutralTo1stCharged(double newValue) { m_CellBased_Combined_DeltaR1stNeutralTo1stCharged= newValue;}
        

//! End of Set Methods
//! ==================

