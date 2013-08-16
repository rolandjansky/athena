/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SiSpacePointMakerTool_H
#define SiSpacePointMakerTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include <string>

// Cluster and space point collections
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "TrkSpacePoint/SpacePointCollection.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h" 
#include "GeoPrimitives/GeoPrimitives.h"
 
namespace Trk
{
 class SpacePoint;
}

namespace InDetDD
{
  class SCT_DetectorManager;
  class SiDetectorElement;
}

namespace InDet{
  class SiCluster;

class SiSpacePointMakerTool : public AthAlgTool {

 public:


  // Constructor 
  SiSpacePointMakerTool( const std::string& type, 
                      const std::string& name, 
                      const IInterface* parent );

  virtual ~SiSpacePointMakerTool();

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize();

  virtual StatusCode finalize();

  // Convert clusters to space points
  Trk::SpacePoint* makeSCT_SpacePoint(const InDet::SiCluster& cluster1, const InDet::SiCluster& cluster2, const Amg::Vector3D& m_vertex,
			const InDetDD::SiDetectorElement *element1, const InDetDD::SiDetectorElement *element2);

  void fillSCT_SpacePointCollection(const InDet::SCT_ClusterCollection* clusters1, const InDet::SCT_ClusterCollection* clusters2, double min, double max, bool m_allClusters, const Amg::Vector3D& m_vertex, const InDetDD::SCT_DetectorManager *m_manager, SpacePointCollection* spacepointCollection);

 void fillPixelSpacePointCollection(const InDet::PixelClusterCollection* clusters, SpacePointCollection* spacepointCollection);

 void fillSCT_SpacePointEtaOverlapCollection(const InDet::SCT_ClusterCollection* clusters1, const InDet::SCT_ClusterCollection* clusters2, double min, double max, bool m_allClusters, const Amg::Vector3D& m_vertex, const InDetDD::SCT_DetectorManager *m_manager, SpacePointOverlapCollection* m_spacepointoverlapCollection);

 void fillSCT_SpacePointPhiOverlapCollection(const InDet::SCT_ClusterCollection* clusters1, const InDet::SCT_ClusterCollection* clusters2, double min1, double max1, double min2, double max2, bool m_allClusters, const Amg::Vector3D& m_vertex, const InDetDD::SCT_DetectorManager *m_manager, SpacePointOverlapCollection* m_spacepointoverlapCollection);

 private:
 double m_stripLengthTolerance;

 // option to use closest approach of SCT strips as position for SpacePoint 
 bool m_usePerpProj;

 std::vector<Trk::SpacePoint*> m_tmpSpacePoints;

};
}
#endif //SiSpacePointMakerTool_H
