/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
const LArFCalTowerBuilderTool::m_caloIndex = CaloCell_ID::LARFCAL;

LArFCalTowerBuilderTool::LArFCalTowerBuilderTool(const std::string& name,
						 const std::string& type,
						 const IInterface* parent)
  : CaloTowerBuilderToolBase(name,type,parent)
    , m_minEt(0.)
    , m_cellIdHelper(nullptr)
    , m_larFCalId(nullptr)
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
                                   const ElementLink<CaloCellContainer>& cellsEL,
                                   CaloTower* tower) const
{
  LArFCalTowerStore::cell_iterator firstC = m_cellStore.firstCellofTower(t);
  LArFCalTowerStore::cell_iterator lastC = m_cellStore.lastCellofTower(t);
  // here get needed size of this vector and use it to reserve tower size.
  int ts=  m_cellStore.towerSize(t);
  double wsumE = tower->getBasicEnergy(); // this is not 0 since some towers already have cells from other calos.
  const CaloCellContainer* cells = cellsEL.getDataPtr();
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
      tower->addUniqueCellNoKine(cellsEL, cndx, weightC, ts); // add cells to tower.
    }
  }
  tower->setE(wsumE);  // update tower kinematics.
}


inline
void
LArFCalTowerBuilderTool::iterateFull (CaloTowerContainer* towers,
                                      const ElementLink<CaloCellContainer>& cellsEL) const
{
  size_t sz = towers->size();
  assert(m_cellStore.size() ==  sz);
  tower_iterator tower_it = m_cellStore.towers();

  for (unsigned int t = 0; t < sz; ++t, ++tower_it) {
    CaloTower* aTower = towers->getTower(t);
    addTower (tower_it, cellsEL, aTower);
  }
}


inline
void
LArFCalTowerBuilderTool::iterateSubSeg (CaloTowerContainer* towers,
                                        const ElementLink<CaloCellContainer>& cellsEL,
                                        const CaloTowerSeg::SubSeg* subseg) const
{
  size_t sz = towers->size();
  assert(subseg->size() ==  sz);
  LArFCalTowerStore::tower_subseg_iterator tower_it = m_cellStore.towers(*subseg);

  for (unsigned int t = 0; t < sz; ++t, ++tower_it) {
    CaloTower* aTower = towers->getTower(tower_it.itower());
    addTower (tower_it, cellsEL, aTower);
  }
}


/////////////////////////
// Find Towers in FCal //
/////////////////////////

StatusCode
LArFCalTowerBuilderTool::execute(const EventContext& ctx,
                                 CaloTowerContainer* theTowers,
                                 const CaloCellContainer* theCells,
                                 const CaloTowerSeg::SubSeg* subseg) const
{

  //Init internal structure m_cellStore on first invocation
  //Alignment updates are not taken into account! 
  if (m_cellStoreInit.load() == false) {
    //Aquire mutex before writing to m_cellStore
    std::scoped_lock guard(m_cellStoreMutex);
    //cast alway const-ness, acceptable since this is protected by a mutex
    LArFCalTowerBuilderTool* thisNC ATLAS_THREAD_SAFE = const_cast<LArFCalTowerBuilderTool*>(this);
    ATH_CHECK( thisNC->rebuildLookup(ctx) );
    m_cellStoreInit.store(true);
  }


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

  const ElementLink<CaloCellContainer> cellsEL (*theCells, 0, ctx);
  if (subseg)
    iterateSubSeg (theTowers, cellsEL, subseg);
  else
    iterateFull (theTowers, cellsEL);

  return StatusCode::SUCCESS;
}


/**
 * @brief Run tower building and add results to the tower container.
 * @param ctx The current event context.
 * @param theContainer The tower container to fill.
 *
 * If the segmentation hasn't been set, take it from the tower container.
 * This is for use by converters.
 */
StatusCode LArFCalTowerBuilderTool::execute (const EventContext& ctx,
                                             CaloTowerContainer* theContainer)
{
  if (m_cellStore.size() == 0) {
    setTowerSeg (theContainer->towerseg());
    ATH_CHECK( rebuildLookup(ctx) );
  }

  return execute (ctx, theContainer, nullptr, nullptr);
}



/**
 * @brief Rebuild the cell lookup table.
 */
StatusCode LArFCalTowerBuilderTool::rebuildLookup(const EventContext& ctx)
{
  ATH_MSG_DEBUG("Building lookup table");
  SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle(m_caloMgrKey,ctx);
  const CaloDetDescrManager* theManager = *caloMgrHandle;
  CaloTowerContainer theTowers (towerSeg());
  if ( m_cellStore.buildLookUp(*m_cellIdHelper,
                               *theManager,
                               &theTowers) )
  {
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}



