/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "BCMPrimeGeoModelXml/BCMPrimeDetectorTool.h"
#include "BCMPrimeGeoModelXml/BCMPrimeDetectorFactory.h"
#include "BCMPrimeGeoModelXml/BCMPrimeOptions.h"
#include "InDetGeoModelUtils/InDetDDAthenaComps.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "DetDescrConditions/AlignableTransformContainer.h"
#include "SGTools/DataProxy.h"

BCMPrimeDetectorTool::BCMPrimeDetectorTool(const std::string &type,
                                           const std::string &name,
                                           const IInterface *parent) :
    GeoModelTool(type, name, parent),
    m_detectorName("BCMPrime"),
    m_alignable(false),
    m_gmxFilename(""),
    m_manager(nullptr),
    m_athenaComps(nullptr),
    m_rdbAccessSvc("RDBAccessSvc", name),
    m_geometryDBSvc("InDetGeometryDBSvc", name),
    m_geoDbTagSvc{"GeoDbTagSvc", name}

    {

    //
    // Get parameter values from jobOptions file
    //
    declareProperty("DetectorName", m_detectorName);
    declareProperty("Alignable", m_alignable);
    declareProperty("GmxFilename", m_gmxFilename);
    declareProperty("RDBAccessSvc", m_rdbAccessSvc);
    declareProperty("GeometryDBSvc", m_geometryDBSvc);
    declareProperty("GeoDbTagSvc", m_geoDbTagSvc);

}

BCMPrimeDetectorTool::~BCMPrimeDetectorTool() {
    delete m_athenaComps;
}

StatusCode BCMPrimeDetectorTool::create() {

    //
    //   Retrieve all services
    //
    // Get the detector configuration.
    ATH_CHECK(m_geoDbTagSvc.retrieve());
    ATH_CHECK(m_rdbAccessSvc.retrieve());
    ATH_CHECK(m_geometryDBSvc.retrieve());
    GeoModelExperiment *theExpt;
    ATH_CHECK(detStore()->retrieve(theExpt, "ATLAS"));

    //
    //    Get their interfaces to pass to the DetectorFactory
    //
    m_athenaComps = new InDetDD::AthenaComps("BCMPrimeGeoModelXml");
    m_athenaComps->setDetStore(&*(detStore()));
    m_athenaComps->setRDBAccessSvc(&*m_rdbAccessSvc);
    m_athenaComps->setGeometryDBSvc(&*m_geometryDBSvc);
    m_athenaComps->setGeoDbTagSvc(&*m_geoDbTagSvc);

    //
    //    Get options from python
    //
    InDetDDSLHC::BCMPrimeOptions options;
    options.setAlignable(m_alignable);
    options.setGmxFilename(m_gmxFilename);
    options.setDetectorName(m_detectorName);

    //
    // Create the BCMPrimeDetectorFactory
    //
    // The * converts a ConstPVLink to a ref to a GeoVPhysVol
    // The & takes the address of the GeoVPhysVol
    GeoPhysVol *world = &*theExpt->getPhysVol();
    InDetDDSLHC::BCMPrimeDetectorFactory bcmPrime(m_athenaComps, options);
    bcmPrime.create(world);

    //
    // Get the manager from the factory and store it in the detector store.
    //
    m_manager = bcmPrime.getDetectorManager();

    if (!m_manager) {
        ATH_MSG_ERROR( "PixelDetectorManager not found; not created in BCMPrimeDetectorFactory?" );
        return(StatusCode::FAILURE);
    }

    StatusCode sc;
    sc = detStore()->record(m_manager, m_manager->getName());
    if (sc.isFailure() ) {
        ATH_MSG_ERROR( "Could not register BCMPrimeDetectorManager" );
        return StatusCode::FAILURE;
    }
    theExpt->addManager(m_manager);

    return StatusCode::SUCCESS;
}

StatusCode BCMPrimeDetectorTool::clear() {
    SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<InDetDD::BCMPrimeDetectorManager>::ID(),m_manager->getName());
    if(proxy) {
        proxy->reset();
        m_manager = nullptr;
    }
    return StatusCode::SUCCESS;
}

StatusCode BCMPrimeDetectorTool::registerCallback() {

    // 
    //    Register call-back for software alignment
    //
    StatusCode sc = StatusCode::FAILURE;
    if (m_alignable) {
        std::string folderName = "/ITk/Align";
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

StatusCode BCMPrimeDetectorTool::align(IOVSVC_CALLBACK_ARGS_P(I, keys)) {

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
