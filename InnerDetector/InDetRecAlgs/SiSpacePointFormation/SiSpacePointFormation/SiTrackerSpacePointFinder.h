/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/***************************************************************************
 This version of PointFinder uses SiCluster to find space points in
 the pixel and SCT detectors. The algorithm retrieves all clusters from 
 StoreGate by type and uses the begin-end iterators to iterate over the 
 clusters. It does not (for efficiency reasons) use the IdentifiableContainer
 iterator here. It then has to see whether the next cluster is a pixel or 
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
 SCT IdentifiableContainer name: default ""
 SpacePointSet name: default ""


 Fatal Errors:
 neither pixels nor SCTs selected
 SCTs selected and no name for SCT Identifiable Collection
 No name for SpacePointSet
 Failure to retrieve event
 StoreGate failure.

 Possible errors for which processing contnues:
 No clusters found for event
 ***************************************************************************
 Implementation of SCT_Neighbours has data members stored as a vector
 ATLAS Collaboration
 ***************************************************************************/

#ifndef SiSpacePointFormation_SI_POINT_FINDER_H
#define SiSpacePointFormation_SI_POINT_FINDER_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthContainers/DataVector.h"
#include "Identifier/Identifier.h"

#include "SiSpacePointTool/SiSpacePointMakerTool.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "TrkSpacePoint/SpacePoint.h" 
#include <string>
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "GeoPrimitives/GeoPrimitives.h"

class Event;
class SpacePointCollection; 
class SpacePointOverlapCollection; 
class SpacePointContainer; 
class SvcLocator;
class SCT_NeighboursTable;
class SCT_ID;
class PixelID;
class IBeamCondSvc;

namespace InDetDD {
  class SCT_DetectorManager;
  class PixelDetectorManager;
}

namespace InDet {

  class SiElementPropertiesTable;

  class SiTrackerSpacePointFinder:public AthReentrantAlgorithm {

  struct SPFCache{//This is a temporary object to aid reentrant coding
     Amg::Vector3D vertex;
     const EventContext& ctx;
     const SCT_ClusterContainer* SCTCContainer;
     SPFCache(const EventContext& inctx) : ctx(inctx), SCTCContainer(nullptr) { }
  };

  public:
    
    SiTrackerSpacePointFinder(const std::string& name,
			      ISvcLocator* pSvcLocator);
    
    ~SiTrackerSpacePointFinder();
    
    StatusCode initialize() override;
    
    StatusCode execute_r (const EventContext& ctx) const override;

    StatusCode finalize() override;

  private:
    // methods

    SiTrackerSpacePointFinder() = delete;
    SiTrackerSpacePointFinder(const SiTrackerSpacePointFinder&) =delete;
    SiTrackerSpacePointFinder &operator=(const SiTrackerSpacePointFinder&) = delete;


    void addSCT_SpacePoints
      (const SCT_ClusterCollection* next, 
       SpacePointCollection* spacepointCollection, SpacePointOverlapCollection* spacepointOverlapCollection, SPFCache&) const; 

    void checkForSCT_Points
      (const SCT_ClusterCollection* clusters1,
       const IdentifierHash id2, double minDiff, double maxDiff,
       SpacePointCollection* spacepointCollection, bool overlapColl, SpacePointOverlapCollection* spacepointOverlapCollection, SPFCache&) const; 

    void checkForSCT_Points
      (const SCT_ClusterCollection* clusters1, 
     const IdentifierHash id2, double min1, double max1,
     double min2, double max2, SpacePointOverlapCollection* spacepointOverlapCollection, SPFCache&) const;
    

    // data members
    SG::ReadHandleKey<SCT_ClusterContainer>  m_Sct_clcontainerKey;
    SG::ReadHandleKey<PixelClusterContainer> m_Pixel_clcontainerKey;
    bool m_selectPixels;
    bool m_selectSCTs;
    bool m_overlap;         // process all overlapping SCT pairs if true.
    bool m_allClusters;     // process all clusters without limits if true.

    // The following are ranges within which clusters must lie to
    // make a spacepoint. Opposite and eta neighbours
    // clusters must lie within range of each other. 
    // Phi clusters must lie in region of each wafer separately.

    float m_overlapLimitOpposite;  // overlap limit for opposite-neighbours.
    float m_overlapLimitPhi;       // overlap limit for phi-neighbours.
    float m_overlapLimitEtaMin;    // low overlap limit for eta-neighbours.
    float m_overlapLimitEtaMax;    // high overlap limit for eta-neighbours.
    float m_epsWidth;		   // safety margin for half-width.

    bool m_overrideBS;
    float m_xVertex;
    float m_yVertex;
    float m_zVertex;
    ServiceHandle<IBeamCondSvc> m_iBeamCondSvc; 

    mutable std::atomic<int> m_numberOfEvents;
    mutable std::atomic<int> m_numberOfPixel;
    mutable std::atomic<int> m_numberOfSCT;
    mutable std::atomic<int> m_sctCacheHits;
    mutable std::atomic<int> m_pixCacheHits;
    bool m_cachemode; //used for online MT counters
    const InDetDD::SCT_DetectorManager* m_manager; 
    // const InDetDD::PixelDetectorManager* m_managerPixel;     // unused
    const SCT_ID* m_idHelper;
    const PixelID* m_idHelperPixel;
    static const SiElementPropertiesTable* s_properties;
    
    SG::WriteHandleKey<SpacePointContainer> m_SpacePointContainer_SCTKey;
    SG::WriteHandleKey<SpacePointContainer> m_SpacePointContainerPixelKey;
    SG::WriteHandleKey<SpacePointOverlapCollection> m_spacepointoverlapCollectionKey;
    SG::UpdateHandleKey<SpacePointCache> m_SpacePointCache_SCTKey;
    SG::UpdateHandleKey<SpacePointCache> m_SpacePointCache_PixKey;
    ToolHandle< SiSpacePointMakerTool > m_SiSpacePointMakerTool;
  };

}

#endif // SiSpacePointFormation_SI_POINT_FINDER_H
