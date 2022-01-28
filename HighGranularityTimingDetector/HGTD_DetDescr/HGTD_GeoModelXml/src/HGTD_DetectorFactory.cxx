/*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "HGTD_GeoModelXml/HGTD_DetectorFactory.h"

#include <string>
#include <sstream>
#include <math.h>

#include "HGTD_GeoModel/HGTD_GeoModelAthenaComps.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

#include "ReadoutGeometryBase/SiCommonItems.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "GeoModelXml/Gmx2Geo.h"
#include "GeoModelXml/GmxInterface.h"
#include "HGTD_GeoModelXml/HGTD_GmxInterface.h"
#include "HGTD_GeoModelXml/HGTD_Options.h"

#include "PathResolver/PathResolver.h"

using namespace std;

HGTD_DetectorFactory::HGTD_DetectorFactory( HGTD_GeoModelAthenaComps* athComps,
                                            InDetDD::SiCommonItems* commonItems,
                                            InDetDDSLHC::HGTD_Options& options ) :
    InDetDD::DetectorFactoryBase( athComps ),
    m_athComps( athComps ),
    m_commonItems(commonItems),
    m_options(&options) {

    // create the detector manager
    m_detectorManager = new HGTD_DetectorManager( detStore() );

}

void HGTD_DetectorFactory::create(GeoPhysVol* world) {

    ATH_MSG_INFO( "Building HGTD detector");

    HGTD_GmxInterface gmxInterface(m_detectorManager, m_commonItems);
    //    To set up solid geometry only, without having to worry about sensitive detectors etc., and get loads of debug output,
    //    comment out above line and uncomment the following line; also, switch header files above.
    //    GmxInterface gmxInterface;

    int flags(0);
    string gmxInput;

    if (m_options->gmxFilename().empty()) {
        ATH_MSG_INFO("gmxFilename not set; getting .gmx from Geometry database Blob");
        flags = 0x1; // Lowest bit ==> string; next bit implies gzip'd but we decided not to gzip
        gmxInput = getBlob();
        string dtdFile = '"' + PathResolver::find_file("geomodel.dtd", "DATAPATH") + '"';
        ATH_MSG_INFO("dtdFile = " << dtdFile);
        size_t index = gmxInput.find("\"geomodel.dtd\"");
        if (index != string::npos) {
            gmxInput.replace(index, 14, dtdFile);
        }
        else {
            ATH_MSG_WARNING("Did not find string geomodel.dtd in the gmx input string."); 
        }
    }
    else {
        flags = 0;
        gmxInput = PathResolver::find_file(m_options->gmxFilename(), "DATAPATH");
        if (gmxInput.empty()) { // File not found
            string errMessage("HGTD_DetectorFactory::create: Unable to find file " + m_options->gmxFilename() +
                                   " with PathResolver; check filename and DATAPATH environment variable");
            throw runtime_error(errMessage);
        }
    }

    Gmx2Geo gmx2Geo(gmxInput, world, gmxInterface, flags);

    //   Add the tree-tops to the detector manager. This also makes them appear as HGTD_Pos and HGTD_Neg in VP1.
    //   They're probably the last (most recently added) things in the world PV so loop from the
    //   back looking for our subdetector name.
    unsigned int nChildren = world->getNChildVols();
    bool foundPosVolume = false;
    bool foundNegVolume = false;

    for (int iChild = nChildren - 1; iChild>=0; --iChild) {
        if (world->getNameOfChildVol(iChild) == "HGTD_Pos") {
            // The * converts from a ConstPVLink to a reference to a GeoVPhysVol;
            // the & takes its address.
            foundPosVolume = true;
            m_detectorManager->addTreeTop(&*world->getChildVol(iChild));
        } else if (world->getNameOfChildVol(iChild) == "HGTD_Neg") {
            // The * converts from a ConstPVLink to a reference to a GeoVPhysVol;
            // the & takes its address.
            foundNegVolume = true;
            m_detectorManager->addTreeTop(&*world->getChildVol(iChild));
        }
        if (foundPosVolume && foundNegVolume)
            break;
    }

    if(!foundPosVolume) {
        ATH_MSG_ERROR("Could not find a logicalVolume named \"HGTD_Pos\" - this is required to provide the Envelope!");
        throw std::runtime_error("logicalVolume named \"HGTD_Pos\" not found.");
    }
    if(!foundNegVolume) {
        ATH_MSG_ERROR("Could not find a logicalVolume named \"HGTD_Neg\" - this is required to provide the Envelope!");
        throw std::runtime_error("logicalVolume named \"HGTD_Neg\" not found.");
    }

}

string HGTD_DetectorFactory::getBlob() {
    DecodeVersionKey versionKey(geoDbTagSvc(), "HGTD");
    const std::string& versionTag  = versionKey.tag();
    const std::string& versionNode = versionKey.node();
    ATH_MSG_INFO("getBlob: versionTag = " << versionTag);
    ATH_MSG_INFO("getBlob: versionNode = " << versionNode);

    IRDBAccessSvc *accessSvc = m_athComps->rdbAccessSvc();
    //   ADA  accessSvc->connect();
    IRDBRecordset_ptr recordSetSct = accessSvc->getRecordsetPtr("ITKXDD", versionTag, versionNode);
    if (!recordSetSct || recordSetSct->size() == 0) {
        ATH_MSG_FATAL("getBlob: Unable to obtain HGTD recordSet");
        throw runtime_error("getBlob: Unable to obtain HGTD recordSet");
    }
    const IRDBRecord *recordSct =  (*recordSetSct)[0];
    string sctString = recordSct->getString("XMLCLOB");
    //   ADA  accessSvc->disconnect();

    return sctString;
}

HGTD_DetectorManager* HGTD_DetectorFactory::getDetectorManager() const {
    return m_detectorManager;
}
