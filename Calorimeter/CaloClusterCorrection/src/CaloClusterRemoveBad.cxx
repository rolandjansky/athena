/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// include header files
#include "CaloClusterRemoveBad.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "GaudiKernel/MsgStream.h"
#include "AthContainers/DataVector.h"

/********************************************************************

NAME:     CaloClusterRemoveBad.cxx

AUTHORS:  H. Ma
CREATED:  July 2010

PURPOSE:  Remove bad cluster after corrections.

********************************************************************/


/**
 * @brief Constructor.
 * @param type The type of the tool.
 * @param name The name of the tool.
 * @param parent The parent algorithm of the tool.
 */
CaloClusterRemoveBad::CaloClusterRemoveBad(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent)
  : CaloClusterProcessor(type, name, parent)  
{ 
  // Energy threshold for cluster removal.
  declareProperty ("EMin", m_eThreshold = 0.0,
                   "Energy threshold for cluster removal.");
}


/**
 * @brief Execute on a single cluster.
 * @param The cluster to process.
 * @param ctx The event context.
 *
 * For this tool, this is a no-op.
 * We only make this available to avoid getting warnings
 * when corrections are called on single clusters from egammaRec.
 */
StatusCode CaloClusterRemoveBad::execute (const EventContext& /*ctx*/,
                                          xAOD::CaloCluster* /*cluster*/) const
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Execute on an entire collection of clusters.
 * @param The container of clusters.
 * @param ctx The event context.
 */
StatusCode
CaloClusterRemoveBad::execute (const EventContext& /*ctx*/,
                               xAOD::CaloClusterContainer* clusColl) const
{
  
  ATH_MSG_DEBUG( "Executing CaloClusterRemoveBad" << endmsg); 
  
  using clus_iterator = xAOD::CaloClusterContainer::iterator;
  clus_iterator iter1 = clusColl->begin();
    
  ATH_MSG_DEBUG( "Collection has before dup rem size: " << clusColl->size() << endmsg);
  for( ;iter1!=clusColl->end(); ) {
    ATH_MSG_DEBUG( " cluster energy  " << (*iter1)->e() << endmsg);
    if ((*iter1)->e()<= m_eThreshold ){
      ATH_MSG_DEBUG( " Removed cluster with energy below threshold, " << (*iter1)->e() << endmsg);
      iter1 = clusColl->erase(iter1);  
      continue ; 
    }

    iter1++;

  } 
  ATH_MSG_DEBUG( "Collection has after dup rem size: " << clusColl->size() << endmsg);

  return StatusCode::SUCCESS;
}

