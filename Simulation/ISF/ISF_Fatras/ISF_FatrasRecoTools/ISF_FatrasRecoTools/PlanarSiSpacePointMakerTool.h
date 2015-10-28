/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FATRASREOCTOOLS_ISF_SISPACEPOINTMAKERTOOL_H
#define ISF_FATRASREOCTOOLS_ISF_SISPACEPOINTMAKERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

// Cluster and space point collections
#include "ISF_FatrasEvent/PlanarClusterCollection.h"
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
}

namespace iFatras{
  class PlanarSiSpacePointMakerTool : public AthAlgTool {
    
  public:
    
    // Constructor 
    PlanarSiSpacePointMakerTool( const std::string& type, 
			   const std::string& name, 
			   const IInterface* parent );
    
    virtual ~PlanarSiSpacePointMakerTool();
    
    static const InterfaceID& interfaceID();
    
    virtual StatusCode initialize();
    
    virtual StatusCode finalize();
    
    void fillPixelSpacePointCollection(const PlanarClusterCollection* clusters, SpacePointCollection* spacepointCollection);
            
  };
} // end namespace

#endif // ISF_FATRASREOCTOOLS_ISF_SISPACEPOINTMAKERTOOL_H


