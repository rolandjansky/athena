/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthVertexFillerTool.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {


TruthVertexFillerTool::TruthVertexFillerTool (const std::string& type,
				    const std::string& name,
				    const IInterface* parent)
  : TruthVertexFillerBase (type, name, parent)
{
  book().ignore();  // Avoid coverity warnings.
}

StatusCode TruthVertexFillerTool::initialize(){

  CHECK( TruthVertexFillerBase::initialize() );

  return StatusCode::SUCCESS;
}

StatusCode TruthVertexFillerTool::book()
{
  // position
  CHECK( addVariable ("x", m_x) );
  CHECK( addVariable ("y", m_y) );
  CHECK( addVariable ("z", m_z) );

  return StatusCode::SUCCESS;
}

StatusCode TruthVertexFillerTool::fill (const TruthVertexFillerObjectType& vertex)
{

  this->clearData();

  // position
  *m_x = vertex.position().x();
  *m_y = vertex.position().y();
  *m_z = vertex.position().z();

  REPORT_MESSAGE (MSG::VERBOSE) << "Filling simulated vertex branches: "
				<< "x = " << *m_x << " "
				<< "y = " << *m_y << " "
				<< "z = " << *m_z;

  return StatusCode::SUCCESS;
}

void TruthVertexFillerTool::clearData(){

  *m_x = 0;
  *m_y = 0;
  *m_z = 0;
}

} // namespace D3PD
