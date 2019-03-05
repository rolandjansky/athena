/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "CLHEP/Units/SystemOfUnits.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
//#include "GaudiKernel/IChronoStatSvc.h"

#include "StoreGate/StoreGateSvc.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloTower.h"
#include "CaloEvent/CaloTowerContainer.h"
#include "CaloUtils/CaloTowerStore.h"
#include "CaloUtils/CaloTowerBuilderTool.h"

#include <string>
#include <cmath>

CaloTowerBuilderTool::CaloTowerBuilderTool(const std::string& name,
					   const std::string& type,
					   const IInterface* parent)
  : CaloTowerBuilderToolBase(name,type,parent),
    m_caloDDM(nullptr)
    //  , m_errorCounter(0)
{
  declareInterface<ICaloTowerBuilderToolBase>(this);    
  m_includedCalos.resize(4);
  m_includedCalos[0] = "LAREM";
  m_includedCalos[1] = "LARHEC";
  m_includedCalos[2] = "LARFCAL";
  m_includedCalos[3] = "TILE";
  // common properties
  declareProperty("IncludedCalos",m_includedCalos);
}

CaloTowerBuilderTool::~CaloTowerBuilderTool()
{ 
}

/////////////////////////////
// Specific Initialization //
/////////////////////////////

// protected!
StatusCode CaloTowerBuilderTool::initializeTool() {
  ATH_CHECK( detStore()->retrieve (m_caloDDM, "CaloMgr") );
  m_caloIndices = parseCalos (m_includedCalos);
  return this->checkSetup(msg());
}

///////////////////////////
// Tower Builder Helpers //
//////////////////////////


inline
void
CaloTowerBuilderTool::addTower (const CaloTowerStore::tower_iterator tower_it,
                                const CaloCellContainer* cells,
                                IProxyDict* sg,
                                CaloTower* tower) const
{
  CaloTowerStore::cell_iterator firstC = tower_it.firstCell();
  CaloTowerStore::cell_iterator lastC = tower_it.lastCell();
  int ts = tower_it.size();
  double wsumE = tower->getBasicEnergy(); // this is not 0 since some towers already have cells from other calos.
  for (; firstC != lastC; ++firstC) {

    unsigned int ci = firstC.hash();
    double weightC = firstC.weight();
    int cndx = cells->findIndex(ci);
    const CaloCell* cellPtr = 0;
    if (cndx >= 0)
      cellPtr = (*cells)[cndx];
    // get weights
    if (cellPtr) {
      wsumE += weightC * cellPtr->e();					// Summ up weighted energies .
      tower->addUniqueCellNoKine(cells, cndx, sg, weightC, ts); // add cells to tower.
    }
    /* for debugging em+hec
       if (t==5214) std::cout<<"N14\tc:"<<ci<<"\tw:"<<weightC<<"\te:"<<cellPtr->e()<<"\teta:"<<cellPtr->eta()<<"\td:"<<cellPtr->caloDDE()<<std::endl;
       if (t==5279) std::cout<<"N79\tc:"<<ci<<"\tw:"<<weightC<<"\te:"<<cellPtr->e()<<"\teta:"<<cellPtr->eta()<<"\td:"<<cellPtr->caloDDE()<<std::endl;
    */

  }
  tower->setE(wsumE);  // update tower kinematics.
}


inline
void
CaloTowerBuilderTool::iterateFull (CaloTowerContainer* towers,
                                   const CaloCellContainer* cells,
                                   IProxyDict* sg) const
{
  size_t sz = towers->size();
  assert(m_cellStore.size() ==  sz);
  CaloTowerStore::tower_iterator tower_it = m_cellStore.towers();

  for (unsigned int t = 0; t < sz; ++t, ++tower_it) {
    CaloTower* aTower = towers->getTower(t);
    addTower (tower_it, cells, sg, aTower);
  }
}


inline
void
CaloTowerBuilderTool::iterateSubSeg (CaloTowerContainer* towers,
                                     const CaloCellContainer* cells,
                                     IProxyDict* sg,
                                     const CaloTowerSeg::SubSeg* subseg) const
{
  size_t sz = towers->size();
  assert(subseg->size() ==  sz);
  CaloTowerStore::tower_subseg_iterator tower_it = m_cellStore.towers(*subseg);

#if 0
  for (unsigned int t = 0; t < sz; ++t, ++tower_it) {
    CaloTower* aTower = towers->getTower(tower_it.itower());
    addTower (tower_it, cells, aTower);
  }
#endif
  // This loop was originally written as above.  However, if we increment
  // tower_it at the end of the iteration, then it will end up doing
  // an out-of-bounds read (in tower_iterator::operator+=).
  // Best to just avoid incrementing it if we don't need to;
  // that also saves a bit of useless work.
  unsigned int t = 0;
  while (true) {
    CaloTower* aTower = towers->getTower(tower_it.itower());
    addTower (tower_it, cells, sg, aTower);
    ++t;
    if (t >= sz) break;
    ++tower_it;
  }
}


///////////////////
// Tower Builder //
///////////////////


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
StatusCode
CaloTowerBuilderTool::execute(CaloTowerContainer* theTowers,
                              const CaloCellContainer* theCells /*= 0*/,
                              const CaloTowerSeg::SubSeg* subseg /*= 0*/) const
{
  if (m_cellStore.size() == 0) {
    ATH_MSG_ERROR("Cell store not initialized.");
    return StatusCode::FAILURE;
  }

  // CaloCellContainer
  if (!theCells) {
    theCells = getCells();
    if (!theCells) {
      return StatusCode::SUCCESS;
    }
  }

  IProxyDict* sg = SG::CurrentEventStore::store();
  if (subseg)
    iterateSubSeg (theTowers, theCells, sg, subseg);
  else
    iterateFull (theTowers, theCells, sg);

  for (unsigned int i = 0; i < m_caloIndices.size(); i++) {
    theTowers->setCalo(m_caloIndices[i]);
  }
	
  ATH_MSG_DEBUG("Number of Towers in CaloTowerContainer: "<< theTowers->size());

  /*    for debug purposes
  double summ=0;
   std::cout << " Number of Towers in CaloTowerContainer: "<< theTowers->size() << std::endl;
   for ( unsigned int t=0; t<theTowers->size(); ++t ) { // TOWER Loop 
	CaloTower* aTower = theTowers->getTower(t);	// get tower
	double E=aTower->getBasicEnergy(); 
	summ+=E;
	std::cout<<"Tower: "<<t<<"\te:"<<E<<std::endl;
	}
	std::cout<<"================ SUMM\t"<<summ<<std::endl;
   */
		  
  return StatusCode::SUCCESS;
}


/**
 * @brief Run tower building and add results to the tower container.
 * @param theContainer The tower container to fill.
 *
 * If the segmentation hasn't been set, take it from the tower container.
 * This is for use by converters.
 */
StatusCode CaloTowerBuilderTool::execute (CaloTowerContainer* theContainer)
{
  if (m_cellStore.size() == 0) {
    setTowerSeg (theContainer->towerseg());
    ATH_CHECK( rebuildLookup() );
  }

  return execute (theContainer, nullptr, nullptr);
}


//////////////////////
// Internal Helpers //
//////////////////////

// protected
StatusCode CaloTowerBuilderTool::checkSetup(MsgStream& /*log*/) {
  // any calos registered
  if (m_caloIndices.size() == 0) {
    ATH_MSG_ERROR("no match in requested calorimeter ranges ("
        << m_includedCalos.size() << " requested)");
    // print out requested keys
    for (unsigned int iCalos = 0; iCalos < m_includedCalos.size(); iCalos++) {
      if (iCalos == 0) {
        msg(MSG::ERROR) << "requested key(s): " << m_includedCalos[iCalos];
      } else if (iCalos == m_includedCalos.size() - 1) {
        ATH_MSG_ERROR("," << m_includedCalos[iCalos] << " all not known!");
      } else {
        msg(MSG::ERROR) << "," << m_includedCalos[iCalos];
      }
    }
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

void CaloTowerBuilderTool::setCalos(const std::vector<CaloCell_ID::SUBCALO>& v)
{
  if (m_caloIndices != v) {
    if (m_cellStore.size() > 0) {
      if (rebuildLookup().isFailure()) {
        ATH_MSG_ERROR("rebuildLookup failed.");
      }
    }
    m_caloIndices = v;
  }
}

void CaloTowerBuilderTool::handle(const Incident&) {
  ATH_MSG_DEBUG("In Incident-handle");
}


/**
 * @brief Convert calorimeter strings to enums.
 * @param includedCalos Property with calorimeter strings.
 */
std::vector<CaloCell_ID::SUBCALO>
CaloTowerBuilderTool::parseCalos 
  (const std::vector<std::string>& includedCalos) const
{
  // convert to enumerators
  std::vector<CaloCell_ID::SUBCALO> indices;

  for (const std::string& s : includedCalos) {
    if (s == "LAREM") {
      indices.push_back(CaloCell_ID::LAREM);
    } else if (s == "LARHEC") {
      indices.push_back(CaloCell_ID::LARHEC);
    } else if (s == "LARFCAL") {
      indices.push_back(CaloCell_ID::LARFCAL);
    } else if (s == "TILE") {
      indices.push_back(CaloCell_ID::TILE);
    }
  }

  return indices;
}


/**
 * @brief Rebuild the cell lookup table.
 */
StatusCode CaloTowerBuilderTool::rebuildLookup()
{
  if (towerSeg().neta() != 0 && towerSeg().nphi() != 0) {
    if (m_cellStore.buildLookUp(*m_caloDDM, towerSeg(), m_caloIndices)) {
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}


/**
 * @brief Mark that cached data are invalid.
 *
 * Called when calibrations are updated.
 */
StatusCode CaloTowerBuilderTool::invalidateCache()
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

