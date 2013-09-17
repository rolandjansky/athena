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
  : Algorithm(name, pSvcLocator),
    m_storeGate(0)
{
  declareProperty("ClusterContainerNames",m_clusterContainerNames);
  
}

//###############################################################################

CaloClusterNavTest::~CaloClusterNavTest()
{ }

//###############################################################################

StatusCode CaloClusterNavTest::initialize()
{
  MsgStream log(messageService(), name());

  StatusCode sg= service("StoreGateSvc", m_storeGate);
  
  if ( !sg.isSuccess() ) 
    log << MSG::ERROR << " Can not retrieve StoreGateSvc"
	<< endreq; 


  log << MSG::DEBUG << " initialized "	<< endreq; 
  return sg; //sg from "initialize the StoreGateSvc ptr"
  
}

//###############################################################################

StatusCode CaloClusterNavTest::finalize()
{
  MsgStream log(messageService(), name());

  return StatusCode::SUCCESS;

}

//###############################################################################

StatusCode CaloClusterNavTest::execute() {
  
  MsgStream log(messageService(), name());
  StatusCode sc;
 
  std::vector<std::string>::const_iterator it =
	m_clusterContainerNames.begin();
  std::vector<std::string>::const_iterator it_e =
	m_clusterContainerNames.end();

  for (; it!=it_e;++it)
   { 

	  const std::string & clusterName = *it; 
	  const CaloClusterContainer * clusterColl ;
	  sc = m_storeGate->retrieve(clusterColl,clusterName);
	  if( !sc.isSuccess() || !clusterColl){ 
	    log << MSG::ERROR << " Can not retrieve CaloClusterContainer: "
		<< clusterName << endreq; 
    		return StatusCode::FAILURE;      
	  }    

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

