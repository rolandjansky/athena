/* Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration */
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "AthenaKernel/Units.h"

#include "CaloTopoTowerFromClusterMaker.h"
#include "CaloTopoClusterFromTowerHelpers.h"

#include "xAODCaloEvent/CaloClusterKineHelper.h"

#include "CaloEvent/CaloClusterCellLink.h"
#include "CaloEvent/CaloCellClusterWeights.h"

#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloGeoHelpers/proxim.h"

#include "CaloProtoCluster.h"
#include "CaloTowerGeometrySvc.h"

#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include <algorithm>

#include <cstdio>
#include <cstdarg>
#include <string>
#include <cmath>
#include <memory>
#include <vector>
#include <atomic>
#include <tuple>

namespace {
  MsgStream& operator<<(MsgStream& mstr,const SG::ReadHandleKey<CaloCellContainer>&          ckey) { mstr << ckey.key(); return mstr; }
  MsgStream& operator<<(MsgStream& mstr,const SG::ReadHandleKey<xAOD::CaloClusterContainer>& ckey) { mstr << ckey.key(); return mstr; }
}

std::atomic<bool> CaloTopoTowerFromClusterMaker_checkCellIndices(false);

///////////////////////////////////
// CaloTopoTowerFromClusterMaker //
///////////////////////////////////

double                                CaloTopoTowerFromClusterMaker::m_energyThresholdDef = -100000000.; // in MeV
double                                CaloTopoTowerFromClusterMaker::m_clusterRangeDef    = 5.;
std::string                           CaloTopoTowerFromClusterMaker::m_defaultKey         = "NONE";
CaloTopoTowerFromClusterMaker::uint_t CaloTopoTowerFromClusterMaker::m_errorValueUINT     = uint_t(-1); 

CaloTopoTowerFromClusterMaker::CaloTopoTowerFromClusterMaker(const std::string& type,
							     const std::string& name,
							     const IInterface* pParent)
  : AthAlgTool(type,name,pParent)
  , m_towerGeometrySvc("CaloTowerGeometrySvc",name)
  , m_clusterContainerKey("CaloTopoCluster")
  , m_cellContainerKey("AllCalo")
  , m_energyThreshold(m_energyThresholdDef-1.)
  , m_clusterRange(m_clusterRangeDef)
  , m_numberOfCells(0)
  , m_maxCellHash(0)
  , m_numberOfSamplings(static_cast<uint_t>(CaloSampling::Unknown))
  , m_numberOfTowers(0)
{
  declareInterface<CaloTowerCollectionProcessor>(this);
  declareProperty("CaloTowerGeometrySvc",        m_towerGeometrySvc=ServiceHandle<CaloTowerGeometrySvc>("CaloTowerGeometrySvc",name), "Service providing tower geometry");
  declareProperty("CaloCellContainerKey",        m_cellContainerKey,                                                                  "SG Key for CaloCellContainer (input)");
  declareProperty("BuildTopoTowers",             m_useCellsFromClusters,                                                              "Turn on/off topo-tower formation");
  declareProperty("CaloTopoClusterContainerKey", m_clusterContainerKey,                                                               "SG Key for CaloClusterContainer (input)");
  declareProperty("OrderClusterByPt",            m_orderByPt,                                                                         "Turn on/off pT-ordering of CaloClusterContainer (output)");
  declareProperty("ApplyCellEnergyThreshold",    m_applyCellEnergyThreshold,                                                          "Turn on/off cell energy thresholds");
  declareProperty("CellEnergyThreshold",         m_energyThreshold,                                                                   "Energy threshold for cells filled in clusters");
  declareProperty("PrepareLCW",                  m_prepareLCW,                                                                        "Prepare data structure to apply LCW");
  declareProperty("ExcludedSamplings",           m_excludedSamplingsName,                                                             "Excluded samplings by name");
  declareProperty("DoCellIndexCheck",            m_doCellIndexCheck,                                                                  "Check cell hash indices for consistency");
  declareProperty("BuildCombinedTopoSignal",     m_buildCombinedSignal,                                                               "Build topo-clusters and topo-towers");
  declareProperty("TopoClusterRange",            m_clusterRange,                                                                      "Rapidity range for using topo-clusters in combined signal mode");
}

StatusCode CaloTopoTowerFromClusterMaker::initialize()
{
  //--------------------//
  // Set up handle keys //
  //--------------------//

  ATH_CHECK(m_cellContainerKey.initialize());
  
  //---------------------//
  // Check configuration //
  //---------------------//

  // tower geometry service
  ATH_MSG_INFO("Allocate tower geometry service:");
  if ( !m_towerGeometrySvc.isValid() ) { 
    ATH_MSG_ERROR("[reject] cannot allocate tower geometry service - fatal");
    return StatusCode::FAILURE; 
  } else {
    ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("Tower geometry service is allocated, describes %6zu towers in grid:", m_towerGeometrySvc->towerBins()) );
    ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("[accept] %3zu eta bins in [%5.2f,%5.2f]",m_towerGeometrySvc->etaBins(),m_towerGeometrySvc->etaMin(),m_towerGeometrySvc->etaMax()) );
    ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("[accept] %3zu phi bins in [%5.2f,%5.2f]",m_towerGeometrySvc->phiBins(),m_towerGeometrySvc->phiMin(),m_towerGeometrySvc->phiMax()) );
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
    ATH_CHECK(m_clusterContainerKey.initialize());
    // // check on request for LCW
    // if ( m_prepareLCW ) { 
    //   ATH_CHECK(m_cellClusterWeightKey.initialize());
    //   ATH_MSG_INFO("[accept] prepare for LCW calibration - initialize CaloCellClusterWeights key object <" << m_cellClusterWeightKey << ">");
    // } else {
    //   ATH_MSG_INFO("[accept] use EM scale");
    // }
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
  m_numberOfCells  = m_towerGeometrySvc->totalNumberCells();
  m_maxCellHash    = m_towerGeometrySvc->maxCellHash();
  m_numberOfTowers = m_towerGeometrySvc->towerBins();
  ATH_MSG_INFO("Additional tool parameters:");
  if ( m_numberOfCells > 0 ) { 
    ATH_MSG_INFO("[accept] maximum cell hash index is " << m_maxCellHash);
    ATH_MSG_INFO("[accept] maximum number of cells is " << m_numberOfCells);
  } else { 
    ATH_MSG_ERROR("[reject] invalid maximum cell hash index/total number of cells " << m_maxCellHash << "/" << m_numberOfCells << " - fatal");
    return StatusCode::FAILURE;
  }
  if ( m_numberOfTowers > 0 ) { 
    ATH_MSG_INFO("[accept] maximum number of towers is " << m_numberOfTowers);
  } else {
    ATH_MSG_ERROR("[reject] invalid maximum number of towers " << m_numberOfTowers << " - fatal");
    return StatusCode::FAILURE;
  }

  if ( m_excludedSamplingsName.empty() ) { 
    m_excludedSamplings.clear();
    m_excludedSamplingsPattern.reset();
    ATH_MSG_INFO("Cells from all samplings used for topo-cluster included"); 
  } else {
    size_t nex(std::min(m_excludedSamplingsName.size(), m_excludedSamplingsPattern.size()));
    if ( m_excludedSamplingsName.size() > m_excludedSamplingsPattern.size() ) { 
      ATH_MSG_WARNING( CaloRec::Helpers::fmtMsg("Configuration problem: number of excluded sampling names %zu exceeds expected maximum %zu - ignore last %zu name(s)",
						m_excludedSamplingsName.size(), m_excludedSamplingsPattern.size(),m_excludedSamplingsName.size()-m_excludedSamplingsPattern.size()) );
    }
    m_excludedSamplings.resize(nex);
    m_excludedSamplingsPattern.reset();
    for ( size_t i(0); i<nex; ++i ) {
      m_excludedSamplings[i] = CaloRec::Lookup::getSamplingId(m_excludedSamplingsName.at(i));
      m_excludedSamplingsPattern.set(i);
      ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("CaloSampling \042%10.10s\042 has id %2zu (name in lookup table \042%10.10s\042)",
					      m_excludedSamplingsName.at(i).c_str(),(size_t)m_excludedSamplings.at(i),CaloRec::Lookup::getSamplingName(m_excludedSamplings.at(i)).c_str()) );
    }
  }

  ATH_MSG_INFO("Other properties:");
  std::map<bool,std::string> blu { { true, "true" }, { false, "false" } };
  ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("PrepareLCW ................. %s",             blu[m_prepareLCW].c_str())               ); 
  ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("BuildTopoTowers ............ %s",             blu[m_useCellsFromClusters].c_str())     );
  ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("ApplyCellEnergyThreshold ... %s",             blu[m_applyCellEnergyThreshold].c_str()) );
  ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("OrderClusterByPt ........... %s",             blu[m_orderByPt].c_str())                );
  ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("DoCellIndexCheck ........... %s",             blu[m_doCellIndexCheck].c_str())         );
  ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("BuildCombinedTopoSignal .... %s",             blu[m_buildCombinedSignal].c_str())      );
  ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("TopoClusterRange ........... %.2f",           m_clusterRange)                          );
  ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("ExcludedSamplings .......... %zu (number of)",m_excludedSamplingsName.size())          );

  return StatusCode::SUCCESS;
}

StatusCode CaloTopoTowerFromClusterMaker::finalize()
{ return StatusCode::SUCCESS; }

StatusCode CaloTopoTowerFromClusterMaker::execute(const EventContext& ctx,
						  xAOD::CaloClusterContainer* pClusCont,
						  CaloCellClusterWeights*     cellWeights) const
{
  /////////////////
  // Check input //
  /////////////////

  // CaloCellContainer is needed to construct CaloProtoCluster
  SG::ReadHandle<CaloCellContainer> pCellCont(m_cellContainerKey,ctx);
  if ( !pCellCont.isValid() ) { 
    ATH_MSG_ERROR("Cannot allocate CaloCellContainer with key <" << m_cellContainerKey << ">");
    return StatusCode::FAILURE;
  }

  if ( msgLvl(MSG::DEBUG) && m_numberOfCells != pCellCont->size() ) { 
    ATH_MSG_DEBUG( CaloRec::Helpers::fmtMsg("[mismatch] number of cells in CaloCellContainer %6zu, total number of cell descriptors %6zu",
					      pCellCont->size(),m_towerGeometrySvc->totalNumberCells()) );
  }

  if ( m_doCellIndexCheck ) { this->checkCellIndices(pCellCont.cptr()); }

  /////////////////////////
  // Set up ProtoCluster //
  /////////////////////////

  // index of CaloProtoCluster in container relates to tower position! DO NOT sort, shuffle, or remove elements!  
  protocont_t pProtoCont; pProtoCont.reserve(m_numberOfTowers);
  for ( uint_t i(0); i<m_numberOfTowers; ++i ) { pProtoCont.push_back(CaloProtoCluster(pCellCont.cptr())); }

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

  uint_t cCtr(0);
  if ( m_useCellsFromClusters ) {
    // retrieve topo-cluster container for topo-towers
    SG::ReadHandle<xAOD::CaloClusterContainer> pTopoClusCont(m_clusterContainerKey,ctx);
    if ( !pTopoClusCont.isValid() ) {
      ATH_MSG_ERROR("Cannot allocate xAOD::CaloClusterContainer with key <" << m_clusterContainerKey << ">");
      return StatusCode::FAILURE;
    } // check on ReadHandle validity
    cCtr = m_prepareLCW && cellWeights != nullptr ? this->buildLCWTopoTowers(*pTopoClusCont,pProtoCont,cellWeights) : this->buildEMTopoTowers(*pTopoClusCont,pProtoCont);
    if ( !isValidIndex(cCtr) ) { ATH_MSG_WARNING("problems building EM or LCW topo-towers"); return StatusCode::SUCCESS; }
  } else {
    // fill inclusive/exclusive towers
    cCtr = m_applyCellEnergyThreshold ?  this->buildExclTowers(*pCellCont,pProtoCont) : this->buildInclTowers(*pCellCont,pProtoCont);
    if ( !isValidIndex(cCtr) ) { ATH_MSG_WARNING("problems building EM inclusive or exclusive towers"); return StatusCode::SUCCESS; }
  } // end topo-towers/inclusive-exclusive towers
  
  
  
  // allocate sufficient space in vector
  pClusCont->reserve(cCtr);
  // pick up cluster size tag and set up counter
  xAOD::CaloCluster::ClusterSize csize = this->getClusterSize(m_numberOfTowers);
  // loop proto-clusters
  for ( uint_t ipc(0); ipc<pProtoCont.size(); ++ipc ) {
    CaloProtoCluster& pProto  = pProtoCont.at(ipc);                     // pick up proto-cluster
    CaloClusterCellLink* lptr = pProto.releaseCellLinks();              // take over CaloClusterCellLink object
    this->cleanupCells(lptr,ipc);                                       // clean up cell links 
    if ( this->filterProtoCluster(*lptr) ) {                            // ignore empty proto-clusters (no cells assigned)
      xAOD::CaloCluster* clptr = new xAOD::CaloCluster();               // new empty cluster
      pClusCont->push_back(clptr);                                      // put into container
      clptr->addCellLink(lptr);                                         // transfer cell links to CaloCluster
      clptr->setClusterSize(csize);                                     // set the cluster size spec
      CaloRec::Helpers::calculateKine(clptr,false);                     // calculate kinematics and other signals from cells
      clptr->setEta0(m_towerGeometrySvc->towerEta(ipc));                // save the tower center eta
      clptr->setPhi0(m_towerGeometrySvc->towerPhi(ipc));                // save the tower center phi
    } else {
      delete lptr;
    }
  } // proto-cluster loop

  // clean up proto-cluster container
  pProtoCont.clear();

  /////////////
  // Sorting //
  /////////////

  // All towers/clusters at this point are on EM scale. Sorting LCW towers by pT should be done in the 
  // CaloTopoClusterFromTowerCalibrator tool to assure desired ordering on the final scale.
  // The link between tower location and index of tower representation (CaloCluster) in its
  // container is definitively broken after sorting (was never ok in mixed cluster/tower mode).
  if ( m_orderByPt ) { 
    std::sort(pClusCont->begin(),pClusCont->end(),[](xAOD::CaloCluster* pc1,xAOD::CaloCluster* pc2) {
   	volatile double pt1(pc1->pt()); // FIXME needed? (this was just copied)
   	volatile double pt2(pc2->pt()); // FIXME needed? (this was just copied)
   	return ( pt1 > pt2 );  
      }
      );
  } // end ordered by pT

  return StatusCode::SUCCESS;
} // end execute

//////////////////////
// Fill topo-towers //
//////////////////////

// EM 
CaloTopoTowerFromClusterMaker::uint_t CaloTopoTowerFromClusterMaker::buildEMTopoTowers(const xAOD::CaloClusterContainer& pClusCont,protocont_t& pProtoCont) const
{
  // presets
  uint_t cCtr(0);
  std::vector<bool> cellTags(m_numberOfCells,false);

  // -- EM scale clusters
  if ( !m_buildCombinedSignal ) { 
    // topo-towers
    for ( auto pClus : pClusCont ) { 
      for ( auto fCell(pClus->cell_begin()); fCell != pClus->cell_end(); ++fCell ) { 
	uint_t cidx(static_cast<uint_t>((*fCell)->caloDDE()->calo_hash()));
	if ( cidx < cellTags.size() ) {
	  if ( !cellTags.at(cidx) ) { cellTags[cidx] = this->addCellToProtoCluster(*fCell,pProtoCont); }  
	} else {
	  ATH_MSG_ERROR( CaloRec::Helpers::fmtMsg("Invalid cell hash index %6zu >= maximum index %6zu for cell in %s at (eta,phi) = (%6.3,%f6.3)",
						  cidx,cellTags.size(),CaloSampling::getSamplingName((*fCell)->caloDDE()->getSampling()).c_str(),(*fCell)->eta(),(*fCell)->phi()) );
	  return m_errorValueUINT;
	}
      } // end cells-in-cluster loop
    } // end cluster loop
  } else { 
    // selected topo-towers for combined signal
    std::vector<std::tuple<const CaloCell*,double> > cellList(m_numberOfCells,std::tuple<const CaloCell*,double>(0,0.));
    for ( auto pClus : pClusCont ) { 
      if ( std::abs(pClus->eta()) > m_clusterRange ) { 
	for ( auto fCell(pClus->cell_begin()); fCell != pClus->cell_end(); ++fCell ) { 
	  uint_t cidx(static_cast<uint_t>((*fCell)->caloDDE()->calo_hash()));
	  if ( cellTags.at(cidx) ) { 
	    std::get<1>(cellList[cidx]) += fCell.weight();
	  } else { 
	    cellList[cidx] = std::tuple<const CaloCell*,double>(*fCell,fCell.weight());
	    cellTags[cidx] = true; 
	  } 
	} // cell in cluster loop
      } else { 
	++cCtr; 
      } // cluster range check
    } // cluster loop
    // fill proto-cluster
    for ( auto tpl : cellList ) { this->addCellToProtoCluster(std::get<0>(tpl),pProtoCont,std::get<1>(tpl)); }
  } // end of fill mode

  //
  return cCtr+pProtoCont.size();
}

// LCW
CaloTopoTowerFromClusterMaker::uint_t CaloTopoTowerFromClusterMaker::buildLCWTopoTowers(const xAOD::CaloClusterContainer& pClusCont,protocont_t& pProtoCont,CaloCellClusterWeights* cellWeights) const
{ 
  // Need to keep track of LCW weights (up to two per cell) available from the topo-cluster(s) the cell is assigned to.
  // Each cell in a topo-cluster is, at first occurance, added to the CaloProtoCluster(s) representing the tower(s) and its
  // LCW calibration weight is stored in a lookup table indexed by the calorimeter hash id of the cell. The second 
  // time the same cell is found in another topo-cluster, only its LCW weight is added to the lookup table (stored in
  // CaloCellClusterWeights for use in the downstream tower calibration tool) - the assignment to tower(s) has already
  // happened.

  uint_t cCtr(0);
  // project cells on tower grid
  if ( !m_buildCombinedSignal ) { 
    // loop original topo-cluster container
    for ( auto pClus : pClusCont ) { 
      // loop over cells in the original topo-cluster
      for ( auto fCell(pClus->cell_begin()); fCell != pClus->cell_end(); ++fCell ) {
	// map to towers only once
	if ( !cellWeights->check(*fCell) ) { this->addCellToProtoCluster(*fCell,pProtoCont); }
	// store all associated LCW weights
	cellWeights->set(*fCell,fCell.weight());
      } // end cells-in-cluster loop
    } // end cluster loop
  } else { 
    // loop topo-cluster
    for ( auto pClus : pClusCont ) {
      // keep top-clusters in configured eta range
      if ( std::abs(pClus->eta()) > m_clusterRange ) { 
	// loop over cells of topo-clusters for the forward towards 
	for ( auto fCell(pClus->cell_begin()); fCell != pClus->cell_end(); ++fCell ) {
	  // map to towers only once
	  if ( !cellWeights->check(*fCell) ) { this->addCellToProtoCluster(*fCell,pProtoCont); }
	  // store all associated LCW weights
	  cellWeights->set(*fCell,fCell.weight());
	} // end cells-in-cluster loop
      } else { 
	++cCtr; 
      } // end range check
    } // end cluster loop
  } // end combined signal check

  //
  return cCtr+pProtoCont.size();
}

/////////////////
// Fill towers //
/////////////////

// inclusive
CaloTopoTowerFromClusterMaker::uint_t CaloTopoTowerFromClusterMaker::buildInclTowers(const CaloCellContainer& pCellCont,protocont_t& pProtoCont) const
{
  // loop cell container - counter icl replaces cell hash index for NULL pointers in cell container
  uint_t icl(0);
  for ( auto cptr : pCellCont ) { 
    if ( cptr == 0 ) { 
      ATH_MSG_ERROR( CaloRec::Helpers::fmtMsg("CaloCellContainer[%6zu] contains invalid cell object pointer %p",icl,(void*)cptr) ); 
      return m_errorValueUINT;
    } else {
      // existing cell with non-zero energy (negative or positive)
      if ( std::fabs(cptr->e()) > 0. ) { this->addCellToProtoCluster(cptr,pProtoCont); }
    } // end pointer check 
    ++icl;
  } // end cell loop
  return pProtoCont.size();
}

// exclusive
CaloTopoTowerFromClusterMaker::uint_t CaloTopoTowerFromClusterMaker::buildExclTowers(const CaloCellContainer& pCellCont,protocont_t& pProtoCont) const
{
  // loop cell container
  uint_t icl(0);
  for ( auto cptr : pCellCont ) {
    if ( cptr == 0 ) { 
      ATH_MSG_ERROR( CaloRec::Helpers::fmtMsg("CaloCellContainer[%6zu] contains invalid cell object pointer %p",icl,(void*)cptr) ); 
      return m_errorValueUINT;
    } else {
      // existing cell with energy above threshold
      if ( cptr->e() > m_energyThreshold ) { this->addCellToProtoCluster(cptr,pProtoCont); }
    } // end pointer check
    ++icl;
  } // end cell loop
  //return StatusCode::SUCCESS;
  return pProtoCont.size();
}

bool CaloTopoTowerFromClusterMaker::addCellToProtoCluster(const CaloCell* cptr,protocont_t& pProtoCont,double weight) const
{
  // invalid input
  if ( cptr == 0 ) { return false; }

  // get towers for cell from geometry service
  uint_t nctr(0);
  for ( auto elm : m_towerGeometrySvc->getTowers(cptr) ) { 
    auto towerIdx(m_towerGeometrySvc->towerIndex(elm));
    if ( !m_towerGeometrySvc->isInvalidIndex(towerIdx) ) {
      if ( !m_excludedSamplingsPattern[(size_t)cptr->caloDDE()->getSampling()] ) {
	uint_t cellIdx(pProtoCont.at(towerIdx).getCellLinks()->getCellContainer()->findIndex(cptr->caloDDE()->calo_hash()));
	pProtoCont[towerIdx].addCell(cellIdx,m_towerGeometrySvc->cellWeight(elm)*weight); ++nctr; 
      }
    }
  }
  return nctr > 0;
}

/////////////
// Helpers //
/////////////

xAOD::CaloCluster::ClusterSize CaloTopoTowerFromClusterMaker::getClusterSize(uint_t etaBins,uint_t phiBins) const
{ return this->getClusterSize(etaBins*phiBins); }

xAOD::CaloCluster::ClusterSize CaloTopoTowerFromClusterMaker::getClusterSize(uint_t nTowers) const
{
  // check for tower sizes
  return nTowers == 6400                    // known "standard" towers 0,1 x 0.1
    ? xAOD::CaloCluster::Tower_01_01 
    : nTowers == 25600                      // known "fine" towers 0.05 x 0.05
    ? xAOD::CaloCluster::Tower_005_005 
    : xAOD::CaloCluster::Tower_fixed_area;  // unspecified towers 
}

int CaloTopoTowerFromClusterMaker::cleanupCells(CaloClusterCellLink* clk,uint_t nclus) const
{
  // Any pathology here probably indicates a configuration problem with the conditions (geometry)
  // database (wrong tag for data?)

  // check on null pointers in cell links
  int nrc(0); int hid(0);
  auto fcell(clk->begin());
  while ( fcell != clk->end() ) {
    const CaloCell* pCell = *fcell;
    auto nc(clk->getCellContainer()->size());
    const CaloCell* aCell = fcell.index() < nc ? clk->getCellContainer()->at(fcell.index()) : (const CaloCell*)0;
    if ( pCell == 0 ) {
      ATH_MSG_WARNING( CaloRec::Helpers::fmtMsg("CaloCellContainer[%6zu/%6zu] - tower %5zu at (%6.3f,%6.3f) - cell pointer invalid (%p/%p) [removed %3i of %3zu cells]",
						fcell.index(),nc-1,nclus,m_towerGeometrySvc->towerEta(nclus),m_towerGeometrySvc->towerPhi(nclus),
						(void*)pCell,(void*)aCell,++nrc,clk->size()) );
      fcell = clk->removeCell(fcell);
    } else {
      uint_t chash(static_cast<uint_t>(pCell->caloDDE()->calo_hash()));
      uint_t csamp(static_cast<uint_t>(pCell->caloDDE()->getSampling()));
      if (chash > m_maxCellHash ) {
	// check cell hash
	ATH_MSG_WARNING( CaloRec::Helpers::fmtMsg("Tower %5zu at (%6.3f,%6.3f) linked cell %3i - cell hash index (%6zu/%6zu) invalid",
						  nclus,m_towerGeometrySvc->towerEta(nclus),m_towerGeometrySvc->towerPhi(nclus),hid,chash,m_maxCellHash) );
	fcell = clk->removeCell(fcell);	++nrc;
      } else if ( csamp >= m_numberOfSamplings ) {
	// check sampling id
	ATH_MSG_WARNING( CaloRec::Helpers::fmtMsg("Tower %5zu at (%6.3f,%6.3f) linked cell %3i -cell sampling id (%3zu/%3zu) invalid",
						  nclus,m_towerGeometrySvc->towerEta(nclus),m_towerGeometrySvc->towerPhi(nclus),hid,csamp,m_numberOfSamplings) );
	fcell = clk->removeCell(fcell); ++nrc;
      } else if ( fcell.weight() <= 0.0000001 ) { 
	// remove cells with 0 weight 
	fcell = clk->removeCell(fcell); ++nrc;
      } else { 
	// next cell 
	++fcell;
      }
    } // end remove cell due to pointer invalid
    ++hid;
  } // end loop on cells in cell link object
  return nrc;
}

bool CaloTopoTowerFromClusterMaker::filterProtoCluster(const CaloClusterCellLink& clnk) const
{ return clnk.size() > 0; }

bool CaloTopoTowerFromClusterMaker::checkCellIndices(const CaloCellContainer* pCellCont) const
{
  ////////////////////////////
  // input and setup checks //
  ////////////////////////////

  // check argument
  if ( pCellCont == 0 ) { 
    ATH_MSG_WARNING( CaloRec::Helpers::fmtMsg("Invalid pointer to CaloCellContainer (%p)",(void*)pCellCont) ); return false;
  } else if ( pCellCont->empty() ) { 
    ATH_MSG_WARNING( CaloRec::Helpers::fmtMsg("CaloCellContainer at %p is empty (size %zu)",(void*)pCellCont,pCellCont->size()) ); return false; 
  }
  // check the atomic state
  if ( CaloTopoTowerFromClusterMaker_checkCellIndices ) { return true; }
  // set the atomic flag
  ATH_MSG_INFO( "Cell hash index check requested" ); 
  CaloTopoTowerFromClusterMaker_checkCellIndices = true;
  // assign output file
  std::string algname(this->name());
  if ( algname.find_last_of('.') != std::string::npos ) { algname = algname.substr(algname.find_last_of('.')+1); }
  std::string logname(CaloRec::Helpers::fmtMsg("%s.cellhash_index_check.dat",this->name().c_str())); 
  std::ofstream logstream; logstream.open(logname);
  if ( !logstream.is_open() ) { 
    ATH_MSG_WARNING( CaloRec::Helpers::fmtMsg("Cannot open log file \042%s\042 - no hash index checking",logname.c_str()) ); 
    return false;
  }
  logstream << "##########################################################################" << std::endl;
  logstream << "### This file contains a list of CaloCell indices in CaloCellContainer ###" << std::endl;
  logstream << "### for which this index is not the same as the calorimeter cell hash  ###" << std::endl;
  logstream << "### identifier. An empty list indicates full consistency between this  ###" << std::endl;
  logstream << "### index and the hash identifier for all cells.                       ###" << std::endl;
  logstream << "##########################################################################" << std::endl;
  logstream << "<begin list>--------------------------------------------------------------" << std::endl;

  /////////////////////////
  // loop cell container //
  /////////////////////////

  // prepare tag store
  size_t ifc(0); std::bitset<200000> chkflg; chkflg.reset();
  for ( size_t i(0); i<pCellCont->size(); ++i ) { 
    if ( pCellCont->at(i) != 0 ) { 
      size_t chash((size_t)pCellCont->at(i)->caloDDE()->calo_hash());
      if ( chash != i ) {
	std::string cni("UKNOWN");
	double etai(0.); double phii(0.);
	const CaloDetDescrElement* iel = i < CaloDetDescrManager::instance()->element_size() ? CaloDetDescrManager::instance()->get_element(i) : 0;
	if ( iel != 0 ) {
	  cni  = CaloRec::Lookup::getSamplingName(iel->getSampling());
	  etai = iel->eta_raw();
	  phii = iel->phi_raw();
	}
	std::string cnc("UNKNOWN");
	double etac(0.); double phic(0.);
	const CaloDetDescrElement* cel = chash < CaloDetDescrManager::instance()->element_size() ? CaloDetDescrManager::instance()->get_element(chash) : 0;
	if ( cel != 0 ) { 
	  cnc  = CaloRec::Lookup::getSamplingName(cel->getSampling());
	  etac = cel->eta_raw();
	  phic = cel->phi_raw();
	}
	size_t cidx(pCellCont->findIndex(chash));
	logstream << CaloRec::Helpers::fmtMsg("[%06zu] Cell %6zu [%12.12s %5.3f %5.3f] non-matching id %6zu [%12.12s %5.3f %5.3f] findCell() index %6zu",
					      ++ifc,i,cni.c_str(),etai,phii,chash,cnc.c_str(),etac,phic,cidx) << std::endl; 
      }
      chkflg.set(chash);
    }
  }
  logstream << "<end list>----------------------------------------------------------------" << std::endl;
  logstream.close();

  /////////////////////////
  // check missed hashes //
  /////////////////////////

  // number of non-matched hashes
  if ( ifc > 0 ) { 
    ATH_MSG_DEBUG( CaloRec::Helpers::fmtMsg("Found %zu non-matching cell hashes",ifc) );
  }
  // list of non-matched hashes
  std::vector<size_t> chl; chl.reserve(m_towerGeometrySvc->totalNumberCells());
  for ( size_t i(0); i<chl.size(); ++i ) { if ( !chkflg.test(i) ) { chl.push_back(i); } }
  if ( !chl.empty() ) { 
    for ( auto h : chl ) { ATH_MSG_DEBUG( CaloRec::Helpers::fmtMsg("Cell hash %6zu not in CaloCellContainer",h) ); }
  }

  return true;
}

