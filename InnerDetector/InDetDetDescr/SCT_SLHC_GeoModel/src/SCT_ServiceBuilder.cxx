/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_ServiceBuilder.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "SCT_SLHC_GeoModel/SCT_MaterialManager.h"

#include "InDetGeoModelUtils/VolumeSplitterUtils.h"
#include "InDetGeoModelUtils/VolumeBuilder.h"
#include "InDetGeoModelUtils/ServiceVolume.h"
#include "InDetGeoModelUtils/IInDetServMatBuilderTool.h"

namespace InDetDDSLHC {
SCT_ServiceBuilder::SCT_ServiceBuilder(InDetDD::Zone * topZone, 
				       const SCT_GeometryManager * geometryManager)
  : m_servMatBuilder(0)
{
  if (geometryManager->athenaComps()->serviceBuilderTool()) {
    const std::vector<const InDetDD::ServiceVolume *> & services = geometryManager->athenaComps()->serviceBuilderTool()->getServices();
    m_servMatBuilder = new InDetDD::VolumeBuilder(*topZone, services);
    m_servMatBuilder->setMaterialManager(geometryManager->materialManager());
  }
  delete topZone; topZone=0;
}

SCT_ServiceBuilder::~SCT_ServiceBuilder(){
delete m_servMatBuilder; m_servMatBuilder=0;

}

}
