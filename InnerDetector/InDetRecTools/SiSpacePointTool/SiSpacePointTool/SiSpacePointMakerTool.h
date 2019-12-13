// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SiSpacePointMakerTool_H
#define SiSpacePointMakerTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "AthenaKernel/SlotSpecificObj.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "SiSpacePointTool/SCTinformation.h"
#include "TrkSpacePoint/SpacePoint.h"

#include <mutex>
#include <string>

class SCT_ID;
class SpacePointCollection;
class SpacePointOverlapCollection;
namespace InDetDD {
  class SiDetectorElementCollection;
  class SiDetectorElement;
}
namespace InDet {
  class SiCluster;
}

namespace InDet {
  /**
   * @class SiSpacePointMakerTool
   * Used by SiTrackerSpacePointFinder.
   * Make PixelSpacePoints from PixelClusters.
   * Make SCT_SpacePoints from SCT_Clusters.
   * Make OverlapSpacePoints from SCT_Clusters.
   */

  class SiSpacePointMakerTool : public AthAlgTool {

  public:
    /// Constructor 
    SiSpacePointMakerTool(const std::string& type, const std::string& name, const IInterface* parent);

    /// Default destructor
    virtual ~SiSpacePointMakerTool() = default;

    /// Return interfaceID
    static const InterfaceID& interfaceID();

    /// Initialize
    virtual StatusCode initialize() override;

    /// Finalize
    virtual StatusCode finalize() override;

    /// To trigger cleanup for new event
    void newEvent() const;

    /// Convert clusters to space points: SCT_Clusters -> SCT_SpacePoints
    Trk::SpacePoint* makeSCT_SpacePoint(const InDet::SiCluster& cluster1, const InDet::SiCluster& cluster2, 
                                        const Amg::Vector3D& vertexVec,
                                        const InDetDD::SiDetectorElement* element1, const InDetDD::SiDetectorElement* element2, double stripLengthGapTolerance) const;

    /// Convert clusters to space points: SCT_Clusters -> SCT_SpacePoints
    void fillSCT_SpacePointCollection(const InDet::SCT_ClusterCollection* clusters1,
                                      const InDet::SCT_ClusterCollection* clusters2, double min, double max, bool allClusters,
                                      const Amg::Vector3D& vertexVec, const InDetDD::SiDetectorElementCollection* elements,
                                      SpacePointCollection* spacepointCollection) const;

    /// Convert clusters to space points: PixelClusters -> PixelSpacePoints
    void fillPixelSpacePointCollection(const InDet::PixelClusterCollection* clusters,
                                       SpacePointCollection* spacepointCollection) const;

    /// Convert clusters to space points using eta direction overlaps: SCT_Clusters -> OverlapSpacePoints
    void fillSCT_SpacePointEtaOverlapCollection(const InDet::SCT_ClusterCollection* clusters1,
                                                const InDet::SCT_ClusterCollection* clusters2, double min, double max, bool allClusters,
                                                const Amg::Vector3D& vertexVec, const InDetDD::SiDetectorElementCollection* elements,
                                                SpacePointOverlapCollection* spacepointOverlapCollection) const;

    /// Convert clusters to space points using phi direction overlaps: SCT_Clusters -> OverlapSpacePoints
    void fillSCT_SpacePointPhiOverlapCollection(const InDet::SCT_ClusterCollection* clusters1,
                                                const InDet::SCT_ClusterCollection* clusters2, double min1, double max1, double min2,
                                                double max2, bool allClusters, const Amg::Vector3D& vertexVec ,
                                                const InDetDD::SiDetectorElementCollection* elements,
                                                SpacePointOverlapCollection* spacepointOverlapCollection) const;

  private:
    /// @name Cut parameters
    //@{
    DoubleProperty m_stripLengthTolerance{this, "StripLengthTolerance", 0.01};
    DoubleProperty m_SCTgapParameter{this, "SCTGapParameter", 0., "Recommend 0.001 - 0.0015 for ITK geometry"};
    //@}

    /// @name option to use closest approach of SCT strips as position for SpacePoint 
    //@{
    BooleanProperty m_usePerpProj{this, "UsePerpendicularProjection", false};
    //@}

    /// @name ID helper
    //@{
    const SCT_ID* m_idHelper{nullptr};
    //@}

    /// @name Mutex to protect data members in const methods
    //@{
    mutable std::mutex m_mutex;
    //@}

    /// @class CacheEntry
    /// To hold event dependent data
    struct CacheEntry {
      EventContext::ContextEvt_t m_evt{EventContext::INVALID_CONTEXT_EVT}; //!< Event number, slot number, used to check if already processed event or not.
      std::vector<Trk::SpacePoint*> m_tmpSpacePoints{}; //!< SpacePoint cache
      std::vector<SCTinformation> m_SCT0{}; //!< SCTinformation vector for 0-th element (side 0)
      std::vector<SCTinformation> m_SCT1{}; //!< SCTinformation vector for 1-st element (side 1)
      const InDetDD::SiDetectorElement* m_element0{nullptr}; //!< SiDetectorElement for 0-th element (side 0)
      const InDetDD::SiDetectorElement* m_element1{nullptr}; //!< SiDetectorElement for 1-st element (side 1)
      const InDetDD::SiDetectorElement* m_elementOLD{nullptr}; //!< SiDetectorElement cache
      /// Clear all members of event dependent data
      void clear() {
        if (m_tmpSpacePoints.size()) {
          for (Trk::SpacePoint* sp : m_tmpSpacePoints) {
            delete sp;
          }
        }
        m_tmpSpacePoints.clear();
        m_SCT0.clear();
        m_SCT1.clear();
        m_element0 = nullptr;
        m_element1 = nullptr;
        m_elementOLD = nullptr;
      };
    };

    /// SG::SlotSpecificObj is used to hold event dependent data cache.
    /// Guarded by m_mutex in const methods.
    mutable SG::SlotSpecificObj<CacheEntry> m_cache ATLAS_THREAD_SAFE;

    /// Get stripLengthGapTolerance and return offset value for two SiDetectorElement's
    double offset(const InDetDD::SiDetectorElement* element1, const InDetDD::SiDetectorElement* element2, double& stripLengthGapTolerance) const;

    /// Get stripLengthGapTolerance for two SiDetectorElement's
    void offset(double& stripLengthGapTolerance, const InDetDD::SiDetectorElement* element1, const InDetDD::SiDetectorElement* element2) const;

    /// Not implemented yet
    bool fillSCT_Information(const InDet::SCT_ClusterCollection* clusters1, const InDet::SCT_ClusterCollection* clusters2,
                             const Amg::Vector3D& vertexVec,
                             const InDetDD::SiDetectorElementCollection* elements) const;

    /// Convert clusters to space points using CacheEntry
    void makeSCT_SpacePoints(const double stripLengthGapTolerance) const;
  };
}

#endif //SiSpacePointMakerTool_H
