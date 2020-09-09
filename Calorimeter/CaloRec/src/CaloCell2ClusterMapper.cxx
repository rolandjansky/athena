/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "CaloCell2ClusterMapper.h"

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
#include "AthAllocators/DataPool.h"
#include "AthenaKernel/errorcheck.h"


//###############################################################################
CaloCell2ClusterMapper::CaloCell2ClusterMapper(const std::string& name, 
					       ISvcLocator* pSvcLocator) 
  : AthReentrantAlgorithm(name, pSvcLocator),
    m_mapOutputKey(""),
    m_clusterKey("")
{

  // Name of Cell2Cluster Map to be registered in TDS
  declareProperty("MapOutputName",m_mapOutputKey);  
  
  // Name of CaloClusterContainer to use as input
  declareProperty("ClustersName",m_clusterKey);
}

//###############################################################################

CaloCell2ClusterMapper::~CaloCell2ClusterMapper()
{ }

//###############################################################################

StatusCode CaloCell2ClusterMapper::initialize() {
  return StatusCode::SUCCESS;
}

//###############################################################################

StatusCode CaloCell2ClusterMapper::finalize()
{
  return StatusCode::SUCCESS;
}

//###############################################################################

StatusCode CaloCell2ClusterMapper::execute(const EventContext& ctx) const {

  // make a DataVector of Navigable CaloClusterContainer 
  ATH_MSG_DEBUG(" Recording Cell2Cluster Map " << m_mapOutputKey.key());

  SG::WriteHandle<CaloCell2ClusterMap> cell2ClusterMap ( m_mapOutputKey, ctx );
  ATH_CHECK( cell2ClusterMap.record(std::make_unique<CaloCell2ClusterMap>
                                    (SG::VIEW_ELEMENTS)) );

   const CaloCell_ID*               calo_id  = nullptr;
   ATH_CHECK(detStore()->retrieve(calo_id,"CaloCell_ID"));

  // resize it to total range of IdentifierHash for all calos
  Identifier::size_type maxRange = calo_id->calo_cell_hash_max();
  cell2ClusterMap->resize(maxRange);
  
  SG::ReadHandle<CaloClusterContainer> clusColl(m_clusterKey, ctx);
  

  ATH_MSG_DEBUG("CaloCluster container: " << clusColl.name()
      <<" contains " << clusColl->size() << " clusters");

  std::vector<double> weightedESum;
  weightedESum.resize(clusColl->size());

  std::vector<int> numberOfCells;
  numberOfCells.resize(clusColl->size());

  DataPool<Navigable<CaloClusterContainer> > navPool(maxRange);

  // loop over cluster collection and add each cluster to the map for 
  // each member cell
  unsigned int iClus = 0;
  for (const CaloCluster* clus : *clusColl) {
    // loop over its cell members
    CaloCluster::cell_iterator cellIter = clus->cell_begin();
    CaloCluster::cell_iterator cellIterEnd = clus->cell_end();
    for (; cellIter != cellIterEnd; cellIter++) {
      // look up the IdentifierHash for the current cell
      IdentifierHash myHashId = calo_id->calo_cell_hash((*cellIter)->ID());
      // get the existing? Navigable for this cell
      Navigable<CaloClusterContainer> *theNav = (*cell2ClusterMap)[myHashId];
      if (!theNav) {
        // create a new Navigable if it doesn't exist
        theNav = navPool.nextElementPtr();
        theNav->removeAll();
        // and store it in the vector
        (*cell2ClusterMap)[myHashId] = theNav;
      }
      // add the current cluster to the list of clusters for this cell
      theNav->putElement(clusColl.cptr(), clus);
      // add the energy*weight for this cell to the weightedESum 
      //if ( messageService()->outputLevel(name()) <= MSG::DEBUG) {
      if (msgLvl(MSG::DEBUG)) {
        weightedESum[iClus] += (clus->getCellWeight(*cellIter))
            * ((*cellIter)->energy());
        numberOfCells[iClus]++;
      }
    }
  }

  // compare weighted sums per cluster from the map with cluster energies
  if (msgLvl(MSG::DEBUG)) {
    double epsilon = 0.001;
    unsigned int iClus = 0;
    for (const CaloCluster* clus : *clusColl) {
      if (fabs(weightedESum[iClus] - clus->e())
          > epsilon * fabs(clus->e()))
      {
        //FIXME: WARNING printed only if msglevel <=
        ATH_MSG_WARNING( "CaloCluster has E = " << clus->e()
                         << " MeV, while weighted sum of cells gives E = "
                         << weightedESum[iClus]
                         << " MeV! Complain with the creator of the CaloClusterContainer named "
                         << m_clusterKey.key()  );
      } else {
        ATH_MSG_DEBUG( "CaloCluster and weighted sum of cell members have E = "
                       << clus->e() << " MeV, good!"  );
      }
      if (numberOfCells[iClus] != static_cast<int>(clus->getNumberOfCells())) {
        //FIXME: WARNING printed only if msglevel <=
        ATH_MSG_WARNING( "CaloCluster has N = "
                         << clus->getNumberOfCells() << " cells, while N = "
                         << numberOfCells[iClus]
                         << " cells are in the map! Something is wrong with the creation of the Map!" );
      } else {
        ATH_MSG_DEBUG( "CaloCluster with N = "
                       << clus->getNumberOfCells() << " cells is in the map, good!" );
      }
    } //end loop over clusters
  } //end if DEBUG

  // print the Map
  if (msgLvl(MSG::VERBOSE)) {
    for (unsigned int iHash = 0; iHash < maxRange; iHash++) {
      // get the existing? Navigable for this cell
      Navigable<CaloClusterContainer> *theNav = (*cell2ClusterMap)[iHash];
      if (theNav) {
        msg(MSG::VERBOSE) << "CaloCell with hashID " << iHash << " belongs to";
        for (const CaloCluster* pClus : *theNav) {
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
              IdentifierHash myHashId = calo_id->calo_cell_hash(pCell->ID());
              if (((unsigned int) myHashId) == iHash)
                break;
              else
                pCell = nullptr;
            }
          }
          if (pCell)
            msg() << " Cluster with E_clus = " << pClus->e()
                << " MeV with weight w_cell = " << pClus->getCellWeight(pCell)
                << " ";
        }
        msg() << endmsg;
      }
    }
  }

  return StatusCode::SUCCESS;
}

