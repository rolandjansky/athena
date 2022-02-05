/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOREC_CALOCELL2CLUSTERMAPPER_H
#define CALOREC_CALOCELL2CLUSTERMAPPER_H

/**
 * @class CaloCell2ClusterMapper
 * @version \$Id: CaloCell2ClusterMapper.h,v 1.3 2008-07-16 09:22:59 cbourdar Exp $
 * @author Sven Menke <menke@mppmu.mpg.de>
 * @date 25-June-2004
 * @brief create a map of CaloCluster objects each CaloCell belongs to.
 *
 * This class is an Algorithm which creates a map pointing for each
 * CaloCell to all CaloCluster objects from a given
 * CaloClusterContainer it belongs to.  Only the member cells of each
 * CaloCluster in the CaloClusterContainer are considered in the map.
 * */

class CaloCell_ID;

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "CaloEvent/CaloCell2ClusterMap.h"
#include "CaloEvent/CaloClusterContainer.h"

class CaloCell2ClusterMapper : public AthReentrantAlgorithm
{

 public:

  CaloCell2ClusterMapper(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CaloCell2ClusterMapper();
  virtual StatusCode initialize() override;
  virtual StatusCode execute (const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

 protected:
  /**
   * @brief Name of the CaloCell2ClusterMap in StoreGate.
   *
   * the map can be retrieved with this key from StoreGate.  */
  SG::WriteHandleKey<CaloCell2ClusterMap> m_mapOutputKey;

  /**
   * @brief Name of the CaloClusterContainer to use.
   *
   * the map is filled with all CaloCell objects which are member of
   * at least one CaloCluster in this CaloClusterContainer.  */
  SG::ReadHandleKey<CaloClusterContainer> m_clusterKey;
  
};
#endif // CALOREC_CALOCELL2CLUSTERMAPPER_H



