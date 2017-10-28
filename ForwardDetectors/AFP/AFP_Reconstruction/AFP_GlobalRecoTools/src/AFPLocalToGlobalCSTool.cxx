/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPLocalToGlobalCSTool.cxx
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-05-13
 * 
 * @brief  Source file for AFPLocalToGlobalCSTool.
 */

// STL includes

// CLHEP and ROOT includes
#include "CLHEP/Geometry/Point3D.h"

// FrameWork includes
#include "AFP_Geometry/AFP_constants.h"
#include "AFPLocalToGlobalCSTool.h"
#include "AFP_GlobalRecoTools/IAFPLocalToGlobalCSTool.h"

AFPLocalToGlobalCSTool::AFPLocalToGlobalCSTool (const std::string& type,
				    const std::string& name,
				    const IInterface* parent)
  : ::AthAlgTool(type, name, parent),
  m_geometry(nullptr)
{
  declareInterface<IAFPLocalToGlobalCSTool>(this);

  declareProperty("xCorrection", m_xCorrection,
                  "Vector of x position correction with respect to AFP_Geometry position.");
}


AFPLocalToGlobalCSTool::~AFPLocalToGlobalCSTool ()
{
  if (m_geometry != nullptr)
    delete m_geometry;
  m_geometry = nullptr;
}

StatusCode AFPLocalToGlobalCSTool::initialize()
{
  // set default geometry configuration
  m_geoConfig.clear();
  m_geometry = new AFP_Geometry(&m_geoConfig);

  // if correction was not set in the job options set default values
  if (m_xCorrection.size() != 4) {
    ATH_MSG_INFO ("No value (or wrong size of the vector) for x position correction set in job options. Using default values.");
    m_xCorrection.resize (4, 0);
  }

  // print information about x correction
  ATH_MSG_INFO ("The following xCorrection is used for local to global CS transformation: ["
		<<m_xCorrection[0]<<", "<<m_xCorrection[1]<<", "<<m_xCorrection[2]<<", "<<m_xCorrection[3]<<"]");
  
  return StatusCode::SUCCESS;
}

StatusCode AFPLocalToGlobalCSTool::finalize() 
{
  if (m_geometry != nullptr)
    delete m_geometry;
  m_geometry = nullptr;

  return StatusCode::SUCCESS;
}

TVector3 AFPLocalToGlobalCSTool::localToGlobalCS(const double localX, const double localY, const double localZ, const int stationID) const
{
  const HepGeom::Point3D<double> localPoint (localX, localY, localZ);
  HepGeom::Point3D<double> globalPoint;

  if (m_geometry->GetPointInSIDSensorGlobalCS(stationID, 0, localPoint, globalPoint).isSuccess()) {
    // if transformation was successful return new coordinates corrected for the offset
    return TVector3(globalPoint.x() + m_xCorrection[stationID], globalPoint.y(), globalPoint.z());
  }
  else {
    // if transformation failed print warning message and return local position
    ATH_MSG_WARNING ("Error occurred when calling m_geometry->GetPointInSIDSensorGlobalCS(). Returning local coordinates.");
    return TVector3(localX, localY, localZ);
  }
}
