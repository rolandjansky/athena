/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ImpactSigmaFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2011
 * @brief Fill d0/z0 uncertainties from an ErrorMatrix.
 */


#include "ImpactSigmaFillerTool.h"
//#include "TrkEventPrimitives/ErrorMatrix.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "AthenaKernel/errorcheck.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include <sstream>


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
//ImpactSigmaFillerTool::ImpactSigmaFillerTool
//    (const std::string& type,
//     const std::string& name,
//     const IInterface* parent)
//      : BlockFillerTool<AmgSymMatrix(5)> (type, name, parent)
ImpactSigmaFillerTool::ImpactSigmaFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : BlockFillerTool<AmgSymMatrix(5)> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode ImpactSigmaFillerTool::book()
{
  CHECK( addVariable ("sigd0",    m_sigd0,
                      "Uncertainty on transverse impact parameter.",
                      -9999) );
  CHECK( addVariable ("sigz0",    m_sigz0,
                      "Uncertainty on longitudinal impact parameter.",
                      -9999) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param m The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
  //StatusCode ImpactSigmaFillerTool::fill (const AmgSymMatrix(5)& m)
  StatusCode ImpactSigmaFillerTool::fill (const AmgSymMatrix(5) &m)
{
  *m_sigd0 = Amg::error(m,Trk::d0);
  *m_sigz0 = Amg::error(m,Trk::z0);
  return StatusCode::SUCCESS;
}


} // namespace D3PD
