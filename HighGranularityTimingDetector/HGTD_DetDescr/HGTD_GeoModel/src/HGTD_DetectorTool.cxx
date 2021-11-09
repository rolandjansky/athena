/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// includes
#include "HGTD_GeoModel/HGTD_DetectorTool.h"
#include "HGTD_GeoModel/HGTD_DetectorFactory.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "StoreGate/StoreGateSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "DetDescrConditions/AlignableTransformContainer.h"
#include "ReadoutGeometryBase/SiCommonItems.h"

#include "AthenaKernel/ClassID_traits.h"
#include "SGTools/DataProxy.h"

HGTD_DetectorTool::HGTD_DetectorTool(const std::string &type,
                                     const std::string &name,
                                     const IInterface *parent) :
  GeoModelTool(type, name, parent),
  m_geoDbTagSvc("GeoDbTagSvc", name),
  m_rdbAccessSvc("RDBAccessSvc", name),
  m_detectorManager(nullptr),
  m_printIDdict(false) {
    // Get parameter values from the python configuration
    declareProperty("GeoDbTagSvc", m_geoDbTagSvc);
    declareProperty("RDBAccessSvc", m_rdbAccessSvc);
    declareProperty("PrintModuleNumberPerRow", m_printIDdict);
}

StatusCode HGTD_DetectorTool::create() {

    ATH_CHECK(m_geoDbTagSvc.retrieve());
    ATH_CHECK(m_rdbAccessSvc.retrieve());

    // Get their interfaces to pass to the DetectorFactory
    m_athenaComps.setDetStore(detStore().operator->());
    m_athenaComps.setGeoDbTagSvc(&*m_geoDbTagSvc);
    m_athenaComps.setRDBAccessSvc(&*m_rdbAccessSvc);
    const HGTD_ID* idHelper{nullptr};
    ATH_CHECK(detStore()->retrieve(idHelper, "HGTD_ID"));
    m_athenaComps.setIdHelper(idHelper);

    GeoModelExperiment *theExpt;
    ATH_CHECK(detStore()->retrieve(theExpt, "ATLAS"));

    // The * converts a ConstPVLink to a ref to a GeoVPhysVol, the & takes the address of the GeoVPhysVol
    GeoPhysVol *world = &*theExpt->getPhysVol();

    HGTD_DetectorFactory theHGTDFactory(&m_athenaComps);
    theHGTDFactory.setPrintIdentifierDict(m_printIDdict);
    theHGTDFactory.create(world);

    // Get the manager from the factory and store it in the detector store.
    m_detectorManager = theHGTDFactory.getDetectorManager();
    if (!m_detectorManager) {
        ATH_MSG_ERROR( "HGTD_DetectorManager not found; not created in HGTD_DetectorFactory?" );
        return(StatusCode::FAILURE);
    }

    ATH_MSG_DEBUG("Registering HGTD_DetectorManager as " << m_detectorManager->getName() );
    ATH_CHECK( detStore()->record(m_detectorManager, m_detectorManager->getName()) );

    theExpt->addManager(m_detectorManager);
    return StatusCode::SUCCESS;
}

StatusCode HGTD_DetectorTool::clear() {

    // Release manager from the detector store
    SG::DataProxy* proxy = detStore()->proxy(ClassID_traits< HGTD_DetectorManager >::ID(),m_detectorManager->getName());
    if (proxy) {
      proxy->reset();
      m_detectorManager = nullptr;
    }

    return StatusCode::SUCCESS;
}

StatusCode HGTD_DetectorTool::registerCallback ATLAS_NOT_THREAD_SAFE () // Thread unsafe detStore()->regFcn (callback) is used.
{

    // Register call-back for software alignment
    StatusCode sc = StatusCode::FAILURE;
    if (m_alignable.value()) {
        std::string folderName = "/Indet/AlignHGTD";
        if ( detStore()->contains<AlignableTransformContainer>(folderName) ) {
        ATH_MSG_DEBUG( "Registering callback on AlignableTransformContainer with folder " << folderName );
            const DataHandle<AlignableTransformContainer> atc;
            ATH_CHECK( detStore()->regFcn( &IGeoModelTool::align, dynamic_cast<IGeoModelTool *>(this), atc, folderName ) );
        sc = StatusCode::SUCCESS;
        }
        else {
        ATH_MSG_WARNING( "Unable to register callback on AlignableTransformContainer with folder " <<
                 folderName << ", Alignment disabled (only if no Run-2 scheme is loaded)!" );
        }
    }
    else {
        ATH_MSG_INFO( "Alignment disabled. No callback registered" );
        // We return failure otherwise it will try and register a GeoModelSvc callback associated with this callback.
    }
    return sc;
}

StatusCode HGTD_DetectorTool::align(IOVSVC_CALLBACK_ARGS_P( I, keys ) ) {

    //   The call-back routine, which just calls the real call-back routine from the manager.

    if ( ! m_detectorManager ) {
        ATH_MSG_WARNING( "Manager does not exist for " << I <<" "<< keys );
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO( "No align method yet implemented for HGTD_DetectorManager. Nothing to do." );

    return StatusCode::SUCCESS;
}
