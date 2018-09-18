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
#include "ActsGeometry/ObjWriterTool.h"
#include "ActsGeometry/IExCellWriterSvc.h"

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
  ServiceHandle<IActsTrackingGeometrySvc> m_trackingGeometrySvc;

  std::shared_ptr<const Acts::TrackingGeometry> m_trackingGeometry;

  ToolHandle<Acts::ObjWriterTool> m_objWriterTool{this, "ObjWriterTool", "Acts__ObjWriterTool"};

};

#endif
