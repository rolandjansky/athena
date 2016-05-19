// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterProcessor.h,v 1.2 2006-07-07 03:50:28 ssnyder Exp $
/**
 * @file  CaloClusterProcessor.h
 * @author scott snyder <snyder@bnl.gov>
 * @date March, 2006
 * @brief Base class for cluster processing tools called from
 *        @c CaloClusterMaker that operate on individual clusters.
 *
 * This class defines an @c execute method that takes as an argument
 * a @c CaloCluster.  It derives from @c CaloClusterCollectionProcessor,
 * and defines the collection @c execute method as looping over
 * all clusters in the collection and calling @c execute on each
 * individually.
 */

#ifndef CALOREC_CALOCLUSTERPROCESSOR_H
#define CALOREC_CALOCLUSTERPROCESSOR_H


#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "xAODCaloEvent/CaloClusterContainer.h"


class CaloClusterProcessor : public AthAlgTool, virtual public CaloClusterCollectionProcessor {
public:
  /**
   * @brief Constructor.
   * @param type The type of the tool.
   * @param name The name of the tool.
   * @param parent The parent algorithm of the tool.
   *
   * This just forwards on to the base class constructor.
   */
  CaloClusterProcessor (const std::string& type,
                        const std::string& name,
                        const IInterface* parent);


  /**
   * @brief Execute on a single cluster.
   * @param The cluster to process.
   */
  virtual StatusCode execute (xAOD::CaloCluster* cluster) = 0;


  /**
   * @brief Execute on an entire collection of clusters.
   * @param The container of clusters.
   *
   * This will iterate over all the clusters in @c collection
   * and call @c execute on each one individually.
   */
  virtual StatusCode execute (xAOD::CaloClusterContainer* collection);
};


#endif // not CALOREC_CALOCLUSTERPROCESSOR_H
