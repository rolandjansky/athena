/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/PAUConversionFillerTool.h
 * @author Mike Hance <hance@hep.upenn.edu>
 * @date January, 2011
 * @brief Fill PAU related variables
 */


#include "PAUConversionFillerTool.h"
#include "AthenaKernel/errorcheck.h"

#include "PhotonAnalysisUtils/PAUegamma.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
PAUConversionFillerTool::PAUConversionFillerTool (const std::string& type,
						  const std::string& name,
						  const IInterface* parent)
  : BlockFillerTool<PAU::egamma> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode PAUConversionFillerTool::book()
{
  CHECK( addVariable ("convTrk1_chi2"               , m_convTrk1_chi2 	              ) );
  CHECK( addVariable ("convTrk2_chi2"               , m_convTrk2_chi2      	      ) );
  CHECK( addVariable ("convTrk1_expectHitInBLayer"  , m_convTrk1_expectHitInBLayer    ) );
  CHECK( addVariable ("convTrk1_numTRTOutliers"     , m_convTrk1_numTRTOutliers       ) );
  CHECK( addVariable ("convTrk1_numTRTHTHits"       , m_convTrk1_numTRTHTHits         ) );
  CHECK( addVariable ("convTrk1_numTRTHTOutliers"   , m_convTrk1_numTRTHTOutliers     ) );
  CHECK( addVariable ("convTrk2_expectHitInBLayer"  , m_convTrk2_expectHitInBLayer    ) );
  CHECK( addVariable ("convTrk2_numTRTOutliers"     , m_convTrk2_numTRTOutliers       ) );
  CHECK( addVariable ("convTrk2_numTRTHTHits"       , m_convTrk2_numTRTHTHits         ) );
  CHECK( addVariable ("convTrk2_numTRTHTOutliers"   , m_convTrk2_numTRTHTOutliers     ) );
  CHECK( addVariable ("convTrk1_trtPID"             , m_convTrk1_trtPID 	      ) );
  CHECK( addVariable ("convTrk2_trtPID"             , m_convTrk2_trtPID 	      ) );
  CHECK( addVariable ("convTrk1_trtPIDHT"           , m_convTrk1_trtPIDHT 	      ) );
  CHECK( addVariable ("convTrk2_trtPIDHT"           , m_convTrk2_trtPIDHT 	      ) );
  CHECK( addVariable ("convTrk1_trtPIDToT"          , m_convTrk1_trtPIDToT 	      ) );
  CHECK( addVariable ("convTrk2_trtPIDToT"          , m_convTrk2_trtPIDToT 	      ) );
  CHECK( addVariable ("convTrk1_ToTvar"             , m_convTrk1_ToTvar 	      ) );
  CHECK( addVariable ("convTrk2_ToTvar"             , m_convTrk2_ToTvar 	      ) );
  CHECK( addVariable ("convTrk1_trtRatioHT"         , m_convTrk1_trtRatioHT 	      ) );
  CHECK( addVariable ("convTrk2_trtRatioHT"         , m_convTrk2_trtRatioHT 	      ) );
  CHECK( addVariable ("convTrk1_charge"             , m_convTrk1_charge 	      ) );
  CHECK( addVariable ("convTrk2_charge"             , m_convTrk2_charge 	      ) );
  CHECK( addVariable ("convTrk1_DeltaEta_track_calo", m_convTrk1_DeltaEta_track_calo  ) );
  CHECK( addVariable ("convTrk2_DeltaEta_track_calo", m_convTrk2_DeltaEta_track_calo  ) );
  CHECK( addVariable ("convTrk1_DeltaPhi_track_calo", m_convTrk1_DeltaPhi_track_calo  ) );
  CHECK( addVariable ("convTrk2_DeltaPhi_track_calo", m_convTrk2_DeltaPhi_track_calo  ) );

  CHECK( addVariable ("XConv"                       , m_XConv                         ) );
  CHECK( addVariable ("YConv"                       , m_YConv                         ) );
  CHECK( addVariable ("conv_chi2"                   , m_conv_chi2 		      ) );
  CHECK( addVariable ("conv_dcottheta"              , m_conv_dcottheta 	              ) );
  CHECK( addVariable ("conv_dphi"                   , m_conv_dphi 		      ) );
  CHECK( addVariable ("conv_dist"                   , m_conv_dist 		      ) );
  CHECK( addVariable ("conv_DR1R2"                  , m_conv_DR1R2 		      ) );
  CHECK( addVariable ("conv_p"                      , m_conv_p        		      ) );
  CHECK( addVariable ("conv_phi"                    , m_conv_phi       		      ) );
  CHECK( addVariable ("conv_theta"                  , m_conv_theta     		      ) );
  CHECK( addVariable ("conv_perr"                   , m_conv_perr     		      ) );
  CHECK( addVariable ("conv_phierr"                 , m_conv_phierr    		      ) );
  CHECK( addVariable ("conv_thetaerr"               , m_conv_thetaerr  		      ) );
  CHECK( addVariable ("conv_GSF_p"                  , m_conv_GSF_p    		      ) );
  CHECK( addVariable ("conv_GSF_phi"                , m_conv_GSF_phi   		      ) );
  CHECK( addVariable ("conv_GSF_theta"              , m_conv_GSF_theta 		      ) );
  CHECK( addVariable ("conv_GSF_perr"               , m_conv_GSF_perr 		      ) );
  CHECK( addVariable ("conv_GSF_phierr"             , m_conv_GSF_phierr		      ) );
  CHECK( addVariable ("conv_GSF_thetaerr"           , m_conv_GSF_thetaerr             ) );
  CHECK( addVariable ("conv_DNA_p"                  , m_conv_DNA_p    		      ) );
  CHECK( addVariable ("conv_DNA_phi"                , m_conv_DNA_phi   		      ) );
  CHECK( addVariable ("conv_DNA_theta"              , m_conv_DNA_theta 		      ) );
  CHECK( addVariable ("conv_DNA_perr"               , m_conv_DNA_perr 		      ) );
  CHECK( addVariable ("conv_DNA_phierr"             , m_conv_DNA_phierr		      ) );
  CHECK( addVariable ("conv_DNA_thetaerr"           , m_conv_DNA_thetaerr             ) );

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
StatusCode PAUConversionFillerTool::fill (const PAU::egamma& p)
{

  * m_convTrk1_chi2 	           = p.convTrk1_chi2 	            ;
  * m_convTrk2_chi2      	   = p.convTrk2_chi2      	    ;
  * m_convTrk1_expectHitInBLayer   = p.convTrk1_expectHitInBLayer   ;
  * m_convTrk1_numTRTOutliers      = p.convTrk1_numTRTOutliers      ;
  * m_convTrk1_numTRTHTHits        = p.convTrk1_numTRTHTHits        ;
  * m_convTrk1_numTRTHTOutliers    = p.convTrk1_numTRTHTOutliers    ;
  * m_convTrk2_expectHitInBLayer   = p.convTrk2_expectHitInBLayer   ;
  * m_convTrk2_numTRTOutliers      = p.convTrk2_numTRTOutliers      ;
  * m_convTrk2_numTRTHTHits        = p.convTrk2_numTRTHTHits        ;
  * m_convTrk2_numTRTHTOutliers    = p.convTrk2_numTRTHTOutliers    ;
  * m_convTrk1_trtPID 	           = p.convTrk1_trtPID 	            ;
  * m_convTrk2_trtPID 	           = p.convTrk2_trtPID 	            ;
  * m_convTrk1_trtPIDHT 	   = p.convTrk1_trtPIDHT 	    ;
  * m_convTrk2_trtPIDHT 	   = p.convTrk2_trtPIDHT 	    ;
  * m_convTrk1_trtPIDToT 	   = p.convTrk1_trtPIDToT 	    ;
  * m_convTrk2_trtPIDToT 	   = p.convTrk2_trtPIDToT 	    ;
  * m_convTrk1_ToTvar 	           = p.convTrk1_ToTvar 	            ;
  * m_convTrk2_ToTvar 	           = p.convTrk2_ToTvar 	            ;
  * m_convTrk1_trtRatioHT 	   = p.convTrk1_trtRatioHT 	    ;
  * m_convTrk2_trtRatioHT 	   = p.convTrk2_trtRatioHT 	    ;
  * m_convTrk1_charge 	           = p.convTrk1_charge 	            ;
  * m_convTrk2_charge 	           = p.convTrk2_charge 	            ;
  * m_convTrk1_DeltaEta_track_calo = p.convTrk1_DeltaEta_track_calo ;
  * m_convTrk2_DeltaEta_track_calo = p.convTrk2_DeltaEta_track_calo ;
  * m_convTrk1_DeltaPhi_track_calo = p.convTrk1_DeltaPhi_track_calo ;
  * m_convTrk2_DeltaPhi_track_calo = p.convTrk2_DeltaPhi_track_calo ;

  * m_XConv                        = p.Xconv                        ;
  * m_YConv                        = p.Yconv                        ;
  * m_conv_chi2 		   = p.convChi2 		    ;
  * m_conv_dcottheta 	           = p.convDcottheta 	            ;
  * m_conv_dphi 		   = p.convDphi 		    ;
  * m_conv_dist 		   = p.convDist 		    ;
  * m_conv_DR1R2 		   = p.convDR1R2 		    ;
  * m_conv_p    		   = p.convp    		    ;
  * m_conv_phi    		   = p.convphi    		    ;
  * m_conv_theta    		   = p.convtheta    		    ;
  * m_conv_perr    		   = p.convperr    		    ;
  * m_conv_phierr    		   = p.convphierr    		    ;
  * m_conv_thetaerr    		   = p.convthetaerr    		    ;
  * m_conv_GSF_p    		   = p.convp_GSF    		    ;
  * m_conv_GSF_phi    		   = p.convphi_GSF    		    ;
  * m_conv_GSF_theta   		   = p.convtheta_GSF   		    ;
  * m_conv_GSF_perr   		   = p.convperr_GSF   		    ;
  * m_conv_GSF_phierr  		   = p.convphierr_GSF  		    ;
  * m_conv_GSF_thetaerr		   = p.convthetaerr_GSF		    ;
  * m_conv_DNA_p    		   = p.convp_DNA    		    ;
  * m_conv_DNA_phi    		   = p.convphi_DNA    		    ;
  * m_conv_DNA_theta   		   = p.convtheta_DNA   		    ;
  * m_conv_DNA_perr   		   = p.convperr_DNA   		    ;
  * m_conv_DNA_phierr  		   = p.convphierr_DNA  		    ;
  * m_conv_DNA_thetaerr		   = p.convthetaerr_DNA		    ;


  return StatusCode::SUCCESS;
}


} // namespace D3PD
