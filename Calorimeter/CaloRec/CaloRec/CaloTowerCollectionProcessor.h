// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloTowerCollectionProcessor.h, 2020-09-16 03:49:56 ssnyder Exp $
/**
 * @file  CaloTowerCollectionProcessor.h
 * @author Tasnuva Chowdhury
 * @date   September, 2020
 * @brief Base class for cluster processing tools called from
 *        @c CaloClusterMaker.
 *
 */

#ifndef CALOREC_CALOTOWERCOLLECTIONPROCESSOR_H
#define CALOREC_CALOTOWERCOLLECTIONPROCESSOR_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "CaloEvent/CaloCellClusterWeights.h"

static const InterfaceID IID_CaloTowerCollectionProcessor ("CaloTowerCollectionProcessor", 1 , 0);

class CaloTowerCollectionProcessor
  : virtual public IAlgTool
{
public:

   /**
   * @brief Execute on an entire collection of clusters.
   * @param collection The container of clusters.
   * param ctx The event context.
   */
  virtual StatusCode execute (const EventContext& ctx,
                              xAOD::CaloClusterContainer* collection,
                              CaloCellClusterWeights* cellWeights) const = 0;
 /**
   * @brief Standard Gaudi interface ID method.
   */
  static const InterfaceID& interfaceID() {return IID_CaloTowerCollectionProcessor;}
};


#endif // not CALOREC_CaloTowerCollectionProcessor_H
