/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// ////////////////////////////////////////////////////////////
/// 
/// Header for persistent version of PanTauDetails class
/// 
/// ////////////////////////////////////////////////////////////


#ifndef TAUEVENTTPCNV_PANTAUDETAILS_P1_H
#define TAUEVENTTPCNV_PANTAUDETAILS_P1_H
/********************************************************************

NAME:     PanTauDetails_p1.h
PACKAGE:  offline/Reconstruction/PanTau

AUTHORS:  Christian Limbach
CREATED:  29.November.2011

PURPOSE:  PanTau::PanTauDetails persistent data class

********************************************************************/

//!Header Files
#include "DataModelAthenaPool/ElementLinkVector_p1.h"

#include <vector>


//!Forward declarations
class PanTauDetailsCnv;


class PanTauDetails_p1
{
    public:
        PanTauDetails_p1();
       ~PanTauDetails_p1() = default;
        
        friend class PanTauDetailsCnv_p1;

    private:
        
    //! ======================================================================
    //! Basic PanTau Variables (validity, reco mode & momenta)
    //! ======================================================================
    
    int         m_isPanTauCandidate; //!< tells you whether this tauRec seed is considered as candidate for PanTau
    std::string m_Name_InputAlg;
    int     m_RecoMode_SubAlg;
    int     m_RecoMode_PanTau;
    double  m_BDTValue_1p0n_vs_1p1n;
    double  m_BDTValue_1p1n_vs_1pXn;
    double  m_BDTValue_3p0n_vs_3pXn;
    double  m_pantau_proto_pt;
    double  m_pantau_proto_eta;
    double  m_pantau_proto_phi;
    double  m_pantau_proto_m;
    double  m_pantau_final_pt;
    double  m_pantau_final_eta;
    double  m_pantau_final_phi;
    double  m_pantau_final_m;
    
    std::vector<double> m_ChargedEFOs_pt;
    std::vector<double> m_ChargedEFOs_eta;
    std::vector<double> m_ChargedEFOs_phi;
    std::vector<double> m_ChargedEFOs_m;
    
    std::vector<double> m_NeutralEFOs_pt;
    std::vector<double> m_NeutralEFOs_eta;
    std::vector<double> m_NeutralEFOs_phi;
    std::vector<double> m_NeutralEFOs_m;
    
    std::vector<double> m_Pi0NeutEFOs_pt;
    std::vector<double> m_Pi0NeutEFOs_eta;
    std::vector<double> m_Pi0NeutEFOs_phi;
    std::vector<double> m_Pi0NeutEFOs_m;
    
    
    //! ======================================================================
    //! ID Variables 
    //! ======================================================================
    
    int     m_eflowRec_Basic_NPi0NeutConsts;
    int     m_eflowRec_Basic_NNeutralConsts;
    double  m_eflowRec_Charged_HLV_SumPt;
    double  m_eflowRec_Charged_Ratio_EtOverEtAllConsts;
    double  m_eflowRec_Neutral_HLV_SumM;
    double  m_eflowRec_Neutral_PID_BDTValues_EtSort_1;
    double  m_eflowRec_Neutral_PID_BDTValues_BDTSort_2;
    double  m_eflowRec_Neutral_Ratio_EtOverEtAllConsts;
    double  m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts;
    double  m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged;
    
    int     m_CellBased_Basic_NNeutralConsts;
    double  m_CellBased_Charged_JetMoment_EtDRxTotalEt;
    double  m_CellBased_Charged_StdDev_Et_WrtEtAllConsts;
    double  m_CellBased_Neutral_HLV_SumM;
    double  m_CellBased_Neutral_PID_BDTValues_BDTSort_1;
    double  m_CellBased_Neutral_PID_BDTValues_BDTSort_2;
    double  m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts;
    double  m_CellBased_Neutral_Ratio_EtOverEtAllConsts;
    double  m_CellBased_Neutral_Shots_NPhotonsInSeed;
    double  m_CellBased_Combined_DeltaR1stNeutralTo1stCharged;
        
};
#endif

