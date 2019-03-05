/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "CLHEP/Units/SystemOfUnits.h"

#include "GaudiKernel/AlgTool.h"

#include "GaudiKernel/Property.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
//#include "GaudiKernel/IChronoStatSvc.h"

#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/CaloCell_ID.h"

#include "CaloGeoHelpers/CaloPhiRange.h"

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloTower.h"
#include "CaloEvent/CaloTowerContainer.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

#include "CaloUtils/CaloTowerStore.h"

#include "CaloEvent/CaloTopoTowerContainer.h"
#include "CaloEvent/CaloCluster.h"

#include "CaloUtils/CaloTopoTowerBuilderTool.h"

#include <string>
#include <cmath>

CaloTopoTowerBuilderTool::CaloTopoTowerBuilderTool(const std::string& name,
					   const std::string& type,
					   const IInterface* parent)
  : CaloTopoTowerBuilderToolBase(name,type,parent),
    m_calo_dd_man(nullptr),
    m_calo_id(nullptr)
{
  declareInterface<ICaloTopoTowerBuilderToolBase>(this);    
}

CaloTopoTowerBuilderTool::~CaloTopoTowerBuilderTool()
{ 
}

/////////////////////////////
// Specific Initialization //
/////////////////////////////

// protected!
StatusCode CaloTopoTowerBuilderTool::initializeTool()
{
  ATH_MSG_INFO( "Initializing CaloTopoTowerAlg"  );

  ATH_CHECK( detStore()->retrieve (m_calo_dd_man, "CaloMgr") );
  m_calo_id   = m_calo_dd_man->getCaloCell_ID();

  return StatusCode::SUCCESS;
}

///////////////////
// Tower Builder //
///////////////////

StatusCode CaloTopoTowerBuilderTool::execute(CaloTopoTowerContainer* theTowers, const CaloCellContainer* /*theCells*/) const
{
  //////////////////////////////////////////////////////////////////////////////
  //Starting loading  variables from CaloTopoTowerContainer
  
  ATH_MSG_DEBUG("Loading variables from theTowers");
  const CaloTowerContainer* towerContainer=theTowers->GetTowers();
  const CaloClusterContainer* clusters=theTowers->GetClusters();
  const CaloCellContainer* Cells=theTowers->GetCells();

  if (!towerContainer || !clusters || !Cells) {
    msg(MSG::WARNING) << " Missing input container : ";
    if (!towerContainer) msg(MSG::WARNING) << " no tower ";
    if (!clusters) msg(MSG::WARNING) << " no TopoClusters ";
    if (!Cells) msg(MSG::WARNING) << " no Cells ";
    msg(MSG::WARNING) << "  .. no CaloTopoTowers are made " << endmsg;
    return StatusCode::SUCCESS;
  }
  const CaloCell2ClusterMap*  cellToClusterMap=theTowers->GetCellToClusterMap();
  bool delete_cellToClusterMap=false;
  if(cellToClusterMap==0  ){
    cellToClusterMap=CreateCaloCell2ClusterMap(clusters);
    delete_cellToClusterMap=true;
  }
  ATH_MSG_DEBUG( "Tower size "   << towerContainer->size()  );
  ATH_MSG_DEBUG( "Cluster size " << clusters->size()  );
  ATH_MSG_DEBUG( "Cell size "    << Cells->size()  );

  double minimumCellEnergy = theTowers->GetMinimumCellEnergy();
  double minimumClusterEnergy = theTowers->GetMinimumClusterEnergy();

  bool useCellWeights = theTowers->GetUseCellWeights();

  ATH_MSG_DEBUG("Energy cuts " << minimumCellEnergy << " " << minimumClusterEnergy << " " << useCellWeights);

  // Noise tool stuff
  bool useNoiseTool = theTowers->GetUseNoiseTool();
  bool usePileUpNoise = theTowers->GetUsePileUpNoise();
  float noiseSigma0 = theTowers->GetNoiseSigma();
  float cellESignificanceThreshold = theTowers->GetCellESignificanceThreshold();


  if (useNoiseTool) {
    ATH_MSG_WARNING( " Using noise tool in CaloTopoTowerBuilderTool no supported. give up => No CaloTopoTowers are made"  );
    if(delete_cellToClusterMap){
      ATH_MSG_DEBUG("Deleting cellToClusterMap Pointer");
      delete cellToClusterMap;
      ATH_MSG_DEBUG("Deleting cellToClusterMap Pointer Finished");
    }
    
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("Noise cuts "<< noiseSigma0 << " " <<  cellESignificanceThreshold << " " << useNoiseTool << " " << usePileUpNoise);

  // List of calorimeters from which to use cells
  std::vector<CaloCell_ID::SUBCALO> caloIndices = theTowers->GetCaloIndices();
  bool caloSelection = theTowers->GetCaloSelection();

  ATH_MSG_DEBUG("caloSelection " << caloSelection << " " << caloIndices.size());
  
  //Finished loading variables from CaloTopoTowerContainer 
  //////////////////////////////////////////////////////////////////////////////
  ///+++ consistency: pick up CaloClusterContainer pointer from map
  const CaloClusterContainer* clusterContainer = 0;
  CaloCell2ClusterMap::const_iterator fClusMap(cellToClusterMap->begin());
  CaloCell2ClusterMap::const_iterator lClusMap(cellToClusterMap->end());
  ATH_MSG_DEBUG("Starting loop over Navigable CaloCell2ClusterMap");
  while ( clusterContainer == 0 && fClusMap != lClusMap ){
    ATH_MSG_VERBOSE("In loop over Navigable CaloCell2ClusterMap");
    if (*fClusMap) {
      // Pick first Navigable and then ask first entry in this
      // Navigable for the pointer to the CaloClusterContainer.
      // This should be sufficient because all entries should 
      // have the same pointer. (TBC)
      ATH_MSG_DEBUG("CaloCell2ClusterMap has entry");
      const nav_t* pNav = (*fClusMap);
      clusterContainer = pNav->getContainer(pNav->begin());
      ATH_MSG_DEBUG("Successfully picked up CaloClusterContainer ");
    }
    else fClusMap++;
  }
  
  // Make sure the cluster container is not NULL
  if ( clusterContainer == 0 ) {
    if (Cells->size() >0 ) {  
      ATH_MSG_WARNING( "No cluster found from  CaloCell2ClusterMap, tool unusable" );
    }
    else {
      ATH_MSG_DEBUG(" empty calorimeter event .. return ");
    }

    if(delete_cellToClusterMap){
      ATH_MSG_DEBUG( "Deleting cellToClusterMap Pointer");
      delete cellToClusterMap;
      ATH_MSG_DEBUG("Deleting cellToClusterMap Pointer Finished");
    }
    
    return StatusCode::SUCCESS;
  }
  else 
    ATH_MSG_DEBUG("Size of CaloClusterContainer = " << clusterContainer->size());
   
  ///////////////////////////////////////////////////////////////////////////
  ///+++ loop on towers
  //  (*towerIter) is the ITERATOR over TOWERS
  //  (*cellInTowerIter) is the ITERATOR over CELLS for this TOWER

  ATH_MSG_DEBUG("Beginning loop over tower grid");
 
  CaloTowerContainer::const_iterator towerIter(towerContainer->begin());
  CaloTowerContainer::const_iterator lastTower(towerContainer->end());
  for ( ; towerIter != lastTower; towerIter++ )
   {
     const CaloTower* tower = (*towerIter);
      int towerIndex = towerContainer->getTowerIndex(tower);

      CaloTower* newTower = theTowers->getTower(towerIndex);

      ///+++ loop cells in old tower
      ATH_MSG_VERBOSE("In loop over tower grid: tower eta-phi" << tower->eta() << " " << tower->phi());
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
      ATH_MSG_VERBOSE("Now looking at all cells in this tower");
      for ( ; cellInTowerIter != lastCellInTower; cellInTowerIter++ ){
	numberOfCellsInTower++;
	// geometrical cell weight in towers
	// **** Note that in the header it says that this gets the kinematic weight
	// **** is this somehow different from the geometrical weight?
	double signedE                = 0.0;                         // zero-out the energy for this cell in case we can't get it from the map
	double weight                 = tower->getCellWeight(cellInTowerIter);     // get the weight of this cell in the tower
	
	const CaloCell* cell = (*cellInTowerIter);
	if (!cell) continue;
	
	size_t globalIndex=0;
	if (!(tower->getCellIndex(cell,globalIndex)) ) {
	  ATH_MSG_WARNING( " cannot find back cell index "  );
	  continue;
	}
	
	if (caloSelection) {
	  CaloCell_ID::SUBCALO iCaloNum = (cell->caloDDE()->getSubCalo());           // keep only cells from desired calorimeter
	  std::vector<CaloCell_ID::SUBCALO>::const_iterator theFound =
	    find (caloIndices.begin(),caloIndices.end(),iCaloNum);
	  if (theFound==caloIndices.end()) continue ;
	}
	
	signedE             = cell->e();                             // get the cell energy if we got a good pointer
	if (!useCellWeights) weight = 1.0;                         // if we chose not to use the cell weights, reset to 1.0
	double cellEnergy             = weight * signedE;             // calculate the energy of this cell in this tower using the weight
	
	
	float signedRatio=0;
	
	float noiseSigma = 1.0;
	if (cellESignificanceThreshold>=0.) {
	  noiseSigma = noiseSigma0;
	  if ( noiseSigma > 0. ) signedRatio = signedE/noiseSigma;
	}
	
	// WARNINGGING
        ATH_MSG_VERBOSE( "         Cell has E = " << signedE << "  eta,phi " << cell->eta() << " " << cell->phi()  );
        ATH_MSG_VERBOSE( "Cell has E in tower = " << cellEnergy  );
        ATH_MSG_VERBOSE( "   Cell noise sigma = " << noiseSigma  );
        ATH_MSG_VERBOSE( "  Cell noise signif = " << signedRatio  );
	/// Require that the cell have a minimum energy and energy significance
	if ( (signedE > minimumCellEnergy) && ( fabs(signedRatio) > cellESignificanceThreshold) ){
	  // find clusters associated to this cell using the hash ID
	  size_t cellIndex(cell->caloDDE()->calo_hash());
	  ATH_MSG_VERBOSE("Cell index from CaloCell2ClusterMap = " << cellIndex);
	  const nav_t* nav = (cellToClusterMap->operator[])(cellIndex);
	  
	  /// Require that the cell be used in at least one cluster
	  if (!nav) {
	    ATH_MSG_VERBOSE("No Cluster container from this cell!");
	  }
	  else{
	    ///+++ Loop over all clusters associated to this cell
	    ATH_MSG_VERBOSE("Cell associated to N clusters = " << nav->size());
	    nav_t::object_iter clusterIterator(nav->begin());
	    nav_t::object_iter lastCluster(nav->end());
	    for ( ; clusterIterator != lastCluster; clusterIterator++ ){
	      const CaloCluster* clusterFromCell = (*clusterIterator);
	      double eClusRaw  = clusterFromCell->getBasicEnergy();
	      double eClus     = clusterFromCell->energy();
	      ATH_MSG_VERBOSE( " Cluster Basic Energy  = " << eClusRaw);
	      ATH_MSG_VERBOSE( " Cluster Normal Energy = " << eClus);
	      
	      /// filter clusters according to cluster energy
	      if ( eClusRaw > minimumClusterEnergy ){
		ATH_MSG_VERBOSE("Cluster has at least E > " << minimumClusterEnergy);
		
		numberOfAttachedCellsInTower++;
		totalNumberOfCellsInAttachedClusters += clusterFromCell->getNumberOfCells();
		totalAttachedClusterEnergy += eClusRaw;
		energyTower += cellEnergy;
		numberOfClustersInTower++;
		
		newTower->addUniqueCellNoKine(Cells,globalIndex,weight, 10);
		
		// now that we found the cell in at least one cluster above threshold, stop looking at associated clusters
		ATH_MSG_VERBOSE(" -- Found at least one cluster passing cuts. 'break'");
		break;
		
	      } // cluster filter
	    } // clusters from cell loop
	    ATH_MSG_VERBOSE("Finished cluster loop");
	  } // cluster associated to cell
	} // cell filter
      } // cell loop
      ATH_MSG_VERBOSE("Finished cell loop");
      
      /// Set tower energy according to the energy from cells in clusters passing selections
      newTower->setE(energyTower);
      
      // Report some information about this Topo-Tower
      if (msgLvl(MSG::VERBOSE)) {
	msg() << endmsg;
	ATH_MSG_VERBOSE( "Old/ new TopoTower energy from all cells               = " << tower->e() << " " << newTower->e()  );
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
	  double weight                 = tower->getCellWeight(cellInTowerIter);     // get the weight of this cell in the tower
	  const CaloCell* cell = (*cellInTowerIter);
	  if (!cell) continue;
	  msg(MSG::VERBOSE) << cell->e()*weight << " " << cell->eta() << " " << cell->phi() << " / ";
	}
	msg(MSG::VERBOSE) << endmsg;
      }
      
   } // tower loop
  if(delete_cellToClusterMap){
    ATH_MSG_DEBUG("Deleting cellToClusterMap Pointer");
    delete cellToClusterMap;
    ATH_MSG_DEBUG("Deleting cellToClusterMap Pointer Finished");
  }
  ATH_MSG_DEBUG("Finished creating TopoTowers");
  
  return StatusCode::SUCCESS;
}




void CaloTopoTowerBuilderTool::handle(const Incident&) {
 
}

StatusCode CaloTopoTowerBuilderTool::LoadCalibration(IOVSVC_CALLBACK_ARGS)
{
  return StatusCode::SUCCESS;
}



const CaloCell2ClusterMap*
CaloTopoTowerBuilderTool::CreateCaloCell2ClusterMap(const CaloClusterContainer* clusColl) const
{
  ATH_MSG_DEBUG("CreateCaloCell2ClusterMap() Starting");
  CaloCell2ClusterMap *cell2ClusterMap;
  cell2ClusterMap = new CaloCell2ClusterMap();
  // resize it to total range of IdentifierHash for all calos
  Identifier::size_type maxRange = m_calo_id->calo_cell_hash_max();
  cell2ClusterMap->resize(maxRange);

  ATH_MSG_DEBUG("CaloCluster container contains " << clusColl->size() << " clusters");
  // loop over cluster collection and add each cluster to the map for 
  // each member cell
  CaloClusterContainer::const_iterator clusIter = clusColl->begin();
  CaloClusterContainer::const_iterator clusIterEnd = clusColl->end();
  unsigned int iClus=0;
  for( ;clusIter!=clusIterEnd;clusIter++,iClus++) {
    // loop over its cell members
    if  (((*clusIter)->getNumberOfCells()) == 0 ) {
      ATH_MSG_DEBUG(" no cells for this cluster... No reverse navigation possible...");
    }
    else {
      CaloCluster::cell_iterator cellIter    = (*clusIter)->cell_begin();
      CaloCluster::cell_iterator cellIterEnd = (*clusIter)->cell_end();
      for( ;cellIter!=cellIterEnd;cellIter++) {
        // look up the IdentifierHash for the current cell
        if (*cellIter) {
          IdentifierHash myHashId = m_calo_id->calo_cell_hash((*cellIter)->ID());
          // get the existing? Navigable for this cell
          Navigable<CaloClusterContainer> *theNav = (*cell2ClusterMap)[myHashId];
          if ( !theNav ) {
  	    // create a new Navigable if it doesn't exist
	    theNav  = new Navigable<CaloClusterContainer>();
	    // and store it in the vector
	    (*cell2ClusterMap)[myHashId] = theNav;
          }
          // add the current cluster to the list of clusters for this cell
          theNav->putElement(clusColl,*clusIter);
          // add the energy*weight for this cell to the weightedESum 
        }
      }
    }
  }
  ATH_MSG_DEBUG("CreateCaloCell2ClusterMap() Finished");
  return cell2ClusterMap;
}



