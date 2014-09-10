/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BeamSpotFillerTool.cxx 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/BeamSpotFillerTool.cxx
 * @author remi zaidan <zaidan@cern.ch>
 * @date Jan, 2011
 * @brief Block filler tool for beam spot parameters.
 */


#include "BeamSpotFillerTool.h"
#include "VxVertex/VxCandidate.h"
#include "AthenaKernel/errorcheck.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrkEventPrimitives/ParamDefs.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
BeamSpotFillerTool::BeamSpotFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
  : Base (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode BeamSpotFillerTool::book()
{

  CHECK( addVariable ("x", m_x, "", -9999) );
  CHECK( addVariable ("y", m_y, "", -9999) );
  CHECK( addVariable ("z", m_z, "", -9999) );
  CHECK( addVariable ("sigma_x", m_sigma_x, "", -1 ) );
  CHECK( addVariable ("sigma_y", m_sigma_y, "", -1 ) );
  CHECK( addVariable ("sigma_z", m_sigma_z, "", -1 ) );
  CHECK( addVariable ("tilt_x", m_tilt_x, "", -1 ) );
  CHECK( addVariable ("tilt_y", m_tilt_y, "", -1 ) );
 
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
StatusCode BeamSpotFillerTool::fill (const Trk::RecVertex& p)
{

  const Amg::Vector3D& pos = p.position();
  *m_x = pos.x();
  *m_y = pos.y();
  *m_z = pos.z();

  const AmgSymMatrix(3)& m = p.covariancePosition();
  //*m_sigma_x = Amg::error(m,Trk::x);
  //*m_sigma_y = Amg::error(m,Trk::y);
  //*m_sigma_z = Amg::error(m,Trk::z);

  *m_sigma_x = sqrt(m(Trk::x,Trk::x));
  *m_sigma_y = sqrt(m(Trk::y,Trk::y));
  *m_sigma_z = sqrt(m(Trk::z,Trk::z));

  double cxz = m(Trk::x, Trk::z);
  double cyz = m(Trk::y, Trk::z);

  if(*m_sigma_z != 0) {
    *m_tilt_x = atan(cxz / ( (*m_sigma_z)*(*m_sigma_z) ));
    *m_tilt_y = atan(cyz / ( (*m_sigma_z)*(*m_sigma_z) ));
  }

  return StatusCode::SUCCESS;
}

} // namespace D3PD
