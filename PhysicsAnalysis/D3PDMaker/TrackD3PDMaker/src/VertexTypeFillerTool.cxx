/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VertexTypeFillerTool.cxx 502118 2012-05-23 19:10:19Z ssnyder $
/**
 * @file TrackD3PDMaker/src/VertexTypeFillerTool.cxx
 * @author Andreas Wildauer
 * @date Aug, 2011
 * @brief Block filler tool for vertex type. The vertex type does not fit into any other filler tool. Hence it gets its own.
 */


#include "VertexTypeFillerTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
VertexTypeFillerTool::VertexTypeFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
  : BlockFillerTool<Trk::VxCandidate> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode VertexTypeFillerTool::book()
{
  CHECK( addVariable ("type",    m_type) );
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
StatusCode VertexTypeFillerTool::fill (const Trk::VxCandidate& p)
{
  *m_type    = (int)p.vertexType();
  return StatusCode::SUCCESS;
}


} // namespace D3PD
