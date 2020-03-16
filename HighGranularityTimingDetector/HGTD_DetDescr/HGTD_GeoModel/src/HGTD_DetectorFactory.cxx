/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HGTD_GeoModel/HGTD_DetectorFactory.h"

#include <string>
#include <sstream>

#include "StoreGate/StoreGateSvc.h" // For alignment getAlignableFolderType()
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "DetDescrConditions/AlignableTransformContainer.h"

#include "InDetGeoModelUtils/InDetDDAthenaComps.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

// // #include "InDetReadoutGeometry/SiCommonItems.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"
// // #include "InDetReadoutGeometry/Version.h"
// // #include "SCT_ReadoutGeometry/SCT_ModuleSideDesign.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

// // #include "SCT_GeoModelXml/SCT_Options.h"
// // #include "GeoModelXml/Gmx2Geo.h"
// // #include "SCT_GeoModelXml/SCT_GmxInterface.h"
// // #include "GeoModelXml/GmxInterface.h"

// // #include "PathResolver/PathResolver.h"

using namespace std;

namespace HGTDGeo {

HGTD_DetectorFactory::HGTD_DetectorFactory(InDetDD::AthenaComps* athenaComps) :
    InDetDD::DetectorFactoryBase(athenaComps),
    m_detectorManager(),
    m_athenaComps(athenaComps)
{
//
//    Create the detector manager
//
    m_detectorManager = new HGTD_DetectorManager();
//
//    Set Detector Manager HGTD version information
//

}

HGTD_DetectorFactory::~HGTD_DetectorFactory() {
    // NB the detector manager (m_detectorManager) is stored in the detector store by the Tool and so we don't delete it.
}

void HGTD_DetectorFactory::create(GeoPhysVol* world) {

}

HGTD_DetectorManager* HGTD_DetectorFactory::getDetectorManager() const {
    return m_detectorManager;
}

} // End HGTDGeo namespace
