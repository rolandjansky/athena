/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArEventTest/CaloClusterNavTest.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloClusterContainer.h"


#include "StoreGate/StoreGateSvc.h" 


//###############################################################################
CaloClusterNavTest::CaloClusterNavTest(const std::string& name, 
					       ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator)
{
  declareProperty("ClusterContainerNames",m_clusterContainerNames);
}

//###############################################################################

CaloClusterNavTest::~CaloClusterNavTest()
{ }

//###############################################################################

StatusCode CaloClusterNavTest::initialize()
{
  ATH_MSG_DEBUG ( " initialized "	);
  return StatusCode::SUCCESS;
}

//###############################################################################

StatusCode CaloClusterNavTest::finalize()
{
  return StatusCode::SUCCESS;
}

//###############################################################################

StatusCode CaloClusterNavTest::execute() {
  
  std::vector<std::string>::const_iterator it =
	m_clusterContainerNames.begin();
  std::vector<std::string>::const_iterator it_e =
	m_clusterContainerNames.end();

  for (; it!=it_e;++it)
   { 

	  const std::string & clusterName = *it; 
	  const CaloClusterContainer * clusterColl ;
	  ATH_CHECK( evtStore()->retrieve(clusterColl,clusterName) );

	  //Got ClusterContainer, loop over clusters;
	  CaloClusterContainer::const_iterator it_cluster=clusterColl->begin();
	  CaloClusterContainer::const_iterator it_cluster_e=clusterColl->end();
	  unsigned clusterCounter=0;
	  for(;it_cluster!=it_cluster_e;++it_cluster) {
	    
	    CaloCluster::cell_iterator it_cell=(*it_cluster)->cell_begin();
	    CaloCluster::cell_iterator it_cell_e=(*it_cluster)->cell_end();
	    for(;it_cell!=it_cell_e;++it_cell) {                
	      double weight = it_cell.getParameter();
	      Identifier id=(*it_cell)->ID();
	      std::cout << "NavTest: " << clusterName << "[" << clusterCounter << "]: Cell " 
			<< id.get_compact() <<", Weight:" << weight << std::endl; 
	     }
	    clusterCounter++;
	  }
	  
   }
 
  return StatusCode::SUCCESS;
}

