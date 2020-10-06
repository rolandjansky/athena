/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSMATERIALMAPPING_H
#define ACTSMATERIALMAPPING_H

// ATHENA
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Gaudi/Property.h"  /*no forward decl: typedef*/
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkGeometry/MaterialStepCollection.h"

// ACTS
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Material/SurfaceMaterialMapper.hpp"

// PACKAGE
#include "ActsGeometry/ActsTrackingGeometryTool.h"

// STL
#include <memory>
#include <vector>
#include <fstream>
#include <mutex>

namespace Acts {
  class ISurfaceMaterial;
  class IVolumeMaterial;

  using SurfaceMaterialMap
      = std::map<GeometryIdentifier, std::shared_ptr<const ISurfaceMaterial>>;

  using VolumeMaterialMap
      = std::map<GeometryIdentifier, std::shared_ptr<const IVolumeMaterial>>;

  using DetectorMaterialMaps = std::pair<SurfaceMaterialMap, VolumeMaterialMap>;
}

class IActsMaterialTrackWriterSvc;
class IActsMaterialStepConverterTool;
class IActsSurfaceMappingTool;
class IActsMaterialJsonWriterTool;

class ActsMaterialMapping : public AthReentrantAlgorithm {
public:
  ActsMaterialMapping (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

private:
  ServiceHandle<IActsMaterialTrackWriterSvc>      m_materialTrackWriterSvc;
  ToolHandle<IActsMaterialStepConverterTool>      m_materialStepConverterTool{this, "MaterialStepConverterTool", "ActsMaterialStepConverterTool"};
  SG::ReadHandleKey<Trk::MaterialStepCollection>  m_inputMaterialStepCollection;
  ToolHandle<IActsSurfaceMappingTool>             m_surfaceMappingTool{this, "SurfaceMappingTool", "ActsSurfaceMappingTool"};
  ToolHandle<IActsMaterialJsonWriterTool>         m_materialJsonWriterTool{this, "MaterialJsonWriterTool", "ActsMaterialJsonWriterTool"};

  Acts::MagneticFieldContext                      m_mctx;
  Acts::GeometryContext                           m_gctx;
  Acts::SurfaceMaterialMapper::State              m_mappingState;
};

#endif // ActsGeometry_ActsExtrapolation_h
