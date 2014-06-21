// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthTauFullDPDFillerTool.h 528845 2012-12-04 15:07:38Z tibristo $
/**
 * @file TauD3PDMaker/src/TruthTauFullDPDFillerTool.h
 * @author Sebastian Fleischmann / modified: Christian Limbach
 * @date Jul, 2010 / May 2012
 * @brief Block filler tool for TruthTau properties.
 */

#ifndef TAUD3PDMAKER_TRUTHTAUFULLDPDFILLERTOOL_H
#define TAUD3PDMAKER_TRUTHTAUFULLDPDFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "PanTauUtils/TauClassificationUtility.h"

#include <vector>

namespace TauID { class TruthTau; }
class TruthParticle;

namespace D3PD {


/**
 * @brief Block filler tool for PanTau details.
 *
 */
class TruthTauFullDPDFillerTool
  : public BlockFillerTool<TruthParticle>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
    TruthTauFullDPDFillerTool (const std::string& type,
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
  virtual StatusCode fill (const TruthParticle& p);

  virtual StatusCode finalize();

private:

    PanTau::TauClassificationUtility    m_tauClassUtil;

    unsigned int*               m_TauTruthType;
    int*                        m_resonancePdgId;
    int*                        m_nPi0;
    unsigned int*               m_nPhotonConversions;
    unsigned int*               m_nChargedProducts;
    unsigned int*               m_nNeutralProducts;
    
    double*                     m_Pi0_1st_E;
    double*                     m_Pi0_1st_Eta;
    double*                     m_Pi0_1st_Phi;
    double*                     m_Pi0_1st_Pt;
    
    double*                     m_Pi0_2nd_E;
    double*                     m_Pi0_2nd_Eta;
    double*                     m_Pi0_2nd_Phi;
    double*                     m_Pi0_2nd_Pt;
    
    double*                     m_Pi0_3rd_E;
    double*                     m_Pi0_3rd_Eta;
    double*                     m_Pi0_3rd_Phi;
    double*                     m_Pi0_3rd_Pt;  

    double*                     m_ChargedEt;
    double*                     m_NeutralEt;
    double*                     m_VisEt;
};


} // namespace D3PD


#endif // TAUD3PDMAKER_TRUTHTAUFULLDPDFILLERTOOL_H
