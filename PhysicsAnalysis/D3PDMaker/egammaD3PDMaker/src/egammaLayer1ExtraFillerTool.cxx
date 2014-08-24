/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/egammaLayer1ExtraFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Fill additional layer 1 quantities not available directly as details.
 */


#include "egammaLayer1ExtraFillerTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
egammaLayer1ExtraFillerTool::egammaLayer1ExtraFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : BlockFillerTool<xAOD::Egamma> (type, name, parent)
{
  book().ignore();  // Avoid coverity warnings.
}



/**
 * Book variables for this block.
 */
StatusCode egammaLayer1ExtraFillerTool::book()
{
  CHECK( addVariable ("deltaEs",    m_deltaEs) );
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
StatusCode egammaLayer1ExtraFillerTool::fill (const xAOD::Egamma& p)
{
  float e2tsts1 = -999;
  float emins1 = -999;
  if (!p.showerShapeValue (e2tsts1, xAOD::EgammaParameters::e2tsts1) ||
      !p.showerShapeValue (emins1,  xAOD::EgammaParameters::emins1))
    *m_deltaEs = -999;
  else
    *m_deltaEs = e2tsts1 - emins1;
  return StatusCode::SUCCESS;
}


} // namespace D3PD
