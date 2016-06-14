/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi includes
#include "GaudiKernel/MsgStream.h"

// Calo includes
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloUtils/CaloTowerBuilderTool.h"

// Tile includes
#include "TileRecUtils/TileTowerBuilderTool.h"

#include <string>

TileTowerBuilderTool::TileTowerBuilderTool(const std::string& name, const std::string& type,
    const IInterface* parent)
    : CaloTowerBuilderTool(name, type, parent)
{
  declareProperty("DumpTowers", m_dumpTowers = false);
  declareProperty("DumpWeightMap", m_dumpWeightMap = false);
}

TileTowerBuilderTool::~TileTowerBuilderTool() {
}

StatusCode TileTowerBuilderTool::initializeTool() {

  // allow only TILE cells!
  for (size_t iCalos = 0; iCalos < m_includedCalos.size(); iCalos++) {
    if (m_includedCalos[iCalos] == "TILE") {
      m_caloIndices.push_back(CaloCell_ID::TILE);
    }
  }

  // check setup
  return this->checkSetup(msg());
}
