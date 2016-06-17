/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModelXml/SCT_DetectorFactory.h"

#include <string>
#include <iostream>

#include "InDetGeoModelUtils/InDetDDAthenaComps.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

#include "InDetReadoutGeometry/SiCommonItems.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/Version.h"
#include "InDetReadoutGeometry/SCT_ModuleSideDesign.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "SCT_GeoModelXml/SCT_Options.h"
#include "GeoModelXml/Gmx2Geo.h"
#include "SCT_GeoModelXml/SCT_GmxInterface.h"
#include "GeoModelXml/GmxInterface.h"

#include "PathResolver/PathResolver.h"

//#include <cerrno>

using namespace std;

namespace InDetDDSLHC {

SCT_DetectorFactory::SCT_DetectorFactory(InDetDD::AthenaComps *athenaComps,
                                         InDetDD::SiCommonItems *commonItems,
					 SCT_Options &options) :
     InDetDD::DetectorFactoryBase(athenaComps),
     m_athenaComps(athenaComps),
     m_commonItems(commonItems),
     m_options(&options) {
//
//    Create the detector manager
//
    m_detectorManager = new InDetDD::SCT_DetectorManager(detStore());
//
//   Set Detector Manager SCT version information
//
    DecodeVersionKey versionKey(geoModelSvc(), "SCT");
    IRDBRecordset_ptr switchSet = rdbAccessSvc()->getRecordsetPtr("SctSwitches", versionKey.tag(), versionKey.node());
    const IRDBRecord *switches = (*switchSet)[0];
    string layout = "SLHC";
    if (!switches->isFieldNull("LAYOUT")) {
        layout = switches->getString("LAYOUT");
    }
    string description = "Test geometry";
    if (!switches->isFieldNull("DESCRIPTION")) {
        description = switches->getString("DESCRIPTION");
    }
    string versionTag = rdbAccessSvc()->getChildTag("SCT", versionKey.tag(), versionKey.node(), false);
    string versionName = switches->getString("VERSIONNAME");
    int versionMajorNumber = 0;
    int versionMinorNumber = 0;
    int versionPatchNumber = 0;
    InDetDD::Version version(versionTag, versionName, layout, description, versionMajorNumber,
                             versionMinorNumber, versionPatchNumber);
    m_detectorManager->setVersion(version);
}

SCT_DetectorFactory::~SCT_DetectorFactory() {
    // NB the detector manager (m_detectorManager) is stored in the detector store by the Tool and so we don't delete it.
}

void SCT_DetectorFactory::create(GeoPhysVol *world) {
    msg(MSG::INFO) << "C R E A T E   W O R L D" << endreq; 
   
    msg(MSG::INFO) << m_detectorManager->getVersion().fullDescription() << endreq;

    SCT_GmxInterface gmxInterface(m_detectorManager, m_commonItems, &m_waferTree);
//    To set up solid geometry only, without having to worry about sensitive detectors etc., and get loads of debug output,
//    comment out above line and uncomment the following line; also, switch header files above.
//    GmxInterface gmxInterface;

    int flags(0);
    string gmxInput;

    if (m_options->gmxFilename() == "") {
        msg(MSG::INFO) << "gmxFilename not set; getting .gmx from Geometry database Blob" << endreq;
        flags = 0x1; // Lowest bit ==> string; next bit implies gzip'd but we decided not to gzip
        gmxInput = getBlob();
        string dtdFile = '"' + PathResolver::find_file("geomodel.dtd", "DATAPATH") + '"';
        cout << "dtdFile = " << dtdFile << endl;
        size_t index = gmxInput.find("\"geomodel.dtd\"");
        if (index != string::npos) {
            gmxInput.replace(index, 14, dtdFile);
        }
        else {
            cerr << "Did not find string geomodel.dtd in the gmx input string.\n"; 
        }
    }
    else {
        flags = 0;
        gmxInput = PathResolver::find_file(m_options->gmxFilename(), "DATAPATH");
        if (gmxInput == "") { // File not found
            string errMessage("SCT_DetectorFactory::create: Unable to find file " + m_options->gmxFilename() +
                                   " with PathResolver; check filename and DATAPATH environment variable");
            throw runtime_error(errMessage);
        }
    }

    Gmx2Geo gmx2Geo(gmxInput, world, gmxInterface, flags);
//
//   Add the tree-top to the detector manager. This also makes it appear as SCT in VP1.
//   It is probably the last (most recently added) thing in the world PV so loop from the
//   back looking for our subdetector name.
//
    unsigned int nChildren = world->getNChildVols();
    for (int iChild = nChildren - 1; iChild; --iChild) {
        if (world->getNameOfChildVol(iChild) == "SCT") {
            // The * converts from a ConstPVLink to a reference to a GeoVPhysVol;
            // the & takes its address.
            m_detectorManager->addTreeTop(&*world->getChildVol(iChild));
            break;
        }
    }

    doNumerology();

    m_detectorManager->initNeighbours();

}

string SCT_DetectorFactory::getBlob() {

    DecodeVersionKey versionKey(geoModelSvc(), "SCT");
    std::string versionTag  = versionKey.tag();
    std::string versionNode = versionKey.node();
    cout << "getBlob: versionTag = " << versionTag << endl;
    cout << "getBlob: versionNode = " << versionNode << endl;

    IRDBAccessSvc *accessSvc = m_athenaComps->rdbAccessSvc();
    const IRDBRecordset *recordSetSct = accessSvc->getRecordset("ITKXDD", versionTag, versionNode);
    if (!recordSetSct || recordSetSct->size() == 0) {
        throw runtime_error("getBlob: Unable to obtain SCT recordSet");
    }
    const IRDBRecord *recordSct =  (*recordSetSct)[0];
    string sctString = recordSct->getString("XMLCLOB");
    accessSvc->shutdown();

    return sctString;
}

InDetDD::SCT_DetectorManager * SCT_DetectorFactory::getDetectorManager() const {
    return m_detectorManager;
}

void SCT_DetectorFactory::doNumerology() {
    InDetDD::SiNumerology n;

    msg(MSG::INFO) << "\n\nSCT Numerology:\n===============\n\nNumber of parts is " << m_waferTree.nParts() << endl << endl;

    bool barrelDone = false;
    for (int b = -1; b <= 1; ++b) {
        if (m_waferTree.count(b)) {
            msg(MSG::INFO) << "    Found barrel with index " << b << endl;
            n.addBarrel(b);
            if (!barrelDone) {
                n.setNumLayers(m_waferTree[b].nLayers());
                msg(MSG::INFO) << "        Number of barrel layers = " << n.numLayers() << endl;
                for (LayerDisk::iterator l = m_waferTree[b].begin(); l != m_waferTree[b].end(); ++l) {
                    n.setNumEtaModulesForLayer(l->first, l->second.nEtaModules());
                    // All staves within a layer are assumed identical, so we can just look at the first eta
                    n.setNumPhiModulesForLayer(l->first, l->second.begin()->second.nPhiModules());
                    msg(MSG::INFO) << "        layer = " << l->first << " has " << n.numEtaModulesForLayer(l->first) << 
                                     " etaModules each with " <<  n.numPhiModulesForLayer(l->first) << " phi modules" << endl;
                }
                barrelDone = true;
            }
        }

    }
    bool endcapDone = false;

    for (int ec = -2; ec <= 2; ec += 4) {
        if (m_waferTree.count(ec)) {
            msg(MSG::INFO) << "    Found endcap with index " << ec << endl;
            n.addEndcap(ec);
            if (!endcapDone) {
                n.setNumDisks(m_waferTree[ec].nLayers());
                msg(MSG::INFO) << "        Number of endcap wheels = " << n.numDisks() << endl;
                for (LayerDisk::iterator l = m_waferTree[ec].begin(); l != m_waferTree[ec].end(); ++l) {
                    n.setNumRingsForDisk(l->first, l->second.nEtaModules());
                    msg(MSG::INFO) << "        Wheel " << l->first << " has " << n.numRingsForDisk(l->first) << " rings" << endl; 
                    for (EtaModule::iterator eta = l->second.begin(); eta != l->second.end(); ++eta) {
                        n.setNumPhiModulesForDiskRing(l->first, eta->first, eta->second.nPhiModules());
                        msg(MSG::DEBUG) << "            Ring " << eta->first << " has " << 
                                           n.numPhiModulesForDiskRing(l->first, eta->first) << " phi modules" << endl;
                    }
                }
                endcapDone = true;
            }
        }
    }

//
//    Used in digitization to create one vector big enough to hold all strips, whichever detector is in consideration.
//    Anyway they are common to pixels and strips! Pixels dominate the EtaCell count (which traditionally the SCT does not set)
//
    n.setMaxNumEtaCells(1);
    for (int d = 0; d < m_detectorManager->numDesigns(); ++d) {
        n.setMaxNumPhiCells(m_detectorManager->getSCT_Design(d)->cells());
    }
    msg(MSG::INFO) << "Max. eta cells is " << n.maxNumEtaCells() << endl;
    msg(MSG::INFO) << "Max. phi cells is " << n.maxNumPhiCells() << endl;
    msg(MSG::INFO) << "Max. no. strips is " << n.maxNumStrips() << endl;

    m_detectorManager->numerology() = n;

    msg(MSG::INFO) << "End of numerology\n" << endreq;

    return;
}

} // End namespace
