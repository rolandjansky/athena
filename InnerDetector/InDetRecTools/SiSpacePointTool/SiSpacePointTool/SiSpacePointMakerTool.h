// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SiSpacePointMakerTool_H
#define SiSpacePointMakerTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"

#include <string>

class SCT_ID;
class SpacePointCollection;
class SpacePointOverlapCollection;
namespace Trk {
  class SpacePoint;
}
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

    double offset(const InDetDD::SiDetectorElement* element1, const InDetDD::SiDetectorElement* element2, double& stripLengthGapTolerance) const;
  };
}

#endif //SiSpacePointMakerTool_H
