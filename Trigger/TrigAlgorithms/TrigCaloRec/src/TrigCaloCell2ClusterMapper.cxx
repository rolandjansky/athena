/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// Modified for trigger
//
// File and Version Information:
// $Id: TrigCaloCell2ClusterMapper.cxx 765962 2016-08-02 02:51:36Z ssnyder $
//
// Description: see TrigCaloCell2ClusterMapper.h
// 
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
//
//-----------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "TrigCaloRec/TrigCaloCell2ClusterMapper.h"

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

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

#include "Gaudi/Property.h"

//###############################################################################
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
TrigCaloCell2ClusterMapper::TrigCaloCell2ClusterMapper(const std::string& name, 
					       ISvcLocator* pSvcLocator) 
  : HLT::FexAlgo(name, pSvcLocator)  
    ,m_calo_dd_man(0)
    ,m_calo_id(0)
{


  // Name of Cell2Cluster Map to be registered in TDS
  declareProperty("MapOutputName",m_mapOutputName="TrigCaloCell2ClusterMap");  
  
}

//###############################################################################
    // DESTRUCTOR:
TrigCaloCell2ClusterMapper::~TrigCaloCell2ClusterMapper()
{ }

//###############################################################################

//StatusCode TrigCaloCell2ClusterMapper::initialize()
HLT::ErrorCode TrigCaloCell2ClusterMapper::hltInitialize()
{
  ATH_MSG_DEBUG( "in initialize() TrigCaloCell2ClusterMapper ");

  m_calo_dd_man  = CaloDetDescrManager::instance(); 
  m_calo_id   = m_calo_dd_man->getCaloCell_ID();

  return HLT::OK;
}

//###############################################################################

//StatusCode TrigCaloCell2ClusterMapper::finalize()
	   
HLT::ErrorCode TrigCaloCell2ClusterMapper::hltFinalize()
{
  ATH_MSG_DEBUG ("in finalize() TrigCaloCell2ClusterMapper ");

  return HLT::OK;
}

//###############################################################################

HLT::ErrorCode TrigCaloCell2ClusterMapper::hltExecute(const HLT::TriggerElement* /*inputTE*/,
						HLT::TriggerElement* outputTE)
{
  
  ATH_MSG_DEBUG("in execute()  TrigCaloCell2ClusterMapper");

  /////////////////////////////////////////////////////////////////////////////////////
  // make the Cell2ClusterMap
  CaloCell2ClusterMap *cell2ClusterMap;
  cell2ClusterMap = new CaloCell2ClusterMap();

  // resize it to total range of IdentifierHash for all calos                                                                
  Identifier::size_type maxRange = m_calo_id->calo_cell_hash_max();
  cell2ClusterMap->resize(maxRange);

  //////CaloCluster Container
  const CaloClusterContainer * clusColl = 0;
  HLT::ErrorCode sc = getFeature(outputTE, clusColl);
  if(sc!=HLT::OK || !clusColl) { ATH_MSG_ERROR("Failed to retrieved TopoClusterContainer"); return HLT::NAV_ERROR; }


  std::vector<double> weightedESum;
  weightedESum.resize(clusColl->size());

  std::vector<int> numberOfCells;
  numberOfCells.resize(clusColl->size());


  // loop over cluster collection and add each cluster to the map for each member cell                                                                                                        
  CaloClusterContainer::const_iterator clusIter = clusColl->begin();
  CaloClusterContainer::const_iterator clusIterEnd = clusColl->end();
  unsigned int iClus=0;
  for( ;clusIter!=clusIterEnd;clusIter++,iClus++) {

    // loop over its cell members                                                                                            
    CaloCluster::cell_iterator cellIter    = (*clusIter)->cell_begin();
    CaloCluster::cell_iterator cellIterEnd = (*clusIter)->cell_end();

    for( ;cellIter!=cellIterEnd;cellIter++) {
      // look up the IdentifierHash for the current cell                                                                     
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
    
      weightedESum[iClus] += ((*clusIter)->getCellWeight(*cellIter))*((*cellIter)->energy());
      numberOfCells[iClus]++;

    }
    
  }


  // compare weighted sums per cluster from the map with cluster energies                                                    
  if (msg().level() <= MSG::DEBUG) {
    double epsilon = 0.001;

    CaloClusterContainer::const_iterator clusIter = clusColl->begin();
    CaloClusterContainer::const_iterator clusIterEnd = clusColl->end();

    unsigned int iClus=0;
    for( ;clusIter!=clusIterEnd;clusIter++,iClus++) {
      if ( fabs(weightedESum[iClus]-(*clusIter)->e()) > epsilon*fabs((*clusIter)->e()) ) {
        ATH_MSG_WARNING("CaloCluster has E = " << (*clusIter)->e() << " MeV, while weighted sum of cells gives E = " << weightedESum[iClus] << " MeV! Complain with the creator of the CaloClusterContainer");
      }
      else {
        ATH_MSG_VERBOSE("CaloCluster and weighted sum of cell members have E = " << (*clusIter)->e() << " MeV, good!");
      }
      if ( std::abs((int)(numberOfCells[iClus]-(*clusIter)->getNumberOfCells())) > 0 ) {
        ATH_MSG_WARNING("CaloCluster has N = " << (*clusIter)->getNumberOfCells() << " cells, while N = " << numberOfCells[iClus] << " cells are in the map! Something is wrong with the creation of the Map!");
      }
      else {
        ATH_MSG_VERBOSE("CaloCluster with N = " << (*clusIter)->getNumberOfCells() << " cells is in the map, good!");
      }
    }
  }



  // attachFeature
 sc = attachFeature(outputTE, cell2ClusterMap, m_mapOutputName );
 if (sc != HLT::OK) {
    ATH_MSG_ERROR("Write of CaloCell2ClusterMap into outputTE failed");
    return HLT::NAV_ERROR;
  }

  // lock the Map
  StatusCode scMap = store()->setConst(cell2ClusterMap);
  if( !scMap.isSuccess() ) {
    ATH_MSG_WARNING("Can not lock the Cell2ClusterMap");
  }


  // print the Map                                                                                                                               
if (msg().level() <= MSG::VERBOSE) {
  for ( unsigned int iHash =0; iHash < maxRange; iHash++) {
    // get the existing? Navigable for this cell                                                                                               
    Navigable<CaloClusterContainer> *theNav = (*cell2ClusterMap)[iHash];
    if ( theNav ) {
      msg()<<MSG::VERBOSE<<"CaloCell with hashID " << iHash << " belongs to";
      Navigable<CaloClusterContainer>::object_iter navIter    = theNav->begin();
      Navigable<CaloClusterContainer>::object_iter navIterEnd = theNav->end();
      for( ;navIter!=navIterEnd;navIter++) {
	const CaloCluster *pClus = (*navIter);
	// the next line assumes all cells of the cluster belong to the same                                                                   
	// cell container ...                                                                                                                  
	const CaloCellContainer *cellColl = pClus->getCellContainer(*(pClus->cell_begin()));
	const CaloCell* pCell = cellColl->findCell(iHash);
	if ( !pCell ) {
	  // need to find the cell in clusters member list because the                                                                         
	  // cluster is made of more than one cell container                                                                                   
	  CaloCluster::cell_iterator cellIter    = pClus->cell_begin();
	  CaloCluster::cell_iterator cellIterEnd = pClus->cell_end();
	  for(; cellIter != cellIterEnd; cellIter++ ){
	    pCell = (*cellIter);
	    // look up the IdentifierHash for the current cell                                                                                 
	    IdentifierHash myHashId = m_calo_id->calo_cell_hash(pCell->ID());
	    if ( ((unsigned int)myHashId) == iHash )
	      break;
	    else
	      pCell = 0;
	  }
	}
	if ( pCell )
	  msg()<<MSG::VERBOSE << " Cluster with E_clus = " << pClus->e() << " MeV with weight w_cell = " << pClus->getCellWeight(pCell) << endmsg;
      }
    }
  }

} //if debug

  return HLT::OK;
}

