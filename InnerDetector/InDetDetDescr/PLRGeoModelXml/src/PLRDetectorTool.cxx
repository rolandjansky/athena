/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PLRGeoModelXml/PLRDetectorTool.h"
#include "PLRGeoModelXml/PLRDetectorFactory.h"
#include "PLRGeoModelXml/PLROptions.h"
#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "InDetGeoModelUtils/InDetDDAthenaComps.h"
#include "ReadoutGeometryBase/SiCommonItems.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "StoreGate/StoreGateSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "DetDescrConditions/AlignableTransformContainer.h"
#include "SGTools/DataProxy.h"

using InDetDD::PixelDetectorManager;
using InDetDD::SiDetectorManager;

PLRDetectorTool::PLRDetectorTool(const std::string &type,
                                             const std::string &name,
                                             const IInterface *parent) :
    GeoModelTool(type, name, parent),
    m_detectorName("PLR"),
    m_alignable(false),
    m_gmxFilename(""),
    m_manager(nullptr),
    m_athenaComps(nullptr),
    m_commonItems(nullptr),
    m_geoModelSvc("GeoModelSvc", name),
    m_rdbAccessSvc("RDBAccessSvc", name), 
    m_geoDbTagSvc{"GeoDbTagSvc", name}

    {
//
// Get parameter values from jobOptions file
//
    declareProperty("DetectorName", m_detectorName);
    declareProperty("Alignable", m_alignable);
    declareProperty("GmxFilename", m_gmxFilename);
    declareProperty("RDBAccessSvc", m_rdbAccessSvc);
    declareProperty("GeoModelSvc", m_geoModelSvc);
    declareProperty("GeoDbTagSvc", m_geoDbTagSvc);

}

PLRDetectorTool::~PLRDetectorTool() {
    delete m_athenaComps;
    delete m_commonItems;
}

StatusCode PLRDetectorTool::create() {
//
//   Retrieve all services except LorentzAngleSvc, which has to be done later
//

    // Get the detector configuration.
    ATH_CHECK(m_geoDbTagSvc.retrieve());
    ATH_CHECK(m_rdbAccessSvc.retrieve());
    GeoModelExperiment *theExpt;
    ATH_CHECK(detStore()->retrieve(theExpt, "ATLAS"));
    const PixelID *idHelper; //DEF?
    ATH_CHECK(detStore()->retrieve(idHelper, "PixelID")); //DEF?

//
//    Get their interfaces to pass to the DetectorFactory
//
    m_athenaComps = new InDetDD::AthenaComps("PLRGeoModelXml");
    m_athenaComps->setDetStore(&*(detStore()));
    m_athenaComps->setRDBAccessSvc(&*m_rdbAccessSvc);
    m_athenaComps->setGeoDbTagSvc(&*m_geoDbTagSvc);


    m_commonItems = new InDetDD::SiCommonItems(idHelper);
//
//    Get options from python
//
    InDetDDSLHC::PLROptions options;
    options.setAlignable(m_alignable);
    options.setGmxFilename(m_gmxFilename);
    options.setDetectorName(m_detectorName);

//
// Create the PLRDetectorFactory
//
    // The * converts a ConstPVLink to a ref to a GeoVPhysVol
    // The & takes the address of the GeoVPhysVol
    GeoPhysVol *world = &*theExpt->getPhysVol();
    InDetDDSLHC::PLRDetectorFactory thePLR(m_athenaComps, m_commonItems, options);
    thePLR.create(world);
//
// Get the manager from the factory and store it in the detector store.
//
    m_manager = thePLR.getDetectorManager();

    if (!m_manager) {
        ATH_MSG_ERROR( "PixelDetectorManager not found; not created in PLRDetectorFactory? (PLR uses PixelDetectorManager)" );
        return(StatusCode::FAILURE);
    }

    StatusCode sc;
    sc = detStore()->record(m_manager, m_manager->getName());
    if (sc.isFailure() ) {
        ATH_MSG_ERROR( "Could not register PixelDetectorManager" );
        return StatusCode::FAILURE;
    }
    theExpt->addManager(m_manager);

    // Create a symLink to the SiDetectorManager base class so it can be accessed as either SiDetectorManager or 
    // PixelDetectorManager
    const SiDetectorManager *siDetManager = m_manager;
    sc = detStore()->symLink(m_manager, siDetManager);
    if(sc.isFailure()){
        ATH_MSG_ERROR( "Could not make link between PixelDetectorManager and SiDetectorManager" );
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode PLRDetectorTool::clear() {
    SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<InDetDD::PixelDetectorManager>::ID(),m_manager->getName());
    if(proxy) {
        proxy->reset();
        m_manager = nullptr;
    }
    return StatusCode::SUCCESS;
}

StatusCode PLRDetectorTool::registerCallback() {
// 
//    Register call-back for software alignment
//
    StatusCode sc = StatusCode::FAILURE;
    if (m_alignable) {
        std::string folderName = "/Indet/Align"; //DEF? BCM has: folderName = "/ITk/Align"
        if (detStore()->contains<AlignableTransformContainer>(folderName)) {
            ATH_MSG_DEBUG( "Registering callback on AlignableTransformContainer with folder " << folderName );
            const DataHandle<AlignableTransformContainer> atc;
            sc =  detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool *>(this), atc, folderName);
            if(sc.isFailure()) {
                ATH_MSG_ERROR( "Could not register callback on AlignableTransformContainer with folder " << 
                                    folderName );
            } 
        } 
        else {
            ATH_MSG_WARNING( "Unable to register callback on AlignableTransformContainer with folder " <<
                                 folderName << ", Alignment disabled (only if no Run2 scheme is loaded)!" );
        }
    } 
    else {
        ATH_MSG_INFO( "Alignment disabled. No callback registered" );
        // We return failure otherwise it will try and register a GeoModelSvc callback associated with this callback.
    }
    return sc;
}

StatusCode PLRDetectorTool::align(IOVSVC_CALLBACK_ARGS_P(I, keys)) {
//
//    The call-back routine, which just calls the real call-back routine from the manager.
//
    if (!m_manager) {
        ATH_MSG_WARNING( "Manager does not exist" );
        return StatusCode::FAILURE;
    }
    if (m_alignable) {
        return m_manager->align(I, keys);
    } 
    else {
        ATH_MSG_DEBUG( "Alignment disabled. No alignments applied" );
        return StatusCode::SUCCESS;
    }
}