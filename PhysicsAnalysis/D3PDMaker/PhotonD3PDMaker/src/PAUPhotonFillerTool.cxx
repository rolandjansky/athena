/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/PAUPhotonFillerTool.h
 * @author Mike Hance <hance@hep.upenn.edu>
 * @date January, 2011
 * @brief Fill PAU related variables
 */


#include "PAUPhotonFillerTool.h"
#include "AthenaKernel/errorcheck.h"

#include "PhotonAnalysisUtils/PAUegamma.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
PAUPhotonFillerTool::PAUPhotonFillerTool (const std::string& type,
			      const std::string& name,
			      const IInterface* parent)
  : BlockFillerTool<PAU::egamma> (type, name, parent)
{
  m_dumpMC = true;
  book().ignore(); // Avoid coverity warnings.

  declareProperty("DumpMC", m_dumpMC = false,
		  "Dump information about truth photon");
}


/**
 * @brief Book variables for this block.
 */
StatusCode PAUPhotonFillerTool::book()
{

  CHECK( addVariable("ambiguityResult"				     , m_ambiguityResult			    ) );

  if(m_dumpMC){
    CHECK( addVariable("parton_pt_max_MC"				     , m_parton_pt_max_MC			    ) );
    CHECK( addVariable("parton_eta_MC"				     , m_parton_eta_MC			            ) );
    CHECK( addVariable("parton_phi_MC"				     , m_parton_phi_MC			            ) );
    CHECK( addVariable("parton_barcode_MC"			     , m_parton_barcode_MC			    ) );
    CHECK( addVariable("parton_pdg_MC"				     , m_parton_pdg_MC			            ) );
    CHECK( addVariable("etaS2_MC"					     , m_etaS2_MC				    ) );
    CHECK( addVariable("phiS2_MC"					     , m_phiS2_MC				    ) );
    CHECK( addVariable("XConv_MC"					     , m_XConv_MC				    ) );
    CHECK( addVariable("YConv_MC"					     , m_YConv_MC				    ) );
    CHECK( addVariable("isG4part"					     , m_isG4part				    ) );
    CHECK( addVariable("partonIsolation15_UE_MC"			     , m_partonIsolation15_UE_MC		    ) );
    CHECK( addVariable("partonIsolation20_UE_MC"			     , m_partonIsolation20_UE_MC		    ) );
    CHECK( addVariable("partonIsolation25_UE_MC"			     , m_partonIsolation25_UE_MC		    ) );
    CHECK( addVariable("partonIsolation30_UE_MC"			     , m_partonIsolation30_UE_MC		    ) );
    CHECK( addVariable("partonIsolation35_UE_MC"			     , m_partonIsolation35_UE_MC		    ) );
    CHECK( addVariable("partonIsolation40_UE_MC"			     , m_partonIsolation40_UE_MC		    ) );
    CHECK( addVariable("partonIsolation45_UE_MC"			     , m_partonIsolation45_UE_MC		    ) );
    CHECK( addVariable("partonIsolation60_UE_MC"			     , m_partonIsolation60_UE_MC		    ) );
    CHECK( addVariable("partonIsolation15_MC"			     , m_partonIsolation15_MC		            ) );
    CHECK( addVariable("partonIsolation20_MC"			     , m_partonIsolation20_MC		            ) );
    CHECK( addVariable("partonIsolation25_MC"			     , m_partonIsolation25_MC		            ) );
    CHECK( addVariable("partonIsolation30_MC"			     , m_partonIsolation30_MC		            ) );
    CHECK( addVariable("partonIsolation35_MC"			     , m_partonIsolation35_MC		            ) );
    CHECK( addVariable("partonIsolation40_MC"			     , m_partonIsolation40_MC		            ) );
    CHECK( addVariable("partonIsolation45_MC"			     , m_partonIsolation45_MC		            ) );
    CHECK( addVariable("partonIsolation60_MC"			     , m_partonIsolation60_MC		            ) );
    CHECK( addVariable("particleIsolation40_MC"			     , m_particleIsolation40_MC		            ) );
    CHECK( addVariable("particleIsolation40_ED_corrected_MC"           , m_particleIsolation40_ED_corrected_MC	    ) );
    CHECK( addVariable("particleIsolation40_ED_corrected_reco_MC"	     , m_particleIsolation40_ED_corrected_reco_MC   ) );
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
StatusCode PAUPhotonFillerTool::fill (const PAU::egamma& p)
{

  * m_ambiguityResult			        = p.ambiguityResult			       ;

  if(m_dumpMC){
    * m_parton_pt_max_MC		        = p.parton_pt_max_MC			       ;
    * m_parton_eta_MC			        = p.parton_eta_MC			       ;
    * m_parton_phi_MC			        = p.parton_phi_MC			       ;
    * m_parton_barcode_MC		        = p.parton_barcode_MC			       ;
    * m_parton_pdg_MC			        = p.parton_pdg_MC			       ;
    * m_etaS2_MC			        = p.etaCaloMC				       ;
    * m_phiS2_MC			        = p.phiCaloMC				       ;
    * m_XConv_MC			        = p.XconvMC				       ;
    * m_YConv_MC			        = p.YconvMC				       ;
    * m_isG4part			        = p.isG4part				       ;
    * m_partonIsolation15_UE_MC		        = p.partonIsolation15_UE		       ;
    * m_partonIsolation20_UE_MC		        = p.partonIsolation20_UE		       ;
    * m_partonIsolation25_UE_MC		        = p.partonIsolation25_UE		       ;
    * m_partonIsolation30_UE_MC		        = p.partonIsolation30_UE		       ;
    * m_partonIsolation35_UE_MC		        = p.partonIsolation35_UE		       ;
    * m_partonIsolation40_UE_MC		        = p.partonIsolation40_UE		       ;
    * m_partonIsolation45_UE_MC		        = p.partonIsolation45_UE		       ;
    * m_partonIsolation60_UE_MC		        = p.partonIsolation60_UE	               ;
    * m_partonIsolation15_MC		        = p.partonIsolation15		               ;
    * m_partonIsolation20_MC		        = p.partonIsolation20		               ;
    * m_partonIsolation25_MC		        = p.partonIsolation25		               ;
    * m_partonIsolation30_MC		        = p.partonIsolation30		               ;
    * m_partonIsolation35_MC		        = p.partonIsolation35		               ;
    * m_partonIsolation40_MC		        = p.partonIsolation40		               ;
    * m_partonIsolation45_MC		        = p.partonIsolation45		               ;
    * m_partonIsolation60_MC		        = p.partonIsolation60		               ;
    * m_particleIsolation40_MC		        = p.particleIsolation40		               ;
    * m_particleIsolation40_ED_corrected_MC     = p.particleIsolation40_ED_corrected	       ;
    * m_particleIsolation40_ED_corrected_reco_MC= p.particleIsolation40_ED_corrected_reco      ;
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
