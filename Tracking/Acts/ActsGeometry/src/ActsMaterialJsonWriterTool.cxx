/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsMaterialJsonWriterTool.h"

#include <fstream>
#include <ios>
#include <iostream>
#include <stdexcept>

ActsMaterialJsonWriterTool::ActsMaterialJsonWriterTool(const std::string &type, const std::string &name,
                                const IInterface *parent)
  : base_class(type, name, parent)
{
}

ActsMaterialJsonWriterTool::~ActsMaterialJsonWriterTool()
{
}

StatusCode
ActsMaterialJsonWriterTool::initialize()
{
  ATH_MSG_INFO("Starting Material writer");

  return StatusCode::SUCCESS;
}

void
ActsMaterialJsonWriterTool::write(const Acts::JsonGeometryConverter::DetectorMaterialMaps& detMaterial) const
{
  Acts::JsonGeometryConverter::Config cfg;
  // Evoke the converter
  Acts::JsonGeometryConverter jmConverter(cfg);
  auto jout = jmConverter.materialMapsToJson(detMaterial);
  // And write the file
  std::ofstream ofj(m_filePath);
  ofj << std::setw(4) << jout << std::endl;
}

void
ActsMaterialJsonWriterTool::write(const Acts::TrackingGeometry& tGeometry) const
{
  Acts::JsonGeometryConverter::Config cfg;
  // Evoke the converter
  Acts::JsonGeometryConverter jmConverter(cfg);
  auto jout = jmConverter.trackingGeometryToJson(tGeometry);
  // And write the file
  std::ofstream ofj(m_filePath);
  ofj << std::setw(4) << jout << std::endl;
}
