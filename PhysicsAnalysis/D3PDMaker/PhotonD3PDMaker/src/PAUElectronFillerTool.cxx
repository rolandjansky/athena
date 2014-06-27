/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/PAUElectronFillerTool.h
 * @author Mike Hance <hance@hep.upenn.edu>
 * @date March, 2010
 * @brief Fill PAU related variables
 */


#include "PAUElectronFillerTool.h"
#include "AthenaKernel/errorcheck.h"

#include "PhotonAnalysisUtils/PAUegamma.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
PAUElectronFillerTool::PAUElectronFillerTool (const std::string& type,
			      const std::string& name,
			      const IInterface* parent)
  : BlockFillerTool<PAU::egamma> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode PAUElectronFillerTool::book()
{
  CHECK( addVariable("ambiguityResult"				     , m_ambiguityResult			    ) );

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
StatusCode PAUElectronFillerTool::fill (const PAU::egamma& p)
{
  * m_ambiguityResult			        = p.ambiguityResult			       ;

  return StatusCode::SUCCESS;
}


} // namespace D3PD
