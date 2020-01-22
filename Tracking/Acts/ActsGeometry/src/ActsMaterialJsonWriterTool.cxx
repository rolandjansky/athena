/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsMaterialJsonWriterTool.h"

#include "Acts/Plugins/Json/JsonGeometryConverter.hpp"

#include <fstream>
#include <ios>
#include <iostream>
#include <stdexcept>

ActsMaterialJsonWriterTool:: ActsMaterialJsonWriterTool(const std::string &type, const std::string &name,
                                const IInterface *parent)
  : base_class(type, name, parent),
    m_fileName("MaterialOutputFile",name)
}

StatusCode
ActsMaterialJsonWriterTool::initialize()
{

  ATH_MSG_INFO("Starting Material writer");

  return StatusCode::SUCCESS;
}


void
ActsMaterialJsonWriterTool::write(const DetectorMaterialMaps& detMaterial)
{
  Acts::JsonGeometryConverter::Config cfg;
  // Evoke the converter
  Acts::JsonGeometryConverter jmConverter(cfg);
  auto jout = jmConverter.materialMapsToJson(detMaterial);
  // And write the file
  std::ofstream ofj(m_fileName);
  ofj << std::setw(4) << jout << std::endl;
}
