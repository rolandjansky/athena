/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// includes
#include "HGTD_GeoModelXml/HGTD_GMX_DetectorTool.h"
#include "HGTD_GeoModelXml/HGTD_GmxInterface.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"
#include "ReadoutGeometryBase/SiCommonItems.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "DetDescrConditions/AlignableTransformContainer.h"

#include "SGTools/DataProxy.h"

HGTD_GMX_DetectorTool::HGTD_GMX_DetectorTool(const std::string &type,
                                             const std::string &name,
                                             const IInterface *parent) :
    GeoModelXmlTool(type, name, parent)
    {
    }

StatusCode HGTD_GMX_DetectorTool::create() {

    //retrieve the common stuff
    ATH_CHECK(createBaseTool());
    
    const HGTD_ID* idHelper{nullptr};
    ATH_CHECK(detStore()->retrieve(idHelper, "HGTD_ID"));

    GeoModelExperiment *theExpt = nullptr;
    ATH_CHECK(detStore()->retrieve(theExpt, "ATLAS"));

    m_commonItems = new InDetDD::SiCommonItems(idHelper); // TODO: use unique_ptr

    // The * converts a ConstPVLink to a ref to a GeoVPhysVol
    // The & takes the address of the GeoVPhysVol
    ATH_MSG_INFO("C R E A T E   W O R L D");
    GeoPhysVol *world = &*theExpt->getPhysVol();

    m_detectorManager  = createManager(world);
    //getName here will return whatever is passed as name to DetectorFactory - make this more explicit?
    ATH_CHECK(detStore()->record(m_detectorManager, m_detectorManager->getName()));

    theExpt->addManager(m_detectorManager);

    return StatusCode::SUCCESS;
}

StatusCode HGTD_GMX_DetectorTool::clear() {

    // Release manager from the detector store
    SG::DataProxy* _proxy = detStore()->proxy(ClassID_traits< HGTD_DetectorManager >::ID(),m_detectorManager->getName());
    if (_proxy) {
      _proxy->reset();
      m_detectorManager = nullptr;
    }

    return StatusCode::SUCCESS;
}

StatusCode HGTD_GMX_DetectorTool::registerCallback ATLAS_NOT_THREAD_SAFE () // Thread unsafe detStore()->regFcn (callback) is used.
{

    // Register call-back for software alignment
    StatusCode sc = StatusCode::FAILURE;
    if (m_alignable) {
        if ( detStore()->contains<AlignableTransformContainer>(m_alignmentFolderName) ) {
        ATH_MSG_DEBUG( "Registering callback on AlignableTransformContainer with folder " << m_alignmentFolderName );
            const DataHandle<AlignableTransformContainer> atc;
            ATH_CHECK( detStore()->regFcn( &IGeoModelTool::align, dynamic_cast<IGeoModelTool *>(this), atc, m_alignmentFolderName ) );
        sc = StatusCode::SUCCESS;
        }
        else {
        ATH_MSG_WARNING( "Unable to register callback on AlignableTransformContainer with folder " <<
                 m_alignmentFolderName << ", Alignment disabled (only if no Run-2 scheme is loaded)!" );
        }
    }
    else {
        ATH_MSG_INFO( "Alignment disabled. No callback registered" );
        // We return failure otherwise it will try and register a GeoModelSvc callback associated with this callback.
    }
    return sc;
}

StatusCode HGTD_GMX_DetectorTool::align(IOVSVC_CALLBACK_ARGS_P( I, keys ) ) {

    //   The call-back routine, which just calls the real call-back routine from the manager.

    if ( ! m_detectorManager ) {
        ATH_MSG_WARNING( "Manager does not exist for " << I <<" "<< keys );
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO( "No align method yet implemented for HGTD_DetectorManager. Nothing to do." );

    return StatusCode::SUCCESS;
}

HGTD_DetectorManager * HGTD_GMX_DetectorTool::createManager(GeoPhysVol * theWorld){

    HGTD_DetectorManager * theManager = new HGTD_DetectorManager(&*detStore());
    //in the factory we would set the version here and write it to the mgr... is it really necessary???
    //Leave it out for now until we find we need it...

    HGTD_GmxInterface gmxInterface(theManager, m_commonItems);

    //Load the geometry, create the volume, 
    //and then find the volume index within the world to allow it to be added
    //last two arguments are the location in the DB to look for the clob
    //(may want to make those configurables)
    int childIndex = createTopVolume(theWorld,gmxInterface, "HGTD", "HGTDXDD");
    if(childIndex != -1){ //-1 represents an error state from the above method
       theManager->addTreeTop(&*theWorld->getChildVol(childIndex));
    }
    return theManager;
  
  }