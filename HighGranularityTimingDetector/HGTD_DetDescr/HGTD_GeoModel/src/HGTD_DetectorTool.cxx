/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// includes
#include "HGTD_GeoModel/HGTD_DetectorTool.h"
#include "HGTD_GeoModel/HGTD_DetectorFactory.h"
// // #include "SCT_GeoModelXml/SCT_Options.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"
#include "InDetGeoModelUtils/InDetDDAthenaComps.h"
// // #include "InDetReadoutGeometry/SiCommonItems.h"
// // #include "InDetCondServices/ISiLorentzAngleSvc.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "DetDescrConditions/AlignableTransformContainer.h"

#include "CLIDSvc/tools/ClassID_traits.h"
#include "SGTools/DataProxy.h"

// using InDetDD::SCT_DetectorManager;
// using InDetDD::SiDetectorManager;

HGTD_DetectorTool::HGTD_DetectorTool(const std::string &type,
                                     const std::string &name,
                                     const IInterface *parent) :
    GeoModelTool(type, name, parent),
    m_detectorName("HGTD"),
    m_alignable(false),
    // // m_gmxFilename(""),
    m_manager(0),
    m_athenaComps(0),
    // // m_commonItems(0),
    m_geoModelSvc("GeoModelSvc", name),
    m_rdbAccessSvc("RDBAccessSvc", name),
    m_geometryDBSvc("InDetGeometryDBSvc", name),
    // // m_lorentzAngleSvc("SCTLorentzAngleSvc", name)
    m_geometryConfig("FULL"),
    m_innerPixelTool(""),
    m_HGTD_isbaseline(true)
{
//
// Get parameter values from jobOptions file
//
    declareProperty("DetectorName", m_detectorName);
    declareProperty("Alignable", m_alignable);
    // declareProperty("GmxFilename", m_gmxFilename);
    declareProperty("GeoModelSvc", m_geoModelSvc);
    declareProperty("RDBAccessSvc", m_rdbAccessSvc);
    declareProperty("GeometryDBSvc", m_geometryDBSvc);
    // // declareProperty("LorentzAngleSvc", m_lorentzAngleSvc);
    declareProperty("GeometryConfig", m_geometryConfig);
    declareProperty("PixelPlanarTool", m_innerPixelTool);
    declareProperty("HGTD_BaselineReadout", m_HGTD_isbaseline);
}

HGTD_DetectorTool::~HGTD_DetectorTool() {
    delete m_athenaComps;
}

StatusCode HGTD_DetectorTool::create(StoreGateSvc *detStore) {
//
//   Retrieve all services except LorentzAngleSvc, which has to be done later
//
    StatusCode sc;
    sc = m_geoModelSvc.retrieve();
    if (sc.isFailure()) {
        msg(MSG::FATAL) << "Could not locate GeoModelSvc" << endmsg;
        return (StatusCode::FAILURE);
    }
    sc = m_rdbAccessSvc.retrieve();
    if (sc.isFailure()) {
        msg(MSG::FATAL) << "Could not locate RDBAccessSvc" << endmsg;
        return StatusCode::FAILURE;
    }
    sc = m_geometryDBSvc.retrieve();
    if (sc.isFailure()) {
        msg(MSG::FATAL) << "Could not locate Geometry DB Interface: " << m_geometryDBSvc.name() << endmsg;
        return (StatusCode::FAILURE);
    }
    GeoModelExperiment *theExpt;
    sc = detStore->retrieve(theExpt, "ATLAS");
    if (sc.isFailure()) {
        msg(MSG::FATAL) << "Could not find GeoModelExperiment ATLAS" << endmsg;
        return (StatusCode::FAILURE);
    }
//
//    Get their interfaces to pass to the DetectorFactory
//
    m_athenaComps = new InDetDD::AthenaComps("HGTD_GeoModel");
    m_athenaComps->setDetStore(detStore);
    m_athenaComps->setGeoModelSvc(&*m_geoModelSvc);
    m_athenaComps->setRDBAccessSvc(&*m_rdbAccessSvc);
    m_athenaComps->setGeometryDBSvc(&*m_geometryDBSvc);

//
//   Get the version and configure things accordingly here
//

//
// Create the HGTD_DetectorFactory
//
    // The * converts a ConstPVLink to a ref to a GeoVPhysVol
    // The & takes the address of the GeoVPhysVol
    GeoPhysVol *world = &*theExpt->getPhysVol();
    // // InDetDDSLHC::SCT_DetectorFactory theSCT(m_athenaComps, m_commonItems, options);
    // // theSCT.create(world);
    HGTDGeo::HGTD_DetectorFactory theHGTDFactory(m_athenaComps, m_geometryConfig=="FULL");
    if (!m_innerPixelTool.empty()) {
        StatusCode sc = m_innerPixelTool.retrieve();
        if (!sc.isFailure()) {
            msg(MSG::INFO) << "Inner Pixel Tool retrieved: " << m_innerPixelTool << endreq;
            theHGTDFactory.setPixelBasics(m_innerPixelTool->getPixelGeoBuilderBasics());
            theHGTDFactory.setHGTDBaseline(m_HGTD_isbaseline);
        } else {
            msg(MSG::INFO) << "Inner Pixel Tool could not retrieve " << m_innerPixelTool << endreq;
        }
    } else msg(MSG::INFO) << "Inner Pixel Tool empty???" << endreq;
    theHGTDFactory.create(world);
//
// Get the manager from the factory and store it in the detector store.
//
    m_manager = theHGTDFactory.getDetectorManager();

    if (!m_manager) {
        msg(MSG::ERROR) << "HGTD_DetectorManager not found; not created in HGTD_DetectorFactory?" << endmsg;
        return(StatusCode::FAILURE);
    }

    sc = detStore->record(m_manager, m_manager->getName());
    if (sc.isFailure() ) {
        msg(MSG::ERROR) << "Could not register HGTD_DetectorManager" << endmsg;
        return StatusCode::FAILURE;
    }
    theExpt->addManager(m_manager);

    // // // Create a symLink to the SiDetectorManager base class so it can be accessed as either SiDetectorManager or 
    // // // SCT_DetectorManager
    // // const SiDetectorManager *siDetManager = m_manager;
    // // sc = detStore->symLink(m_manager, siDetManager);
    // // if(sc.isFailure()){
    // //     msg(MSG::ERROR) << "Could not make link between SCT_DetectorManager and SiDetectorManager" << endmsg;
    // //     return StatusCode::FAILURE;
    // // }

//
//    And retrieve the LorentzAngleService. Has to be after the symLink just made,
//    which has to be after the manager is made by the DetectorFactory.
//

    return StatusCode::SUCCESS;
}

StatusCode HGTD_DetectorTool::clear(StoreGateSvc* detStore) {

    // Release manager from the detector store
    SG::DataProxy* _proxy = detStore->proxy(ClassID_traits<HGTD_DetectorManager>::ID(),m_manager->getName());
    if(_proxy) {
      _proxy->reset();
      m_manager = 0;
    }

    return StatusCode::SUCCESS;
}

StatusCode HGTD_DetectorTool::registerCallback(StoreGateSvc* detStore) {
//
//    Register call-back for software alignment
//
    msg(MSG::WARNING) << "No callback registration has yet been implemented for HGTD, nothing to do!" << endmsg;

    return StatusCode::SUCCESS;
}

StatusCode HGTD_DetectorTool::align(IOVSVC_CALLBACK_ARGS_P(I, keys)) {
//
//    The call-back routine, which just calls the real call-back routine from the manager.
//
    msg(MSG::WARNING) << "No alignment method in HGTD_DetectorManager implemented yet, nothing to do!" << endmsg;

    return StatusCode::SUCCESS;
}
