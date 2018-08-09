/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SiSpacePointMakerTool_H
#define SiSpacePointMakerTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include <string>
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "SiSpacePointTool/SCTinformation.h" 
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
        
        void newEvent();

		void fillSCT_SpacePointCollection( const InDet::SCT_ClusterCollection* clusters1, 
                                           const InDet::SCT_ClusterCollection* clusters2,
                                           double min, double max, bool allClusters, 
                                           const Amg::Vector3D& vertexVec, 
                                           const InDetDD::SCT_DetectorManager *SCT_Manager, 
                                           SpacePointCollection* spacepointCollection );

		void fillPixelSpacePointCollection( const InDet::PixelClusterCollection* clusters, 
                                            SpacePointCollection* spacepointCollection );

		void fillSCT_SpacePointEtaOverlapCollection( const InDet::SCT_ClusterCollection* clusters1, 
                                                     const InDet::SCT_ClusterCollection* clusters2, 
                                                     double min, double max, bool allClusters, 
                                                     const Amg::Vector3D& vertexVec, 
                                                     const InDetDD::SCT_DetectorManager *SCT_Manager, 
                                                     SpacePointOverlapCollection* spacepointOverlapCollection );

		void fillSCT_SpacePointPhiOverlapCollection( const InDet::SCT_ClusterCollection* clusters1, 
                                                     const InDet::SCT_ClusterCollection* clusters2, 
                                                     double min1, double max1, double min2, double max2, bool allClusters, 
                                                     const Amg::Vector3D& vertexVec, 
                                                     const InDetDD::SCT_DetectorManager *SCT_Manager, 
                                                     SpacePointOverlapCollection* spacepointOverlapCollection );

	private:
		double m_stripLengthTolerance;
		double m_SCTgapParameter;
		double m_stripLengthGapTolerance;

		std::vector<Trk::SpacePoint*> m_tmpSpacePoints;
		const SCT_ID* m_idHelper; 
		void offset(const InDetDD::SiDetectorElement *element1, const InDetDD::SiDetectorElement *element2);
        
        std::vector<SCTinformation> m_SCT0;
        std::vector<SCTinformation> m_SCT1;
        const InDetDD::SiDetectorElement* m_element0  ;
        const InDetDD::SiDetectorElement* m_element1  ;
        const InDetDD::SiDetectorElement* m_elementOLD;

        bool fillSCT_Information(const InDet::SCT_ClusterCollection* clusters1,const InDet::SCT_ClusterCollection* clusters2,
                                 const Amg::Vector3D& vertexVec, const InDetDD::SCT_DetectorManager *SCT_Manager);
        
        // Convert clusters to space points
		void makeSCT_SpacePoints();

  };
}
#endif //SiSpacePointMakerTool_H
