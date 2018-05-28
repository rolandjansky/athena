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
  /**
   * @brief Convert calorimeter strings to enums.
   * @param includedCalos Property with calorimeter strings.
   */
  virtual std::vector<CaloCell_ID::SUBCALO> parseCalos
    (const std::vector<std::string>& includedCalos) const override;
};
#endif // not TILERECUTILS_TILETOWERBUILDERTOOL_H
