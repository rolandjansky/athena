/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PanTauEvent/PanTauDetails_eflowRec.h"
#include "PanTauEvent/TauFeature.h"
#include "PanTauEvent/TauSeed.h"

//#include "GaudiKernel/MsgStream.h"
#include <string>

std::string PanTau::PanTauDetails_eflowRec::s_className = "PanTauDetails_eflowRec";

/** Default constructor */
PanTau::PanTauDetails_eflowRec::PanTauDetails_eflowRec()
    :
    PanTau::PanTauDetails(),
    m_eflowRec_Basic_NPi0NeutConsts(-5),
    m_eflowRec_Basic_NNeutralConsts(-5),
    m_eflowRec_Charged_HLV_SumPt(-1111),
    m_eflowRec_Charged_Ratio_EtOverEtAllConsts(-5),
    m_eflowRec_Neutral_HLV_SumM(-1111),
    m_eflowRec_Neutral_PID_BDTValues_EtSort_1(-5),
    m_eflowRec_Neutral_PID_BDTValues_BDTSort_2(-5),
    m_eflowRec_Neutral_Ratio_EtOverEtAllConsts(-5),
    m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts(-5),
    m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged(-5)
{
}



PanTau::PanTauDetails_eflowRec::PanTauDetails_eflowRec(PanTau::PanTauSeed* inputSeed) 
    :
    PanTau::PanTauDetails(inputSeed)
{
    if(inputSeed->getIsValidSeed() == true) {
        bool        isValid         = false;
        TauFeature* curFeatures     = inputSeed->getFeatures();
        
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
        
    }
    else {
      m_eflowRec_Basic_NPi0NeutConsts = -5;
      m_eflowRec_Basic_NNeutralConsts = -5;
      m_eflowRec_Charged_HLV_SumPt = -1111;
      m_eflowRec_Charged_Ratio_EtOverEtAllConsts = -5;
      m_eflowRec_Neutral_HLV_SumM = -1111;
      m_eflowRec_Neutral_PID_BDTValues_EtSort_1 = -5;
      m_eflowRec_Neutral_PID_BDTValues_BDTSort_2 = -5;
      m_eflowRec_Neutral_Ratio_EtOverEtAllConsts = -5;
      m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts = -5;
      m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged = -5;
    }
}



/** Copy constructor */
PanTau::PanTauDetails_eflowRec::PanTauDetails_eflowRec(
    const PanTau::PanTauDetails_eflowRec& rhs
    ):
    PanTau::PanTauDetails(rhs.m_PanTauSeed),
    m_eflowRec_Basic_NPi0NeutConsts(rhs.m_eflowRec_Basic_NPi0NeutConsts),
    m_eflowRec_Basic_NNeutralConsts(rhs.m_eflowRec_Basic_NNeutralConsts),
    m_eflowRec_Charged_HLV_SumPt(rhs.m_eflowRec_Charged_HLV_SumPt),
    m_eflowRec_Charged_Ratio_EtOverEtAllConsts(rhs.m_eflowRec_Charged_Ratio_EtOverEtAllConsts),
    m_eflowRec_Neutral_HLV_SumM(rhs.m_eflowRec_Neutral_HLV_SumM),
    m_eflowRec_Neutral_PID_BDTValues_EtSort_1(rhs.m_eflowRec_Neutral_PID_BDTValues_EtSort_1),
    m_eflowRec_Neutral_PID_BDTValues_BDTSort_2(rhs.m_eflowRec_Neutral_PID_BDTValues_BDTSort_2),
    m_eflowRec_Neutral_Ratio_EtOverEtAllConsts(rhs.m_eflowRec_Neutral_Ratio_EtOverEtAllConsts),
    m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts(rhs.m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts),
    m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged(rhs.m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged)
{
}


PanTau::PanTauDetails_eflowRec::~PanTauDetails_eflowRec()
{
}

/** Assignment operator */
PanTau::PanTauDetails_eflowRec& PanTau::PanTauDetails_eflowRec::operator=(const PanTau::PanTauDetails_eflowRec& rhs)
{
    if (this!=&rhs){
        
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
    }
    return *this;
}


const std::string& PanTau::PanTauDetails_eflowRec::className() const {
    return s_className;
}


void PanTau::PanTauDetails_eflowRec::addGeneralDetails(PanTau::PanTauDetails* pantauDetails) {
    
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



bool PanTau::PanTauDetails_eflowRec::setScalarDetailByEnum(double newValue, int detEnum_index) {
    
    //if it is not, check if it is in these specific details
    PanTau::PanTauDetails_eflowRec::DetailsEnumeration detail = (PanTau::PanTauDetails_eflowRec::DetailsEnumeration)detEnum_index;
    switch(detail) {
        
        case PanTau::PanTauDetails_eflowRec::t_eflowRec_Basic_NPi0NeutConsts:                    set_eflowRec_Basic_NPi0NeutConsts(newValue); break;
        case PanTau::PanTauDetails_eflowRec::t_eflowRec_Basic_NNeutralConsts:                    set_eflowRec_Basic_NNeutralConsts(newValue); break;
        case PanTau::PanTauDetails_eflowRec::t_eflowRec_Charged_HLV_SumPt:                       set_eflowRec_Charged_HLV_SumPt(newValue); break;
        case PanTau::PanTauDetails_eflowRec::t_eflowRec_Charged_Ratio_EtOverEtAllConsts:         set_eflowRec_Charged_Ratio_EtOverEtAllConsts(newValue); break;
        case PanTau::PanTauDetails_eflowRec::t_eflowRec_Neutral_HLV_SumM:                        set_eflowRec_Neutral_HLV_SumM(newValue); break;
        case PanTau::PanTauDetails_eflowRec::t_eflowRec_Neutral_PID_BDTValues_EtSort_1:          set_eflowRec_Neutral_PID_BDTValues_EtSort_1(newValue); break;
        case PanTau::PanTauDetails_eflowRec::t_eflowRec_Neutral_PID_BDTValues_BDTSort_2:         set_eflowRec_Neutral_PID_BDTValues_BDTSort_2(newValue); break;
        case PanTau::PanTauDetails_eflowRec::t_eflowRec_Neutral_Ratio_EtOverEtAllConsts:         set_eflowRec_Neutral_Ratio_EtOverEtAllConsts(newValue); break;
        case PanTau::PanTauDetails_eflowRec::t_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts: set_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts(newValue); break;
        case PanTau::PanTauDetails_eflowRec::t_eflowRec_Combined_DeltaR1stNeutralTo1stCharged:   set_eflowRec_Combined_DeltaR1stNeutralTo1stCharged(newValue); break;
        
        default:
            std::cout << "PanTauDetails_eflowRec - detail not found" << std::endl;
            return false;
    }
    
    return true;
}



bool PanTau::PanTauDetails_eflowRec::setVectorDetailByEnum(std::vector<double> newVec, int detEnum_index) {
    
    //check if the detail to be set is in the general list of details
    bool isInGeneralDetails = PanTau::PanTauDetails::setVectorDetailByEnum(newVec, detEnum_index);
    if(isInGeneralDetails == true) return true;
    
    //if it is not, check if it is in these specific details
    PanTau::PanTauDetails_eflowRec::DetailsEnumeration detail = (PanTau::PanTauDetails_eflowRec::DetailsEnumeration)detEnum_index;
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

int    PanTau::PanTauDetails_eflowRec::eflowRec_Basic_NPi0NeutConsts() const {return m_eflowRec_Basic_NPi0NeutConsts;}
int    PanTau::PanTauDetails_eflowRec::eflowRec_Basic_NNeutralConsts() const {return m_eflowRec_Basic_NNeutralConsts;}
double PanTau::PanTauDetails_eflowRec::eflowRec_Charged_HLV_SumPt() const {return m_eflowRec_Charged_HLV_SumPt;}
double PanTau::PanTauDetails_eflowRec::eflowRec_Charged_Ratio_EtOverEtAllConsts() const {return m_eflowRec_Charged_Ratio_EtOverEtAllConsts;}
double PanTau::PanTauDetails_eflowRec::eflowRec_Neutral_HLV_SumM() const {return m_eflowRec_Neutral_HLV_SumM;}
double PanTau::PanTauDetails_eflowRec::eflowRec_Neutral_PID_BDTValues_EtSort_1() const {return m_eflowRec_Neutral_PID_BDTValues_EtSort_1;}
double PanTau::PanTauDetails_eflowRec::eflowRec_Neutral_PID_BDTValues_BDTSort_2() const {return m_eflowRec_Neutral_PID_BDTValues_BDTSort_2;}
double PanTau::PanTauDetails_eflowRec::eflowRec_Neutral_Ratio_EtOverEtAllConsts() const {return m_eflowRec_Neutral_Ratio_EtOverEtAllConsts;}
double PanTau::PanTauDetails_eflowRec::eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts() const {return m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts;}
double PanTau::PanTauDetails_eflowRec::eflowRec_Combined_DeltaR1stNeutralTo1stCharged() const {return m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged;}

//! Set Methods
//! ===========

void PanTau::PanTauDetails_eflowRec::set_eflowRec_Basic_NPi0NeutConsts(double newValue) { m_eflowRec_Basic_NPi0NeutConsts= newValue;}
void PanTau::PanTauDetails_eflowRec::set_eflowRec_Basic_NNeutralConsts(double newValue) { m_eflowRec_Basic_NNeutralConsts= newValue;}
void PanTau::PanTauDetails_eflowRec::set_eflowRec_Charged_HLV_SumPt(double newValue) { m_eflowRec_Charged_HLV_SumPt= newValue;}
void PanTau::PanTauDetails_eflowRec::set_eflowRec_Charged_Ratio_EtOverEtAllConsts(double newValue) { m_eflowRec_Charged_Ratio_EtOverEtAllConsts= newValue;}
void PanTau::PanTauDetails_eflowRec::set_eflowRec_Neutral_HLV_SumM(double newValue) { m_eflowRec_Neutral_HLV_SumM= newValue;}
void PanTau::PanTauDetails_eflowRec::set_eflowRec_Neutral_PID_BDTValues_EtSort_1(double newValue) { m_eflowRec_Neutral_PID_BDTValues_EtSort_1= newValue;}
void PanTau::PanTauDetails_eflowRec::set_eflowRec_Neutral_PID_BDTValues_BDTSort_2(double newValue) { m_eflowRec_Neutral_PID_BDTValues_BDTSort_2= newValue;}
void PanTau::PanTauDetails_eflowRec::set_eflowRec_Neutral_Ratio_EtOverEtAllConsts(double newValue) { m_eflowRec_Neutral_Ratio_EtOverEtAllConsts= newValue;}
void PanTau::PanTauDetails_eflowRec::set_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts(double newValue) { m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts= newValue;}
void PanTau::PanTauDetails_eflowRec::set_eflowRec_Combined_DeltaR1stNeutralTo1stCharged(double newValue) { m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged= newValue;}
  

//! End of Set Methods
//! ==================

