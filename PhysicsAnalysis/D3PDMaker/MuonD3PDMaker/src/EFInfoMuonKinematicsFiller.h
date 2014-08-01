/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/EFInfoMuonKinematicsFiller.h
 * @author srivas prasad <srivas.prasad@cern.ch>
 * @date Jan 2010
 * @brief Block filler tool for TrigMuonEFInfo, detail level 1
 */

#ifndef MUOND3PDMAKER_EFINFOMUONKINEMATICSFILLER_H
#define MUOND3PDMAKER_EFINFOMUONKINEMATICSFILLER_H 1


#include "D3PDMakerUtils/BlockFillerTool.h"
class TrigMuonEFInfo;


namespace D3PD {


/**
 * @brief Block filler tool for TrigMuonEF
 */
class EFInfoMuonKinematicsFiller
  : public BlockFillerTool<TrigMuonEFInfo>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  EFInfoMuonKinematicsFiller (const std::string& type,
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
  virtual StatusCode fill (const TrigMuonEFInfo& p);


private:
  /// Combined trigger track kinematics
  /// Variable: pt combined
  float* m_cb_pt;
  /// Variable: eta combined
  float* m_cb_eta;
  /// Variable: phi combined
  float* m_cb_phi;

  /// MS trigger track kinematics
  /// Variable: pt combined
  float* m_ms_pt;
  /// Variable: eta combined
  float* m_ms_eta;
  /// Variable: phi combined
  float* m_ms_phi;

  /// extrapolated MS trigger track kinematics
  /// Variable: pt combined
  float* m_me_pt;
  /// Variable: eta combined
  float* m_me_eta;
  /// Variable: phi combined
  float* m_me_phi;



  
};


}

#endif 
