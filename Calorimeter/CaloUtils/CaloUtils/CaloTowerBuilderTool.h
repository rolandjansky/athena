/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
class CaloDetDescrManager;
class StoreGateSvc;
class MsgStream;

class CaloTowerBuilderTool : public CaloTowerBuilderToolBase
{
public:
  CaloTowerBuilderTool (const CaloTowerBuilderTool&) = delete;
  CaloTowerBuilderTool& operator= (const CaloTowerBuilderTool&) = delete;
  
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
                             const CaloTowerSeg::SubSeg* subseg = 0) const override;


  /**
   * @brief Run tower building and add results to the tower container.
   * @param theContainer The tower container to fill.
   *
   * If the segmentation hasn't been set, take it from the tower container.
   * This is for use by converters.
   */
  virtual StatusCode execute (CaloTowerContainer* theContainer) override;


  virtual void setCalos( const std::vector<CaloCell_ID::SUBCALO>& v);

  virtual StatusCode initializeTool() override;

  virtual void handle(const Incident&) override;


protected:
  /**
   * @brief Convert calorimeter strings to enums.
   * @param includedCalos Property with calorimeter strings.
   */
  virtual std::vector<CaloCell_ID::SUBCALO> parseCalos
    (const std::vector<std::string>& includedCalos) const;


private:

  ////////////////
  // Properties //
  ////////////////

  std::vector<std::string> m_includedCalos;

  ////////////////////////
  // Store and Services //
  ////////////////////////

  std::vector<CaloCell_ID::SUBCALO> m_caloIndices;

  /////////////////////////////
  // Specific Initialization //
  /////////////////////////////

  const CaloDetDescrManager* m_caloDDM;
  CaloTowerStore m_cellStore;

  virtual StatusCode checkSetup(MsgStream& log);
  void addTower (const CaloTowerStore::tower_iterator tower_it,
                 const CaloCellContainer* cells,
                 IProxyDict* sg,
                 CaloTower* tower) const;
  void iterateFull (CaloTowerContainer* towers,
                    const CaloCellContainer* cells,
                    IProxyDict* sg) const;
  void iterateSubSeg (CaloTowerContainer* towers,
                      const CaloCellContainer* cells,
                      IProxyDict* sg,
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
};
#endif
