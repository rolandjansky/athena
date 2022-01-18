/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

class LArFCalTowerBuilderTool : public CaloTowerBuilderToolBase
{
 public:
  LArFCalTowerBuilderTool (const LArFCalTowerBuilderTool&) = delete;
  LArFCalTowerBuilderTool& operator= (const LArFCalTowerBuilderTool&) = delete;

  /// AlgTool constructor 
  LArFCalTowerBuilderTool(const std::string& name, const std::string& type,
			  const IInterface* parent);

  virtual ~LArFCalTowerBuilderTool();

  virtual StatusCode execute(const EventContext& ctx,
                             CaloTowerContainer* theContainer,
                             const CaloCellContainer* theCell=0,
                             const CaloTowerSeg::SubSeg* subseg = 0) const override;


  /**
   * @brief Run tower building and add results to the tower container.
   * @param ctx The current event context.
   * @param theContainer The tower container to fill.
   *
   * If the segmentation hasn't been set, take it from the tower container.
   * This is for use by converters.
   */
  virtual StatusCode execute (const EventContext& ctx,
                              CaloTowerContainer* theContainer) override;


private:

  double m_minEt;

  virtual StatusCode initializeTool() override;

  typedef LArFCalTowerStore::tower_iterator tower_iterator;

  void addTower (const tower_iterator& t,
                 const ElementLink<CaloCellContainer>& cellsEL,
                 CaloTower* tower) const;
  void iterateFull (CaloTowerContainer* towers,
                    const ElementLink<CaloCellContainer>& cellsEL) const;
  void iterateSubSeg (CaloTowerContainer* towers,
                      const ElementLink<CaloCellContainer>& cellsEL,
                      const CaloTowerSeg::SubSeg* subseg) const;


  /**
   * @brief Rebuild the cell lookup table.
   */
  StatusCode rebuildLookup(const EventContext& ctx);


  // FCal only
  static const CaloCell_ID::SUBCALO m_caloIndex;

  const CaloCell_ID* m_cellIdHelper;
  const LArFCAL_ID* m_larFCalId;

  LArFCalTowerStore m_cellStore;
  mutable std::mutex m_cellStoreMutex;
  mutable std::atomic_bool m_cellStoreInit{false};

};
#endif
