/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCLUSTERREC_LARFCALTOWERBUILDERTOOL_H
#define LARCLUSTERREC_LARFCALTOWERBUILDERTOOL_H
/**
@class LArFCalTowerBuilderTool
\brief special tower builder for FCal

 LArFCalTowerBuilderTool is FCal specific. It first produces "mini-cells",
 which are FCal cell fragments defining the overlap between the (eta,phi)
 tower grid and the rectangular FCal cell. These mini-cells are then
 collected onto the tower grid.

 \author Peter Loch <loch@phsyics.arizona.edu>
 \date April 30, 2004 - first implementation

*/

#include "LArFCalTowerStore.h"
#include "CaloUtils/CaloTowerBuilderToolBase.h"

#include "CaloIdentifier/CaloCell_ID.h"


class LArFCalTowerStore;

class CaloTowerContainer;
class CaloTower;
class CaloDetDescrManager;

class LArFCAL_ID;

//class LArMiniCellContainer;

class LArFCalTowerBuilderTool : public CaloTowerBuilderToolBase
{
 public:
  LArFCalTowerBuilderTool (const LArFCalTowerBuilderTool&) = delete;
  LArFCalTowerBuilderTool& operator= (const LArFCalTowerBuilderTool&) = delete;

  /// AlgTool constructor 
  LArFCalTowerBuilderTool(const std::string& name, const std::string& type,
			  const IInterface* parent);

  virtual ~LArFCalTowerBuilderTool();

  virtual StatusCode execute(CaloTowerContainer* theContainer,
                             const CaloCellContainer* theCell=0,
                             const CaloTowerSeg::SubSeg* subseg = 0) const override;


  /**
   * @brief Run tower building and add results to the tower container.
   * @param theContainer The tower container to fill.
   *
   * If the segmentation hasn't been set, take it from the tower container.
   * This is for use by converters.
   */
  virtual StatusCode execute (CaloTowerContainer* theContainer) override;


  virtual void handle(const Incident&) override;

private:

  double m_minEt;

  virtual StatusCode initializeTool() override;

  typedef LArFCalTowerStore::tower_iterator tower_iterator;

  void addTower (const tower_iterator& t,
                 const CaloCellContainer* cells,
                 CaloTower* tower) const;
  void iterateFull (CaloTowerContainer* towers,
                    const CaloCellContainer* cells) const;
  void iterateSubSeg (CaloTowerContainer* towers,
                      const CaloCellContainer* cells,
                      const CaloTowerSeg::SubSeg* subseg) const;


  /**
   * @brief Rebuild the cell lookup table.
   */
  StatusCode rebuildLookup();


  /**
   * @brief Mark that cached data are invalid.
   *
   * Called when calibrations are updated.
   */
  virtual StatusCode invalidateCache() override;

  // FCal only
  static CaloCell_ID::SUBCALO m_caloIndex;

  const CaloCell_ID* m_cellIdHelper;
  const LArFCAL_ID* m_larFCalId;
  const CaloDetDescrManager* m_theManager;

  LArFCalTowerStore m_cellStore;
};
#endif
