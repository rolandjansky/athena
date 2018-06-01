#include "GeomACTS/ObjWriterTool.h"

// std
#include <iostream>
#include <vector>

// Gaudi / Athena
#include "GaudiKernel/IInterface.h"

// ACTS
#include "Acts/Detector/TrackingGeometry.hpp"

// Package
#include "GeomACTS/obj/ObjSurfaceWriter.hpp"
#include "GeomACTS/obj/ObjTrackingGeometryWriter.hpp"


Acts::ObjWriterTool::ObjWriterTool(const std::string& type, const std::string& name,
    const IInterface* parent) 
  : AthAlgTool(type, name, parent)
{
  //declareProperty("OutputDirectory", m_outputDirectory = "");
  //declareProperty("SubDetectors", m_subDetectors = {});
}
  
StatusCode 
Acts::ObjWriterTool::initialize()
{

  using namespace std::string_literals;

  std::vector<std::shared_ptr<ObjSurfaceWriter>> subWriters;
  std::vector<std::shared_ptr<std::ofstream>>           subStreams;

  for (const auto& sdet : m_subDetectors) {
    auto        sdStream = std::shared_ptr<std::ofstream>(new std::ofstream);
    std::string sdOutputName = m_outputDirectory + "/"s + sdet + ".obj"s;
    sdStream->open(sdOutputName);
    // object surface writers
    ObjSurfaceWriter::Config sdObjWriterConfig(sdet,
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
      = std::make_shared<ObjSurfaceWriter>(sdObjWriterConfig);
    // push back
    subWriters.push_back(sdObjWriter);
    subStreams.push_back(sdStream);
  }

  // configure the tracking geometry writer
  ObjTrackingGeometryWriter::Config tgObjWriterConfig(
      "ObjTrackingGeometryWriter", Acts::Logging::INFO);
  tgObjWriterConfig.surfaceWriters       = subWriters;
  tgObjWriterConfig.filePrefix           = "";
  tgObjWriterConfig.sensitiveGroupPrefix = "";
  tgObjWriterConfig.layerPrefix          = "";
  // the tracking geometry writers
  m_tgObjWriter
    = std::make_shared<ObjTrackingGeometryWriter>(tgObjWriterConfig);


  return StatusCode::SUCCESS;
}

void
Acts::ObjWriterTool::write(const Acts::TrackingGeometry& tg)
{
  m_tgObjWriter->write(tg);
}

