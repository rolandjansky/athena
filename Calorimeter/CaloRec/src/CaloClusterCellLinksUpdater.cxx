///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CaloClusterCellLinksUpdater.cxx 
// Implementation file for class CaloClusterCellLinksUpdater
// Author: Walter Lampl
/////////////////////////////////////////////////////////////////// 

// Calo includes
#include "CaloRec/CaloClusterCellLinksUpdater.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "AthenaKernel/errorcheck.h"

CaloClusterCellLinksUpdater::CaloClusterCellLinksUpdater( const std::string& name, 
							  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_firstEvent(true) {
  declareProperty("NewCaloCellName", m_newCaloCellName="AODCellContainer",
		  "Name of the CaloCellContainer the links should point to"
		  );
  declareProperty("CaloClusterNames", m_caloClusterNames,
		  " CaloClusters to be considered"
		  );
}

CaloClusterCellLinksUpdater::~CaloClusterCellLinksUpdater()
{}


StatusCode CaloClusterCellLinksUpdater::initialize() {

  //Create a set of cluster names (clear out duplicate names):
  m_caloClusterNameSet.insert(m_caloClusterNames.begin(),m_caloClusterNames.end());

  if (msgLvl(MSG::INFO)) {
    msg(MSG::INFO) << "Will update links for clusters in container(s): ";
    for (auto clusterName : m_caloClusterNameSet) 
      msg(MSG::INFO) << clusterName << ", ";    
    msg(MSG::INFO) << "based on the CaloCellContainer " << m_newCaloCellName << endreq;
  }
  return StatusCode::SUCCESS;
}

StatusCode CaloClusterCellLinksUpdater::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode CaloClusterCellLinksUpdater::execute() {  
  

  //On first event check if all containers exist. Remove missing ones
  if (m_firstEvent) {
    m_firstEvent = false;
    std::set<std::string>::iterator it1 = m_caloClusterNameSet.begin();
    while (it1 != m_caloClusterNameSet.end()) {
      if (evtStore()->contains < xAOD::CaloClusterContainer > (*it1)) {
        ++it1; //Container found, everything ok
      } else {
        ATH_MSG_WARNING( "Cluster container with key " << *it1
            << " not found on first event. Removing from list of clusters." );

        // This is c++11.  We're compiling with c++11; however, it's not
        // safe to use this if we're linking against code compiled with
        // c++98, as is ROOT currently.
        //it1 = m_caloClusterNameSet.erase(it1);
        auto it_erase = it1;
        ++it1;
        m_caloClusterNameSet.erase (it_erase);
      }
    } // end loop over cluster container names
  }


  //retrieve new CaloCellContainer
  const CaloCellContainer* cellCont;
  CHECK(evtStore()->retrieve(cellCont,m_newCaloCellName));

  //Loop over cluster names
  for (auto clusterName : m_caloClusterNameSet) {
    ATH_MSG_DEBUG("Now working on cluster container with key " << clusterName);

    const xAOD::CaloClusterContainer* clusterCont;
    CHECK(evtStore()->retrieve(clusterCont,clusterName));

    //Loop over clusters in cluster container
    for (auto cluster : *clusterCont) {
      //Get CaloCellLinkContainer:
      const CaloClusterCellLink* cellLinks_cst=cluster->getCellLinks();
      if (!cellLinks_cst) {
	ATH_MSG_WARNING("Container " << clusterName << ", encountered cluster without cell links.");
	continue;
      }
      //cast-away const-ness. Note that this algo is a friend of <CaloClusterCellLink
      CaloClusterCellLink* cellLinks=const_cast<CaloClusterCellLink*>(cellLinks_cst);
      
      //use non-const iterators to loop over cells in cluster
      CaloClusterCellLink::iterator lnk_it=cellLinks->begin();
      CaloClusterCellLink::iterator lnk_it_e=cellLinks->end();
      for (;lnk_it!=lnk_it_e;++lnk_it) {
	//Ask cell for it's hash
	const IdentifierHash cellHash = (*lnk_it)->caloDDE()->calo_hash();
	const int newIdx=cellCont->findIndex(cellHash);
	if (newIdx<0) {
	  msg(MSG::ERROR) << "Cell with hash " << cellHash << " not found in new cell container with key "  
			  << m_newCaloCellName << endreq;
	  return StatusCode::FAILURE; //FIXME: More robust handling
	}
	
	//Update link:
	ATH_MSG_VERBOSE("Cell with hash " << cellHash << ": Replace index " << lnk_it.m_it->first << " with " << newIdx);
	lnk_it.m_it->first=newIdx;
      }//end loop over cells in this cluster

      //Replace cell container ptr (DataLink):
      ATH_MSG_VERBOSE("Replacing DataLink to CellContainer");
      cellLinks->m_cellCont=cellCont;
      //cellLinks->m_cellCont=DataLink<CaloCellContainer>(cellCont);
    }//end loop over clusters in cluster container
  }//end loop over cluster names
  
  return StatusCode::SUCCESS;
}

