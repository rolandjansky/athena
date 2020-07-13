// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SiSpacePointFormation_SI_POINT_FINDER_H
#define SiSpacePointFormation_SI_POINT_FINDER_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "AthContainers/DataVector.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "Identifier/Identifier.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SiSpacePointFormation/SiElementPropertiesTable.h"
#include "SiSpacePointTool/SiSpacePointMakerTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrkSpacePoint/SpacePointOverlapCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "BeamSpotConditionsData/BeamSpotData.h"

#include "GaudiKernel/ToolHandle.h"

#include <string>

class SpacePointCollection; 
class SpacePointOverlapCollection; 
class SpacePointContainer; 
class SCT_ID;
class PixelID;

namespace InDet {

/**
@class SiTrackerSpacePointFinder
 This version of PointFinder uses SiCluster to find space points in
 the pixel and SCT detectors. The algorithm retrieves all clusters from 
 StoreGate using SG::ReadHandle(Key) and uses the begin-end iterators to iterate over the 
 clusters. It then has to see whether the next cluster is a pixel or 
 SCT cluster. Pixel space points are obtained directly from the clusters. 
 To make  SCT space points we also need the IdentifiableContainer for the 
 SCT clusters, since we need random access to find overlapping detectors on the
 stereo layer. Space points are made by combining clusters from pairs of 
 overlapping detectors. The user can choose just to process the phi wafer and 
 its opposite on the stereo layer, or also to consider overlaps with the 
 four nearest neighbours of the opposite wafer.   
 The space points are wrtten to Storegate as SpacePointContainer for Pixels 
 and non-overlapping SCT space points and as SpacePointOverlapCollection for 
 the overlapping ones.
  

 Job Options:
 Process pixels:  default true
 Process SCTs: default true
 Process overlaps: default true
 SCT IdentifiableContainer name: default "SCT_Clusters"
 SCT SpacePointSet name: default "SCT_SpacePoints"


 Fatal Errors:
 neither pixels nor SCTs selected
 SCTs selected and no name for SCT Identifiable Collection
 No name for SpacePointSet
 Failure to retrieve event
 StoreGate failure.

 Possible errors for which processing continues:
 No clusters found for event
 Implementation of SCT_Neighbours is SiElementPropertiesTable,
 is prepared by SiElementPropertiesTableCondAlg
 and is stored in StoreGate using SG::WriteCondHandle(Key). 
*/

  class SiTrackerSpacePointFinder:public AthReentrantAlgorithm {

  public:
  
    /**
     * @name AthReentrantAlgorithm methods
     */
    //@{
    SiTrackerSpacePointFinder(const std::string& name,
			      ISvcLocator* pSvcLocator);
    
    virtual ~SiTrackerSpacePointFinder() = default;
    
    virtual StatusCode initialize() override;
    
    virtual StatusCode execute (const EventContext& ctx) const override;

    virtual StatusCode finalize() override;

    /** Make this algorithm clonable. */
    virtual bool isClonable() const override { return true; };
    //@}

  private:
    /// This is a temporary object to aid reentrant coding
    struct SPFCache{
      Amg::Vector3D vertex;
      const EventContext& ctx;
      const SCT_ClusterContainer* SCTCContainer;
      SPFCache(const EventContext& inctx) : ctx(inctx), SCTCContainer(nullptr) { }
    };

    // methods

    /**
     * @name Disallow constructor without parameters, copy constructor, assignment operator
     */
    //@{
    SiTrackerSpacePointFinder() = delete;
    SiTrackerSpacePointFinder(const SiTrackerSpacePointFinder&) =delete;
    SiTrackerSpacePointFinder &operator=(const SiTrackerSpacePointFinder&) = delete;
    //@}

    /**
     * @name Main SCT space point methods
     */
    //@{
    void addSCT_SpacePoints
      (const SCT_ClusterCollection* next,
       const SiElementPropertiesTable* properties,
       const InDetDD::SiDetectorElementCollection* elements,
       SpacePointCollection* spacepointCollection, SpacePointOverlapCollection* spacepointOverlapCollection, SPFCache&) const;
    //@}

    // data members

    /**
     * @name Input data using SG::ReadHandleKey
     */
    //@{
    SG::ReadHandleKey<SCT_ClusterContainer>  m_Sct_clcontainerKey{this, "SCT_ClustersName", "SCT clContainer"};
    SG::ReadHandleKey<PixelClusterContainer> m_Pixel_clcontainerKey{this, "PixelsClustersName", "PixelClusters", "Pixel clContainer"};
    //@}

    /**
     * @name Input condition data using SG::ReadCondHandleKey
     */
    //@{
    /// For beam spot
    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
    /// To get SCT neighbours
    SG::ReadCondHandleKey<InDet::SiElementPropertiesTable> m_SCTPropertiesKey{this, "SCTPropertiesKey", "SCT_ElementPropertiesTable", "Key of input SiElementPropertiesTable for SCT"};
    /// For SCT alignment
    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
    //@}

    /**
     * @name Output data using SG::WriteHandleKey
     */
    //@{
    SG::WriteHandleKey<SpacePointContainer> m_SpacePointContainer_SCTKey{this, "SpacePointsSCTName", "SCT_SpacePoints", "SpacePoint SCT container"};
    SG::WriteHandleKey<SpacePointContainer> m_SpacePointContainerPixelKey{this, "SpacePointsPixelName", "PixelSpacePoints", "SpacePoint Pixel container"};
    SG::WriteHandleKey<SpacePointOverlapCollection> m_spacepointoverlapCollectionKey{this, "SpacePointsOverlapName", "OverlapSpacePoints", "Space Point Overlap collection"};
    //@}

    /**
     * @name Caches for HLT using SG::UpdateHandleKey
     */
    //@{
    SG::UpdateHandleKey<SpacePointCache> m_SpacePointCache_SCTKey;
    SG::UpdateHandleKey<SpacePointCache> m_SpacePointCache_PixKey;
    //@}

    /**
     * @name ToolHandle
     */
    //@{
    ToolHandle< SiSpacePointMakerTool > m_SiSpacePointMakerTool{this, "SiSpacePointMakerTool", "InDet::SiSpacePointMakerTool"};
    ToolHandle<GenericMonitoringTool> m_monTool{this, "monTool", "", "Monitoring tool"};
    //@}

    /**
     * @name Id helpers
     */
    //@{
    const PixelID* m_idHelperPixel{nullptr};
    const SCT_ID* m_idHelper{nullptr};
    //@}

    /**
     * @name Configuration flags
     */
    //@{
    bool m_selectPixels;
    bool m_selectSCTs;
    bool m_overlap; //!< process all overlapping SCT pairs if true.
    bool m_allClusters; //!< process all clusters without limits if true.

    bool m_cachemode{false}; //!< used for online MT counters
    //@}

    /**
     * @name Cut parameters
     * @brief The following are ranges within which clusters must lie to
     * make a spacepoint. Opposite and eta neighbours
     * clusters must lie within range of each other. 
     * Phi clusters must lie in region of each wafer separately.
     */
    //@{
    float m_overlapLimitOpposite;  //!< overlap limit for opposite-neighbours.
    float m_overlapLimitPhi;       //!< overlap limit for phi-neighbours.
    float m_overlapLimitEtaMin;    //!< low overlap limit for eta-neighbours.
    float m_overlapLimitEtaMax;    //!< high overlap limit for eta-neighbours.
    //@}

    /**
     * @name Beam spot override
     */
    //@{
    bool m_overrideBS;
    float m_xVertex;
    float m_yVertex;
    float m_zVertex;
    //@}

    /**
     * @name Counters
     * Use mutable to be updated in const methods.
     * AthReentrantAlgorithm is const during event processing.
     * Use std::atomic to be multi-thread safe.
     */
    //@{
    mutable std::atomic<int> m_numberOfEvents{0};
    mutable std::atomic<int> m_numberOfPixel{0};
    mutable std::atomic<int> m_numberOfSCT{0};
    mutable std::atomic<int> m_sctCacheHits{0};
    mutable std::atomic<int> m_pixCacheHits{0};
    //@}
    
  };

}

#endif // SiSpacePointFormation_SI_POINT_FINDER_H
