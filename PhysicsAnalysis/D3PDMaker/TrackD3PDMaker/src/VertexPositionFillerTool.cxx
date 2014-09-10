/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VertexPositionFillerTool.cxx 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/VertexPositionFillerTool.cxx
 * @author maarten boonekamp <maarten.boonekamp@cea.fr>
 * @date Sep, 2009
 * @brief Block filler tool for vertex position.
 */


#include "VertexPositionFillerTool.h"
#include "VxVertex/VxCandidate.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
VertexPositionFillerTool::VertexPositionFillerTool
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
StatusCode VertexPositionFillerTool::book()
{
  CHECK( addVariable ("x",  m_x)  );
  CHECK( addVariable ("y",  m_y)  );
  CHECK( addVariable ("z",  m_z)  );
 
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
StatusCode VertexPositionFillerTool::fill (const Trk::VxCandidate& p)
{
  const Amg::Vector3D& pos = p.recVertex().position();
  *m_x = pos.x();
  *m_y = pos.y();
  *m_z = pos.z();
  return StatusCode::SUCCESS;
}


} // namespace D3PD
