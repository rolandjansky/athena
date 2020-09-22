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

  m_cfg = Acts::JsonGeometryConverter::Config("JsonGeometryConverter",Acts::Logging::INFO);
  m_cfg.processSensitives = m_processSensitives;
  m_cfg.processApproaches = m_processApproaches;
  m_cfg.processRepresenting = m_processRepresenting;
  m_cfg.processBoundaries = m_processBoundaries;
  m_cfg.processVolumes = m_processVolumes;
  m_cfg.processDenseVolumes = m_processDenseVolumes;
  m_cfg.processnonmaterial = m_processnonmaterial;
  
  return StatusCode::SUCCESS;
}

void
ActsMaterialJsonWriterTool::write(const Acts::JsonGeometryConverter::DetectorMaterialMaps& detMaterial) const
{
  // Evoke the converter
  Acts::JsonGeometryConverter jmConverter(m_cfg);
  auto jout = jmConverter.materialMapsToJson(detMaterial);
  // And write the file
  std::ofstream ofj(m_filePath);
  ofj << std::setw(4) << jout << std::endl;
}

void
ActsMaterialJsonWriterTool::write(const Acts::TrackingGeometry& tGeometry) const
{
  // Evoke the converter
  Acts::JsonGeometryConverter jmConverter(m_cfg);
  auto jout = jmConverter.trackingGeometryToJson(tGeometry);
  // And write the file
  std::ofstream ofj(m_filePath);
  ofj << std::setw(4) << jout << std::endl;
}
