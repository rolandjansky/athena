/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "LArRecUtils/LArFCalTowerStore.h"
#include "CaloUtils/CaloTowerBuilderToolBase.h"

#include "CaloIdentifier/CaloCell_ID.h"

#include <list>

class LArFCalTowerStore;

class CaloTowerContainer;
class CaloTower;

class LArFCAL_ID;

//class LArMiniCellContainer;

class LArFCalTowerBuilderTool : public CaloTowerBuilderToolBase
{
 public:

  /// AlgTool constructor 
  LArFCalTowerBuilderTool(const std::string& name, const std::string& type,
			  const IInterface* parent);

  virtual ~LArFCalTowerBuilderTool();

  virtual StatusCode execute(CaloTowerContainer* theContainer,
                             const CaloCellContainer* theCell=0,
                             const CaloTowerSeg::SubSeg* subseg = 0);
  virtual void handle(const Incident&);

 protected:

  double m_minEt;

  virtual StatusCode initializeTool();

 private:
  typedef LArFCalTowerStore::tower_iterator tower_iterator;

  void addTower (const tower_iterator& t,
                 const CaloCellContainer* cells,
                 CaloTower* tower);
  void iterateFull (CaloTowerContainer* towers,
                    const CaloCellContainer* cells);
  void iterateSubSeg (CaloTowerContainer* towers,
                      const CaloCellContainer* cells,
                      const CaloTowerSeg::SubSeg* subseg);

  bool m_firstEvent;

  // FCal only
  static CaloCell_ID::SUBCALO m_caloIndex;

  const LArFCAL_ID* m_larFCalId;

  LArFCalTowerStore* m_cellStore;

  LArFCalTowerBuilderTool (const LArFCalTowerBuilderTool&);
  LArFCalTowerBuilderTool& operator= (const LArFCalTowerBuilderTool&);
};
#endif
