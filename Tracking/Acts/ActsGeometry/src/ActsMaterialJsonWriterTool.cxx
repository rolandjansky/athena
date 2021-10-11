/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsGeometryContext.h"
#include "ActsGeometry/ActsMaterialJsonWriterTool.h"

#include "ActsInterop/Logger.h"

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
ActsMaterialJsonWriterTool::write(const ActsGeometryContext& gctx, const Acts::MaterialMapJsonConverter::DetectorMaterialMaps& detMaterial) const
{
  // Setup the converter config
  Acts::MaterialMapJsonConverter::Config cfg;
  cfg.context = gctx.context();
  cfg.processSensitives = m_processSensitives;
  cfg.processApproaches = m_processApproaches;
  cfg.processRepresenting = m_processRepresenting;
  cfg.processBoundaries = m_processBoundaries;
  cfg.processVolumes = m_processVolumes;
  cfg.processDenseVolumes = m_processDenseVolumes;
  cfg.processNonMaterial = m_processNonMaterial;

  // Evoke the converter
  Acts::MaterialMapJsonConverter jmConverter(cfg, Acts::Logging::INFO);
  auto jout = jmConverter.materialMapsToJson(detMaterial);
  // And write the file
  std::ofstream ofj(m_filePath);
  ofj << std::setw(4) << jout << std::endl;
}

void
ActsMaterialJsonWriterTool::write(const ActsGeometryContext& gctx, const Acts::TrackingGeometry& tGeometry) const
{
  // Setup the converter config
  Acts::MaterialMapJsonConverter::Config cfg;
  cfg.context = gctx.context();
  cfg.processSensitives = m_processSensitives;
  cfg.processApproaches = m_processApproaches;
  cfg.processRepresenting = m_processRepresenting;
  cfg.processBoundaries = m_processBoundaries;
  cfg.processVolumes = m_processVolumes;
  cfg.processDenseVolumes = m_processDenseVolumes;
  cfg.processNonMaterial = m_processNonMaterial;

  // Evoke the converter
  Acts::MaterialMapJsonConverter jmConverter(cfg, Acts::Logging::INFO);
  auto jout = jmConverter.trackingGeometryToJson(tGeometry);
  // And write the file
  std::ofstream ofj(m_filePath);
  ofj << std::setw(4) << jout << std::endl;
}
