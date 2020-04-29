/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenVertexFillerTool.cxx 586037 2014-03-03 23:23:18Z zmarshal $
/**
 * @file EventCommonD3PDMaker/src/GenVertexFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2010
 * @brief Fill the position of a vertex.
 */


#include "GenVertexFillerTool.h"
#include "AtlasHepMC/GenVertex.h"
#include "CLHEP/Geometry/Point3D.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
GenVertexFillerTool::GenVertexFillerTool (const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent)
  : BlockFillerTool<HepMC::GenVertex> (type, name, parent)
{
  m_do_id = false;
  book().ignore();  // Avoid coverity warnings.
  declareProperty ("WriteID",        m_do_id        = false);
}


/**
 * @brief Book variables for this block.
 */
StatusCode GenVertexFillerTool::book()
{
  CHECK( addVariable ("x", m_x, "Vertex x position") );
  CHECK( addVariable ("y", m_y, "Vertex y position") );
  CHECK( addVariable ("z", m_z, "Vertex z position") );
  CHECK( addVariable ("barcode", m_barcode, "Vertex barcode") );
  if (m_do_id){
    CHECK( addVariable ("id", m_id, "Vertex ID") );
  }
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
StatusCode GenVertexFillerTool::fill (const HepMC::GenVertex& p)
{
  HepMC::FourVector pos = p.position();
  *m_x = pos.x();
  *m_y = pos.y();
  *m_z = pos.z();
  *m_barcode = p.barcode();
  if (m_do_id) *m_id = p.id();

  return StatusCode::SUCCESS;
}


} // namespace D3PD
