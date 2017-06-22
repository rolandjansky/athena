/* Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration */
#include "CaloRec/CaloTopoClusterFromTowerMaker.h"
#include "CaloRec/CaloTopoClusterFromTowerHelpers.h"


#include "xAODCaloEvent/CaloTower.h"
#include "xAODCaloEvent/CaloTower.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"

#include "CaloEvent/CaloClusterCellLink.h"
#include "CaloEvent/CaloCellTowerLink.h"
#include "CaloEvent/CaloCellClusterWeights.h"

#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloGeoHelpers/proxim.h"

#include "CaloRec/CaloProtoCluster.h"

#include <algorithm>

#include <cstdio>
#include <string>
#include <cmath>
#include <array>
#include <memory>
#include <vector>

//////////////////////////////////////////////////
// CaloTopoClusterFromTowerMaker::CellPathology //
//////////////////////////////////////////////////

CaloTopoClusterFromTowerMaker::CellPathology::CellPathology()
{ }

void CaloTopoClusterFromTowerMaker::CellPathology::setInvalidPointer(int cellhash,const CaloCell* cptr)
{
  std::string msg(CaloRec::Helpers::fmtMsg("CaloCell [%06i] has invalid pointer %p",cellhash,(void*)cptr));
  _addMsg(msg,_invalidPointer);
}

void CaloTopoClusterFromTowerMaker::CellPathology::setInvalidHash(int cellhash,const CaloCell* cptr)
{
  std::string msg(CaloRec::Helpers::fmtMsg("CaloCell [%06i] at %p has invalid hash %i",cellhash,(void*)cptr,cellhash));
  _addMsg(msg,_invalidHash);
}

void CaloTopoClusterFromTowerMaker::CellPathology::setInvalidSampling(int cellhash,const CaloCell* cptr)
{
  int hash(CaloSampling::Unknown);
  if ( cptr != 0 ) { hash = (int)cptr->caloDDE()->getSampling(); } 
  std::string msg(CaloRec::Helpers::fmtMsg("CaloCell [%06i] at %p has invalid sampling %2i (not in [0,%2i]",cellhash,(void*)cptr,hash,(int)CaloSampling::Unknown));
  _addMsg(msg,_invalidSampling);
}

///////////////////////////////////
// CaloTopoClusterFromTowerMaker //
///////////////////////////////////

double CaloTopoClusterFromTowerMaker::m_energyThresholdDef = -100000000.; // in MeV
std::string CaloTopoClusterFromTowerMaker::m_defaultKey    = "NONE";

CaloTopoClusterFromTowerMaker::CaloTopoClusterFromTowerMaker(const std::string& type,
							     const std::string& name,
							     const IInterface* pParent)
  : AthAlgTool(type,name,pParent)
  , m_towerContainerKey(m_defaultKey)
  , m_cellTowerLinksKey(m_defaultKey)
  , m_cellContainerKey("AllCalo")
  , m_clusterContainerKey(m_defaultKey)
  , m_cellClusterWeightKey(m_defaultKey)
  , m_orderByPt(false)
  , m_energyThreshold(m_energyThresholdDef-1.)
  , m_applyLCW(false)
  , m_applyEnergyThreshold(false)
  , m_useCellsFromClusters(false)
  , m_prepareLCW(false)
  , m_numberOfCells(0)
  , m_numberOfSamplings(CaloSampling::Unknown)
  , m_cellTowerLinksHandle(0)
  , m_cellClusterWeights(0)
{
  declareInterface<CaloClusterCollectionProcessor>(this);
  declareProperty("CaloTowerContainerKey",       m_towerContainerKey,   "SG Key for CaloTowerContainer (input)");
  declareProperty("CaloCellContainerKey",        m_cellContainerKey,    "SG Key for CaloCellContainer (input)");
  declareProperty("CaloTopoClusterContainerKey", m_clusterContainerKey, "SG Key for CaloClusterContainer (input)");
  declareProperty("CellClusterWeightKey",        m_cellClusterWeightKey,"SG Key for CellClusterWeights (output)");
  declareProperty("OrderClusterByPt",            m_orderByPt,           "Turn on/off pT-ordering of CaloClusterContainer (output)");
  declareProperty("CellEnergyThreshold",         m_energyThreshold,     "Energy threshold for cells filled in clusters");
  declareProperty("ApplyLCW",                    m_applyLCW,            "Prepare data structure to apply LCW");
}

StatusCode CaloTopoClusterFromTowerMaker::initialize()
{
  // build SG key for cell-tower link map object
  m_cellTowerLinksKey = m_towerContainerKey + CaloCellTowerLink::extTag;
  ATH_MSG_INFO("Input retrieved from CaloTowerxAODContainer <" << m_towerContainerKey << "> and CaloCellTowerLink::Map <" << m_cellTowerLinksKey << ">");
  // check if energy threshold is to be applied
  m_applyEnergyThreshold = m_energyThreshold > m_energyThresholdDef;
  // check if filtered cells to be used
  m_useCellsFromClusters = m_clusterContainerKey != m_defaultKey;
  // analyze configuration
  if ( m_useCellsFromClusters ) {
    if ( m_applyEnergyThreshold ) { 
      ATH_MSG_WARNING("Misconfiguration - cannot apply energy threshold to cells from TopoClusters, will accept all clustered cells"); 
      m_applyEnergyThreshold = false;
    } else { 
      ATH_MSG_INFO("Filtered mode, cluster from towers filled with cells from TopoClusters");
    }
  } else { 
    if ( m_applyEnergyThreshold ) { 
      ATH_MSG_INFO(CaloRec::Helpers::fmtMsg("Exclusive mode, cluster from towers filled with cells with energy > %.3f MeV",m_energyThreshold)); 
    } else { 
      ATH_MSG_INFO("Inclusive mode, all cells are used");
    }
  }
  // build SG key for cell weight lookup
  m_prepareLCW = m_applyLCW && m_useCellsFromClusters && ( m_cellClusterWeightKey != m_defaultKey );  
  if ( m_prepareLCW ) { ATH_MSG_INFO("Prepare LCW calibration"); }
  else { ATH_MSG_INFO("No preparation for LCW calibration"); }

  return StatusCode::SUCCESS;
}

StatusCode CaloTopoClusterFromTowerMaker::finalize()
{
  ATH_MSG_INFO(" ");
  ATH_MSG_INFO("Summary of warnings in execute():");
  ATH_MSG_INFO("Count:  Warning:");
  for ( auto fiter(m_warning.begin()); fiter!=m_warning.end(); ++fiter ) {
    ATH_MSG_INFO(CaloRec::Helpers::fmtMsg("%6i  \042%s\042",(int)fiter->second,fiter->first.c_str()));
  }
  ATH_MSG_INFO(" ");

  ATH_MSG_INFO("Summary of messages in execute():");
  ATH_MSG_INFO("Count:  Message:");
  for ( auto fiter(m_message.begin()); fiter!=m_message.end(); ++fiter ) {
    ATH_MSG_INFO(CaloRec::Helpers::fmtMsg("%6i  \042%s\042",(int)fiter->second,fiter->first.c_str()));
  }
  if ( !m_cellProblems.invalidPointer().empty() ) {
    ATH_MSG_INFO(" ");
    ATH_MSG_INFO("Observed cell pathologies [invalid pointer]:");
    printProblems(m_cellProblems.invalidPointer());
  }
  if ( !m_cellProblems.invalidHash().empty() ) { 
    ATH_MSG_INFO(" ");
    ATH_MSG_INFO("Observed cell pathologies [invalid hash]:");
    printProblems(m_cellProblems.invalidHash());
  }
  if ( !m_cellProblems.invalidSampling().empty() ) { 
    ATH_MSG_INFO(" ");
    ATH_MSG_INFO("Observed cell pathologies [invalid sampling]:");
    printProblems(m_cellProblems.invalidSampling());
  }
  ATH_MSG_INFO(" ");

  return StatusCode::SUCCESS;
}

StatusCode CaloTopoClusterFromTowerMaker::execute(xAOD::CaloClusterContainer* pClusCont)
{
  // find cell-tower links
  CHECK(detStore()->retrieve(m_cellTowerLinksHandle,m_cellTowerLinksKey));

  // find input: CaloCell container
  const CaloCellContainer* pCellCont = 0;
  CHECK(evtStore()->retrieve(pCellCont,m_cellContainerKey));
  m_numberOfCells = (int)pCellCont->size();

  // cross check
  this->addMessage(CaloRec::Helpers::fmtMsg("CaloCellContainer::%s size %i, CaloCellTowerlink::Map size %i",
			     m_cellContainerKey.c_str(),(int)pCellCont->size(),(int)m_cellTowerLinksHandle->size()));
  for ( size_t itl(0); itl<m_cellTowerLinksHandle->size(); ++itl ) { 
    if ( itl >= pCellCont->size() ) {
      this->addWarning(CaloRec::Helpers::fmtMsg("CaloCell [%06i] has hash out of reach (max hash is %i)",(int)itl,(int)pCellCont->size()-1));
    } else { 
      if ( pCellCont->at(itl) == 0 ) { this->addWarning(CaloRec::Helpers::fmtMsg("CaloCell [%06i] has invalid pointer",(int)itl)); }
    }
  }
  //  int icell(0);
  //  int ncell(0);
  //  for ( auto cptr : *pCellCont ) { printf("CaloCell [%06i/%06i] ptr = %p\n",icell++,m_numberOfCells,(void*)cptr); if ( cptr == 0 ) { ++ncell; } }
  //  this->addMessage(CaloRec::Helpers::fmtMsg("Allocated CaloCellContainer with key <%s> at %p",m_cellContainerKey.c_str(),(void*)pCellCont));

  // find input: tower container 
  const xAOD::CaloTowerContainer* pTowCont = 0;
  CHECK(evtStore()->retrieve(pTowCont,m_towerContainerKey));
  //  this->addMessage(CaloRec::Helpers::fmtMsg("Allocated CaloTowerContainer with key <%s> at %p",m_towerContainerKey.c_str(),(void*)pTowCont));

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

  protocont_t pProtoCont(pTowCont->size(),(CaloProtoCluster*)0);
  for ( size_t itow(0); itow<pTowCont->size(); ++itow) { pProtoCont[itow] = new CaloProtoCluster(pCellCont); }

  //////////////////////////////////////////////////////
  // Apply overall cell filter and fill protoclusters //
  //////////////////////////////////////////////////////

  if ( m_useCellsFromClusters ) {                                         ///////////////////////////////////////////////////////
    // retrieve TopoCluster container                                     // The weights extracted here are the cell signal    //
    const xAOD::CaloClusterContainer* pTopoClusCont = 0;                  // weights after LCW (typically)! These are products //
    CHECK(evtStore()->retrieve(pTopoClusCont,m_clusterContainerKey));     // of the geometrical weight in cell splitting       //
    // instantiate cell-cluster link object                               // between clusters and the calibration weights.     //
    m_cellClusterWeights = new CaloCellClusterWeights(pCellCont->size()); // The geometrical weight of a cell in the towers is //
    // fill the weights: cluster loop                                     // used in the addCellToProtoCluster method.         //
    for ( auto pClus : *pTopoClusCont ) {                                 ///////////////////////////////////////////////////////
      // cell loop
      for ( auto fCell(pClus->cell_begin()); fCell != pClus->cell_end(); ++fCell ) {
	if ( !m_cellClusterWeights->check(*fCell) ) { this->addCellToProtoCluster(*fCell,pProtoCont);  }  // fill protocluster only once/cell 
	m_cellClusterWeights->set(*fCell,fCell.weight());                                                 // prep for calibration tool
      } // cell in cluster loop
    } // cluster loop
    // store LCW weights for calibrator
    if ( m_prepareLCW ) { 
      CHECK(evtStore()->record(m_cellClusterWeights,CaloCellClusterWeights::key(m_cellClusterWeightKey))); 
    } else {
      delete m_cellClusterWeights; m_cellClusterWeights = 0; 
    }

  /////////////////////////////////////////////////////////////
  // Generate and fill inclusive or exclusive proto-clusters //
  /////////////////////////////////////////////////////////////
	
  } else { 
    if ( !m_applyEnergyThreshold ) { buildInclClusters(*pCellCont,pProtoCont); } else { buildExclClusters(*pCellCont,pProtoCont); }
  }

  ////////////////////////////
  // Convert proto-clusters //
  ////////////////////////////

  pClusCont->reserve(pProtoCont.size());

  xAOD::CaloCluster::ClusterSize csize = this->getClusterSize(*pTowCont);
  int ictr(0);
  for ( size_t ipc(0); ipc<pProtoCont.size(); ++ipc ) {
    // auto fproto(pProtoCont.begin());
    // while ( fproto != pProtoCont.end() ) { 
    //   CaloProtoCluster* pProto = *fproto;
    CaloProtoCluster* pProto = pProtoCont.at(ipc);
    if ( pProto != 0 ) { 
      CaloClusterCellLink* lptr = pProto->releaseCellLinks(); // take over CaloClusterCellLink object
      // convert proto-cluster to cluster
      if ( lptr->size() > 0 ) {                                            // remove empty proto-clusters
	xAOD::CaloCluster* clptr = new xAOD::CaloCluster();                // new empty cluster
	pClusCont->push_back(clptr);                                       // put into container
	clptr->addCellLink(lptr);                                          // transfer cell links to CaloCluster
	clptr->setClusterSize(csize);                                      // set the cluster size spec
	CaloRec::Helpers::calculateKine(clptr,false);                      // calculate kinematics and other signals from cells
	clptr->setEta0(pTowCont->eta(ipc));                                // save the tower center eta
	clptr->setPhi0(pTowCont->phi(ipc));                                // save the tower center phi
	++ictr;                                                            // counts the number of CaloCluster produced in the end
      } // tower has cells
      delete pProto;                                                       // delete the proto-cluster object
      pProtoCont[ipc] = (CaloProtoCluster*)0;                              // FIXME (needed?)
    } // valid proto-cluster
  } // proto-cluster loop

  pProtoCont.clear();  // clean up (objects in this container have already been destroyed)

  ////////////////////
  // Check clusters //
  ////////////////////

  size_t idc(0);
  for ( auto pClus : *pClusCont ) { 
    if ( pClus->getCellLinks() == 0 || pClus->getCellLinks()->size() == 0 ) { 
      this->addWarning(CaloRec::Helpers::fmtMsg("Cluster #%05i has invalid pointer (%p) or no cells",idc,(void*)pClus));
    }
    ++idc;
  } 

  ////////////////////////
  // Sort protoclusters //
  ////////////////////////

  // definitively removes the index-to-tower center relation in all cases!
  if ( m_orderByPt ) { 
    std::sort(pClusCont->begin(),pClusCont->end(),[](xAOD::CaloCluster* pc1,xAOD::CaloCluster* pc2) {
   	volatile double pt1(pc1->pt()); // FIXME needed? (this was just copied)
   	volatile double pt2(pc2->pt()); // FIXME needed? (this was just copied)
   	return ( pt1 > pt2 );  
      }
      );
  }

  return StatusCode::SUCCESS;
}

xAOD::CaloCluster::ClusterSize CaloTopoClusterFromTowerMaker::getClusterSize(const xAOD::CaloTowerContainer& towerCont)
{
  return towerCont.nTowers() == 6400 
    ? xAOD::CaloCluster::Tower_01_01   : towerCont.nTowers() == 25600 
    ? xAOD::CaloCluster::Tower_005_005 : xAOD::CaloCluster::CSize_Unknown;  
}

int CaloTopoClusterFromTowerMaker::cleanupCells(CaloClusterCellLink* clk)
{
  // check on null pointers in cell links
  int nrc(0); int hid(0);
  auto fcell(clk->begin());
  while ( fcell != clk->end() ) {
    const CaloCell* pCell = *fcell;
    this->addMessage(CaloRec::Helpers::fmtMsg("CaloCell* = %p in CaloClusterCellLink* = %p"));
    bool removeCell(false);
    if ( pCell == 0  ) {
      m_cellProblems.setInvalidPointer(hid,pCell);
      removeCell = true;
    } else {
      int hash((int)pCell->caloDDE()->calo_hash());
      if ( hash >= m_numberOfCells ) { m_cellProblems.setInvalidHash(hash,pCell); removeCell = true; }
      int samp((int)pCell->caloDDE()->getSampling());
      if ( samp >= m_numberOfSamplings ) { m_cellProblems.setInvalidSampling(hash,pCell); removeCell = true; } 
    }
    if ( removeCell ) { fcell = clk->removeCell(fcell); ++nrc; } else { ++fcell; }
    ++hid;
  }
  return nrc;
}

// bool CaloTopoClusterFromTowerMaker::calculateKine(xAOD::CaloCluster* pClus)
// { return CaloRec::Helpers::updateKine
//   // input
//   if ( pClus == 0 ) { 
//     ATH_MSG_WARNING("Invalid (NULL) pointer to xAOD::CaloCluster object");
//     return false;
//   }
//   // copy from CaloKineHelper::calculateKine(...) - but without the prefetch!
//   const CaloClusterCellLink* clk = pClus->getCellLinks();
//   if ( clk == 0 ) { 
//     this->addWarning(CaloRec::Helpers::fmtMsg("Invalid reference (pointer %p) to CaloClusterCellLink object",(void*)clk));
//     return false;
//   }
//   if ( clk->size() == 0 ) { 
//     this->addWarning(CaloRec::Helpers::fmtMsg("Number of linked cells in cluster is %i",(int)clk->size()));
//     return false;
//   }

//   // accumulator object
//   CaloRec::Helpers::CaloClusterSignalAccumulator accum;
//   // accumulate cells
//   for ( auto cIter(clk->begin()); cIter!=clk->end(); ++cIter) { 
//     const CaloCell* pCell = *cIter;
//     if ( pCell != 0 ) {
//       if ( !CaloRec::Helpers::caloCellAccumulator(*pCell,accum,cIter.weight(),false) ) { 
// 	this->addWarning(CaloRec::Helpers::fmtMsg("CaloCell [%06i] has invalid sampling id %2i (not in [0,%2i])",(int)(*cIter)->caloDDE()->calo_hash(),(int)(*cIter)->caloDDE()->getSampling(),(int)CaloSampling::Unknown));
//       }
//     } else {
//       this->addWarning(CaloRec::Helpers::fmtMsg("Unexpected CaloCell pointer %p detected",(void*)pCell)); 
//     }
//   }

//   // set cluster kinematics: energy & mass
//   pClus->setE(accum.cluster.accumE);
//   pClus->setM(0.);

//   // set cluster kinematics: directions
//   if ( accum.cluster.accumAbsE != 0. ) {
//     double invPosNorm(1./accum.cluster.accumAbsE);
//     pClus->setEta(accum.cluster.accumEta*invPosNorm);
//     pClus->setPhi(CaloPhiRange::fix(accum.cluster.accumPhi*invPosNorm));
//   } else {
//     pClus->setEta(0.);
//     pClus->setPhi(0.);
//   }

//   // set cluster kinematics: time
//   if ( accum.cluster.accumTimeNorm != 0. ) {
//     pClus->setTime(accum.cluster.accumTime/accum.cluster.accumTimeNorm);
//   } else {
//     pClus->setTime(0.);
//   }

//   // set sampling pattern
//   uint32_t samplingPattern(0);
//   for ( int i(0); i<(int)CaloSampling::Unknown; ++i ) {
//     if ( accum.sampling.presenceInSample[i] ) { samplingPattern |= (0x1U<<i); }
//     if ( samplingPattern != pClus->samplingPattern() ) { 
//       pClus->clearSamplingData();
//       pClus->setSamplingPattern(samplingPattern,true);
//     }
//   }

//   // set sampling variables
//   for ( int i(0); i<(int)CaloSampling::Unknown; ++i ) { 
//     if ( accum.presenceInSample[i] ) { 
//       CaloSampling::CaloSample sam = (CaloSampling::CaloSample)i;
//       pClus->setEnergy(sam,accum.energyInSample[i]);
//       if ( accum.posNormInSample[i] != 0. ) { 
// 	pClus->setEta(sam,accum.etaInSample[i]/accum.posNormInSample[i]);
// 	pClus->setPhi(sam,accum.phiInSample[i]/accum.posNormInSample[i]);
//       } else { 
// 	pClus->setEta(sam,accum.etaInSample[i]);
// 	pClus->setPhi(sam,accum.phiInSample[i]);
//       }
//       pClus->setEmax(sam,accum.maxEnergyInSample[i]);
//       pClus->setEtamax(sam,accum.etaMaxEnergyInSample[i]);
//       pClus->setPhimax(sam,accum.phiMaxEnergyInSample[i]);
//     } // check if sampling is in cluster
//   } // loop on samplings

//   return true;
// }

void CaloTopoClusterFromTowerMaker::addWarning(const std::string& msg)
{
  ATH_MSG_WARNING(msg);
  auto fiter(m_warning.find(msg)); 
  if ( fiter == m_warning.end() ) { m_warning[msg] = 0; }
  ++m_warning[msg];
}

void CaloTopoClusterFromTowerMaker::addMessage(const std::string& msg)
{
  ATH_MSG_DEBUG(msg);
  auto fiter(m_message.find(msg)); 
  if ( fiter == m_message.end() ) { m_message[msg] = 0; }
  ++m_message[msg];
}

void CaloTopoClusterFromTowerMaker::printProblems(const std::map<std::string,int>& map)
{
  static std::string _dstr = "....................................................................";
  static size_t      _lds = _dstr.length();
  static size_t      _occ = _lds - 3;
  //
  for ( auto fmap(map.begin()); fmap!=map.end(); ++fmap ) { 
    std::string msg = fmap->first.length() > _occ ? fmap->first.substr(0,_occ) : fmap->first;
    std::string prn(std::string(_dstr).replace(0,msg.length(),msg));
    ATH_MSG_INFO(CaloRec::Helpers::fmtMsg("%s %i",prn.c_str(),fmap->second));
  }
}

bool CaloTopoClusterFromTowerMaker::buildInclClusters(const CaloCellContainer& pCellCont,protocont_t& pProtoCont)
{
  // inclusive mode
  this->addMessage(CaloRec::Helpers::fmtMsg("Found %6i cell links in CaloCellTowerLink::Map::%s",(int)m_cellTowerLinksHandle->size(),m_cellTowerLinksKey.c_str()));

  // loop cell container
  size_t icl(0);
  for ( auto cptr : pCellCont ) { 
    // FIXME check pointer value
    if ( cptr == 0 ) { m_cellProblems.setInvalidPointer(icl,cptr); }
    // existing cell
    else {
      if ( std::fabs(cptr->e()) > 0. ) { this->addCellToProtoCluster(cptr,pProtoCont); }
    }
    ++icl;
  } // cell loop
  return true;
}

bool CaloTopoClusterFromTowerMaker::buildExclClusters(const CaloCellContainer& pCellCont,protocont_t& pProtoCont)
{
  // inclusive mode
  this->addMessage(CaloRec::Helpers::fmtMsg("Found %6i cell links in CaloCellTowerLink::Map::%s",(int)m_cellTowerLinksHandle->size(),m_cellTowerLinksKey.c_str()));

  // loop cell container
  for ( auto cptr : pCellCont ) { 
    // FIXME check pointer value
    if ( cptr == 0 ) { m_cellProblems.setInvalidPointer(999999,cptr); }
    // existing cell
    else {
      if ( cptr->e() > m_energyThreshold ) { this->addCellToProtoCluster(cptr,pProtoCont); }
    }
  } // cell loop
  return true;
}

// bool CaloTopoClusterFromTowerMaker::buildFilteredClusters(const CaloCellContainer& pCellCont,protocont_t& pProtoCont)
// {
//   // inclusive mode
//   this->addMessage(CaloRec::Helpers::fmtMsg("Found %6i cell links in CaloCellTowerLink::Map::%s",(int)m_cellTowerLinksHandle->size(),m_cellTowerLinksKey.c_str()));

//   // loop cell container
//   size_t icl(0);
//   for ( auto cptr : pCellCont ) { 
//     // FIXME check pointer value
//     if ( cptr == 0 ) { m_cellProblems.setInvalidPointer(icl,cptr); }
//     // existing cell
//     else {
//       size_t idx(static_cast<size_t>(cptr->caloDDE()->calo_hash_id()));
//       if ( m_cellTagStore[idx].get<0>() ) { if ( std::fabs(cptr->e()) > 0. ) { this->addCellToProtoCluster(cptr,pProtoCont); } }
//     }
//     ++icl;
//   } // cell loop
//   return true;
// }

bool CaloTopoClusterFromTowerMaker::addCellToProtoCluster(const CaloCell* cptr,protocont_t& pProtoCont) 
{
  if ( cptr == 0 ) { return false; }
  // cell hash
  int cellidx(cptr->caloDDE()->calo_hash());
  if ( cellidx >= (int)m_cellTowerLinksHandle->size() ) { 
    this->addWarning(CaloRec::Helpers::fmtMsg("CaloCell [%06i] has hash outside of link lookup (max hash is %i)",cellidx,(int)m_cellTowerLinksHandle->size())); 
    return false; 
  }
  if ( cellidx < m_numberOfCells ) {
    for ( auto link : m_cellTowerLinksHandle->elementList(cellidx) ) {
      int towidx(CaloCellTowerLink::towerIndex(link));
      if ( towidx < (int)pProtoCont.size() ) { pProtoCont[towidx]->addCell(cellidx,CaloCellTowerLink::cellWeight(link)); }
      else { this->addWarning(CaloRec::Helpers::fmtMsg("Invalid tower index %i (not in [0,%i])",(int)towidx,(int)pProtoCont.size()-1)); }
    } // loop all cell->tower links
  } // valid hash index
  return true;
}
