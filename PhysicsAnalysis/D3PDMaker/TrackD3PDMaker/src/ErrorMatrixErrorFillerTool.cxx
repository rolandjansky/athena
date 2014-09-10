/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/ErrorMatrixErrorFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2012
 * @brief Fill error elements from ErrorMatrix.
 */


#include "ErrorMatrixErrorFillerTool.h"
//#include "TrkEventPrimitives/ErrorMatrix.h"
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
ErrorMatrixErrorFillerTool::ErrorMatrixErrorFillerTool (const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent)
  : D3PD::BlockFillerTool<AmgSymMatrix(5)> (type, name, parent)
{
  declareProperty ("LocalType", m_localType = "CYLINDRICAL",
                   "Type of local variables: "
                   "CYLINDRICAL -- d0/z0; CARTESIAN -- locX/loxY; "
                   "NONE -- not written.");
  declareProperty ("FillThetaAndQoverP", m_fillThetaAndQoverP = true,
                   "Should theta and qoverp be filled?");
  declareProperty ("FillPhi", m_fillPhi=true,
                   "Should the phi variable be filled?");
  declareProperty ("DefaultValue", m_defaultValue = 0,
                   "Default value to which to set unfilled variables.");

  book().ignore();  // Silence coverity warnings
}


/**
 * @brief Book variables for this block.
 */
StatusCode ErrorMatrixErrorFillerTool::book()
{
  if (m_localType == "CYLINDRICAL") {
    CHECK( addVariable ("d0_err", m_loc1_err,
                        "Local transverse impact", m_defaultValue) );
    CHECK( addVariable ("z0_err", m_loc2_err, 
                        "Local longitudinal impact", m_defaultValue) );
  }
  else if (m_localType == "CARTESIAN") {
    CHECK( addVariable ("locX_err", m_loc1_err,
                        "Local x coordinate", m_defaultValue) );
    CHECK( addVariable ("locY_err", m_loc2_err,
                        "Local y coordinate", m_defaultValue) );
  }
  else {
    m_loc1_err = 0;
    m_loc2_err = 0;
  }

  if (m_fillPhi)
    CHECK( addVariable ("phi_err", m_phi_err,
                        "Azimuthal angle", m_defaultValue) );

  if(m_fillThetaAndQoverP){
    CHECK( addVariable ("theta_err", m_theta_err,
                        "Polar angle", m_defaultValue) );
    CHECK( addVariable ("qoverp_err", m_qoverp_err,
                        "Charge over momentum", m_defaultValue) );
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
  StatusCode ErrorMatrixErrorFillerTool::fill (const AmgSymMatrix(5) &p)
{
  // Either d0/z0 or locX/locY
  if (m_loc1_err) {
    *m_loc1_err = Amg::error(p,Trk::loc1);
    *m_loc2_err = Amg::error(p,Trk::loc2);
  }

  if (m_fillPhi)
    *m_phi_err = Amg::error(p,Trk::phi0);

  if(m_fillThetaAndQoverP){
    *m_theta_err  = Amg::error(p,Trk::theta);
    *m_qoverp_err = Amg::error(p,Trk::qOverP);
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
