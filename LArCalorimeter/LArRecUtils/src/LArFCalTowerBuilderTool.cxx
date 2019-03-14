/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "LArFCalTowerBuilderTool.h"
#include "LArFCalTowerStore.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include "CaloUtils/CaloTowerBuilderToolBase.h"

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloTower.h"
#include "CaloEvent/CaloTowerContainer.h"

#include <string>
#include <cmath>

CaloCell_ID::SUBCALO 
LArFCalTowerBuilderTool::m_caloIndex = CaloCell_ID::LARFCAL;

LArFCalTowerBuilderTool::LArFCalTowerBuilderTool(const std::string& name,
						 const std::string& type,
						 const IInterface* parent)
  : CaloTowerBuilderToolBase(name,type,parent)
    , m_minEt(0.)
    , m_cellIdHelper(nullptr)
    , m_larFCalId(nullptr)
    , m_theManager(nullptr)
{
  // Et cut for minicells
  declareProperty("MinimumEt",m_minEt);
}

LArFCalTowerBuilderTool::~LArFCalTowerBuilderTool(){
}

/////////////////////////////
// Specific Initialization //
/////////////////////////////

StatusCode LArFCalTowerBuilderTool::initializeTool(){
  ATH_CHECK( detStore()->retrieve (m_cellIdHelper, "CaloCell_ID") );
  m_larFCalId = m_cellIdHelper->fcal_idHelper();

  ATH_CHECK( detStore()->retrieve (m_theManager, "CaloMgr") );

  // ignore other input!
  ATH_MSG_INFO( "CaloTowerBuilder for the FCal initiated"  );

  return StatusCode::SUCCESS;
}

///////////////////////////
// Tower Builder Helpers //
//////////////////////////


inline
void
LArFCalTowerBuilderTool::addTower (const tower_iterator& t,
                                   const CaloCellContainer* cells,
                                   CaloTower* tower) const
{
  LArFCalTowerStore::cell_iterator firstC = m_cellStore.firstCellofTower(t);
  LArFCalTowerStore::cell_iterator lastC = m_cellStore.lastCellofTower(t);
  // here get needed size of this vector and use it to reserve tower size.
  int ts=  m_cellStore.towerSize(t);
  double wsumE = tower->getBasicEnergy(); // this is not 0 since some towers already have cells from other calos.
  for (; firstC != lastC; ++firstC) {

    unsigned int ci = firstC->first;
    double weightC = firstC->second;
    int cndx = cells->findIndex(ci);
    const CaloCell* cellPtr = 0;
    if (cndx >= 0)
      cellPtr = (*cells)[cndx];
    // get weights
    if (cellPtr) {
      wsumE += weightC * cellPtr->e();					// Summ up weighted energies .
      tower->addUniqueCellNoKine(cells, cndx, weightC, ts); // add cells to tower.
    }
  }
  tower->setE(wsumE);  // update tower kinematics.
}


inline
void
LArFCalTowerBuilderTool::iterateFull (CaloTowerContainer* towers,
                                      const CaloCellContainer* cells) const
{
  size_t sz = towers->size();
  assert(m_cellStore.size() ==  sz);
  tower_iterator tower_it = m_cellStore.towers();

  for (unsigned int t = 0; t < sz; ++t, ++tower_it) {
    CaloTower* aTower = towers->getTower(t);
    addTower (tower_it, cells, aTower);
  }
}


inline
void
LArFCalTowerBuilderTool::iterateSubSeg (CaloTowerContainer* towers,
                                        const CaloCellContainer* cells,
                                        const CaloTowerSeg::SubSeg* subseg) const
{
  size_t sz = towers->size();
  assert(subseg->size() ==  sz);
  LArFCalTowerStore::tower_subseg_iterator tower_it = m_cellStore.towers(*subseg);

  for (unsigned int t = 0; t < sz; ++t, ++tower_it) {
    CaloTower* aTower = towers->getTower(tower_it.itower());
    addTower (tower_it, cells, aTower);
  }
}


/////////////////////////
// Find Towers in FCal //
/////////////////////////

StatusCode
LArFCalTowerBuilderTool::execute(CaloTowerContainer* theTowers,
                                 const CaloCellContainer* theCells,
                                 const CaloTowerSeg::SubSeg* subseg) const
{
  if (m_cellStore.size() == 0) {
    ATH_MSG_ERROR("Cell store not initialized.");
    return StatusCode::FAILURE;
  }

  // retrieve cells
  if (!theCells) {
    theCells = getCells();
    if (!theCells) {
      return StatusCode::SUCCESS;
    }
  }

  // check if any FCal in container
  if ( theCells->nCellsCalo(m_caloIndex) == 0 ){
    ATH_MSG_DEBUG( "no FCal cells in CaloCellContainer, skip tool!" );
    return StatusCode::SUCCESS;
  }

  /////////////////////////
  // Map Cells on Towers // 
  /////////////////////////
	
  // register this calorimeter
  theTowers->setCalo(m_caloIndex);

  
  if (subseg)
    iterateSubSeg (theTowers, theCells, subseg);
  else
    iterateFull (theTowers, theCells);

  return StatusCode::SUCCESS;
}


/**
 * @brief Run tower building and add results to the tower container.
 * @param theContainer The tower container to fill.
 *
 * If the segmentation hasn't been set, take it from the tower container.
 * This is for use by converters.
 */
StatusCode LArFCalTowerBuilderTool::execute (CaloTowerContainer* theContainer)
{
  if (m_cellStore.size() == 0) {
    setTowerSeg (theContainer->towerseg());
    ATH_CHECK( rebuildLookup() );
  }

  return execute (theContainer, nullptr, nullptr);
}


void  LArFCalTowerBuilderTool::handle(const Incident&) 
{
  ATH_MSG_DEBUG( "In Incident-handle"  );
}


/**
 * @brief Rebuild the cell lookup table.
 */
StatusCode LArFCalTowerBuilderTool::rebuildLookup()
{
  CaloTowerContainer theTowers (towerSeg());
  if ( m_cellStore.buildLookUp(*m_cellIdHelper,
                               *m_theManager,
                               &theTowers) )
  {
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}


/**
 * @brief Mark that cached data are invalid.
 *
 * Called when calibrations are updated.
 */
StatusCode LArFCalTowerBuilderTool::invalidateCache()
{
  // FIXME: We don't currently handle changing alignments during a run.
  //        This could be done if caloDD is updated to the new alignment
  //        scheme.  Otherwise, it's incompatible with MT.
  if (m_cellStore.size() > 0) {
    ATH_MSG_ERROR("Cell store already filled.  FIXME: changing alignments is not handled.");
    return StatusCode::FAILURE;
  }

  ATH_CHECK( rebuildLookup() );
  return StatusCode::SUCCESS;
}

