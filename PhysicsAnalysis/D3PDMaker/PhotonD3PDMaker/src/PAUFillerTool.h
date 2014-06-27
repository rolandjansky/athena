// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/PAUFillerTool.h
 * @author Mike Hance <hance@hep.upenn.edu>
 * @date March, 2010
 * @brief Fill PAU related variables
 */


#ifndef EGAMMAD3PDMAKER_PAUFILLERTOOL_H
#define EGAMMAD3PDMAKER_PAUFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include <vector>
#include <string>

namespace PAU{
  class egamma;
}


namespace D3PD {


/**
 * @brief Fill author code and related flags.
 */
class PAUFillerTool
  : public BlockFillerTool<PAU::egamma>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  PAUFillerTool (const std::string& type,
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
  virtual StatusCode fill (const PAU::egamma& p);


private:
  float* m_convIP;
  float* m_convIPRev;
  
  float* m_PtIsolationCone;
  float* m_PtIsolationConePhAngle;

  float* m_Etcone40_pt_corrected;
  float* m_Etcone40_ED_corrected;
  float* m_Etcone40_corrected   ;

  float* m_Etcone35_pt_corrected;
  float* m_Etcone30_pt_corrected;
  float* m_Etcone25_pt_corrected;
  float* m_Etcone20_pt_corrected;
  float* m_Etcone15_pt_corrected;

  bool m_fillPhotons;

  /// 0: Don't fill any pt_corrected variables;
  /// 1: Fill only Etcone40_pt_corrected;
  /// 2: Fill all pt_corrected variables.
  int m_fillPtCorrectedIsolation;
};


} // namespace D3PD


#endif // not EGAMMAD3PDMAKER_PAUFILLERTOOL_H
