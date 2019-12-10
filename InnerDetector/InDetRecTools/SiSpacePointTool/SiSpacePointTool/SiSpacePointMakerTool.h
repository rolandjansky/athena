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
  class SiSpacePointMakerTool : public AthAlgTool {
  public:
    // Constructor 
    SiSpacePointMakerTool(const std::string& type, const std::string& name, const IInterface* parent);

    virtual ~SiSpacePointMakerTool() = default;

    static const InterfaceID& interfaceID();

    virtual StatusCode initialize() override;

    virtual StatusCode finalize() override;

    void newEvent() const;

    // Convert clusters to space points
    Trk::SpacePoint* makeSCT_SpacePoint(const InDet::SiCluster& cluster1, const InDet::SiCluster& cluster2, 
                                        const Amg::Vector3D& vertexVec,
                                        const InDetDD::SiDetectorElement* element1, const InDetDD::SiDetectorElement* element2, double stripLengthGapTolerance) const;

    void fillSCT_SpacePointCollection(const InDet::SCT_ClusterCollection* clusters1,
                                      const InDet::SCT_ClusterCollection* clusters2, double min, double max, bool allClusters,
                                      const Amg::Vector3D& vertexVec, const InDetDD::SiDetectorElementCollection* elements,
                                      SpacePointCollection* spacepointCollection) const;

    void fillPixelSpacePointCollection(const InDet::PixelClusterCollection* clusters,
                                       SpacePointCollection* spacepointCollection) const;

    void fillSCT_SpacePointEtaOverlapCollection(const InDet::SCT_ClusterCollection* clusters1,
                                                const InDet::SCT_ClusterCollection* clusters2, double min, double max, bool allClusters,
                                                const Amg::Vector3D& vertexVec, const InDetDD::SiDetectorElementCollection* elements,
                                                SpacePointOverlapCollection* spacepointOverlapCollection) const;

    void fillSCT_SpacePointPhiOverlapCollection(const InDet::SCT_ClusterCollection* clusters1,
                                                const InDet::SCT_ClusterCollection* clusters2, double min1, double max1, double min2,
                                                double max2, bool allClusters, const Amg::Vector3D& vertexVec ,
                                                const InDetDD::SiDetectorElementCollection* elements,
                                                SpacePointOverlapCollection* spacepointOverlapCollection) const;

  private:
    DoubleProperty m_stripLengthTolerance{this, "StripLengthTolerance", 0.01};
    DoubleProperty m_SCTgapParameter{this, "SCTGapParameter", 0., "Recommend 0.001 - 0.0015 for ITK geometry"};

    // option to use closest approach of SCT strips as position for SpacePoint 
    BooleanProperty m_usePerpProj{this, "UsePerpendicularProjection", false};

    const SCT_ID* m_idHelper{nullptr};

    mutable std::mutex m_mutex;
    struct CacheEntry {
      EventContext::ContextEvt_t m_evt{EventContext::INVALID_CONTEXT_EVT};
      std::vector<Trk::SpacePoint*> m_tmpSpacePoints{};
      std::vector<SCTinformation> m_SCT0{};
      std::vector<SCTinformation> m_SCT1{};
      const InDetDD::SiDetectorElement* m_element0{nullptr};
      const InDetDD::SiDetectorElement* m_element1{nullptr};
      const InDetDD::SiDetectorElement* m_elementOLD{nullptr};
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
    mutable SG::SlotSpecificObj<CacheEntry> m_cache ATLAS_THREAD_SAFE; // Guarded by m_mutex

    double offset(const InDetDD::SiDetectorElement* element1, const InDetDD::SiDetectorElement* element2, double& stripLengthGapTolerance) const;
    void offset(double& stripLengthGapTolerance, const InDetDD::SiDetectorElement* element1, const InDetDD::SiDetectorElement* element2) const;

    bool fillSCT_Information(const InDet::SCT_ClusterCollection* clusters1, const InDet::SCT_ClusterCollection* clusters2,
                             const Amg::Vector3D& vertexVec,
                             const InDetDD::SiDetectorElementCollection* elements) const;

    // Convert clusters to space points
    void makeSCT_SpacePoints(const double stripLengthGapTolerance) const;
  };
}

#endif //SiSpacePointMakerTool_H
