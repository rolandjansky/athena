/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file   JetD3PDMaker/src/JetROIWordFillerTool.h
 * @author Haifeng Li <Haifeng.Li@cern.ch>
 * @date   July 13, 2010
 * @brief Block filler tool for Jet ROIWord
 */


#include "JetROIWordFillerTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {
  
  
/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
JetROIWordFillerTool::JetROIWordFillerTool (const std::string& type,
					    const std::string& name,
					    const IInterface* parent)
  : BlockFillerTool<Jet> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetROIWordFillerTool::book()
{
  
  CHECK( addVariable ( "RoIword", m_RoIword )  );
  
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object. The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode JetROIWordFillerTool::fill(const Jet& p)
{
 
  *m_RoIword = p.RoIword();
  return StatusCode::SUCCESS;

}


} // namespace D3PD
