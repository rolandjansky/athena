// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauPanTauDetailsFillerTool.h 570071 2013-11-12 21:22:49Z limbach $
/**
 * @file D3PDMaker/TauD3PDMaker/src/TauPanTauDetailsFillerTool.h
 * @author Christian Limbach  (limbach -at- physik.uni-bonn.de)
 * @date Jan, 2012
 * @brief Block filler tool for TauPanTauDetails variables stored in TauPanTauDetails container linked to from the Analysis::TauJet Object.
 */

#ifndef TAUD3PDMAKER_TAUPANTAUDETAILSFILLERTOOL_H
#define TAUD3PDMAKER_TAUPANTAUDETAILSFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"

#include <string>

namespace Analysis { class TauJet; }

namespace D3PD {


/**
 * @brief Block filler tool for PanTau details stored in TauJet.
 *
 */
class TauPanTauDetailsFillerTool
  : public BlockFillerTool<Analysis::TauJet>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TauPanTauDetailsFillerTool (const std::string& type,
                     const std::string& name,
                     const IInterface* parent);


  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const Analysis::TauJet& p);

  virtual StatusCode finalize();

private:
    
    std::string m_Name_InputAlgorithm;
    std::string m_otherProp;
    
    //validity
    int     *m_isPanTauCandidate;
    
    //decay mode
    int     *m_RecoMode_SubAlg;
    int     *m_RecoMode_PanTau;
    
    //four momentum plain
    double  *m_pantau_proto_pt;
    double  *m_pantau_proto_eta;
    double  *m_pantau_proto_phi;
    double  *m_pantau_proto_m;
    
    //four momentum optimised
    double  *m_pantau_final_pt;
    double  *m_pantau_final_eta;
    double  *m_pantau_final_phi;
    double  *m_pantau_final_m;
    
    //4-vectors of tau constituents
    std::vector<double> *m_ChargedEFOs_pt;
    std::vector<double> *m_ChargedEFOs_eta;
    std::vector<double> *m_ChargedEFOs_phi;
    std::vector<double> *m_ChargedEFOs_m;
    
    std::vector<double> *m_NeutralEFOs_pt;
    std::vector<double> *m_NeutralEFOs_eta;
    std::vector<double> *m_NeutralEFOs_phi;
    std::vector<double> *m_NeutralEFOs_m;
    
    std::vector<double> *m_Pi0NeutEFOs_pt;
    std::vector<double> *m_Pi0NeutEFOs_eta;
    std::vector<double> *m_Pi0NeutEFOs_phi;
    std::vector<double> *m_Pi0NeutEFOs_m;
    
    //ID variables
    
    //eflowRec
    int     *m_eflowRec_Basic_NPi0NeutConsts;
    int     *m_eflowRec_Basic_NNeutralConsts;
    double  *m_eflowRec_Charged_HLV_SumPt;
    double  *m_eflowRec_Charged_Ratio_EtOverEtAllConsts;
    double  *m_eflowRec_Neutral_HLV_SumM;
    double  *m_eflowRec_Neutral_PID_BDTValues_EtSort_1;
    double  *m_eflowRec_Neutral_PID_BDTValues_BDTSort_2;
    double  *m_eflowRec_Neutral_Ratio_EtOverEtAllConsts;
    double  *m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts;
    double  *m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged;
    
    //CellBased
    int     *m_CellBased_Basic_NNeutralConsts;
    double  *m_CellBased_Charged_JetMoment_EtDRxTotalEt;
    double  *m_CellBased_Charged_StdDev_Et_WrtEtAllConsts;
    double  *m_CellBased_Neutral_HLV_SumM;
    double  *m_CellBased_Neutral_PID_BDTValues_BDTSort_1;
    double  *m_CellBased_Neutral_PID_BDTValues_BDTSort_2;
    double  *m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts;
    double  *m_CellBased_Neutral_Ratio_EtOverEtAllConsts;
    double  *m_CellBased_Neutral_Shots_NPhotonsInSeed;
    double  *m_CellBased_Combined_DeltaR1stNeutralTo1stCharged;
    
};


} // namespace D3PD


#endif // TAUD3PDMAKER_TAUPANTAUDETAILSFILLERTOOL_H
