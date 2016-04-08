/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PanTauEvent/PanTauDetails_CellBased.h"
#include "PanTauEvent/TauFeature.h"
#include "PanTauEvent/TauSeed.h"

//#include "GaudiKernel/MsgStream.h"
#include <string>

std::string PanTau::PanTauDetails_CellBased::s_className = "PanTauDetails_CellBased";

/** Default constructor */
PanTau::PanTauDetails_CellBased::PanTauDetails_CellBased()
    :
    PanTau::PanTauDetails(),
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



PanTau::PanTauDetails_CellBased::PanTauDetails_CellBased(PanTau::PanTauSeed* inputSeed) 
    :
    PanTau::PanTauDetails(inputSeed)
{
    if(inputSeed->getIsValidSeed() == true) {
        bool        isValid         = false;
        TauFeature* curFeatures     = inputSeed->getFeatures();

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
    else {
      m_CellBased_Basic_NNeutralConsts = -5;
      m_CellBased_Charged_JetMoment_EtDRxTotalEt = -1111;
      m_CellBased_Charged_StdDev_Et_WrtEtAllConsts = -1111;
      m_CellBased_Neutral_HLV_SumM = -1111;
      m_CellBased_Neutral_PID_BDTValues_BDTSort_1 = -5;
      m_CellBased_Neutral_PID_BDTValues_BDTSort_2 = -5;
      m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts = -5;
      m_CellBased_Neutral_Ratio_EtOverEtAllConsts = -5;
      m_CellBased_Neutral_Shots_NPhotonsInSeed = -5;
      m_CellBased_Combined_DeltaR1stNeutralTo1stCharged = -5;
    }
}



/** Copy constructor */
PanTau::PanTauDetails_CellBased::PanTauDetails_CellBased(
    const PanTau::PanTauDetails_CellBased& rhs
    ):
    PanTau::PanTauDetails(rhs.m_PanTauSeed),
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



PanTau::PanTauDetails_CellBased::~PanTauDetails_CellBased()
{
}



/** Assignment operator */
PanTau::PanTauDetails_CellBased& PanTau::PanTauDetails_CellBased::operator=(const PanTau::PanTauDetails_CellBased& rhs)
{
    if (this!=&rhs){
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



const std::string& PanTau::PanTauDetails_CellBased::className() const {
    return s_className;
}



void PanTau::PanTauDetails_CellBased::addGeneralDetails(PanTau::PanTauDetails* pantauDetails) {
    
    m_isPanTauCandidate = pantauDetails->isPanTauCandidate();
    m_RecoMode_SubAlg   = pantauDetails->RecoModeSubAlg();
    m_RecoMode_PanTau   = pantauDetails->RecoModePanTau();
    m_BDTValue_1p0n_vs_1p1n = pantauDetails->BDTValue_1p0n_vs_1p1n();
    m_BDTValue_1p1n_vs_1pXn = pantauDetails->BDTValue_1p1n_vs_1pXn();
    m_BDTValue_3p0n_vs_3pXn = pantauDetails->BDTValue_3p0n_vs_3pXn();
    m_pantau_proto_pt   = pantauDetails->pantau_proto_pt();
    m_pantau_proto_eta  = pantauDetails->pantau_proto_eta();
    m_pantau_proto_phi  = pantauDetails->pantau_proto_phi();
    m_pantau_proto_m    = pantauDetails->pantau_proto_m();
    m_pantau_final_pt   = pantauDetails->pantau_final_pt();
    m_pantau_final_eta  = pantauDetails->pantau_final_eta();
    m_pantau_final_phi  = pantauDetails->pantau_final_phi();
    m_pantau_final_m    = pantauDetails->pantau_final_m();
    
    m_ChargedEFOs_pt    = pantauDetails->ChargedEFOs_pt();
    m_ChargedEFOs_eta   = pantauDetails->ChargedEFOs_eta();
    m_ChargedEFOs_phi   = pantauDetails->ChargedEFOs_phi();
    m_ChargedEFOs_m     = pantauDetails->ChargedEFOs_m();
    
    m_NeutralEFOs_pt    = pantauDetails->NeutralEFOs_pt();
    m_NeutralEFOs_eta   = pantauDetails->NeutralEFOs_eta();
    m_NeutralEFOs_phi   = pantauDetails->NeutralEFOs_phi();
    m_NeutralEFOs_m     = pantauDetails->NeutralEFOs_m();
    
    m_Pi0NeutEFOs_pt    = pantauDetails->Pi0NeutEFOs_pt();
    m_Pi0NeutEFOs_eta   = pantauDetails->Pi0NeutEFOs_eta();
    m_Pi0NeutEFOs_phi   = pantauDetails->Pi0NeutEFOs_phi();
    m_Pi0NeutEFOs_m     = pantauDetails->Pi0NeutEFOs_m();
    
    return;
}

//////////////////////////////////////////////////////////////////
///             Implementations of Set & Get Functions
//////////////////////////////////////////////////////////////////



bool PanTau::PanTauDetails_CellBased::setScalarDetailByEnum(double newValue, int detEnum_index) {
    //if it is not, check if it is in these specific details
    PanTau::PanTauDetails_CellBased::DetailsEnumeration detail = (PanTau::PanTauDetails_CellBased::DetailsEnumeration)detEnum_index;
    switch(detail) {
        
        case PanTau::PanTauDetails_CellBased::t_CellBased_Basic_NNeutralConsts:                   set_CellBased_Basic_NNeutralConsts(newValue); break;
        case PanTau::PanTauDetails_CellBased::t_CellBased_Charged_JetMoment_EtDRxTotalEt:         set_CellBased_Charged_JetMoment_EtDRxTotalEt(newValue); break;
        case PanTau::PanTauDetails_CellBased::t_CellBased_Charged_StdDev_Et_WrtEtAllConsts:       set_CellBased_Charged_StdDev_Et_WrtEtAllConsts(newValue); break;
        case PanTau::PanTauDetails_CellBased::t_CellBased_Neutral_HLV_SumM:                       set_CellBased_Neutral_HLV_SumM(newValue); break;
        case PanTau::PanTauDetails_CellBased::t_CellBased_Neutral_PID_BDTValues_BDTSort_1:        set_CellBased_Neutral_PID_BDTValues_BDTSort_1(newValue); break;
        case PanTau::PanTauDetails_CellBased::t_CellBased_Neutral_PID_BDTValues_BDTSort_2:        set_CellBased_Neutral_PID_BDTValues_BDTSort_2(newValue); break;
        case PanTau::PanTauDetails_CellBased::t_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts:  set_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts(newValue); break;
        case PanTau::PanTauDetails_CellBased::t_CellBased_Neutral_Ratio_EtOverEtAllConsts:        set_CellBased_Neutral_Ratio_EtOverEtAllConsts(newValue); break;
        case PanTau::PanTauDetails_CellBased::t_CellBased_Neutral_Shots_NPhotonsInSeed:           set_CellBased_Neutral_Shots_NPhotonsInSeed(newValue); break;
        case PanTau::PanTauDetails_CellBased::t_CellBased_Combined_DeltaR1stNeutralTo1stCharged:  set_CellBased_Combined_DeltaR1stNeutralTo1stCharged(newValue); break;
        
        default:
            std::cout << "PanTauDetails_CellBased - detail not found" << std::endl;
            return false;
    }
    
    return true;
}



bool PanTau::PanTauDetails_CellBased::setVectorDetailByEnum(std::vector<double> newVec, int detEnum_index) {
    
    //check if the detail to be set is in the general list of details
    bool isInGeneralDetails = PanTau::PanTauDetails::setVectorDetailByEnum(newVec, detEnum_index);
    if(isInGeneralDetails == true) return true;
    
    //if it is not, check if it is in these specific details
    PanTau::PanTauDetails_CellBased::DetailsEnumeration detail = (PanTau::PanTauDetails_CellBased::DetailsEnumeration)detEnum_index;
    switch(detail) {
        
        default:
            return false;
    }
    
    return true;
}


//! =============================================================
//! Corfu ID Variables
//! =============================================================

//! Get Methods
//! ===========

int    PanTau::PanTauDetails_CellBased::CellBased_Basic_NNeutralConsts() const {return m_CellBased_Basic_NNeutralConsts;}
double PanTau::PanTauDetails_CellBased::CellBased_Charged_JetMoment_EtDRxTotalEt() const {return m_CellBased_Charged_JetMoment_EtDRxTotalEt;}
double PanTau::PanTauDetails_CellBased::CellBased_Charged_StdDev_Et_WrtEtAllConsts() const {return m_CellBased_Charged_StdDev_Et_WrtEtAllConsts;}
double PanTau::PanTauDetails_CellBased::CellBased_Neutral_HLV_SumM() const {return m_CellBased_Neutral_HLV_SumM;}
double PanTau::PanTauDetails_CellBased::CellBased_Neutral_PID_BDTValues_BDTSort_1() const {return m_CellBased_Neutral_PID_BDTValues_BDTSort_1;}
double PanTau::PanTauDetails_CellBased::CellBased_Neutral_PID_BDTValues_BDTSort_2() const {return m_CellBased_Neutral_PID_BDTValues_BDTSort_2;}
double PanTau::PanTauDetails_CellBased::CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts() const {return m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts;}
double PanTau::PanTauDetails_CellBased::CellBased_Neutral_Ratio_EtOverEtAllConsts() const {return m_CellBased_Neutral_Ratio_EtOverEtAllConsts;}
double PanTau::PanTauDetails_CellBased::CellBased_Neutral_Shots_NPhotonsInSeed() const {return m_CellBased_Neutral_Shots_NPhotonsInSeed;}
double PanTau::PanTauDetails_CellBased::CellBased_Combined_DeltaR1stNeutralTo1stCharged() const {return m_CellBased_Combined_DeltaR1stNeutralTo1stCharged;}


//! Set Methods
//! ===========

void PanTau::PanTauDetails_CellBased::set_CellBased_Basic_NNeutralConsts(double newValue) { m_CellBased_Basic_NNeutralConsts= newValue;}
void PanTau::PanTauDetails_CellBased::set_CellBased_Charged_JetMoment_EtDRxTotalEt(double newValue) { m_CellBased_Charged_JetMoment_EtDRxTotalEt= newValue;}
void PanTau::PanTauDetails_CellBased::set_CellBased_Charged_StdDev_Et_WrtEtAllConsts(double newValue) { m_CellBased_Charged_StdDev_Et_WrtEtAllConsts= newValue;}
void PanTau::PanTauDetails_CellBased::set_CellBased_Neutral_HLV_SumM(double newValue) { m_CellBased_Neutral_HLV_SumM= newValue;}
void PanTau::PanTauDetails_CellBased::set_CellBased_Neutral_PID_BDTValues_BDTSort_1(double newValue) { m_CellBased_Neutral_PID_BDTValues_BDTSort_1= newValue;}
void PanTau::PanTauDetails_CellBased::set_CellBased_Neutral_PID_BDTValues_BDTSort_2(double newValue) { m_CellBased_Neutral_PID_BDTValues_BDTSort_2= newValue;}
void PanTau::PanTauDetails_CellBased::set_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts(double newValue) { m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts= newValue;}
void PanTau::PanTauDetails_CellBased::set_CellBased_Neutral_Ratio_EtOverEtAllConsts(double newValue) { m_CellBased_Neutral_Ratio_EtOverEtAllConsts= newValue;}
void PanTau::PanTauDetails_CellBased::set_CellBased_Neutral_Shots_NPhotonsInSeed(double newValue) { m_CellBased_Neutral_Shots_NPhotonsInSeed= newValue;}
void PanTau::PanTauDetails_CellBased::set_CellBased_Combined_DeltaR1stNeutralTo1stCharged(double newValue) { m_CellBased_Combined_DeltaR1stNeutralTo1stCharged= newValue;}
        

//! End of Set Methods
//! ==================

