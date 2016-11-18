/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArFCAL_ID.h"

#include "CaloUtils/CaloTowerBuilderToolBase.h"

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloTower.h"
#include "CaloEvent/CaloTowerContainer.h"

#include "LArRecUtils/LArFCalTowerStore.h"
#include "LArRecUtils/LArFCalTowerBuilderTool.h"

#include <string>
#include <cmath>

CaloCell_ID::SUBCALO 
LArFCalTowerBuilderTool::m_caloIndex = CaloCell_ID::LARFCAL;

LArFCalTowerBuilderTool::LArFCalTowerBuilderTool(const std::string& name,
						 const std::string& type,
						 const IInterface* parent)
  : CaloTowerBuilderToolBase(name,type,parent)
    , m_minEt(0.)
    , m_firstEvent(true)
{
  // Et cut for minicells
  declareProperty("MinimumEt",m_minEt);
  m_larFCalId = (CaloIdManager::instance())->getFCAL_ID();
  // initialize intermediate store
  m_cellStore = new LArFCalTowerStore();
}

LArFCalTowerBuilderTool::~LArFCalTowerBuilderTool(){
  delete m_cellStore; 
}

/////////////////////////////
// Specific Initialization //
/////////////////////////////

StatusCode LArFCalTowerBuilderTool::initializeTool(){
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
                                   CaloTower* tower)
{
  LArFCalTowerStore::cell_iterator firstC = m_cellStore->firstCellofTower(t);
  LArFCalTowerStore::cell_iterator lastC = m_cellStore->lastCellofTower(t);
  // here get needed size of this vector and use it to reserve tower size.
  int ts=  m_cellStore->towerSize(t);
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
                                      const CaloCellContainer* cells)
{
  size_t sz = towers->size();
  assert(m_cellStore->size() ==  sz);
  tower_iterator tower_it = m_cellStore->towers();

  for (unsigned int t = 0; t < sz; ++t, ++tower_it) {
    CaloTower* aTower = towers->getTower(t);
    addTower (tower_it, cells, aTower);
  }
}


inline
void
LArFCalTowerBuilderTool::iterateSubSeg (CaloTowerContainer* towers,
                                        const CaloCellContainer* cells,
                                        const CaloTowerSeg::SubSeg* subseg)
{
  size_t sz = towers->size();
  assert(subseg->size() ==  sz);
  LArFCalTowerStore::tower_subseg_iterator tower_it = m_cellStore->towers(*subseg);

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
                                 const CaloTowerSeg::SubSeg* subseg)
{
  ///////////////////////////
  // Create Cell Fragments //
  ///////////////////////////

  // only once if not done alread in begin run (for converter usage)
  if (!m_cacheValid )
    {
      ATH_MSG_DEBUG( " m_cacheValid false, initializing Fcal lookup in first event "  );
      if ( ! m_cellStore->buildLookUp(theTowers) ){
        ATH_MSG_ERROR( "cannot construct cell fragment lookup, fatal!"  );
        return StatusCode::FAILURE;
      }
      m_cacheValid=true;
    }

  // retrieve cells
  if (!theCells) {
    StatusCode checkOut = m_storeGate->retrieve(theCells,m_cellContainerName);
    if ( checkOut.isFailure() ){
      ATH_MSG_WARNING( "cannot allocate CaloCellContainer with key <"
                       << m_cellContainerName<< ">, skip tool!" );
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

void  LArFCalTowerBuilderTool::handle(const Incident&) 
{
  ATH_MSG_DEBUG( "In Incident-handle"  );
  if (m_cacheValid) {
    ATH_MSG_DEBUG( "Cached data already computed."  );
    return; 
  }
  CaloTowerContainer* theTowers = new CaloTowerContainer(m_theTowerSeg);

  if ( ! m_cellStore->buildLookUp(theTowers) ){
    ATH_MSG_ERROR( "cannot construct cell fragment lookup, fatal!"  );
    m_cacheValid=false;
  }
  else {
    m_cacheValid=true;
  }
  ATH_MSG_DEBUG( " built Fcal tower lookup " << m_cacheValid  );
  delete theTowers;

}
