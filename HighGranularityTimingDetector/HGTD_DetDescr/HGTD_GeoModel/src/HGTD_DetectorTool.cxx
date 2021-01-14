/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// includes
#include "HGTD_GeoModel/HGTD_DetectorTool.h"
#include "HGTD_GeoModel/HGTD_DetectorFactory.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"
#include "InDetGeoModelUtils/InDetDDAthenaComps.h"
#include "ReadoutGeometryBase/SiCommonItems.h"
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

HGTD_DetectorTool::HGTD_DetectorTool(const std::string &type,
                                     const std::string &name,
                                     const IInterface *parent) :
    GeoModelTool(type, name, parent),
    m_detectorName("HGTD"),
    m_alignable(false),
    m_manager(0),
    m_athenaComps(0),
    m_commonItems(0),
    m_geoModelSvc("GeoModelSvc", name),
    m_rdbAccessSvc("RDBAccessSvc", name),
    m_geometryDBSvc("InDetGeometryDBSvc", name),
    m_geometryConfig("FULL"),
    m_HGTD_isbaseline(true)
{
//
// Get parameter values from jobOptions file
//
    declareProperty("DetectorName", m_detectorName);
    declareProperty("Alignable", m_alignable);
    declareProperty("GeoModelSvc", m_geoModelSvc);
    declareProperty("RDBAccessSvc", m_rdbAccessSvc);
    declareProperty("GeometryDBSvc", m_geometryDBSvc);
    declareProperty("GeometryConfig", m_geometryConfig);
    declareProperty("HGTD_BaselineReadout", m_HGTD_isbaseline);
}

HGTD_DetectorTool::~HGTD_DetectorTool() {
    delete m_athenaComps;
}

StatusCode HGTD_DetectorTool::create(StoreGateSvc* detStore) {
//
//   Retrieve all services except LorentzAngleSvc, which has to be done later
//

    ATH_CHECK(m_geoModelSvc.retrieve());
    ATH_CHECK(m_rdbAccessSvc.retrieve());
    ATH_CHECK(m_geometryDBSvc.retrieve());

    GeoModelExperiment *theExpt;
    ATH_CHECK(detStore->retrieve(theExpt, "ATLAS"));

    const HGTD_ID *idHelper;
    // TODO: fix the following - HGTD_ID not retrieving properly during runtime
    ATH_CHECK(detStore->retrieve(idHelper, "HGTD_ID"));

//
//    Get their interfaces to pass to the DetectorFactory
//
    m_athenaComps = new InDetDD::AthenaComps("HGTD_GeoModel");
    m_athenaComps->setDetStore(detStore);
    m_athenaComps->setGeoModelSvc(&*m_geoModelSvc);
    m_athenaComps->setRDBAccessSvc(&*m_rdbAccessSvc);
    m_athenaComps->setGeometryDBSvc(&*m_geometryDBSvc);

    m_commonItems = new InDetDD::SiCommonItems(idHelper);

//
//   Get the version and configure things accordingly here
//

//
// Create the HGTD_DetectorFactory
//
    // The * converts a ConstPVLink to a ref to a GeoVPhysVol
    // The & takes the address of the GeoVPhysVol
    GeoPhysVol *world = &*theExpt->getPhysVol();
    HGTDGeo::HGTD_DetectorFactory theHGTDFactory(m_athenaComps, m_commonItems, m_geometryConfig=="FULL");
    theHGTDFactory.setHGTDBaseline(m_HGTD_isbaseline);

    theHGTDFactory.create(world);
//
// Get the manager from the factory and store it in the detector store.
//
    m_manager = theHGTDFactory.getDetectorManager();

    if (!m_manager) {
        msg(MSG::ERROR) << "HGTD_DetectorManager not found; not created in HGTD_DetectorFactory?" << endmsg;
        return(StatusCode::FAILURE);
    }

    ATH_CHECK(detStore->record(m_manager, m_manager->getName()));
    theExpt->addManager(m_manager);

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

StatusCode HGTD_DetectorTool::registerCallback(StoreGateSvc* /*detStore*/) {
//
//    Register call-back for software alignment
//
    msg(MSG::WARNING) << "No callback registration has yet been implemented for HGTD, nothing to do!" << endmsg;

    return StatusCode::SUCCESS;
}

StatusCode HGTD_DetectorTool::align(IOVSVC_CALLBACK_ARGS_P(/*I*/, /*keys*/)) {
//
//    The call-back routine, which just calls the real call-back routine from the manager.
//
    msg(MSG::WARNING) << "No alignment method in HGTD_DetectorManager implemented yet, nothing to do!" << endmsg;

    return StatusCode::SUCCESS;
}
