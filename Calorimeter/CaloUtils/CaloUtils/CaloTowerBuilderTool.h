/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOTOWERBUILDERTOOL_H
#define CALOREC_CALOTOWERBUILDERTOOL_H
///////////////////////////////////////////////////////////////////////////////
/// \brief CaloTowerBuilderTool is a tower builder tool implementation class
///
/// CaloTowerBuilderTool is a tower builder tool implementation class.
///
/// \author Peter Loch <loch@physics.arizona.edu>
/// \date April 30, 2004 - first implementation
///
///////////////////////////////////////////////////////////////////////////////

#include "CaloIdentifier/CaloCell_ID.h"

#include "CaloUtils/CaloTowerBuilderToolBase.h"
#include "CaloUtils/CaloTowerStore.h"

#include <string>
#include <vector>

class CaloTower;
class CaloTowerStore;
class CaloTowerContainer;
class StoreGateSvc;
class MsgStream;

class CaloTowerBuilderTool : public CaloTowerBuilderToolBase
{
 public:
  
  /// AlgTool constructor
  CaloTowerBuilderTool(const std::string& name, const std::string& type,
		       const IInterface* parent);
  virtual ~CaloTowerBuilderTool();


  /**
   * @brief Run tower building and add results to the tower container.
   *        If a cell container is provided, use that; otherwise, fetch
   *        from SG (the key is given by a job property).
   *        If @c subseg is provided, then we do the building
   *        only within the rectangular region that it describes.
   *        The segmentation of the tower container must match
   *        the region over which we're running the tower building.
   *
   * @param theContainer The tower container to fill.
   * @param theCell The cell container to read.  If null, we fetch from SG.
   * @param subseg If provided, run tower building only within this window.
   *               The tower container segmentation must match.
   */
  virtual StatusCode execute(CaloTowerContainer* theContainer,
                             const CaloCellContainer* theCell=0,
                             const CaloTowerSeg::SubSeg* subseg = 0);

  virtual void setCalos( const std::vector<CaloCell_ID::SUBCALO>& v);

  virtual StatusCode initializeTool();

  virtual void handle(const Incident&);

 protected:

  ////////////////
  // Properties //
  ////////////////

  std::vector<std::string> m_includedCalos;

  ////////////////////////
  // Store and Services //
  ////////////////////////

  //  unsigned int m_errorCounter;
  std::vector<CaloCell_ID::SUBCALO> m_caloIndices;

  /////////////////////////////
  // Specific Initialization //
  /////////////////////////////

  bool m_firstEvent;
  CaloTowerStore* m_cellStore;

  virtual StatusCode checkSetup(MsgStream& log);
  void addTower (const CaloTowerStore::tower_iterator tower_it,
                 const CaloCellContainer* cells,
                 IProxyDictWithPool* sg,
                 CaloTower* tower);
  void iterateFull (CaloTowerContainer* towers,
                    const CaloCellContainer* cells,
                    IProxyDictWithPool* sg);
  void iterateSubSeg (CaloTowerContainer* towers,
                      const CaloCellContainer* cells,
                      IProxyDictWithPool* sg,
                      const CaloTowerSeg::SubSeg* subseg);

  CaloTowerBuilderTool (const CaloTowerBuilderTool&);
  CaloTowerBuilderTool& operator= (const CaloTowerBuilderTool&);
};
#endif
