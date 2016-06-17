/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModelXml/SCT_GMX_DetectorTool.h"
#include "SCT_GeoModelXml/SCT_DetectorFactory.h"
#include "SCT_GeoModelXml/SCT_Options.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetGeoModelUtils/InDetDDAthenaComps.h"
#include "InDetReadoutGeometry/SiCommonItems.h"
#include "InDetCondServices/ISiLorentzAngleSvc.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "InDetGeoModelUtils/IInDetServMatBuilderTool.h"

#include "CLIDSvc/tools/ClassID_traits.h"
#include "SGTools/DataProxy.h"

using InDetDD::SCT_DetectorManager;
using InDetDD::SiDetectorManager;


SCT_GMX_DetectorTool::SCT_GMX_DetectorTool(const std::string &type,
                                             const std::string &name,
                                             const IInterface *parent) :
    GeoModelTool(type, name, parent),
    m_detectorName("SCT"),
    m_alignable(false),
    m_gmxFilename(""),
    m_manager(0),
    m_athenaComps(0),
    m_commonItems(0),
    m_geoModelSvc("GeoModelSvc", name),
    m_rdbAccessSvc("RDBAccessSvc", name),
    m_geometryDBSvc("InDetGeometryDBSvc", name),
    m_lorentzAngleSvc("SCTLorentzAngleSvc", name),
    m_serviceBuilderTool("") {
//
// Get parameter values from jobOptions file
//
    declareProperty("DetectorName", m_detectorName);
    declareProperty("Alignable", m_alignable);
    declareProperty("GmxFilename", m_gmxFilename);
    declareProperty("ServiceBuilderTool", m_serviceBuilderTool);
    declareProperty("RDBAccessSvc", m_rdbAccessSvc);
    declareProperty("GeometryDBSvc", m_geometryDBSvc);
    declareProperty("GeoModelSvc", m_geoModelSvc);
    declareProperty("LorentzAngleSvc", m_lorentzAngleSvc);
}

SCT_GMX_DetectorTool::~SCT_GMX_DetectorTool() {
    delete m_athenaComps;
std::cerr << "To fix: Delete m_commonItems at " << m_commonItems << ". But commented out because it causes a crash\n";
//    delete m_commonItems;
//std::cerr << "OK! managed to delete it.\n";
}

StatusCode SCT_GMX_DetectorTool::create(StoreGateSvc *detStore) {
//
//   Retrieve all services except LorentzAngleSvc, which has to be done later
//
    StatusCode sc;
    sc = m_geoModelSvc.retrieve();
    if (sc.isFailure()) {
        msg(MSG::FATAL) << "Could not locate GeoModelSvc" << endreq;
        return (StatusCode::FAILURE);
    }
    sc = m_rdbAccessSvc.retrieve();
    if (sc.isFailure()) {
        msg(MSG::FATAL) << "Could not locate RDBAccessSvc" << endreq;
        return StatusCode::FAILURE;
    }
    sc = m_geometryDBSvc.retrieve();
    if (sc.isFailure()) {
        msg(MSG::FATAL) << "Could not locate Geometry DB Interface: " << m_geometryDBSvc.name() << endreq;
        return (StatusCode::FAILURE);
    }
    GeoModelExperiment *theExpt;
    sc = detStore->retrieve(theExpt, "ATLAS");
    if (sc.isFailure()) {
        msg(MSG::FATAL) << "Could not find GeoModelExperiment ATLAS" << endreq;
        return (StatusCode::FAILURE);
    }
    const SCT_ID *idHelper;
    if (detStore->retrieve(idHelper, "SCT_ID").isFailure()) {
        msg(MSG::FATAL) << "Could not get SCT ID helper" << endreq;
        return StatusCode::FAILURE;
    }
//
//    Get their interfaces to pass to the DetectorFactory
//
    m_athenaComps = new InDetDD::AthenaComps("SCT_GeoModelXml");
    m_athenaComps->setDetStore(detStore);
    m_athenaComps->setGeoModelSvc(&*m_geoModelSvc);
    m_athenaComps->setRDBAccessSvc(&*m_rdbAccessSvc);
    m_athenaComps->setGeometryDBSvc(&*m_geometryDBSvc);

    m_commonItems = new InDetDD::SiCommonItems(idHelper);
std::cerr << "commonItems created with address " << m_commonItems << "\n";
    m_commonItems->setLorentzAngleSvc(m_lorentzAngleSvc);
//
//    Get options from python
//
    InDetDDSLHC::SCT_Options options;
    options.setAlignable(m_alignable);
    options.setGmxFilename(m_gmxFilename);
//
//   Get the version
//
    DecodeVersionKey versionKey(&*m_geoModelSvc, "SCT");
    if (versionKey.tag() == "AUTO"){
        msg(MSG::ERROR) << "Atlas version tag is AUTO. You must set a version-tag like ATLAS_P2_ITK-00-00-00." << endreq;
        return StatusCode::FAILURE;
    }
    if (versionKey.custom()) 
        msg(MSG::INFO) << "Building custom ";
    else
        msg(MSG::INFO) << "Building ";
    msg(MSG::INFO) << "SCT SLHC with Version Tag: "<< versionKey.tag() << " at Node: " << versionKey.node() << endreq;
//
//    Get the Database Access Service and from there the SCT version tag
//
    std::string sctVersionTag = m_rdbAccessSvc->getChildTag("SCT", versionKey.tag(), versionKey.node(), false);
    msg(MSG::INFO) << "SCT Version: " << sctVersionTag <<  "  Package Version: " << PACKAGE_VERSION << endreq;
//
//   Check if SCT version tag is empty. If so, then the SCT cannot be built.
//   This may or may not be intentional. We just issue an INFO message.
//
    if (sctVersionTag.empty()) {
        msg(MSG::INFO) <<  "No SCT Version. SCT_SLHC will not be built." << endreq;
        return StatusCode::SUCCESS;
    }
//
// Create the SCT_DetectorFactory
//
    // The * converts a ConstPVLink to a ref to a GeoVPhysVol
    // The & takes the address of the GeoVPhysVol
    GeoPhysVol *world = &*theExpt->getPhysVol();
    InDetDDSLHC::SCT_DetectorFactory theSCT(m_athenaComps, m_commonItems, options);
    theSCT.create(world);
//
// Get the manager from the factory and store it in the detector store.
//
    m_manager = theSCT.getDetectorManager();
    if (!m_manager) {
        msg(MSG::ERROR) << "SCT_DetectorManager not found; not created in SCT_DetectorFactory?" << endreq;
        return(StatusCode::FAILURE);
    }
    msg(MSG::DEBUG) << "Registering SCT_DetectorManager. " << endreq;
    sc = detStore->record(m_manager, m_manager->getName());
    if (sc.isFailure() ) {
        msg(MSG::ERROR) << "Could not register SCT_DetectorManager" << endreq;
        return StatusCode::FAILURE;
    }
    theExpt->addManager(m_manager);

    // Create a symLink to the SiDetectorManager base class so it can be accessed as either SiDetectorManager or 
    // SCT_DetectorManager
    const SiDetectorManager *siDetManager = m_manager;
    sc = detStore->symLink(m_manager, siDetManager);
    if(sc.isFailure()){
        msg(MSG::ERROR) << "Could not make link between SCT_DetectorManager and SiDetectorManager" << endreq;
        return StatusCode::FAILURE;
    }
//
//    And retrieve the LorentzAngleService. Has to be after the symLink just made,
//    which has to be after the manager is made by the DetectorFactory.
//
    if (m_lorentzAngleSvc.empty()) {
        msg(MSG::INFO) << "Lorentz angle service not requested." << endreq;
    }
    else {
        sc = m_lorentzAngleSvc.retrieve();
        if (sc.isFailure()) {
            msg(MSG::INFO) << "Could not retrieve Lorentz angle service:" <<  m_lorentzAngleSvc << endreq;
        }
        else {
            msg(MSG::INFO) << "Lorentz angle service retrieved: " << m_lorentzAngleSvc << endreq;
        }
    }

    return StatusCode::SUCCESS;
}

StatusCode SCT_GMX_DetectorTool::clear(StoreGateSvc* detStore) {
    SG::DataProxy* _proxy = detStore->proxy(ClassID_traits<InDetDD::SCT_DetectorManager>::ID(),m_manager->getName());
    if(_proxy) {
        _proxy->reset();
        m_manager = 0;
    }
    return StatusCode::SUCCESS;
}
