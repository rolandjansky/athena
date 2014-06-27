/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/PAUTruthPhotonFillerTool.h
 * @author Mike Hance <hance@hep.upenn.edu>
 * @date January, 2011
 * @brief Fill PAU related variables
 */


#include "PAUTruthPhotonFillerTool.h"
#include "AthenaKernel/errorcheck.h"

#include "PhotonAnalysisUtils/PAUegammaTruth.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
PAUTruthPhotonFillerTool::PAUTruthPhotonFillerTool (const std::string& type,
						    const std::string& name,
						    const IInterface* parent)
  : BlockFillerTool<PAU::egammaTruth> (type, name, parent)
{
  m_fillNewVars = true;
  book().ignore(); // Avoid coverity warnings.

  declareProperty( "FillNewVars", m_fillNewVars=false,
		   "Fill isolation variables with sliding eta window for correction" );
}


/**
 * @brief Book variables for this block.
 */
StatusCode PAUTruthPhotonFillerTool::book()
{
  CHECK( addVariable ("PartonIsolation15_UE"        , m_partonIsolation15_UE       ) );
  CHECK( addVariable ("PartonIsolation20_UE"        , m_partonIsolation20_UE       ) );
  CHECK( addVariable ("PartonIsolation25_UE"        , m_partonIsolation25_UE       ) );
  CHECK( addVariable ("PartonIsolation30_UE"        , m_partonIsolation30_UE       ) );
  CHECK( addVariable ("PartonIsolation35_UE"        , m_partonIsolation35_UE       ) );
  CHECK( addVariable ("PartonIsolation40_UE"        , m_partonIsolation40_UE       ) );
  CHECK( addVariable ("PartonIsolation45_UE"        , m_partonIsolation45_UE       ) );
  CHECK( addVariable ("PartonIsolation60_UE"        , m_partonIsolation60_UE       ) );

  CHECK( addVariable ("PartonIsolation15"           , m_partonIsolation15          ) );
  CHECK( addVariable ("PartonIsolation20"           , m_partonIsolation20          ) );
  CHECK( addVariable ("PartonIsolation25"           , m_partonIsolation25          ) );
  CHECK( addVariable ("PartonIsolation30"           , m_partonIsolation30          ) );
  CHECK( addVariable ("PartonIsolation35"           , m_partonIsolation35          ) );
  CHECK( addVariable ("PartonIsolation40"           , m_partonIsolation40          ) );
  CHECK( addVariable ("PartonIsolation45"           , m_partonIsolation45          ) );
  CHECK( addVariable ("PartonIsolation60"           , m_partonIsolation60          ) );

  CHECK( addVariable ("ParticleIsolation40"                      , m_particleIsolation40                    ) );
  CHECK( addVariable ("ParticleIsolation40_ED_corrected"         , m_particleIsolation40_ED_corrected       ) );
  CHECK( addVariable ("ParticleIsolation40_ED_corrected_reco"    , m_particleIsolation40_ED_corrected_reco  ) );

  if(m_fillNewVars){
    CHECK( addVariable ("ParticleIsolation40_ED_corrected_new"         , m_particleIsolation40_ED_corrected_new       ) );
    CHECK( addVariable ("ParticleIsolation40_ED_corrected_reco_new"    , m_particleIsolation40_ED_corrected_reco_new  ) );
  }

  CHECK( addVariable ("isBremPhoton"                , m_isBremPhoton               ) );
  CHECK( addVariable ("isHardProcPhoton"            , m_isHardProcPhoton           ) );

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
StatusCode PAUTruthPhotonFillerTool::fill (const PAU::egammaTruth& p)
{

  * m_partonIsolation15_UE  = p.partonIsolation15_UE;
  * m_partonIsolation20_UE  = p.partonIsolation20_UE;
  * m_partonIsolation25_UE  = p.partonIsolation25_UE;
  * m_partonIsolation30_UE  = p.partonIsolation30_UE;
  * m_partonIsolation35_UE  = p.partonIsolation35_UE;
  * m_partonIsolation40_UE  = p.partonIsolation40_UE;
  * m_partonIsolation45_UE  = p.partonIsolation45_UE;
  * m_partonIsolation60_UE  = p.partonIsolation60_UE;

  * m_partonIsolation15     = p.partonIsolation15   ;
  * m_partonIsolation20     = p.partonIsolation20   ;
  * m_partonIsolation25     = p.partonIsolation25   ;
  * m_partonIsolation30     = p.partonIsolation30   ;
  * m_partonIsolation35     = p.partonIsolation35   ;
  * m_partonIsolation40     = p.partonIsolation40   ;
  * m_partonIsolation45     = p.partonIsolation45   ;
  * m_partonIsolation60     = p.partonIsolation60   ;

  * m_particleIsolation40                     = p.particleIsolation40                   ;
  * m_particleIsolation40_ED_corrected        = p.particleIsolation40_ED_corrected      ;
  * m_particleIsolation40_ED_corrected_reco   = p.particleIsolation40_ED_corrected_reco ;

  if(m_fillNewVars){
    * m_particleIsolation40_ED_corrected_new        = p.particleIsolation40_ED_corrected_new      ;
    * m_particleIsolation40_ED_corrected_reco_new   = p.particleIsolation40_ED_corrected_new_reco ;
  }

  * m_isHardProcPhoton      = p.isHardProc          ;
  * m_isBremPhoton          = p.isBrem              ;

  return StatusCode::SUCCESS;
}


} // namespace D3PD
