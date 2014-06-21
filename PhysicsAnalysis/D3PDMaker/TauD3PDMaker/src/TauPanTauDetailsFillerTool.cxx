/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauPanTauDetailsFillerTool.cxx 570071 2013-11-12 21:22:49Z limbach $
/**
 * @file PhysicsAnalysis/D3PDMaker/TauD3PDMaker/src/TauPanTauDetailsFillerTool.cxx
 * @author Christian Limbach  (limbach -at- physik.uni-bonn.de)
 * @date Oct, 2011
 * @brief Block filler tool for TauPanTau details.
 */


#include "TauPanTauDetailsFillerTool.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauDetails.h"
#include "PanTauEvent/PanTauDetails.h"
#include "PanTauEvent/PanTauDetails_eflowRec.h"
#include "PanTauEvent/PanTauDetails_CellBased.h"
#include "AthenaKernel/errorcheck.h"
#include "TRandom2.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TauPanTauDetailsFillerTool::TauPanTauDetailsFillerTool (const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent)
    : BlockFillerTool<Analysis::TauJet> (type, name, parent)
{
  declareProperty("InputAlgorithm",                       m_Name_InputAlgorithm,                            "name of input algorithm");
  declareProperty("otherProp",                       m_otherProp,                            "test");
  
  book().ignore(); // Avoid coverity warnings
  
}


/**
 * @brief Book variables for this block.
 */
StatusCode TauPanTauDetailsFillerTool::book()
{
    CHECK( addVariable ("pantau_" + m_Name_InputAlgorithm + "_isPanTauCandidate", m_isPanTauCandidate)  );
    
    CHECK( addVariable ("pantau_" + m_Name_InputAlgorithm + "_RecoMode" + m_Name_InputAlgorithm,    m_RecoMode_SubAlg)  );
    CHECK( addVariable ("pantau_" + m_Name_InputAlgorithm + "_RecoModePanTau",                      m_RecoMode_PanTau)  );
    
    CHECK( addVariable ("pantau_" + m_Name_InputAlgorithm + "_proto_pt",  m_pantau_proto_pt)  );
    CHECK( addVariable ("pantau_" + m_Name_InputAlgorithm + "_proto_eta", m_pantau_proto_eta)  );
    CHECK( addVariable ("pantau_" + m_Name_InputAlgorithm + "_proto_phi", m_pantau_proto_phi)  );
    CHECK( addVariable ("pantau_" + m_Name_InputAlgorithm + "_proto_m",   m_pantau_proto_m)  );
    
    CHECK( addVariable ("pantau_" + m_Name_InputAlgorithm + "_final_pt",  m_pantau_final_pt)  );
    CHECK( addVariable ("pantau_" + m_Name_InputAlgorithm + "_final_eta", m_pantau_final_eta)  );
    CHECK( addVariable ("pantau_" + m_Name_InputAlgorithm + "_final_phi", m_pantau_final_phi)  );
    CHECK( addVariable ("pantau_" + m_Name_InputAlgorithm + "_final_m",   m_pantau_final_m)  );
    
    CHECK( addVariable( "pantau_" + m_Name_InputAlgorithm + "_ChargedEFOs_pt",  m_ChargedEFOs_pt) );
    CHECK( addVariable( "pantau_" + m_Name_InputAlgorithm + "_ChargedEFOs_eta", m_ChargedEFOs_eta) );
    CHECK( addVariable( "pantau_" + m_Name_InputAlgorithm + "_ChargedEFOs_phi", m_ChargedEFOs_phi) );
    CHECK( addVariable( "pantau_" + m_Name_InputAlgorithm + "_ChargedEFOs_m",   m_ChargedEFOs_m) );
    
    CHECK( addVariable( "pantau_" + m_Name_InputAlgorithm + "_NeutralEFOs_pt",  m_NeutralEFOs_pt) );
    CHECK( addVariable( "pantau_" + m_Name_InputAlgorithm + "_NeutralEFOs_eta", m_NeutralEFOs_eta) );
    CHECK( addVariable( "pantau_" + m_Name_InputAlgorithm + "_NeutralEFOs_phi", m_NeutralEFOs_phi) );
    CHECK( addVariable( "pantau_" + m_Name_InputAlgorithm + "_NeutralEFOs_m",   m_NeutralEFOs_m) );
    
    CHECK( addVariable( "pantau_" + m_Name_InputAlgorithm + "_Pi0NeutEFOs_pt",  m_Pi0NeutEFOs_pt) );
    CHECK( addVariable( "pantau_" + m_Name_InputAlgorithm + "_Pi0NeutEFOs_eta", m_Pi0NeutEFOs_eta) );
    CHECK( addVariable( "pantau_" + m_Name_InputAlgorithm + "_Pi0NeutEFOs_phi", m_Pi0NeutEFOs_phi) );
    CHECK( addVariable( "pantau_" + m_Name_InputAlgorithm + "_Pi0NeutEFOs_m",   m_Pi0NeutEFOs_m) );
    
    ATH_MSG_DEBUG("Booking inputAlg specific ID vars now for input alg " << m_Name_InputAlgorithm << ".");
    if(m_Name_InputAlgorithm == "eflowRec") {
        ATH_MSG_DEBUG("eflowRec");
        CHECK( addVariable( "pantau_eflowRec_IDVars_Basic_NPi0NeutConsts",                    m_eflowRec_Basic_NPi0NeutConsts) );
        CHECK( addVariable( "pantau_eflowRec_IDVars_Basic_NNeutralConsts",                    m_eflowRec_Basic_NNeutralConsts) );
        CHECK( addVariable( "pantau_eflowRec_IDVars_Charged_HLV_SumPt",                       m_eflowRec_Charged_HLV_SumPt) );
        CHECK( addVariable( "pantau_eflowRec_IDVars_Charged_Ratio_EtOverEtAllConsts",         m_eflowRec_Charged_Ratio_EtOverEtAllConsts) );
        CHECK( addVariable( "pantau_eflowRec_IDVars_Neutral_HLV_SumM",                        m_eflowRec_Neutral_HLV_SumM) );
        CHECK( addVariable( "pantau_eflowRec_IDVars_Neutral_PID_BDTValues_EtSort_1",          m_eflowRec_Neutral_PID_BDTValues_EtSort_1) );
        CHECK( addVariable( "pantau_eflowRec_IDVars_Neutral_PID_BDTValues_BDTSort_2",         m_eflowRec_Neutral_PID_BDTValues_BDTSort_2) );
        CHECK( addVariable( "pantau_eflowRec_IDVars_Neutral_Ratio_EtOverEtAllConsts",         m_eflowRec_Neutral_Ratio_EtOverEtAllConsts) );
        CHECK( addVariable( "pantau_eflowRec_IDVars_Neutral_Mean_DRToLeading_WrtEtAllConsts", m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts) );
        CHECK( addVariable( "pantau_eflowRec_IDVars_Combined_DeltaR1stNeutralTo1stCharged",   m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged) );
    }
    
    if(m_Name_InputAlgorithm == "CellBased") {
        ATH_MSG_DEBUG("CellBased");
        CHECK( addVariable( "pantau_CellBased_IDVars_Basic_NNeutralConsts",                    m_CellBased_Basic_NNeutralConsts) );
        CHECK( addVariable( "pantau_CellBased_IDVars_Charged_JetMoment_EtDRxTotalEt",          m_CellBased_Charged_JetMoment_EtDRxTotalEt) );
        CHECK( addVariable( "pantau_CellBased_IDVars_Charged_StdDev_Et_WrtEtAllConsts",        m_CellBased_Charged_StdDev_Et_WrtEtAllConsts) );
        CHECK( addVariable( "pantau_CellBased_IDVars_Neutral_HLV_SumM",                        m_CellBased_Neutral_HLV_SumM) );
        CHECK( addVariable( "pantau_CellBased_IDVars_Neutral_PID_BDTValues_BDTSort_1",         m_CellBased_Neutral_PID_BDTValues_BDTSort_1) );
        CHECK( addVariable( "pantau_CellBased_IDVars_Neutral_PID_BDTValues_BDTSort_2",         m_CellBased_Neutral_PID_BDTValues_BDTSort_2) );
        CHECK( addVariable( "pantau_CellBased_IDVars_Neutral_Ratio_1stBDTEtOverEtAllConsts",   m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts) );
        CHECK( addVariable( "pantau_CellBased_IDVars_Neutral_Ratio_EtOverEtAllConsts",         m_CellBased_Neutral_Ratio_EtOverEtAllConsts) );
        CHECK( addVariable( "pantau_CellBased_IDVars_Neutral_Shots_NPhotonsInSeed",            m_CellBased_Neutral_Shots_NPhotonsInSeed) );
        CHECK( addVariable( "pantau_CellBased_IDVars_Combined_DeltaR1stNeutralTo1stCharged",   m_CellBased_Combined_DeltaR1stNeutralTo1stCharged) );
    }
    
    return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode TauPanTauDetailsFillerTool::fill (const Analysis::TauJet& p)
{
    /// get TauPanTauDetails from TauJet
    const PanTau::PanTauDetails*            p_taudetails(0);
    const PanTau::PanTauDetails_CellBased*  p_tauDetails_CellBased(0);
    const PanTau::PanTauDetails_eflowRec*   p_tauDetails_eflowRec(0);

    ATH_MSG_DEBUG("Trying to get details for a inputAlg " << m_Name_InputAlgorithm);
    bool foundIt = false;
    if(m_Name_InputAlgorithm == "eflowRec") {
        p_tauDetails_eflowRec   = p.details<const PanTau::PanTauDetails_eflowRec>();
        p_taudetails            = p_tauDetails_eflowRec;
        foundIt = true;
    }
    
    if(m_Name_InputAlgorithm == "CellBased") {
        p_tauDetails_CellBased  = p.details<const PanTau::PanTauDetails_CellBased>();
        p_taudetails            = p_tauDetails_CellBased;
        foundIt = true;
    }
    
    if(foundIt == false) {
        ATH_MSG_WARNING("Could not find details for eflowRec / CellBased");
        return StatusCode::SUCCESS;
    }
    
    ATH_MSG_DEBUG("Found details for inputAlg " << m_Name_InputAlgorithm);
    
    if (!p_taudetails){
        ATH_MSG_DEBUG( "TauJet has no associated PanTauDetails --- This must not be bad - if PanTau rejected a tauRec seed, that tauRec seed will not have PanTauDetails");
        return StatusCode::SUCCESS;
    }
    

    /// add variables from details
    *m_isPanTauCandidate = p_taudetails->isPanTauCandidate();
    
    *m_RecoMode_SubAlg  = p_taudetails->RecoModeSubAlg();
    *m_RecoMode_PanTau  = p_taudetails->RecoModePanTau();
    
    *m_pantau_proto_pt  = p_taudetails->pantau_proto_pt();
    *m_pantau_proto_eta = p_taudetails->pantau_proto_eta();
    *m_pantau_proto_phi = p_taudetails->pantau_proto_phi();
    *m_pantau_proto_m   = p_taudetails->pantau_proto_m();
    
    *m_pantau_final_pt  = p_taudetails->pantau_final_pt();
    *m_pantau_final_eta = p_taudetails->pantau_final_eta();
    *m_pantau_final_phi = p_taudetails->pantau_final_phi();
    *m_pantau_final_m   = p_taudetails->pantau_final_m();
    
    *m_ChargedEFOs_pt  = p_taudetails->ChargedEFOs_pt();
    *m_ChargedEFOs_eta = p_taudetails->ChargedEFOs_eta();
    *m_ChargedEFOs_phi = p_taudetails->ChargedEFOs_phi();
    *m_ChargedEFOs_m   = p_taudetails->ChargedEFOs_m();
    
    *m_NeutralEFOs_pt  = p_taudetails->NeutralEFOs_pt();
    *m_NeutralEFOs_eta = p_taudetails->NeutralEFOs_eta();
    *m_NeutralEFOs_phi = p_taudetails->NeutralEFOs_phi();
    *m_NeutralEFOs_m   = p_taudetails->NeutralEFOs_m();
    
    *m_Pi0NeutEFOs_pt  = p_taudetails->Pi0NeutEFOs_pt();
    *m_Pi0NeutEFOs_eta = p_taudetails->Pi0NeutEFOs_eta();
    *m_Pi0NeutEFOs_phi = p_taudetails->Pi0NeutEFOs_phi();
    *m_Pi0NeutEFOs_m   = p_taudetails->Pi0NeutEFOs_m();
    
    if(m_Name_InputAlgorithm == "eflowRec") {
        *m_eflowRec_Basic_NPi0NeutConsts                    = p_tauDetails_eflowRec->eflowRec_Basic_NPi0NeutConsts();
        *m_eflowRec_Basic_NNeutralConsts                    = p_tauDetails_eflowRec->eflowRec_Basic_NNeutralConsts();
        *m_eflowRec_Charged_HLV_SumPt                       = p_tauDetails_eflowRec->eflowRec_Charged_HLV_SumPt();
        *m_eflowRec_Charged_Ratio_EtOverEtAllConsts         = p_tauDetails_eflowRec->eflowRec_Charged_Ratio_EtOverEtAllConsts();
        *m_eflowRec_Neutral_HLV_SumM                        = p_tauDetails_eflowRec->eflowRec_Neutral_HLV_SumM();
        *m_eflowRec_Neutral_PID_BDTValues_EtSort_1          = p_tauDetails_eflowRec->eflowRec_Neutral_PID_BDTValues_EtSort_1();
        *m_eflowRec_Neutral_PID_BDTValues_BDTSort_2         = p_tauDetails_eflowRec->eflowRec_Neutral_PID_BDTValues_BDTSort_2();
        *m_eflowRec_Neutral_Ratio_EtOverEtAllConsts         = p_tauDetails_eflowRec->eflowRec_Neutral_Ratio_EtOverEtAllConsts();
        *m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts = p_tauDetails_eflowRec->eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts();
        *m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged   = p_tauDetails_eflowRec->eflowRec_Combined_DeltaR1stNeutralTo1stCharged();
    }
    
    if(m_Name_InputAlgorithm == "CellBased") {
        *m_CellBased_Basic_NNeutralConsts                   = p_tauDetails_CellBased->CellBased_Basic_NNeutralConsts();
        *m_CellBased_Charged_JetMoment_EtDRxTotalEt         = p_tauDetails_CellBased->CellBased_Charged_JetMoment_EtDRxTotalEt();
        *m_CellBased_Charged_StdDev_Et_WrtEtAllConsts       = p_tauDetails_CellBased->CellBased_Charged_StdDev_Et_WrtEtAllConsts();
        *m_CellBased_Neutral_HLV_SumM                       = p_tauDetails_CellBased->CellBased_Neutral_HLV_SumM();
        *m_CellBased_Neutral_PID_BDTValues_BDTSort_1        = p_tauDetails_CellBased->CellBased_Neutral_PID_BDTValues_BDTSort_1();
        *m_CellBased_Neutral_PID_BDTValues_BDTSort_2        = p_tauDetails_CellBased->CellBased_Neutral_PID_BDTValues_BDTSort_2();
        *m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts  = p_tauDetails_CellBased->CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts();
        *m_CellBased_Neutral_Ratio_EtOverEtAllConsts        = p_tauDetails_CellBased->CellBased_Neutral_Ratio_EtOverEtAllConsts();
        *m_CellBased_Neutral_Shots_NPhotonsInSeed           = p_tauDetails_CellBased->CellBased_Neutral_Shots_NPhotonsInSeed();
        *m_CellBased_Combined_DeltaR1stNeutralTo1stCharged  = p_tauDetails_CellBased->CellBased_Combined_DeltaR1stNeutralTo1stCharged();
    }
    return StatusCode::SUCCESS;
}

StatusCode TauPanTauDetailsFillerTool::finalize() {
    return StatusCode::SUCCESS;
}

} // namespace D3PD
