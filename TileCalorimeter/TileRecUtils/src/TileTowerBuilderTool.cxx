/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi includes
#include "GaudiKernel/MsgStream.h"

// Calo includes
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloUtils/CaloTowerBuilderTool.h"

// Tile includes
#include "TileTowerBuilderTool.h"

#include <string>

TileTowerBuilderTool::TileTowerBuilderTool(const std::string& name, const std::string& type,
    const IInterface* parent)
    : CaloTowerBuilderTool(name, type, parent)
{
}

TileTowerBuilderTool::~TileTowerBuilderTool() {
}


/**
 * @brief Convert calorimeter strings to enums.
 * @param includedCalos Property with calorimeter strings.
 */
std::vector<CaloCell_ID::SUBCALO>
TileTowerBuilderTool::parseCalos 
  (const std::vector<std::string>& includedCalos) const
{
  // convert to enumerators
  std::vector<CaloCell_ID::SUBCALO> indices;

  for (const std::string& s : includedCalos) {
    if (s == "TILE") {
      indices.push_back(CaloCell_ID::TILE);
    }
  }

  return indices;
}
