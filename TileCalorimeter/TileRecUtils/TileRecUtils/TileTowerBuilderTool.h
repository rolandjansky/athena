/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_TILETOWERBUILDERTOOL_H
#define TILERECUTILS_TILETOWERBUILDERTOOL_H
/**
 @class TileTowerBuilderTool
 @brief Concrete tool for CaloTower building in Tile

 TileTowerBuilderTool fills CaloCells into CaloTowers for the Tile
 Calorimeters.
 */

#include "CaloUtils/CaloTowerBuilderTool.h"

#include <string>

class CaloTowerContainer;

class TileTowerBuilderTool: public CaloTowerBuilderTool {
  public:

    /// AlgTool constructor
    TileTowerBuilderTool(const std::string& name, const std::string& type,
        const IInterface* parent);
    virtual ~TileTowerBuilderTool();

  protected:

    virtual StatusCode initializeTool();

  private:
    bool m_dumpTowers;
    bool m_dumpWeightMap;
};
#endif // not TILERECUTILS_TILETOWERBUILDERTOOL_H
