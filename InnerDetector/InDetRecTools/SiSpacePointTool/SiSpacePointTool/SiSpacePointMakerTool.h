/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SiSpacePointMakerTool_H
#define SiSpacePointMakerTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "SiSpacePointTool/SCTinformation.h"
#include "TrkSpacePoint/SpacePoint.h"

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
  
  constexpr size_t nNeighbours{6};
  
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

    /// Convert clusters to space points: SCT_Clusters -> SCT_SpacePoints
    Trk::SpacePoint* makeSCT_SpacePoint(const InDet::SiCluster& cluster1, const InDet::SiCluster& cluster2, 
                                        const Amg::Vector3D& vertexVec,
                                        const InDetDD::SiDetectorElement* element1, const InDetDD::SiDetectorElement* element2, double stripLengthGapTolerance) const;
    
    Trk::SpacePoint* makeSCT_SpacePoint(InDet::SCTinformation&,InDet::SCTinformation&,IdentifierHash,IdentifierHash,double,double) const;

    /// Convert clusters to space points: SCT_Clusters -> SCT_SpacePoints
    void fillSCT_SpacePointCollection(const InDet::SCT_ClusterCollection* clusters1,
                                      const InDet::SCT_ClusterCollection* clusters2, double min, double max, bool allClusters,
                                      const Amg::Vector3D& vertexVec, const InDetDD::SiDetectorElementCollection* elements,
                                      SpacePointCollection* spacepointCollection) const;

    void fillSCT_SpacePointCollection(std::array<const InDetDD::SiDetectorElement*, nNeighbours>&,
                                      std::array<const SCT_ClusterCollection*, nNeighbours>&,
                                      std::array<double, 14>&,bool,const Amg::Vector3D&,
                                      SpacePointCollection*,SpacePointOverlapCollection*) const;

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

    /// update range accordingly to the gap between the stereo modules
    void updateRange(const InDetDD::SiDetectorElement* element1, 
                     const InDetDD::SiDetectorElement* element2, 
                     double& stripLengthGapTolerance, double& min, double& max) const;    
    
    /// Get stripLengthGapTolerance and return offset value for two SiDetectorElement's
    double offset(const InDetDD::SiDetectorElement* element1, 
                  const InDetDD::SiDetectorElement* element2, 
                  double& stripLengthGapTolerance) const;

    /// Not implemented yet
    bool fillSCT_Information(const InDet::SCT_ClusterCollection* clusters1, 
                             const InDet::SCT_ClusterCollection* clusters2,
                             const Amg::Vector3D& vertexVec,
                             const InDetDD::SiDetectorElementCollection* elements) const;
                             
  };
}

#endif //SiSpacePointMakerTool_H
