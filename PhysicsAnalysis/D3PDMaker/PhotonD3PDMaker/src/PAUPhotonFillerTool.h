// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/PAUPhotonFillerTool.h
 * @author Mike Hance <hance@hep.upenn.edu>
 * @date March, 2010
 * @brief Fill PAU related variables
 */


#ifndef PHOTOND3PDMAKER_PAUPHOTONFILLERTOOL_H
#define PHOTOND3PDMAKER_PAUPHOTONFILLERTOOL_H


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
class PAUPhotonFillerTool
  : public BlockFillerTool<PAU::egamma>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  PAUPhotonFillerTool (const std::string& type,
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

  bool m_dumpMC;

  float * m_ambiguityResult;

  float * m_parton_pt_max_MC;
  float * m_parton_eta_MC;
  float * m_parton_phi_MC;
  float * m_parton_barcode_MC;
  float * m_parton_pdg_MC;
  float * m_etaS2_MC;
  float * m_phiS2_MC;
  float * m_XConv_MC;
  float * m_YConv_MC;
  int   * m_isG4part;
  float * m_partonIsolation15_UE_MC;
  float * m_partonIsolation20_UE_MC;
  float * m_partonIsolation25_UE_MC;
  float * m_partonIsolation30_UE_MC;
  float * m_partonIsolation35_UE_MC;
  float * m_partonIsolation40_UE_MC;
  float * m_partonIsolation45_UE_MC;
  float * m_partonIsolation60_UE_MC;
  float * m_partonIsolation15_MC;
  float * m_partonIsolation20_MC;
  float * m_partonIsolation25_MC;
  float * m_partonIsolation30_MC;
  float * m_partonIsolation35_MC;
  float * m_partonIsolation40_MC;
  float * m_partonIsolation45_MC;
  float * m_partonIsolation60_MC;
  float * m_particleIsolation40_MC;
  float * m_particleIsolation40_ED_corrected_MC;
  float * m_particleIsolation40_ED_corrected_reco_MC;
};


} // namespace D3PD


#endif // not PHOTOND3PDMAKER_PAUPHOTONFILLERTOOL_H
