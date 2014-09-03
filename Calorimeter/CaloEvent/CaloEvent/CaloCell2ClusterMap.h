/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOCELL2CLUSTERMAP_H
#define CALOEVENT_CALOCELL2CLUSTERMAP_H

/**
 * @class CaloCell2ClusterMap
 * @version \$Id: CaloCell2ClusterMap.h,v 1.3 2005-04-05 17:19:30 menke Exp $
 * @author Sven Menke <menke@mppmu.mpg.de>
 * @date 25-June-2004
 * @brief map of @c CaloCluster objects each @c CaloCell belongs to
 *
 * This class is used to store a map pointing from a @c CaloCell to all 
 * @c CaloCluster objects from a given @c CaloClusterContainer it belongs to.
 * Only the member cells of each @c CaloCluster in the @c CaloClusterContainer 
 * are considered in the map
 */

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "Navigation/Navigable.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

class CaloCell2ClusterMap : public DataVector< Navigable<CaloClusterContainer> > 
{

 public:
   /**
   * @brief Constructor.
   *
   * Calls the base class constructor.
   */
	CaloCell2ClusterMap(SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS) :
			DataVector<Navigable<CaloClusterContainer> >(ownPolicy) {};

	virtual ~CaloCell2ClusterMap() {};

};

CLASS_DEF(CaloCell2ClusterMap, 219524388, 1)

#endif
