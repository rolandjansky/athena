/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloTowerContainer.h"
#include "CaloEvent/CaloCell2ClusterMap.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloTopoTowerAlg.h"
#include <string>

///==============================================
/// DEFAULT CONSTRUCTOR
///==============================================
CaloTopoTowerAlg::CaloTopoTowerAlg(const std::string& name,ISvcLocator* pSvcLocator):
   AthReentrantAlgorithm(name, pSvcLocator),
   m_cellToClusterMapKey("CaloCell2TopoCluster"),
   m_cellContainerKey("AllCalo"),
   m_towerContainerKey("CmbTower"),
   m_newTowerContainerKey("TopoTower"),
   m_caloSelection(false)
{ 

  // Containers needed for this algorithm
  declareProperty("Cell2ClusterMapName",    m_cellToClusterMapKey);
  declareProperty("CellContainerName"  ,    m_cellContainerKey);
  declareProperty("InputTowerContainerName" ,    m_towerContainerKey);
  declareProperty("OutputTowerContainerName",    m_newTowerContainerKey );
  
  // Declare configurable properties of the algorithm
  declareProperty("MinimumCellEnergy",      m_minimumCellEnergy    = -1000000000.0);  
  declareProperty("MinimumClusterEnergy",   m_minimumClusterEnergy = -1000000000.0);  

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
  ATH_MSG_INFO( "Initializing CaloTopoTowerAlg"  );

  ATH_CHECK(m_cellToClusterMapKey.initialize());
  ATH_CHECK(m_cellContainerKey.initialize());
  ATH_CHECK(m_towerContainerKey.initialize());
  ATH_CHECK(m_newTowerContainerKey.initialize());
  
  ATH_CHECK(m_noiseCDOKey.initialize());

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
  if (!m_caloIndices.empty() && m_caloIndices.size()<nSubCalo) m_caloSelection=true;

  ATH_MSG_INFO( " Calo selection applied ? " << m_caloSelection  );
  if (m_caloSelection) {
    msg(MSG::INFO) << "   subcalo selected ";
    for (unsigned int iCalos=0;iCalos< m_includedCalos.size(); iCalos++ ) msg(MSG::INFO) << " " << m_includedCalos[iCalos];
    msg(MSG::INFO) << " " << endmsg;
  }

  // check setup
  return StatusCode::SUCCESS;
}

///==============================================
/// EXECUTE
///==============================================
StatusCode CaloTopoTowerAlg::execute (const EventContext& ctx) const
{
  /// Say hello
  ATH_MSG_DEBUG( "In CaloTopoTowerAlg::execute()"  );

  /// retrieve existing Tower container
  
  SG::ReadHandle<CaloTowerContainer> towerContainer( m_towerContainerKey, ctx );
  if ( !towerContainer.isValid()) {
     ATH_MSG_WARNING( " cannot retrieve tower container with key " << towerContainer.name()  );
     return StatusCode::SUCCESS;
  }


  /// create new tower container with same tower segmentation and record it to StoreGate
  SG::WriteHandle<CaloTowerContainer> newTowerContainer(m_newTowerContainerKey, ctx);  
  if (newTowerContainer.record( std::make_unique<CaloTowerContainer> (towerContainer->towerseg()) ).isFailure()){
    ATH_MSG_WARNING( " cannot record new tower container with key " << towerContainer.name()  );
    return StatusCode::SUCCESS;
  }
  newTowerContainer->init();

  /// get CaloCell container from StoreGate
  SG::ReadHandle<CaloCellContainer> theCells(m_cellContainerKey, ctx);
  if ( !theCells.isValid()) {
     ATH_MSG_WARNING( " cannot retrieve cell container with key " << theCells.name()  );
     return StatusCode::SUCCESS;
  }

  ///+++ pick up CaloCell2ClusterMap from StoreGate
  const CaloClusterContainer* clusterContainer = nullptr;
  
  SG::ReadHandle<CaloCell2ClusterMap> cellToClusterMap(  m_cellToClusterMapKey, ctx );
  
  if ( !cellToClusterMap.isValid() )
  {
    ATH_MSG_WARNING( "cannot retrieve CaloCell2ClusterMap with key <"
                     << cellToClusterMap.name() << ">" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "Successfully retrieved CaloCell2ClusterMap <"<< cellToClusterMap.name() << ">" );

  /// Get CaloNoise CDO
  SG::ReadCondHandle<CaloNoise> noiseHdl{m_noiseCDOKey,ctx};
  const CaloNoise* noise=*noiseHdl;


  ///+++ consistency: pick up CaloClusterContainer pointer from map
  CaloCell2ClusterMap::const_iterator fClusMap(cellToClusterMap->begin());
  CaloCell2ClusterMap::const_iterator lClusMap(cellToClusterMap->end());
  ATH_MSG_DEBUG( "Starting loop over Navigable CaloCell2ClusterMap"  );
  while ( clusterContainer == nullptr && fClusMap != lClusMap )
  {
    ATH_MSG_VERBOSE( "In loop over Navigable CaloCell2ClusterMap"  );
    if (*fClusMap) 
    {
      // Pick first Navigable and then ask first entry in this
      // Navigable for the pointer to the CaloClusterContainer.
      // This should be sufficient because all entries should 
      // have the same pointer. (TBC)
      ATH_MSG_DEBUG( "CaloCell2ClusterMap has entry"  );
      const nav_t* pNav = (*fClusMap);
      clusterContainer = pNav->getContainer(pNav->begin());
      ATH_MSG_DEBUG( "Successfully picked up CaloClusterContainer "  );
    }
    else fClusMap++;
  }
  
  // Make sure the cluster container is not NULL
  if ( clusterContainer == nullptr )
  {
    if (!theCells->empty() ) {  
      ATH_MSG_WARNING( "No cluster found from  CaloCell2ClusterMap, tool unusable" );
      return StatusCode::SUCCESS;
    }
    else {
      ATH_MSG_DEBUG( " empty calorimeter event .. return "  );
      return StatusCode::SUCCESS;
    }
  }
  else ATH_MSG_DEBUG( "Size of CaloClusterContainer = " << clusterContainer->size()  );
  
  ///+++ loop on towers
  //  (*towerIter) is the ITERATOR over TOWERS
  //  (*cellInTowerIter) is the ITERATOR over CELLS for this TOWER

  ATH_MSG_DEBUG( "Beginning loop over tower grid"  );

  for (const CaloTower* theTower : *towerContainer) {
    int towerIndex = towerContainer->getTowerIndex(theTower);

    CaloTower* newTower = newTowerContainer->getTower(towerIndex);
  
    ///+++ loop cells in old tower
    ATH_MSG_VERBOSE( "In loop over tower grid: tower eta-phi" << theTower->eta() << " " << theTower->phi()  );
    CaloTower::cell_iterator cellInTowerIter(theTower->cell_begin());
    CaloTower::cell_iterator lastCellInTower(theTower->cell_end());
    
    /// Various counters for keeping track of energy added to this tower
    double energyTower = 0.0;
    double totalAttachedClusterEnergy = 0.0;
    int    numberOfCellsInTower = 0;
    int    numberOfAttachedCellsInTower = 0;
    int    numberOfClustersInTower = 0;
    int    totalNumberOfCellsInAttachedClusters = 0;
    
    ///+++ Look at all cells in this tower
    ATH_MSG_VERBOSE( "Now looking at all cells in this tower"  );
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
        ATH_MSG_WARNING( " cannot find back cell index "  );
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
	noiseSigma=noise->getNoise(cell->ID(),cell->gain());
        if ( noiseSigma > 0. ) signedRatio = signedE/noiseSigma;
      }
      
      // DEBUGGING
      ATH_MSG_VERBOSE( "         Cell has E = " << signedE << "  eta,phi " << cell->eta() << " " << cell->phi()  );
      ATH_MSG_VERBOSE( "Cell has E in tower = " << cellEnergy  );
      ATH_MSG_VERBOSE( "   Cell noise sigma = " << noiseSigma  );
      ATH_MSG_VERBOSE( "  Cell noise signif = " << signedRatio  );

      /// Require that the cell have a minimum energy and energy significance
      if ( (signedE > m_minimumCellEnergy) && ( fabs(signedRatio) > m_cellESignificanceThreshold) )
      {
        // find clusters associated to this cell using the hash ID
        size_t cellIndex(cell->caloDDE()->calo_hash());
        ATH_MSG_VERBOSE( "Cell index from CaloCell2ClusterMap = " << cellIndex  );
        const nav_t* nav = (cellToClusterMap->operator[])(cellIndex);
        
        /// Require that the cell be used in at least one cluster
        if (!nav) {
          ATH_MSG_VERBOSE( "No Cluster container from this cell!"  );
        }
        else 
        {
          ///+++ Loop over all clusters associated to this cell
          ATH_MSG_VERBOSE( "Cell associated to N clusters = " << nav->size()  );
          for (const CaloCluster* clusterFromCell : *nav) {
            double eClusRaw  = clusterFromCell->getBasicEnergy();
            double eClus     = clusterFromCell->energy();
            ATH_MSG_VERBOSE( " Cluster Basic Energy  = " << eClusRaw  );
            ATH_MSG_VERBOSE( " Cluster Normal Energy = " << eClus  );
          
            /// filter clusters according to cluster energy
            if ( eClusRaw > m_minimumClusterEnergy )
            {
              ATH_MSG_VERBOSE( "Cluster has at least E > " << m_minimumClusterEnergy  );
 
              numberOfAttachedCellsInTower++;
              totalNumberOfCellsInAttachedClusters += clusterFromCell->getNumberOfCells(); 
              totalAttachedClusterEnergy += eClusRaw;
              energyTower += cellEnergy;
              numberOfClustersInTower++;           

              newTower->addUniqueCellNoKine(theCells.cptr(),globalIndex,weight, 10);
            
              // now that we found the cell in at least one cluster above threshold, stop looking at associated clusters
              ATH_MSG_VERBOSE( " -- Found at least one cluster passing cuts. 'break'"  );
              break;
            
            } // cluster filter
          } // clusters from cell loop
          ATH_MSG_VERBOSE( "Finished cluster loop"  );
        } // cluster associated to cell
      } // cell filter
    } // cell loop
    ATH_MSG_VERBOSE( "Finished cell loop"  );
    
    /// Set tower energy according to the energy from cells in clusters passing selections
    newTower->setE(energyTower);
    
    // Report some information about this Topo-Tower
    if (msgLvl(MSG::VERBOSE)) {
      ATH_MSG_VERBOSE( "" );
      ATH_MSG_VERBOSE( "Old/ new TopoTower energy from all cells               = " << theTower->e() << " " << newTower->e()  );
      ATH_MSG_VERBOSE( "TopoTower energy adding all cells in clusters = " << energyTower  );
      ATH_MSG_VERBOSE( "Total attached cluster energy                 = " << totalAttachedClusterEnergy  );
      ATH_MSG_VERBOSE( "Total number of attached clusters             = " << numberOfClustersInTower  );
      ATH_MSG_VERBOSE( "Number of cells in attached clusters          = " << totalNumberOfCellsInAttachedClusters  );
      ATH_MSG_VERBOSE( "Total number of cells originally in tower     = " << numberOfCellsInTower  );
      ATH_MSG_VERBOSE( "Total number of cells from clusters           = " << numberOfAttachedCellsInTower  );
      CaloTower::cell_iterator cellInTowerIter(newTower->cell_begin());
      CaloTower::cell_iterator lastCellInTower(newTower->cell_end());
      msg(MSG::VERBOSE) << " E*weight, eta, phi of cells in new tower ";
      for ( ; cellInTowerIter != lastCellInTower; cellInTowerIter++ ) {
         double weight                 = theTower->getCellWeight(cellInTowerIter);     // get the weight of this cell in the tower
         const CaloCell* cell = (*cellInTowerIter);
         if (!cell) continue;
         msg(MSG::VERBOSE) << cell->e()*weight << " " << cell->eta() << " " << cell->phi() << " / ";
      } 
      msg(MSG::VERBOSE) << endmsg;
    }
    
  } // tower loop
  
  ATH_MSG_DEBUG( "Finished creating TopoTowers"  );
  
  return StatusCode::SUCCESS;
  
}
