/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**   @file SCT_Clusterization
 *   Header file for the SCT_Clusterization class (an Algorithm).
 *   @author Paul Bell, Tommaso Lari, Shaun Roe, Carl Gwilliam
 *   @date 08 July 2008
 */

#ifndef INDETPREPRAWDATAFORMATION_SCT_CLUSTERIZATION_H
#define INDETPREPRAWDATAFORMATION_SCT_CLUSTERIZATION_H

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

//InDet includes
//Stored by value, cannot be fwd declared
#include "Identifier/IdentifierHash.h"
//template parameter, so should not be possible to just fwd declare
#include "Identifier/Identifier.h"
//Next contains a typedef so cannot be fwd declared
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "SCT_ConditionsData/SCT_FlaggedCondData.h"
//tool/service handle template parameters
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "SiClusterizationTool/ISCT_ClusteringTool.h"

//Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

//STL
#include <map>
#include <string>

class SCT_ID;
class ISvcLocator;
class StatusCode;

namespace InDet {
/**
 *    @class SCT_Clusterization
 *    @brief Form clusters from SCT Raw Data Objects
 *    The class loops over an RDO grouping strips and creating collections of clusters, subsequently recorded in StoreGate
 *    Uses SCT_ConditionsTools to determine which strips to include.
 */
class SCT_Clusterization : public AthReentrantAlgorithm {
public:
  /// Constructor with parameters:
  SCT_Clusterization(const std::string& name, ISvcLocator* pSvcLocator);
  
  /**    @name Usual algorithm methods */
  //@{
  ///Retrieve the tools used and initialize variables
  virtual StatusCode initialize() override;
  ///Form clusters and record them in StoreGate (detector store)
  virtual StatusCode execute(const EventContext& ctx) const override;
  ///Clean up and release the collection containers
  virtual StatusCode finalize() override;
  //@}

private:
  /**    @name Disallow default instantiation, copy, assignment */
  //@{
  SCT_Clusterization() = delete;
  SCT_Clusterization(const SCT_Clusterization&) = delete;
  SCT_Clusterization &operator=(const SCT_Clusterization&) = delete;
  //@}

  ServiceHandle<IRegSelSvc> m_regionSelector; //!< region selector service
  ToolHandle<ISCT_ClusteringTool> m_clusteringTool{this, "clusteringTool", "InDet::SCT_ClusteringTool"};
  ToolHandle<IInDetConditionsTool> m_pSummaryTool{this, "conditionsTool", "SCT_ConditionsSummaryTool/InDetSCT_ConditionsSummaryTool", "Tool to retrieve SCT conditions summary"};
  const SCT_ID* m_idHelper;

  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey{this, "RoIs", "", "RoIs to read in"};
  SG::ReadHandleKey<SCT_RDO_Container> m_rdoContainerKey{this, "DataObjectName", "SCT_RDOs", "SCT RDOs"};
  SG::WriteHandleKey<SCT_ClusterContainer> m_clusterContainerKey{this, "ClustersName", "SCT_Clusters", "SCT cluster container"};
  SG::WriteHandleKey<SiClusterContainer> m_clusterContainerLinkKey{this, "ClustersLinkName_", "SCT_Clusters", "SCT cluster container link name (don't set this)"};
  SG::WriteHandleKey<SCT_FlaggedCondData> m_flaggedCondDataKey{this, "SCT_FlaggedCondData", "SCT_FlaggedCondData", "SCT flagged conditions data"};
  SG::UpdateHandleKey<SCT_ClusterContainerCache> m_clusterContainerCacheKey;

  UnsignedIntegerProperty m_maxFiredStrips{this, "maxFiredStrips", 384, "Threshold of number of fired strips per wafer"}; // 0 disables the per-wafer cut
  UnsignedIntegerProperty m_maxTotalOccupancyPercent{this, "maxTotalOccupancyInPercent", 100, "Threshold of percentage of fired strips for the whole SCT"}; // 100 disables the whole SCT cut
  BooleanProperty m_roiSeeded{this, "isRoI_Seeded", false, "Use RoI"};
  BooleanProperty m_checkBadModules{this, "checkBadModules", true};
};

}

#endif // INDETRIOMAKER_SCT_CLUSTERIZATION_H
