/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/egammaRetaphiFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Fill reta/rphi.
 */


#include "egammaRetaphiFillerTool.h"
#include "xAODEgamma/Egamma.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
egammaRetaphiFillerTool::egammaRetaphiFillerTool
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
StatusCode egammaRetaphiFillerTool::book()
{
  CHECK( addVariable ("reta",    m_reta) );
  CHECK( addVariable ("rphi",    m_rphi) );
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
StatusCode egammaRetaphiFillerTool::fill (const xAOD::Egamma& p)
{
  float e233 = 0;
  float e237 = 0;
  float e277 = 0;
  e233 = p.showerShapeValue (xAOD::EgammaParameters::e233);
  e237 = p.showerShapeValue (xAOD::EgammaParameters::e237);
  e277 = p.showerShapeValue (xAOD::EgammaParameters::e277);

  *m_reta = e277 ? e237 / e277 : 0;
  *m_rphi = e237 ? e233 / e237 : 0;

  return StatusCode::SUCCESS;
}


} // namespace D3PD
