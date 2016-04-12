/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloCell2ClusterMapper.cxx,v 1.6 2009-04-18 02:56:18 ssnyder Exp $
//
// Description: see CaloCell2ClusterMapper.h
// 
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "CaloRec/CaloCell2ClusterMapper.h"

//----------------------------
// This Class's Base Header --
//----------------------------

//---------------
// C++ Headers --
//---------------
#include <list>
#include <iterator>
#include <sstream>
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloCell2ClusterMap.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "AthAllocators/DataPool.h"
#include "AthenaKernel/errorcheck.h"


//###############################################################################
CaloCell2ClusterMapper::CaloCell2ClusterMapper(const std::string& name, 
					       ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_calo_dd_man(0),
    m_calo_id(0)
{

  // Name of Cell2Cluster Map to be registered in TDS
  declareProperty("MapOutputName",m_mapOutputName);  
  
  // Name of CaloClusterContainer to use as input
  declareProperty("ClustersName",m_clustersName);
}

//###############################################################################

CaloCell2ClusterMapper::~CaloCell2ClusterMapper()
{ }

//###############################################################################

StatusCode CaloCell2ClusterMapper::initialize() {
  const IGeoModelSvc *geoModel = 0;
  CHECK(service("GeoModelSvc", geoModel));

  // dummy parameters for the callback:
  int dummyInt = 0;
  std::list<std::string> dummyList;

  if (geoModel->geoInitialized()) {
    return geoInit(dummyInt, dummyList);
  }

  CHECK(detStore()->regFcn(&IGeoModelSvc::geoInit, geoModel,
        &CaloCell2ClusterMapper::geoInit, this));

  return StatusCode::SUCCESS;
}

StatusCode
CaloCell2ClusterMapper::geoInit(IOVSVC_CALLBACK_ARGS) {
  // pointer to detector manager:
  m_calo_dd_man = CaloDetDescrManager::instance();
  m_calo_id = m_calo_dd_man->getCaloCell_ID();

  return StatusCode::SUCCESS;
}

//###############################################################################

StatusCode CaloCell2ClusterMapper::finalize()
{
  return StatusCode::SUCCESS;
}

//###############################################################################

StatusCode CaloCell2ClusterMapper::execute() {

  // make a DataVector of Navigable CaloClusterContainer 
  ATH_MSG_DEBUG(" Recording Cell2Cluster Map " << m_mapOutputName);
  // make the Cell2ClusterMap
  CaloCell2ClusterMap *cell2ClusterMap = new CaloCell2ClusterMap(
#ifndef ATHENAHIVE
      SG::VIEW_ELEMENTS);
#else
      SG::OWN_ELEMENTS);
#endif


  // resize it to total range of IdentifierHash for all calos

  Identifier::size_type maxRange = m_calo_id->calo_cell_hash_max();
  cell2ClusterMap->resize(maxRange);

  CHECK(evtStore()->record(cell2ClusterMap, m_mapOutputName));

  const CaloClusterContainer * clusColl;
  CHECK(evtStore()->retrieve(clusColl, m_clustersName));

  ATH_MSG_DEBUG("CaloCluster container: " << m_clustersName
      <<" contains " << clusColl->size() << " clusters");

  std::vector<double> weightedESum;
  weightedESum.resize(clusColl->size());

  std::vector<int> numberOfCells;
  numberOfCells.resize(clusColl->size());

#ifndef ATHENAHIVE
  DataPool<Navigable<CaloClusterContainer> > navPool(maxRange);
#endif

  // loop over cluster collection and add each cluster to the map for 
  // each member cell
  CaloClusterContainer::const_iterator clusIter = clusColl->begin();
  CaloClusterContainer::const_iterator clusIterEnd = clusColl->end();
  unsigned int iClus = 0;
  for (; clusIter != clusIterEnd; clusIter++, iClus++) {
    // loop over its cell members
    CaloCluster::cell_iterator cellIter = (*clusIter)->cell_begin();
    CaloCluster::cell_iterator cellIterEnd = (*clusIter)->cell_end();
    for (; cellIter != cellIterEnd; cellIter++) {
      // look up the IdentifierHash for the current cell
      IdentifierHash myHashId = m_calo_id->calo_cell_hash((*cellIter)->ID());
      // get the existing? Navigable for this cell
      Navigable<CaloClusterContainer> *theNav = (*cell2ClusterMap)[myHashId];
      if (!theNav) {
        // create a new Navigable if it doesn't exist
#ifndef ATHENAHIVE	
        theNav = navPool.nextElementPtr();
#else
        theNav = new Navigable<CaloClusterContainer>();
#endif
        theNav->removeAll();
        // and store it in the vector
        (*cell2ClusterMap)[myHashId] = theNav;
      }
      // add the current cluster to the list of clusters for this cell
      theNav->putElement(clusColl, *clusIter);
      // add the energy*weight for this cell to the weightedESum 
      //if ( messageService()->outputLevel(name()) <= MSG::DEBUG) {
      if (msgLvl(MSG::DEBUG)) {
        weightedESum[iClus] += ((*clusIter)->getCellWeight(*cellIter))
            * ((*cellIter)->energy());
        numberOfCells[iClus]++;
      }
    }
  }

  // compare weighted sums per cluster from the map with cluster energies
  if (msgLvl(MSG::DEBUG)) {
    double epsilon = 0.001;
    CaloClusterContainer::const_iterator clusIter = clusColl->begin();
    CaloClusterContainer::const_iterator clusIterEnd = clusColl->end();
    unsigned int iClus = 0;
    for (; clusIter != clusIterEnd; clusIter++, iClus++) {
      if (fabs(weightedESum[iClus] - (*clusIter)->e())
          > epsilon * fabs((*clusIter)->e())) {
        msg(MSG::WARNING) << "CaloCluster has E = " << (*clusIter)->e()
            << " MeV, while weighted sum of cells gives E = "
            << weightedESum[iClus]
            << " MeV! Complain with the creator of the CaloClusterContainer named "
            << m_clustersName << endreq; //FIXME: WARNING printed only if msglevel <=
      } else {
        msg(MSG::DEBUG)
            << "CaloCluster and weighted sum of cell members have E = "
            << (*clusIter)->e() << " MeV, good!" << endreq;
      }
      if (numberOfCells[iClus] != static_cast<int>((*clusIter)->getNumberOfCells())) {
        msg(MSG::WARNING) << "CaloCluster has N = "
            << (*clusIter)->getNumberOfCells() << " cells, while N = "
            << numberOfCells[iClus]
            << " cells are in the map! Something is wrong with the creation of the Map!"
            << endreq; //FIXME: WARNING printed only if msglevel <=
      } else {
        msg(MSG::DEBUG) << "CaloCluster with N = "
            << (*clusIter)->getNumberOfCells() << " cells is in the map, good!"
            << endreq;
      }
    } //end loop over clusters
  } //end if DEBUG

  // lock the Map
  CHECK(evtStore()->setConst(cell2ClusterMap));

  // print the Map
  if (msgLvl(MSG::VERBOSE)) {
    for (unsigned int iHash = 0; iHash < maxRange; iHash++) {
      // get the existing? Navigable for this cell
      Navigable<CaloClusterContainer> *theNav = (*cell2ClusterMap)[iHash];
      if (theNav) {
        msg(MSG::VERBOSE) << "CaloCell with hashID " << iHash << " belongs to";
        Navigable<CaloClusterContainer>::object_iter navIter = theNav->begin();
        Navigable<CaloClusterContainer>::object_iter navIterEnd = theNav->end();
        for (; navIter != navIterEnd; navIter++) {
          const CaloCluster *pClus = (*navIter);
          // the next line assumes all cells of the cluster belong to the same
          // cell container ...
          const CaloCellContainer *cellColl = pClus->getCellContainer(
              *(pClus->cell_begin()));
          const CaloCell* pCell = cellColl->findCell(iHash);
          if (!pCell) {
            // need to find the cell in clusters member list because the
            // cluster is made of more than one cell container
            CaloCluster::cell_iterator cellIter = pClus->cell_begin();
            CaloCluster::cell_iterator cellIterEnd = pClus->cell_end();
            for (; cellIter != cellIterEnd; cellIter++) {
              pCell = (*cellIter);
              // look up the IdentifierHash for the current cell
              IdentifierHash myHashId = m_calo_id->calo_cell_hash(pCell->ID());
              if (((unsigned int) myHashId) == iHash)
                break;
              else
                pCell = 0;
            }
          }
          if (pCell)
            msg() << " Cluster with E_clus = " << pClus->e()
                << " MeV with weight w_cell = " << pClus->getCellWeight(pCell)
                << " ";
        }
        msg() << endreq;
      }
    }
  }

  return StatusCode::SUCCESS;
}

