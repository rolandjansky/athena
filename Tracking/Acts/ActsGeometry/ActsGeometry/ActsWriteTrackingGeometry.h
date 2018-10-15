/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSWRITETRACKINGGEOMETRY_H
#define ACTSGEOMETRY_ACTSWRITETRACKINGGEOMETRY_H

// ATHENA
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "GaudiKernel/Property.h"  /*no forward decl: typedef*/
#include "GaudiKernel/ISvcLocator.h"

// PACKAGE
#include "ActsGeometry/ActsObjWriterTool.h"
#include "ActsGeometry/ActsTrackingGeometryTool.h"

// STL
#include <fstream>
#include <memory>
#include <vector>

namespace Acts {
  class TrackingGeometry;
}

class IActsTrackingGeometrySvc;

class ActsWriteTrackingGeometry : public AthAlgorithm {
public:
  ActsWriteTrackingGeometry (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;

private:

  ToolHandle<ActsTrackingGeometryTool> m_trackingGeometryTool{this, "TrackingGeometryTool", "ActsTrackingGeometryTool"};

  ToolHandle<ActsObjWriterTool> m_objWriterTool{this, "ObjWriterTool", "ActsObjWriterTool"};

};

#endif
