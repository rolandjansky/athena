/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// includes
#include "HGTD_GeoModel/HGTD_DetectorTool.h"
#include "HGTD_GeoModel/HGTD_DetectorFactory.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "DetDescrConditions/AlignableTransformContainer.h"
#include "ReadoutGeometryBase/SiCommonItems.h"
#include "InDetCondServices/ISiLorentzAngleSvc.h"

#include "InDetGeoModelUtils/InDetDDAthenaComps.h"

#include "CLIDSvc/tools/ClassID_traits.h"
#include "SGTools/DataProxy.h"

HGTD_DetectorTool::HGTD_DetectorTool(const std::string &type,
                                     const std::string &name,
                                     const IInterface *parent) :
  GeoModelTool(type, name, parent),
  m_detectorName("HGTD"),
  m_alignable(false),
  m_gmxFilename(""),
  m_geoModelSvc("GeoModelSvc", name),
  m_rdbAccessSvc("RDBAccessSvc", name),
  m_geometryDBSvc("InDetGeometryDBSvc", name),
  m_lorentzAngleSvc( "HGTDLorentzAngleSvc", name ),
  m_detectorManager(nullptr),
  m_athenaComps(0),
  m_commonItems(0),
  m_printIDdict(false)  {
    // Get parameter values from the python configuration
    declareProperty("DetectorName", m_detectorName);
    declareProperty("Alignable", m_alignable);
    declareProperty("GmxFilename", m_gmxFilename);
    declareProperty("GeoModelSvc", m_geoModelSvc);
    declareProperty("RDBAccessSvc", m_rdbAccessSvc);
    declareProperty("GeometryDBSvc", m_geometryDBSvc);
    declareProperty("LorentzAngleSvc", m_lorentzAngleSvc);
    declareProperty("PrintModuleNumberPerRow", m_printIDdict );
}

HGTD_DetectorTool::~HGTD_DetectorTool() {
    delete m_athenaComps;
}

StatusCode HGTD_DetectorTool::create(StoreGateSvc* detStore) {
    // Retrieve all services except LorentzAngleSvc, which has to be done later
    StatusCode sc = StatusCode::SUCCESS ;

    ATH_CHECK(m_geoModelSvc.retrieve());
    ATH_CHECK(m_rdbAccessSvc.retrieve());
    ATH_CHECK(m_geometryDBSvc.retrieve());
    ATH_CHECK(m_geoModelSvc.retrieve());

    // Get their interfaces to pass to the DetectorFactory
    m_athenaComps = new InDetDD::AthenaComps( "HGTD_GeoModel" );
    m_athenaComps->setDetStore( detStore );
    m_athenaComps->setGeoModelSvc(&*m_geoModelSvc);
    m_athenaComps->setRDBAccessSvc(&*m_rdbAccessSvc);
    m_athenaComps->setGeometryDBSvc(&*m_geometryDBSvc);

    const HGTD_ID * idHelper;
    ATH_CHECK(detStore->retrieve(idHelper, "HGTD_ID"));
    m_commonItems = new InDetDD::SiCommonItems( idHelper );

    GeoModelExperiment *theExpt;
    ATH_CHECK(detStore->retrieve(theExpt, "ATLAS"));

    // The * converts a ConstPVLink to a ref to a GeoVPhysVol, the & takes the address of the GeoVPhysVol
    GeoPhysVol *world = &*theExpt->getPhysVol();

    HGTDGeo::HGTD_DetectorFactory theHGTDFactory( m_athenaComps, m_commonItems );
    theHGTDFactory.setPrintIdentifierDict( m_printIDdict ) ;
    theHGTDFactory.create(world);

    // Get the manager from the factory and store it in the detector store.
    m_detectorManager = theHGTDFactory.getDetectorManager();
    if (!m_detectorManager) {
        msg(MSG::ERROR) << "HGTD_DetectorManager not found; not created in HGTD_DetectorFactory?" << endmsg;
        return(StatusCode::FAILURE);
    }

    ATH_MSG_DEBUG("Registering HGTD_DetectorManager as " << m_detectorManager->getName() );
    sc = detStore->record(m_detectorManager, m_detectorManager->getName()) ;
    if ( sc.isFailure() ) {
      msg(MSG::ERROR) << "Could not register HGTD_DetectorManager as " << m_detectorManager->getName() << endmsg;
      return StatusCode::FAILURE;
    } else
      msg(MSG::INFO) << "Successfuly register HGTD_DetectorManager as " << m_detectorManager->getName() << endmsg;

    theExpt->addManager(m_detectorManager);

    return StatusCode::SUCCESS;
}

StatusCode HGTD_DetectorTool::clear(StoreGateSvc* detStore) {

    // Release manager from the detector store
    SG::DataProxy* _proxy = detStore->proxy(ClassID_traits< HGTD_DetectorManager >::ID(),m_detectorManager->getName());
    if (_proxy) {
      _proxy->reset();
      m_detectorManager = nullptr;
    }

    return StatusCode::SUCCESS;
}

StatusCode HGTD_DetectorTool::registerCallback(StoreGateSvc* /*detStore*/) {

    // Register call-back for software alignment
    StatusCode sc = StatusCode::FAILURE;
    if (m_alignable) {
        std::string folderName = "/Indet/AlignHGTD";
        if ( detStore()->contains<AlignableTransformContainer>(folderName) ) {
            msg(MSG::DEBUG) << "Registering callback on AlignableTransformContainer with folder " << folderName << endmsg;
            const DataHandle<AlignableTransformContainer> atc;
            sc =  detStore()->regFcn( &IGeoModelTool::align, dynamic_cast<IGeoModelTool *>(this), atc, folderName );
            if(sc.isFailure()) {
                msg(MSG::ERROR) << "Could not register callback on AlignableTransformContainer with folder " << 
                                    folderName << endmsg;
            }
        }
        else {
            msg(MSG::WARNING) << "Unable to register callback on AlignableTransformContainer with folder " <<
                                 folderName << ", Alignment disabled (only if no Run2 scheme is loaded)!" << endmsg;
        }
    }
    else {
        msg(MSG::INFO) << "Alignment disabled. No callback registered" << endmsg;
        // We return failure otherwise it will try and register a GeoModelSvc callback associated with this callback.
    }
    return sc;
}

StatusCode HGTD_DetectorTool::align(IOVSVC_CALLBACK_ARGS_P( I, keys ) ) {

    //   The call-back routine, which just calls the real call-back routine from the manager.

    if ( ! m_detectorManager ) {
        msg(MSG::WARNING) << "Manager does not exist for " << I <<" "<< keys << endmsg;
        return StatusCode::FAILURE;
    }

    // TODO : implement align() in detMgr side
    // detMgr has no align() in HGTD_ReadoutGeometry of R21.9 yet
    /**
    if (m_alignable) {
        return m_detectorManager->align(I, keys);
    } 
    else {
        msg(MSG::DEBUG) << "Alignment disabled. No alignments applied" << endmsg;
        return StatusCode::SUCCESS;
    }
    **/
    return StatusCode::SUCCESS;

}
