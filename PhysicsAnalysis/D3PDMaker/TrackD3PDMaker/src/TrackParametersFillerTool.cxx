/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/TrackParametersFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2012
 * @brief Fill variables from track parameters.
 */


#include "TrackParametersFillerTool.h"
#include "TrkParameters/TrackParameters.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrackParametersFillerTool::TrackParametersFillerTool (const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent)
  : D3PD::BlockFillerTool<Trk::TrackParameters> (type, name, parent)
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
StatusCode TrackParametersFillerTool::book()
{
  if (m_localType == "CYLINDRICAL") {
    CHECK( addVariable ("d0", m_loc1,
                        "Local transverse impact", m_defaultValue) );
    CHECK( addVariable ("z0", m_loc2, 
                        "Local longitudinal impact", m_defaultValue) );
  }
  else if (m_localType == "CARTESIAN") {
    CHECK( addVariable ("locX", m_loc1, "Local x coordinate", m_defaultValue) );
    CHECK( addVariable ("locY", m_loc2, "Local y coordinate", m_defaultValue) );
  }
  else {
    m_loc1 = 0;
    m_loc2 = 0;
  }

  if (m_fillPhi)
    CHECK( addVariable ("phi", m_phi, "Azimuthal angle", m_defaultValue) );

  if(m_fillThetaAndQoverP){
    CHECK( addVariable ("theta", m_theta, "Polar angle", m_defaultValue) );
    CHECK( addVariable ("qoverp", m_qoverp,
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
StatusCode TrackParametersFillerTool::fill (const Trk::TrackParameters& p)
{
  // Either d0/z0 or locX/locY
  if (m_loc1) {
    *m_loc1 = p.parameters()[Trk::loc1];
    *m_loc2 = p.parameters()[Trk::loc2];
  }

  if (m_fillPhi)
    *m_phi = p.parameters()[Trk::phi0];

  if(m_fillThetaAndQoverP){
    *m_theta = p.parameters()[Trk::theta];
    *m_qoverp = p.parameters()[Trk::qOverP];
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
