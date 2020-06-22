/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsObjWriterTool.h"

// std
#include <iostream>
#include <vector>

// Gaudi / Athena
#include "GaudiKernel/IInterface.h"

#include "ActsGeometry/ActsGeometryContext.h"
// ACTS
#include "Acts/Geometry/TrackingGeometry.hpp"

// PRIVATE
#include "util/ObjSurfaceWriter.h"
#include "util/ObjTrackingGeometryWriter.h"


ActsObjWriterTool::ActsObjWriterTool(const std::string& type, const std::string& name,
    const IInterface* parent)
  : AthAlgTool(type, name, parent)
{
  // declareProperty("OutputDirectory", m_outputDirectory = "");
  // declareProperty("SubDetectors", m_subDetectors = {});
}

StatusCode
ActsObjWriterTool::initialize()
{


  return StatusCode::SUCCESS;
}

void
ActsObjWriterTool::write(const ActsGeometryContext& gctx, const Acts::TrackingGeometry& tg) const
{ 
  const auto& ctx = Gaudi::Hive::currentContext();
  std::stringstream ss;
  ss << ctx.eventID().run_number() << "_" << ctx.eventID().event_number();

  using namespace std::string_literals;

  std::vector<std::shared_ptr<Acts::ObjSurfaceWriter>> subWriters;
  std::vector<std::shared_ptr<std::ofstream>>           subStreams;

  for (const auto& sdet : m_subDetectors) {
    auto        sdStream = std::shared_ptr<std::ofstream>(new std::ofstream);
    std::string sdOutputName = m_outputDirectory + "/"s + sdet + "_" + ss.str() + ".obj"s;
    sdStream->open(sdOutputName);
    // object surface writers
    Acts::ObjSurfaceWriter::Config sdObjWriterConfig(sdet,
        Acts::Logging::INFO);
    sdObjWriterConfig.filePrefix         = "";
    sdObjWriterConfig.outputPhiSegments  = 10;
    sdObjWriterConfig.outputPrecision    = 6;
    sdObjWriterConfig.outputScalor       = 1.;
    sdObjWriterConfig.outputThickness    = 1.;
    sdObjWriterConfig.outputSensitive    = true;
    sdObjWriterConfig.outputLayerSurface = false;
    sdObjWriterConfig.outputStream       = sdStream;
    auto sdObjWriter
      = std::make_shared<Acts::ObjSurfaceWriter>(sdObjWriterConfig);
    // push back
    subWriters.push_back(sdObjWriter);
    subStreams.push_back(sdStream);
  }


  // configure the tracking geometry writer
  Acts::ObjTrackingGeometryWriter::Config tgObjWriterConfig(
      "ObjTrackingGeometryWriter", Acts::Logging::INFO);
  tgObjWriterConfig.surfaceWriters       = subWriters;
  tgObjWriterConfig.filePrefix           = "";
  tgObjWriterConfig.sensitiveGroupPrefix = "";
  tgObjWriterConfig.layerPrefix          = "";
  // the tracking geometry writers
  auto tgObjWriter
    = std::make_shared<Acts::ObjTrackingGeometryWriter>(tgObjWriterConfig);

  tgObjWriter->write(gctx.any(), tg);
}
