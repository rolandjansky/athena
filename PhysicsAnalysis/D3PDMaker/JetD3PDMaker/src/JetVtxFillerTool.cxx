/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetVtxFillerTool.cxx
 * @author Pier-Olivier DeViveiros < viveiros AT physics.utoronto.ca > 
   @author Haifeng Li <Haifeng.Li@cern.ch>
 * @date September, 2009
 * @brief Block filler tool for Jet Vertex Fraction
 */


#include "JetVtxFillerTool.h"
#include "JetEvent/Jet.h"
#include "VxVertex/RecVertex.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {
  
  
/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
JetVtxFillerTool::JetVtxFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent)
{
  book().ignore();  // Avoid coverity warnings
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetVtxFillerTool::book()
{
  CHECK( addVariable ("jvtx_x",    m_jvtx_x )  );
  CHECK( addVariable ("jvtx_y",    m_jvtx_y )  );
  CHECK( addVariable ("jvtx_z",    m_jvtx_z )  );
  
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
StatusCode JetVtxFillerTool::fill(const Jet& p)
{
  if (const Trk::RecVertex* jvtx = p.origin()) {
    Amg::Vector3D jvtx_pos = jvtx->position();
    *m_jvtx_x = jvtx_pos.x();
    *m_jvtx_y = jvtx_pos.y();
    *m_jvtx_z = jvtx_pos.z();
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
