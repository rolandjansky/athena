// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterCollectionProcessor.h,v 1.3 2006-07-07 03:49:56 ssnyder Exp $
/**
 * @file  CaloClusterCollectionProcessor.h
 * @author scott snyder <snyder@bnl.gov>
 * @date March, 2006
 * @brief Base class for cluster processing tools called from
 *        @c CaloClusterMaker.
 *
 * This class defines an @c execute method that takes as an argument
 * a @c CaloClusterContainer.  Tools that operate on individual clusters
 * should derive from @c CaloClusterProcessor instead of this.
 */

#ifndef CALOREC_CALOCLUSTERCOLLECTIONPROCESSOR_H
#define CALOREC_CALOCLUSTERCOLLECTIONPROCESSOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

static const InterfaceID IID_CaloClusterCollectionProcessor ("CaloClusterCollectionProcessor", 1 , 0);

class CaloClusterCollectionProcessor : virtual public IAlgTool {

public:

  /**
   * @brief Execute on an entire collection of clusters.
   * @param The container of clusters.
   */
  virtual StatusCode execute (xAOD::CaloClusterContainer* collection) = 0;

  /**
   * @brief Standard Gaudi interface ID method.
   */
  static const InterfaceID& interfaceID() {return IID_CaloClusterCollectionProcessor;}
};


#endif // not CALOREC_CALOCLUSTERCOLLECTIONPROCESSOR_H
