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


#include "PAUFillerTool.h"
#include "AthenaKernel/errorcheck.h"

#include "PhotonAnalysisUtils/PAUegamma.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
PAUFillerTool::PAUFillerTool (const std::string& type,
			      const std::string& name,
			      const IInterface* parent)
  : BlockFillerTool<PAU::egamma> (type, name, parent)
{
  m_fillPhotons = true;
  m_fillPtCorrectedIsolation = 2;
  book().ignore(); // Avoid coverity warnings.

  declareProperty("FillPhotons"              , m_fillPhotons = true               , "If true, fill photon specific quantities"               );
  declareProperty("FillPtCorrectedIsolation" , m_fillPtCorrectedIsolation = 1,
                  "0: Don't fill any pt_corrected variables; "
                  "1: Fill only Etcone40_pt_corrected; "
                  "2: Fill all pt_corrected variables.");
}


/**
 * @brief Book variables for this block.
 */
StatusCode PAUFillerTool::book()
{
  if(m_fillPhotons){

    CHECK( addVariable ("convIP"                    , m_convIP                     ) );
    CHECK( addVariable ("convIPRev"                 , m_convIPRev                  ) );

    CHECK( addVariable ("ptIsolationCone"           , m_PtIsolationCone            ) );
    CHECK( addVariable ("ptIsolationConePhAngle"    , m_PtIsolationConePhAngle     ) );
  }

  if (m_fillPtCorrectedIsolation) {
    CHECK( addVariable ("Etcone40_pt_corrected", m_Etcone40_pt_corrected  ) );
    if (m_fillPtCorrectedIsolation > 1) {
      CHECK( addVariable ("Etcone35_pt_corrected", m_Etcone35_pt_corrected  ) );
      CHECK( addVariable ("Etcone30_pt_corrected", m_Etcone30_pt_corrected  ) );
      CHECK( addVariable ("Etcone25_pt_corrected", m_Etcone25_pt_corrected  ) );
      CHECK( addVariable ("Etcone20_pt_corrected", m_Etcone20_pt_corrected  ) );
      CHECK( addVariable ("Etcone15_pt_corrected", m_Etcone15_pt_corrected  ) );
    }
  }

  CHECK( addVariable ("Etcone40_ED_corrected"       , m_Etcone40_ED_corrected      ) );
  CHECK( addVariable ("Etcone40_corrected"          , m_Etcone40_corrected         ) );

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
StatusCode PAUFillerTool::fill (const PAU::egamma& p)
{

  if(m_fillPhotons){
    *m_convIP = p.convIP;
    *m_convIPRev = p.convIPRev;

    *m_PtIsolationCone = p.PtIsolationCone;
    *m_PtIsolationConePhAngle = p.PtIsolationConePhAngle;
  }

  if (m_fillPtCorrectedIsolation) {
    *m_Etcone40_pt_corrected = p.etcone40_pt_corrected;
    if (m_fillPtCorrectedIsolation > 1) {
      *m_Etcone30_pt_corrected = p.etcone30_pt_corrected;
      *m_Etcone20_pt_corrected = p.etcone20_pt_corrected;

      if (p.isElectron()) {
        // PAU bug workaround: PAU leaves these fields uninitialized
        // for electrons.
        *m_Etcone35_pt_corrected = PAU::UNKNOWN;
        *m_Etcone25_pt_corrected = PAU::UNKNOWN;
        *m_Etcone15_pt_corrected = PAU::UNKNOWN;
      }
      else {
        *m_Etcone35_pt_corrected = p.etcone35_pt_corrected;
        *m_Etcone25_pt_corrected = p.etcone25_pt_corrected;
        *m_Etcone15_pt_corrected = p.etcone15_pt_corrected;
      }
    }
  }

  *m_Etcone40_ED_corrected = p.etcone40_ED_corrected;
  *m_Etcone40_corrected    = p.etcone40_corrected   ;

  return StatusCode::SUCCESS;
}


} // namespace D3PD
