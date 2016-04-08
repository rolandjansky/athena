/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
/* 
 * "$Id: ITrigSCT_SpacePointTool.h,v 1.1 2008-12-13 14:23:42 masik Exp $"
 *
 * author:  Jiri Masik 
 *
 * Description: the interface for the trigger SpacepointTool
 *
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef ITRIGSCT_SPACEPOINTTOOL_H
#define ITRIGSCT_SPACEPOINTTOOL_H

#include "GaudiKernel/IAlgTool.h"

//typedefs
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"

#include "Identifier/IdentifierHash.h"

//fwd decl
class SpacePointCollection;
class SpacePointOverlapCollection;

namespace InDet {

  static const InterfaceID IID_ITrigSCT_SpacePointTool("InDet::ITrigSCT_SpacePointTool",1,0);

  class ITrigSCT_SpacePointTool : virtual public IAlgTool {

  public:

    static const InterfaceID& interfaceID();

    /**
     * Convert clusters to SpacePoints
     * @param[in]  clusCollection        Cluster collection
     * @param[in]  clusterContainer      Cluster container
     * @param[out] spacepointCollection  SpacePoint collection
     */
    virtual void addSCT_SpacePoints (const SCT_ClusterCollection* clusCollection,
				     const SCT_ClusterContainer* clusterContainer,
				     SpacePointCollection* spacepointCollection)=0;

    /**
     * Convert clusters to SpacePoints
     * @param[in]  clusCollection        Cluster collection
     * @param[in]  clusterContainer      Cluster container
     * @param[out] spacepointCollection  SpacePoint collection
     * @param      overlapColl           collection of SP with overlaps
     */
    virtual void addSCT_SpacePoints (const SCT_ClusterCollection* clusCollection,
				     const SCT_ClusterContainer* clusterContainer,
				     SpacePointCollection* spacepointCollection,
				     SpacePointOverlapCollection* overlapColl)=0;

    virtual void checkForSCT_Points (const SCT_ClusterCollection* clusters1,
				     const IdentifierHash& id2, double minDiff, 
				     double maxDiff,
				     SpacePointCollection* spacepointCollection, 
				     bool overlapColl) = 0;

    virtual void checkForSCT_Points (const SCT_ClusterCollection* clusters1, 
				     const IdentifierHash& id2, double min1, 
				     double max1,
				     double min2, double max2) = 0;

  };

  inline const InterfaceID& InDet::ITrigSCT_SpacePointTool::interfaceID(){
    return IID_ITrigSCT_SpacePointTool;
  }
}

#endif
