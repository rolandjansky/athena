/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCLUSTERREC_LARTOWERBUILDERTOOL_H
#define LARCLUSTERREC_LARTOWERBUILDERTOOL_H
/**
@class LArTowerBuilderTool
@brief Concrete tool for CaloTower building in LAr

LArTowerBuilderTool fills CaloCells into CaloTowers for the LAr 
Calorimeters. Note that this is not the appropriate tool for the FCal,
which is by default excluded.
\author Peter Loch <loch@physics.arizona.edu>
\date   April 30, 2004 - first implementation
*/


#include "CaloUtils/CaloTowerBuilderTool.h"

#include <string>

class CaloTowerContainer;

class LArTowerBuilderTool : public CaloTowerBuilderTool
{
 public:

  /// AlgTool constructor
  LArTowerBuilderTool(const std::string& name, const std::string& type,
		      const IInterface* parent);
  virtual ~LArTowerBuilderTool();

 protected:
  /**
   * @brief Convert calorimeter strings to enums.
   * @param includedCalos Property with calorimeter strings.
   */
  virtual std::vector<CaloCell_ID::SUBCALO> parseCalos
    (const std::vector<std::string>& includedCalos) const override;
};
#endif
