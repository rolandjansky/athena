/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///============================================================
/// CaloTopoTowerAlg.cxx, (c) ATLAS Detector software
/// Calorimeter/CaloRec/CaloTopoTowerAlg 
///
/// Build towers from raw cells that have been taken
/// out of topo clusters. In this way, we can create
/// noise suppressed raw towers to be saved in the AOD
/// and thus allows re-calibration and re-do MET.
///
/// Authors : 
/// \author Peter Loch <loch@physics.arizona.edu>
/// \author Ariel Schwartzman <sch@slac.stanford.edu>
/// \author David W. Miller <David.Miller@slac.stanford.edu>
///
/// Date : 25 February 2009
///============================================================

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloTowerContainer.h"
#include "CaloEvent/CaloCell2ClusterMap.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"
#include "CaloRec/CaloTopoTowerAlg.h"

#include <string>

///==============================================
/// DEFAULT CONSTRUCTOR
///==============================================
CaloTopoTowerAlg::CaloTopoTowerAlg(const std::string& name,ISvcLocator* pSvcLocator):
   AthAlgorithm(name, pSvcLocator),
   m_noiseTool("CaloNoiseToolDefault"),
   m_caloSelection(false)
{ 

  // Containers needed for this algorithm
  declareProperty("Cell2ClusterMapName",    m_cellToClusterMapName = "CaloCell2TopoCluster");
  declareProperty("CellContainerName"  ,    m_cellContainerName = "AllCalo");
  declareProperty("InputTowerContainerName" ,    m_towerContainerName = "CmbTower");
  declareProperty("OutputTowerContainerName",    m_newTowerContainerName = "TopoTower");
  
  // Declare configurable properties of the algorithm
  declareProperty("MinimumCellEnergy",      m_minimumCellEnergy    = -1000000000.0);  
  declareProperty("MinimumClusterEnergy",   m_minimumClusterEnergy = -1000000000.0);  

  // Noise Tool stuff
  declareProperty("CaloNoiseTool",          m_noiseTool            ,"Tool Handle for noise tool");
  declareProperty("DefaultNoiseSigma",      m_noiseSigma           = 10.0);
  declareProperty("UseCaloNoiseTool",       m_useNoiseTool         = true);
  declareProperty("UsePileUpNoise",         m_usePileUpNoise       = true);
  declareProperty("CellEnergySignificance", m_cellESignificanceThreshold = -1);
  
  // Calo from which to use cells
  declareProperty("IncludedCalos",          m_includedCalos);

  declareProperty("useCellWeight",          m_useCellWeights=true);

}

///==============================================
/// DESTRUCTOR
///==============================================
CaloTopoTowerAlg::~CaloTopoTowerAlg() 
{ 
}

///==============================================
/// finalize
///==============================================
StatusCode CaloTopoTowerAlg::finalize()
{
 return StatusCode::SUCCESS;
}

///==============================================
/// INITIALIZATION
///==============================================
StatusCode CaloTopoTowerAlg::initialize()
{
  // services
  MsgStream mLog(msgSvc(),name());
  mLog  << MSG::INFO << "Initializing CaloTopoTowerAlg" << endreq;

  // retrieve noise tool from the tool svc
  if (m_useNoiseTool) {
    if (m_noiseTool.retrieve().isFailure()) {
      mLog  << MSG::INFO << "Unable to find noise tool " << endreq;
      return StatusCode::FAILURE;
    }  
    mLog  << MSG::INFO << "Noise Tool retrieved" << endreq;
  }

  // Report some information regarding the noise tool
  if ( m_useNoiseTool && m_usePileUpNoise && (mLog.level() <= MSG::DEBUG)) {
    mLog << MSG::DEBUG 
         << "Pile-Up Noise from Noise Tool " 
         << " is selected! The noise sigma will be the"
         << " quadratic sum of the electronics noise and the pile up!" << endreq;
  }
  
  m_caloIndices.clear();
  for ( unsigned int iCalos=0; iCalos< m_includedCalos.size(); iCalos++ )
  {
    if  ( m_includedCalos[iCalos] == "LAREM" )
    {
      m_caloIndices.push_back(CaloCell_ID::LAREM);
    }
        else if ( m_includedCalos[iCalos] == "LARHEC")
    {
      m_caloIndices.push_back(CaloCell_ID::LARHEC);
    }
        else if ( m_includedCalos[iCalos] == "LARFCAL" )
    {
      m_caloIndices.push_back(CaloCell_ID::LARFCAL);
    }
        else if ( m_includedCalos[iCalos] == "TILE" )
    {
      m_caloIndices.push_back(CaloCell_ID::TILE);
    }
  }

  m_caloSelection=false;
  unsigned int nSubCalo=static_cast<int>(CaloCell_ID::NSUBCALO) ;
  if (m_caloIndices.size()>0 && m_caloIndices.size()<nSubCalo) m_caloSelection=true;

  mLog << MSG::INFO << " Calo selection applied ? " << m_caloSelection << endreq;
  if (m_caloSelection) {
    mLog << MSG::INFO << "   subcalo selected ";
    for (unsigned int iCalos=0;iCalos< m_includedCalos.size(); iCalos++ ) mLog << MSG::INFO << " " << m_includedCalos[iCalos];
    mLog << MSG::INFO << " " << endreq;
  }

  // check setup
  return StatusCode::SUCCESS;
}

///==============================================
/// EXECUTE
///==============================================
StatusCode CaloTopoTowerAlg::execute()
{
  /// Say hello
  MsgStream mLog(msgSvc(),name());
  if (mLog.level() <= MSG::DEBUG) mLog << MSG::DEBUG << "In CaloTopoTowerAlg::execute()" << endreq;

  /// retrieve existing Tower container
  const CaloTowerContainer* towerContainer=0;
  if ( (evtStore()->retrieve(towerContainer,m_towerContainerName)).isFailure()) {
     mLog << MSG::WARNING << " cannot retrieve tower container with key " << m_towerContainerName << endreq;
     return StatusCode::SUCCESS;
  }

  /// create new tower container with same tower segmentation and record it to StoreGate
  CaloTowerContainer* newTowerContainer = new CaloTowerContainer(towerContainer->towerseg());
  newTowerContainer->init();
  if ( (evtStore()->record(newTowerContainer,m_newTowerContainerName)).isFailure()) {
     mLog << MSG::WARNING << " cannot record new tower container with key " << m_newTowerContainerName << endreq;
    return StatusCode::SUCCESS;
  }

  /// get CaloCell container from StoreGate
  const CaloCellContainer* theCells =0;
  if ( (evtStore()->retrieve(theCells,m_cellContainerName)).isFailure()) {
     mLog << MSG::WARNING << " cannot retrieve cell container with key " << m_cellContainerName << endreq;
     return StatusCode::SUCCESS;
  }

  ///+++ pick up CaloCell2ClusterMap from StoreGate
  const CaloCell2ClusterMap*  cellToClusterMap = 0;
  const CaloClusterContainer* clusterContainer = 0;
  if ( (evtStore()->retrieve(cellToClusterMap,m_cellToClusterMapName)).isFailure() )
  {
    mLog  << MSG::WARNING
            << "cannot retrieve CaloCell2ClusterMap with key <"
            << m_cellToClusterMapName << ">" 
            << endreq;
    return StatusCode::SUCCESS;
  }
  if (mLog.level() <= MSG::DEBUG) mLog  << MSG::DEBUG << "Successfully retrieved CaloCell2ClusterMap <"<< m_cellToClusterMapName << ">"<< endreq;

  ///+++ consistency: pick up CaloClusterContainer pointer from map
  CaloCell2ClusterMap::const_iterator fClusMap(cellToClusterMap->begin());
  CaloCell2ClusterMap::const_iterator lClusMap(cellToClusterMap->end());
  if (mLog.level() <= MSG::DEBUG) mLog  << MSG::DEBUG << "Starting loop over Navigable CaloCell2ClusterMap" << endreq;
  while ( clusterContainer == 0 && fClusMap != lClusMap )
  {
    if (mLog.level() <= MSG::VERBOSE) mLog  << MSG::VERBOSE << "In loop over Navigable CaloCell2ClusterMap" << endreq;
    if (*fClusMap) 
    {
      // Pick first Navigable and then ask first entry in this
      // Navigable for the pointer to the CaloClusterContainer.
      // This should be sufficient because all entries should 
      // have the same pointer. (TBC)
      if (mLog.level() <= MSG::DEBUG) mLog  << MSG::DEBUG << "CaloCell2ClusterMap has entry" << endreq;
      const nav_t* pNav = (*fClusMap);
      clusterContainer = pNav->getContainer(pNav->begin());
      if (mLog.level() <= MSG::DEBUG) mLog  << MSG::DEBUG << "Successfully picked up CaloClusterContainer " << endreq;
    }
    else fClusMap++;
  }
  
  // Make sure the cluster container is not NULL
  if ( clusterContainer == 0 )
  {
    if (theCells->size() >0 ) {  
      mLog << MSG::WARNING
           << "No cluster found from  CaloCell2ClusterMap, tool unusable"
           << endreq;
         return StatusCode::SUCCESS;
    }
    else {
      if (mLog.level() <= MSG::DEBUG) mLog  << MSG::DEBUG << " empty calorimeter event .. return " << endreq;
      return StatusCode::SUCCESS;
    }
  }
  else if (mLog.level() <= MSG::DEBUG) mLog  << MSG::DEBUG << "Size of CaloClusterContainer = " << clusterContainer->size() << endreq;
  
  ///+++ loop on towers
  //  (*towerIter) is the ITERATOR over TOWERS
  //  (*cellInTowerIter) is the ITERATOR over CELLS for this TOWER

  if (mLog.level() <= MSG::DEBUG) mLog  << MSG::DEBUG << "Beginning loop over tower grid" << endreq;

  CaloTowerContainer::const_iterator towerIter(towerContainer->begin());
  CaloTowerContainer::const_iterator lastTower(towerContainer->end());
  for ( ; towerIter != lastTower; towerIter++ )
  {

    const CaloTower* theTower = (*towerIter);
    int towerIndex = towerContainer->getTowerIndex(theTower);

    CaloTower* newTower = newTowerContainer->getTower(towerIndex);
  
    ///+++ loop cells in old tower
    if (mLog.level() <= MSG::VERBOSE) mLog  << MSG::VERBOSE << "In loop over tower grid: tower eta-phi" << theTower->eta() << " " << theTower->phi() << endreq;
    CaloTower::cell_iterator cellInTowerIter((*towerIter)->cell_begin());
    CaloTower::cell_iterator lastCellInTower((*towerIter)->cell_end());
    
    /// Various counters for keeping track of energy added to this tower
    double energyTower = 0.0;
    double totalAttachedClusterEnergy = 0.0;
    int    numberOfCellsInTower = 0;
    int    numberOfAttachedCellsInTower = 0;
    int    numberOfClustersInTower = 0;
    int    totalNumberOfCellsInAttachedClusters = 0;
    
    ///+++ Look at all cells in this tower
    if (mLog.level() <= MSG::VERBOSE) mLog  << MSG::VERBOSE << "Now looking at all cells in this tower" << endreq;
    for ( ; cellInTowerIter != lastCellInTower; cellInTowerIter++ )
    {
      numberOfCellsInTower++;
      // geometrical cell weight in towers
      // **** Note that in the header it says that this gets the kinematic weight
      // **** is this somehow different from the geometrical weight?
      double signedE                = 0.0;                         // zero-out the energy for this cell in case we can't get it from the map
      double weight                 = theTower->getCellWeight(cellInTowerIter);     // get the weight of this cell in the tower

      const CaloCell* cell = (*cellInTowerIter);
      if (!cell) continue;
      
      size_t globalIndex=0;
      if (!(theTower->getCellIndex(cell,globalIndex)) ) {
        mLog << MSG::WARNING << " cannot find back cell index " << endreq;
        continue;
      }

      if (m_caloSelection) {
        CaloCell_ID::SUBCALO iCaloNum = (cell->caloDDE()->getSubCalo());           // keep only cells from desired calorimeter
        std::vector<CaloCell_ID::SUBCALO>::const_iterator theFound =
          find (m_caloIndices.begin(),m_caloIndices.end(),iCaloNum);
        if (theFound==m_caloIndices.end()) continue ;
      }

      signedE             = cell->e();                             // get the cell energy if we got a good pointer
      if (!m_useCellWeights) weight = 1.0;                         // if we chose not to use the cell weights, reset to 1.0
      double cellEnergy             = weight * signedE;             // calculate the energy of this cell in this tower using the weight

      
      float signedRatio=0;

      float noiseSigma = 1.0;
      if (m_cellESignificanceThreshold>=0.) {
        // Noise tool to calculate cell energy significance
        if ( m_useNoiseTool ) {
          if ( m_usePileUpNoise ) noiseSigma = m_noiseTool->getNoise(cell,ICalorimeterNoiseTool::TOTALNOISE);
          else noiseSigma = m_noiseTool->getNoise(cell,ICalorimeterNoiseTool::ELECTRONICNOISE);
        } 
        else noiseSigma = m_noiseSigma;
        if ( noiseSigma > 0. ) signedRatio = signedE/noiseSigma;
      }
      
      // DEBUGGING
      if (mLog.level() <= MSG::VERBOSE) {
       mLog  << MSG::VERBOSE << "         Cell has E = " << signedE << "  eta,phi " << cell->eta() << " " << cell->phi() << endreq;
       mLog  << MSG::VERBOSE << "Cell has E in tower = " << cellEnergy << endreq;
       mLog  << MSG::VERBOSE << "   Cell noise sigma = " << noiseSigma << endreq;
       mLog  << MSG::VERBOSE << "  Cell noise signif = " << signedRatio << endreq;
      }

      /// Require that the cell have a minimum energy and energy significance
      if ( (signedE > m_minimumCellEnergy) && ( fabs(signedRatio) > m_cellESignificanceThreshold) )
      {
        // find clusters associated to this cell using the hash ID
        size_t cellIndex(cell->caloDDE()->calo_hash());
        if (mLog.level() <= MSG::VERBOSE) mLog  << MSG::VERBOSE << "Cell index from CaloCell2ClusterMap = " << cellIndex << endreq;
        const nav_t* nav = (cellToClusterMap->operator[])(cellIndex);
        
        /// Require that the cell be used in at least one cluster
        if (!nav) {
          if (mLog.level() <= MSG::VERBOSE) mLog  << MSG::VERBOSE << "No Cluster container from this cell!" << endreq;
        }
        else 
        {
          ///+++ Loop over all clusters associated to this cell
          if (mLog.level() <= MSG::VERBOSE) mLog  << MSG::VERBOSE << "Cell associated to N clusters = " << nav->size() << endreq;
          nav_t::object_iter clusterIterator(nav->begin());
          nav_t::object_iter lastCluster(nav->end());
          for ( ; clusterIterator != lastCluster; clusterIterator++ )
          {
            const CaloCluster* clusterFromCell = (*clusterIterator);
            double eClusRaw  = clusterFromCell->getBasicEnergy();
            double eClus     = clusterFromCell->energy();
            if (mLog.level() <= MSG::VERBOSE) mLog  << MSG::VERBOSE << " Cluster Basic Energy  = " << eClusRaw << endreq;
            if (mLog.level() <= MSG::VERBOSE) mLog  << MSG::VERBOSE << " Cluster Normal Energy = " << eClus << endreq;
          
            /// filter clusters according to cluster energy
            if ( eClusRaw > m_minimumClusterEnergy )
            {
              if (mLog.level() <= MSG::VERBOSE) mLog  << MSG::VERBOSE << "Cluster has at least E > " << m_minimumClusterEnergy << endreq;
 
              numberOfAttachedCellsInTower++;
              totalNumberOfCellsInAttachedClusters += clusterFromCell->getNumberOfCells(); 
              totalAttachedClusterEnergy += eClusRaw;
              energyTower += cellEnergy;
              numberOfClustersInTower++;           

              newTower->addUniqueCellNoKine(theCells,globalIndex,weight, 10);
            
              // now that we found the cell in at least one cluster above threshold, stop looking at associated clusters
              if (mLog.level() <= MSG::VERBOSE) mLog  << MSG::VERBOSE << " -- Found at least one cluster passing cuts. 'break'" << endreq;
              break;
            
            } // cluster filter
          } // clusters from cell loop
          if (mLog.level() <= MSG::VERBOSE) mLog  << MSG::VERBOSE << "Finished cluster loop" << endreq;
        } // cluster associated to cell
      } // cell filter
    } // cell loop
    if (mLog.level() <= MSG::VERBOSE) mLog  << MSG::VERBOSE << "Finished cell loop" << endreq;
    
    /// Set tower energy according to the energy from cells in clusters passing selections
    newTower->setE(energyTower);
    
    // Report some information about this Topo-Tower
    if (mLog.level() <= MSG::VERBOSE) {
      mLog << endreq;
      mLog << MSG::VERBOSE << "Old/ new TopoTower energy from all cells               = " << theTower->e() << " " << newTower->e() << endreq;
      mLog << MSG::VERBOSE << "TopoTower energy adding all cells in clusters = " << energyTower << endreq;
      mLog << MSG::VERBOSE << "Total attached cluster energy                 = " << totalAttachedClusterEnergy << endreq;
      mLog << MSG::VERBOSE << "Total number of attached clusters             = " << numberOfClustersInTower << endreq;
      mLog << MSG::VERBOSE << "Number of cells in attached clusters          = " << totalNumberOfCellsInAttachedClusters << endreq;
      mLog << MSG::VERBOSE << "Total number of cells originally in tower     = " << numberOfCellsInTower << endreq;
      mLog << MSG::VERBOSE << "Total number of cells from clusters           = " << numberOfAttachedCellsInTower << endreq;     
      CaloTower::cell_iterator cellInTowerIter(newTower->cell_begin());
      CaloTower::cell_iterator lastCellInTower(newTower->cell_end());
      mLog <<  MSG::VERBOSE << " E*weight, eta, phi of cells in new tower ";
      for ( ; cellInTowerIter != lastCellInTower; cellInTowerIter++ ) {
         double weight                 = theTower->getCellWeight(cellInTowerIter);     // get the weight of this cell in the tower
         const CaloCell* cell = (*cellInTowerIter);
         if (!cell) continue;
         mLog << cell->e()*weight << " " << cell->eta() << " " << cell->phi() << " / ";
      } 
      mLog << MSG::VERBOSE << endreq;
    }
    
  } // tower loop
  
  if (mLog.level() <= MSG::DEBUG) mLog  << MSG::DEBUG << "Finished creating TopoTowers" << endreq;  
  
  return StatusCode::SUCCESS;
  
}
