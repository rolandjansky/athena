// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/PAUConversionFillerTool.h
 * @author Mike Hance <hance@hep.upenn.edu>
 * @date March, 2010
 * @brief Fill PAU related variables
 */


#ifndef PHOTOND3PDMAKER_PAUCONVERSIONFILLERTOOL_H
#define PHOTOND3PDMAKER_PAUCONVERSIONFILLERTOOL_H


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
class PAUConversionFillerTool
  : public BlockFillerTool<PAU::egamma>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  PAUConversionFillerTool (const std::string& type,
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
  float
    * m_convTrk1_chi2, 
    * m_convTrk2_chi2, 
    * m_convTrk1_expectHitInBLayer, 
    * m_convTrk1_numTRTOutliers, 
    * m_convTrk1_numTRTHTHits, 
    * m_convTrk1_numTRTHTOutliers, 
    * m_convTrk2_expectHitInBLayer, 
    * m_convTrk2_numTRTOutliers, 
    * m_convTrk2_numTRTHTHits, 
    * m_convTrk2_numTRTHTOutliers, 
    * m_convTrk1_trtPID, 
    * m_convTrk2_trtPID, 
    * m_convTrk1_trtPIDHT, 
    * m_convTrk2_trtPIDHT, 
    * m_convTrk1_trtPIDToT, 
    * m_convTrk2_trtPIDToT, 
    * m_convTrk1_ToTvar, 
    * m_convTrk2_ToTvar, 
    * m_convTrk1_trtRatioHT, 
    * m_convTrk2_trtRatioHT, 
    * m_convTrk1_charge, 
    * m_convTrk2_charge, 
    * m_convTrk1_DeltaEta_track_calo, 
    * m_convTrk2_DeltaEta_track_calo, 
    * m_convTrk1_DeltaPhi_track_calo, 
    * m_convTrk2_DeltaPhi_track_calo;

  float 
    * m_XConv,
    * m_YConv,
    * m_conv_chi2, 
    * m_conv_dcottheta, 
    * m_conv_dphi, 
    * m_conv_dist, 
    * m_conv_DR1R2, 
    * m_conv_p, 
    * m_conv_phi, 
    * m_conv_theta,
    * m_conv_perr,  
    * m_conv_phierr,  
    * m_conv_thetaerr,  
    * m_conv_GSF_p, 
    * m_conv_GSF_phi, 
    * m_conv_GSF_theta,
    * m_conv_GSF_perr,  
    * m_conv_GSF_phierr,  
    * m_conv_GSF_thetaerr,  
    * m_conv_DNA_p, 
    * m_conv_DNA_phi, 
    * m_conv_DNA_theta,
    * m_conv_DNA_perr,  
    * m_conv_DNA_phierr,  
    * m_conv_DNA_thetaerr;
};


} // namespace D3PD


#endif // not PHOTOND3PDMAKER_PAUPHOTONFILLERTOOL_H
