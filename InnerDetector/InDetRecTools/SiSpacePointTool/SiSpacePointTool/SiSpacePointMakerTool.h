/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SiSpacePointMakerTool_H
#define SiSpacePointMakerTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include <string>
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"

#include "GeoPrimitives/GeoPrimitives.h"

class SCT_ID;
class SpacePointCollection;
class SpacePointOverlapCollection;
namespace Trk{
 class SpacePoint;
}

namespace InDetDD{
  class SCT_DetectorManager;
  class SiDetectorElement;
}
namespace InDet{
  class SiCluster;
  //class SCT_ClusterCollection; can't fwd declare due to typedef
  //class PixelClusterCollection;
}

namespace InDet{
  class SiSpacePointMakerTool : public AthAlgTool {
	public:
		// Constructor 
		SiSpacePointMakerTool( const std::string& type, const std::string& name, const IInterface* parent );

		virtual ~SiSpacePointMakerTool();

		static const InterfaceID& interfaceID();

		virtual StatusCode initialize();

		virtual StatusCode finalize();

		// Convert clusters to space points
		Trk::SpacePoint* makeSCT_SpacePoint(const InDet::SiCluster& cluster1, const InDet::SiCluster& cluster2, 
		  const Amg::Vector3D& vertexVec,
		  const InDetDD::SiDetectorElement *element1, const InDetDD::SiDetectorElement *element2, double stripLengthGapTolerance) const;

		void fillSCT_SpacePointCollection(const InDet::SCT_ClusterCollection* clusters1, 
		  const InDet::SCT_ClusterCollection* clusters2, double min, double max, bool allClusters, 
		  const Amg::Vector3D& vertexVec, const InDetDD::SCT_DetectorManager *SCT_Manager, 
		  SpacePointCollection* spacepointCollection) const;

		void fillPixelSpacePointCollection(const InDet::PixelClusterCollection* clusters, 
		  SpacePointCollection* spacepointCollection) const;

		void fillSCT_SpacePointEtaOverlapCollection(const InDet::SCT_ClusterCollection* clusters1, 
		  const InDet::SCT_ClusterCollection* clusters2, double min, double max, bool allClusters, 
		  const Amg::Vector3D& vertexVec, const InDetDD::SCT_DetectorManager *SCT_Manager, 
		  SpacePointOverlapCollection* spacepointOverlapCollection) const;

		void fillSCT_SpacePointPhiOverlapCollection(const InDet::SCT_ClusterCollection* clusters1, 
		  const InDet::SCT_ClusterCollection* clusters2, double min1, double max1, double min2, 
		  double max2, bool allClusters, const Amg::Vector3D& vertexVec , 
		  const InDetDD::SCT_DetectorManager *SCT_Manager, 
		  SpacePointOverlapCollection* spacepointOverlapCollection) const;

	private:
		double m_stripLengthTolerance;
		double m_SCTgapParameter;

		// option to use closest approach of SCT strips as position for SpacePoint 
		bool m_usePerpProj;

		const SCT_ID* m_idHelper; 
		double offset(const InDetDD::SiDetectorElement *element1, const InDetDD::SiDetectorElement *element2, double&) const;

  };
}
#endif //SiSpacePointMakerTool_H
