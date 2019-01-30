/* Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration */
#include "AthenaKernel/Units.h"

#include "CaloRec/CaloTopoClusterFromTowerMaker.h"
#include "CaloRec/CaloTopoClusterFromTowerHelpers.h"

#include "xAODCaloEvent/CaloClusterKineHelper.h"

#include "CaloEvent/CaloClusterCellLink.h"
#include "CaloEvent/CaloCellClusterWeights.h"

#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloGeoHelpers/proxim.h"

#include "CaloRec/CaloProtoCluster.h"
#include "CaloRec/CaloTowerGeometrySvc.h"

#include <algorithm>

#include <cstdio>
#include <cstdarg>
#include <string>
#include <cmath>
#include <memory>
#include <vector>

///////////////////////////////////
// CaloTopoClusterFromTowerMaker //
///////////////////////////////////

double      CaloTopoClusterFromTowerMaker::m_energyThresholdDef = -100000000.; // in MeV
std::string CaloTopoClusterFromTowerMaker::m_defaultKey         = "NONE";

CaloTopoClusterFromTowerMaker::CaloTopoClusterFromTowerMaker(const std::string& type,
							     const std::string& name,
							     const IInterface* pParent)
  : AthAlgTool(type,name,pParent)
  , m_towerGeometrySvc("CaloTowerGeometrySvc",name)
  , m_clusterContainerKey("CaloTopoCluster")
  , m_cellContainerKey("AllCalo")
  , m_cellClusterWeightKey("CaloTopoClusterCellWeights")
  , m_orderByPt(false)
  , m_prepareLCW(false)
  , m_useCellsFromClusters(true)
  , m_applyCellEnergyThreshold(false)
  , m_energyThreshold(m_energyThresholdDef-1.)
  , m_numberOfCells(0)
  , m_numberOfSamplings((std::size_t)CaloSampling::Unknown)
  , m_numberOfTowers(0)
{
  declareInterface<CaloClusterCollectionProcessor>(this);
  declareProperty("CaloTowerGeometrySvc",        m_towerGeometrySvc,         "Service providing tower geometry");
  declareProperty("CaloCellContainerKey",        m_cellContainerKey,         "SG Key for CaloCellContainer (input)");
  declareProperty("CaloTopoClusterContainerKey", m_clusterContainerKey,      "SG Key for CaloClusterContainer (input)");
  declareProperty("CellClusterWeightKey",        m_cellClusterWeightKey,     "SG Key for CellClusterWeights (output)");
  declareProperty("OrderClusterByPt",            m_orderByPt,                "Turn on/off pT-ordering of CaloClusterContainer (output)");
  declareProperty("ApplyCellEnergyThreshold",    m_applyCellEnergyThreshold, "Turn on/off cell energy thresholds");
  declareProperty("CellEnergyThreshold",         m_energyThreshold,          "Energy threshold for cells filled in clusters");
  declareProperty("PrepareLCW",                  m_prepareLCW,               "Prepare data structure to apply LCW");
}

StatusCode CaloTopoClusterFromTowerMaker::initialize()
{
  //---------------------//
  // Check configuration //
  //---------------------//

  // tower geometry service
  ATH_MSG_INFO("Allocate tower geometry service:");
  if ( !m_towerGeometrySvc.isValid() ) { 
    ATH_MSG_ERROR("[reject] cannot allocate tower geometry service - fatal");
    return StatusCode::FAILURE; 
  } else {
    ATH_MSG_INFO( formatStr("Tower geometry service is allocated, describes %6zu towers in grid:", m_towerGeometrySvc->towerBins()) );
    ATH_MSG_INFO( formatStr("[accept] %3zu eta bins in [%5.2f,%5.2f]",m_towerGeometrySvc->etaBins(),m_towerGeometrySvc->etaMin(),m_towerGeometrySvc->etaMax()) );
    ATH_MSG_INFO( formatStr("[accept] %3zu phi bins in [%5.2f,%5.2f]",m_towerGeometrySvc->phiBins(),m_towerGeometrySvc->phiMin(),m_towerGeometrySvc->phiMax()) );
  }

  // tower builder configurations
  if ( m_useCellsFromClusters ) {
    // topo-tower
    ATH_MSG_INFO("Configure for building topo-towers (filtered mode):");
    // energy threshold not (yet) implemented for topo-towers
    if ( m_applyCellEnergyThreshold ) {
      ATH_MSG_WARNING("[ignore] cannot apply energy thresholds to topo-towers!");
      m_applyCellEnergyThreshold = false;
    }
    // check on request for LCW
    if ( m_prepareLCW ) { 
      ATH_MSG_INFO("[accept] prepare for LCW calibration - create CaloCellClusterWeights object with key <" << m_cellClusterWeightKey << ">");
    } else {
      ATH_MSG_INFO("[accept] use EM scale");
    }
  } else { 
    // inclusive/exclusive towers
    ATH_MSG_INFO("Configure for building cell towers:");
    if ( m_applyCellEnergyThreshold ) { 
      ATH_MSG_INFO("[accept] configure exclusive towers: use cell energy threshold");
      if ( m_energyThreshold < m_energyThresholdDef ) { 
	ATH_MSG_ERROR("######## [reject] invalid cell energy threshold " << m_energyThreshold/Athena::Units::GeV 
		      << " GeV is smaller than default (no-op) " << m_energyThresholdDef/Athena::Units::GeV << " GeV - fatal");
	return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("######## [accept] energy threshold for cells to contribute to towers is " << m_energyThreshold/Athena::Units::GeV << " GeV");
    } else {
      ATH_MSG_INFO("[accept] configure inclusive towers");
    } // end inclusive/exclusive tower configuration
  } // end tower builder configuration

  // local data (constant parameters)
  m_numberOfCells  = m_towerGeometrySvc->maxCellHash();
  m_numberOfTowers = m_towerGeometrySvc->towerBins();
  ATH_MSG_INFO("Additional tool parameters:");
  if ( m_numberOfCells > 0 ) { 
    ATH_MSG_INFO("[accept] maximum cell hash index is " << m_numberOfCells);
  } else { 
    ATH_MSG_ERROR("[reject] invalid maximum cell hash index " << m_numberOfCells << " - fatal");
    return StatusCode::FAILURE;
  }
  if ( m_numberOfTowers > 0 ) { 
    ATH_MSG_INFO("[accept] maximum number of towers is " << m_numberOfTowers);
  } else {
    ATH_MSG_ERROR("[reject] invalid maximum number of towers " << m_numberOfTowers << " - fatal");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode CaloTopoClusterFromTowerMaker::finalize()
{ return StatusCode::SUCCESS; }

StatusCode CaloTopoClusterFromTowerMaker::execute(xAOD::CaloClusterContainer* pClusCont)
{
  ////////////////////////////
  // Need CaloCellContainer //
  ////////////////////////////

  SG::ReadHandle<CaloCellContainer> pCellCont(m_cellContainerKey);
  if ( !pCellCont.isValid() ) { 
    ATH_MSG_ERROR("Cannot allocate CaloCellContainer with key <" << m_cellContainerKey << ">");
    return StatusCode::FAILURE;
  }

  //////////////////////////////////////////////////////////////////////////////////////////
  // Tricky bit:                                                                          //
  // Especially for fine tower grids some towers may have 0 energy - in particular if the //
  // grid spans -5.0 < eta < 5.0. The proto-cluster container has the same size as the    //
  // tower container, so it will have empty proto-clusters. These are removed when the    //
  // proto-clusters are converted to full CaloCluster objects. The tower center in        //
  // (eta,phi) is preserved in eta0(), phi0() of the CaloCluster. The cluster container   //
  // index has no relation to the tower position anymore.                                 //
  ////////////////////////////////////////////////////////////////////////////////////////// 

  /////////////////////////
  // Set up ProtoCluster //
  /////////////////////////

  protocont_t pProtoCont(m_numberOfTowers,(CaloProtoCluster*)0);
  for ( auto itow(0); itow<m_numberOfTowers; ++itow) { pProtoCont[itow] = new CaloProtoCluster(pCellCont.cptr()); }

  //////////////////////////////////////////////////////
  // Apply overall cell filter and fill protoclusters //
  //////////////////////////////////////////////////////

  // The weights extracted for cells from clusters are LCW weights (typically). The total
  // contribution of a LCW-weighted cell to towers is Ecell*Weight_LCW*Weight_tower.

  // If EM clusters are used, the weights of a clustered cell are completely defined
  // by the tower grid. As cells can shared between clusters, each cell can only be
  // projected onto the towergrid once, with Ecell*Weight_tower

  // The CaloCellClusterWeights object is used to store the combined LCW weight.
  // for each clustered cell. In case of EM, the LCW weights are ignored and this
  // object is not used - a simple vector<bool> tags cells already put into towers.

  if ( m_useCellsFromClusters ) {
    // retrieve topo-cluster container for topo-towers
    SG::ReadHandle<xAOD::CaloClusterContainer> pTopoClusCont(m_clusterContainerKey);
    if ( !pTopoClusCont.isValid() ) {
      ATH_MSG_ERROR("Cannot allocate xAOD::CaloClusterContainer with key <" << m_clusterContainerKey << ">");
      return StatusCode::FAILURE;
    } // check on ReadHandle validity
    // mode-dependent processing
    if ( !m_prepareLCW ) { 
      if ( !this->buildEMTopoTowers(*pTopoClusCont,pProtoCont) ) { 
	ATH_MSG_WARNING("Problem building CaloProtoCluster for EM topo-towers");
	return StatusCode::SUCCESS; 
      }
    } else { 
      if ( !this->buildLCWTopoTowers(*pTopoClusCont,pProtoCont) ) { 
	ATH_MSG_WARNING("Problem building CaloProtoCluster for LCW topo-towers");
	return StatusCode::SUCCESS;
      }
    }
  } else {
    // fill inclusive/exclusive towers 
    if ( !m_applyCellEnergyThreshold ) { buildInclTowers(*pCellCont,pProtoCont); } else { buildExclTowers(*pCellCont,pProtoCont); }
  } // end mode dependent processing

  // common tasks: convert CaloProtoCluster to CaloCluster
  pClusCont->reserve(pProtoCont.size()); // reserve space
  // pick up cluster size tag and set up counter
  xAOD::CaloCluster::ClusterSize csize = this->getClusterSize(m_numberOfTowers);
  int ictr(0);
  // loop proto-clusters
  for ( std::size_t ipc(0); ipc<pProtoCont.size(); ++ipc ) {
    CaloProtoCluster* pProto = pProtoCont.at(ipc);            // pick up pointer to proto-cluster
    if ( pProto != 0 ) {                                      // this should never be the case (FIXME: needed?)
      CaloClusterCellLink* lptr = pProto->releaseCellLinks(); // take over CaloClusterCellLink object
      if ( lptr->size() > 0 ) {                               // remove empty proto-clusters (no cells assigned)
	xAOD::CaloCluster* clptr = new xAOD::CaloCluster();   // new empty cluster
	pClusCont->push_back(clptr);                          // put into container
	clptr->addCellLink(lptr);                             // transfer cell links to CaloCluster
	clptr->setClusterSize(csize);                         // set the cluster size spec
	CaloRec::Helpers::calculateKine(clptr,false);         // calculate kinematics and other signals from cells
	clptr->setEta0(m_towerGeometrySvc->towerEta(ipc));    // save the tower center eta
	clptr->setPhi0(m_towerGeometrySvc->towerPhi(ipc));    // save the tower center phi
	++ictr;                                               // counts the number of CaloCluster produced in the end
      } // end tower proto-cluster has cells
      delete pProto;                                          // delete the proto-cluster object
    } // valid proto-cluster
  } // proto-cluster loop

  pProtoCont.clear();  // clean up (objects in this container have already been destroyed)

  // common tasks: check clusters
  if ( this->msgLvl(MSG::DEBUG) ) {  
    std::size_t idc(0);
    for ( auto pClus : *pClusCont ) { 
      if ( pClus->getCellLinks() == 0 || pClus->getCellLinks()->size() == 0 ) {
	ATH_MSG_WARNING( formatStr("Cluster at index #%05i has invalid pointer (%p) or no cells",idc,(void*)pClus) );
      }
      ++idc;
    } // end cluster loop
  } // end debug mode 

  // common tasks: sort clusters if requested - removes link between index and tower geometry for good!
  if ( m_orderByPt ) { 
    std::sort(pClusCont->begin(),pClusCont->end(),[](xAOD::CaloCluster* pc1,xAOD::CaloCluster* pc2) {
   	volatile double pt1(pc1->pt()); // FIXME needed? (this was just copied)
   	volatile double pt2(pc2->pt()); // FIXME needed? (this was just copied)
   	return ( pt1 > pt2 );  
      }
      );
  }

  return StatusCode::SUCCESS;
} // end execute

//////////////////////
// Fill topo-towers //
//////////////////////

// EM 
bool CaloTopoClusterFromTowerMaker::buildEMTopoTowers(const xAOD::CaloClusterContainer& pClusCont,protocont_t& pProtoCont)
{
  // -- EM scale clusters
  std::vector<bool> cellTags(m_numberOfCells,false); std::size_t nctr(0);
  for ( auto pClus : pClusCont ) { 
    for ( auto fCell(pClus->cell_begin()); fCell != pClus->cell_end(); ++fCell ) { 
      std::size_t cidx((std::size_t)(*fCell)->caloDDE()->calo_hash());
      if ( cidx < cellTags.size() ) {
	if ( !cellTags.at(cidx) ) { cellTags[cidx] = this->addCellToProtoCluster(*fCell,pProtoCont); ++nctr; }  
      } else {
	ATH_MSG_WARNING( formatStr("Invalid cell hash index %6zu >= maximum index %6zu for cell in %s at (eta,phi) = (%6.3,%f6.3)",
				   cidx,cellTags.size(),
				   CaloSampling::getSamplingName((*fCell)->caloDDE()->getSampling()).c_str(),(*fCell)->eta(),(*fCell)->phi()) );
      }
    } // end cells-in-cluster loop
  } // end cluster loop
  //
  return nctr > 0;
}

// LCW
bool CaloTopoClusterFromTowerMaker::buildLCWTopoTowers(const xAOD::CaloClusterContainer& pClusCont,protocont_t& pProtoCont)
{ 
  // -- LCW scale clusters
  SG::WriteHandle<CaloCellClusterWeights> cellClusterWeights(m_cellClusterWeightKey);
  if ( !cellClusterWeights.isValid() ) { 
    ATH_MSG_ERROR("Cannot create write handle for CaloCellClusterWeights with key <" << m_cellClusterWeightKey << ">");
    return StatusCode::FAILURE;
  } // end cannot establish write handle!
  std::size_t nctr(0);
  for ( auto pClus : pClusCont ) { 
    for ( auto fCell(pClus->cell_begin()); fCell != pClus->cell_end(); ++fCell ) {
      if ( !cellClusterWeights->check(*fCell) ) { this->addCellToProtoCluster(*fCell,pProtoCont); ++nctr; }
      cellClusterWeights->set(*fCell,fCell.weight());
    } // end cells-in-cluster loop
  } // end cluster loop
  //
  return nctr > 0;
}

/////////////////
// Fill towers //
/////////////////

// inclusive
bool CaloTopoClusterFromTowerMaker::buildInclTowers(const CaloCellContainer& pCellCont,protocont_t& pProtoCont)
{
  // loop cell container - counter icl replaces cell hash index for NULL pointers in cell container
  std::size_t icl(0); std::size_t nctr(0);
  for ( auto cptr : pCellCont ) { 
    if ( cptr == 0 ) { 
      ATH_MSG_WARNING( formatStr("CaloCellContainer[%6zu] contains invalid cell object pointer %p",icl,(void*)cptr) );  
    } else {
      // existing cell with non-zero energy
      if ( std::fabs(cptr->e()) > 0. ) { this->addCellToProtoCluster(cptr,pProtoCont); ++nctr; }
    } // end pointer check 
    ++icl;
  } // end cell loop
  return nctr > 0;
}

// exclusive
bool CaloTopoClusterFromTowerMaker::buildExclTowers(const CaloCellContainer& pCellCont,protocont_t& pProtoCont)
{
  // loop cell container
  std::size_t icl(0); std::size_t nctr(0);
  for ( auto cptr : pCellCont ) {
    if ( cptr == 0 ) { 
      // FIXME check pointer value
      ATH_MSG_WARNING( formatStr("CaloCellContainer[%6zu] contains invalid cell object pointer %p",icl,(void*)cptr) );
    } else {
      // existing cell with energy above threshold
      if ( cptr->e() > m_energyThreshold ) { this->addCellToProtoCluster(cptr,pProtoCont); ++nctr; }
    } // end pointer check
    ++icl;
  } // end cell loop
  return nctr > 0;
}

bool CaloTopoClusterFromTowerMaker::addCellToProtoCluster(const CaloCell* cptr,protocont_t& pProtoCont) 
{
  // invalid input
  if ( cptr == 0 ) { return false; }

  // get towers for cell from geometry service
  std::size_t nctr(0);
  for ( auto elm : m_towerGeometrySvc->getTowers(cptr) ) { 
    auto towerIdx(m_towerGeometrySvc->towerIndex(elm));
    if ( !m_towerGeometrySvc->isInvalidIndex(towerIdx) ) { pProtoCont[towerIdx]->addCell(static_cast<std::size_t>(cptr->caloDDE()->calo_hash()),m_towerGeometrySvc->cellWeight(elm)); ++nctr; }
  }
  return nctr > 0;
}

/////////////
// Helpers //
/////////////

xAOD::CaloCluster::ClusterSize CaloTopoClusterFromTowerMaker::getClusterSize(std::size_t etaBins,std::size_t phiBins)
{ return this->getClusterSize(etaBins*phiBins); }

xAOD::CaloCluster::ClusterSize CaloTopoClusterFromTowerMaker::getClusterSize(std::size_t nTowers)
{
  // check for tower sizes
  return nTowers == 6400                    // known "standard" towers 0,1 x 0.1
    ? xAOD::CaloCluster::Tower_01_01 
    : nTowers == 25600                      // known "fine" towers 0.05 x 0.05
    ? xAOD::CaloCluster::Tower_005_005 
    : xAOD::CaloCluster::Tower_fixed_area;  // unspecified towers 
}

int CaloTopoClusterFromTowerMaker::cleanupCells(CaloClusterCellLink* clk)
{
  // Any pathology here probably indicates a configuration problem with the conditions (geometry)
  // database (wrong tag for data?)

  // check on null pointers in cell links
  int nrc(0); int hid(0);
  auto fcell(clk->begin());
  while ( fcell != clk->end() ) {
    const CaloCell* pCell = *fcell;
    ATH_MSG_DEBUG( formatStr("CaloCell* = %p in CaloClusterCellLink* = %p") );
    if ( pCell == 0 ) {
      ATH_MSG_WARNING( formatStr("CaloCellContainer[%6i] - removed from tower - cell pointer invalid (%p)",hid,(void*)pCell) );
      fcell = clk->removeCell(fcell); ++nrc;
    } else {
      if ( pCell->caloDDE()->calo_hash() >= m_numberOfCells || pCell->caloDDE()->getSampling() >= m_numberOfSamplings ) {
	ATH_MSG_WARNING( formatStr("CaloCellContainer[%6i] - removed from tower - cell hash index (%6zu/%6zu) or sampling id (%3zu/%3zu) invalid",
				   hid,pCell->caloDDE()->calo_hash(),m_numberOfCells,pCell->caloDDE()->getSampling(),m_numberOfSamplings) );
	fcell = clk->removeCell(fcell); ++nrc; 
      } else {
	++fcell;
      } // end remove cell due to invalid hash or sampling id
    } // end remove cell due to pointer invalid
    ++hid;
  } // end loop on cells in cell link object
  return nrc;
}

std::string CaloTopoClusterFromTowerMaker::formatStr(const char* fmt, ... ) const
{
  // compose the formatted string
  char buffer[1024];
  va_list args;
  va_start( args, fmt );
  int nbuf(vsprintf(buffer,fmt,args));
  va_end(args);
  // return result
  return nbuf > 0 ? std::string(buffer,nbuf) : std::string(); 
}
