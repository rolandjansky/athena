/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// include header files
#include "CaloClusterRemoveDuplicates.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "GaudiKernel/MsgStream.h"
#include "AthContainers/DataVector.h"

/********************************************************************

NAME:     CaloClusterRemoveDuplicates.cxx

AUTHORS:  M. Cooke
CREATED:  May 2008

PURPOSE:  Remove duplicate clusters (pathology under investigation)

********************************************************************/


// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------

CaloClusterRemoveDuplicates::CaloClusterRemoveDuplicates(const std::string& type,
							 const std::string& name,
							 const IInterface* parent)
  : AthAlgTool(type, name, parent)  
{ 
  declareProperty ("deta_cut", m_deta_cut = 0.05 );
  declareProperty ("dphi_cut", m_dphi_cut = 0.05 );
}

StatusCode
CaloClusterRemoveDuplicates::execute(const EventContext& /*ctx*/,
                                     xAOD::CaloClusterContainer*  clusColl) const
{
  
  ATH_MSG_DEBUG( "Executing CaloClusterRemoveDuplicates" << endmsg); 
  
  using clus_iterator = xAOD::CaloClusterContainer::iterator;
  clus_iterator iter1 = clusColl->begin();
    
  ATH_MSG_DEBUG( "Collection has before dup rem size: " << clusColl->size() << endmsg);
  for( ;iter1!=clusColl->end(); ) {
    int comparison = 0;
    
    clus_iterator iter2 = iter1;
    for( iter2++ ; iter2!=clusColl->end() ; ){
      // Compare cluster pairs
      comparison =  compare( (*iter1) , (*iter2) ); 
      
      if( comparison == 1 ){
	break; // Want to delete 1, break out of loop over 2nd list and delete below
      }
      else if( comparison == 2 ){
	//std::cout << "about to delete 2" << std::endl;
	// Want to delete 2, delete here and continue through 2nd list
	iter2 = clusColl->erase(iter2);
      } 
      else{
	// No match, keep going through 2nd list
	iter2++;
      }
    }
    
    if( comparison == 1){
      //std::cout << "about to delete 1" << std::endl;
      // Want to delete 1, delete here and continue through 1st list
      iter1 = clusColl->erase(iter1);
    }
    else{
      // Did not need to delete 1, continue going through 1st list 
      iter1++;
    }
  } 
  ATH_MSG_DEBUG( "Collection has after dup rem size: " << clusColl->size() << endmsg);

  return StatusCode::SUCCESS;
}


int CaloClusterRemoveDuplicates::compare( xAOD::CaloCluster* clus1 ,
                                          xAOD::CaloCluster* clus2 ) const
{
  
  double deta = fabs ( clus1->eta() - clus2->eta() );
  double dphi = fabs ( clus1->phi() - clus2->phi() );
  
  // Check for special equivalence by 2pi case
  if( fabs( dphi - 6.283185 ) < m_dphi_cut ) dphi = dphi - 6.283185; 
  
  if( deta <= m_deta_cut && dphi <= m_dphi_cut ){
    
    if( clus2->et() > clus1->et() ){ // Cluster 2 has greater E, request to eliminate Cluster 1
      return 1;
    } 
    else if( clus1->et() >= clus2->et() ){ // Cluster 1 has greater E, request to eliminate Cluster 2
      return 2;
    }
  }
  
  return 0 ; // No Match
  
}
