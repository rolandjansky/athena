/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOCLUSTERREMOVEBAD_H
#define CALOCLUSTERCORRECTION_CALOCLUSTERREMOVEBAD_H

/**************************************************************************

NAME:     CaloClusterRemoveBad.h
AUTHORS:  H. Ma

CREATED:  July 2010 

PURPOSE: remove clusters that have low energy after corrections.

Base class: CaloClusterProcessor

***************************************************************************/

// INCLUDE HEADER FILES:
#include "CaloRec/CaloClusterProcessor.h"
#include "CaloEvent/CaloCluster.h"

class CaloClusterRemoveBad : public CaloClusterProcessor
{
public:
  /**
   * @brief Constructor.
   * @param type The type of the tool.
   * @param name The name of the tool.
   * @param parent The parent algorithm of the tool.
   */
  CaloClusterRemoveBad (const std::string& type,
                        const std::string& name,
                        const IInterface* parent);
  

  using CaloClusterProcessor::execute;


  /**
   * @brief Execute on a single cluster.
   * @param The cluster to process.
   * @param ctx The event context.
   *
   * For this tool, this is a no-op.
   * We only make this available to avoid getting warnings
   * when corrections are called on single clusters from egammaRec.
   */
  virtual StatusCode execute (const EventContext& ctx,
                              xAOD::CaloCluster* cluster) const override;


  /**
   * @brief Execute on an entire collection of clusters.
   * @param The container of clusters.
   * @param ctx The event context.
   */
  virtual StatusCode execute (const EventContext& ctx,
                              xAOD::CaloClusterContainer* clusColl) const override;

    
 private:
 
  CaloClusterRemoveBad();
  
  // property, energy threshold below which a cluster will be rejected.
  float  m_eThreshold ; 
};

#endif
