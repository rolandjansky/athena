/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSWRITETRACKINGGEOMETRY_H
#define ACTSGEOMETRY_ACTSWRITETRACKINGGEOMETRY_H

// ATHENA
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "Gaudi/Property.h"  /*no forward decl: typedef*/
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
class IActsMaterialJsonWriterTool;

class ActsWriteTrackingGeometry : public AthReentrantAlgorithm {
public:
  ActsWriteTrackingGeometry (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

private:

  ToolHandle<ActsTrackingGeometryTool> m_trackingGeometryTool{this, "TrackingGeometryTool", "ActsTrackingGeometryTool"};

  ToolHandle<ActsObjWriterTool> m_objWriterTool{this, "ObjWriterTool", "ActsObjWriterTool"};
  ToolHandle<IActsMaterialJsonWriterTool> m_materialJsonWriterTool{this, "MaterialJsonWriterTool", "ActsMaterialJsonWriterTool"};

};

#endif
