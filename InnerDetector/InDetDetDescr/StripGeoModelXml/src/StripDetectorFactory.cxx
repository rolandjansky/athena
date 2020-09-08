/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "StripGeoModelXml/StripDetectorFactory.h"

#include <string>
#include <sstream>

#include "StoreGate/StoreGateSvc.h" // For alignment getAlignableFolderType()
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "DetDescrConditions/AlignableTransformContainer.h"

#include "InDetGeoModelUtils/InDetDDAthenaComps.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

#include "InDetReadoutGeometry/SiCommonItems.h"
#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/Version.h"
#include "SCT_ReadoutGeometry/SCT_ModuleSideDesign.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "StripGeoModelXml/StripOptions.h"
#include "GeoModelXml/Gmx2Geo.h"
#include "StripGeoModelXml/StripGmxInterface.h"
#include "GeoModelXml/GmxInterface.h"

#include "PathResolver/PathResolver.h"

using namespace std;

namespace InDetDDSLHC {

StripDetectorFactory::StripDetectorFactory(InDetDD::AthenaComps *athenaComps,
                                         InDetDD::SiCommonItems *commonItems,
					 StripOptions &options) :
     InDetDD::DetectorFactoryBase(athenaComps),
     m_athenaComps(athenaComps),
     m_commonItems(commonItems),
     m_options(&options) {
//
//    Create the detector manager... should allow the name to be set
//
    m_detectorManager = new InDetDD::SCT_DetectorManager(detStore(),m_options->detectorName());
//
//   Set Detector Manager SCT version information
//
//At some point we may want to decouple also this DB stuff, and make this ITkStrip specific?
    DecodeVersionKey versionKey(geoDbTagSvc(), "SCT");
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
    string versionTag = rdbAccessSvc()->getChildTag("SCT", versionKey.tag(), versionKey.node());
    string versionName = switches->getString("VERSIONNAME");
    int versionMajorNumber = 0;
    int versionMinorNumber = 0;
    int versionPatchNumber = 0;
    InDetDD::Version version(versionTag, versionName, layout, description, versionMajorNumber,
                             versionMinorNumber, versionPatchNumber);
    m_detectorManager->setVersion(version);
}

StripDetectorFactory::~StripDetectorFactory() {
    // NB the detector manager (m_detectorManager) is stored in the detector store by the Tool and so we don't delete it.
}

void StripDetectorFactory::create(GeoPhysVol *world) {
    msg(MSG::INFO) << "C R E A T E   W O R L D" << endmsg; 
   
    msg(MSG::INFO) << m_detectorManager->getVersion().fullDescription() << endmsg;

    StripGmxInterface gmxInterface(m_detectorManager, m_commonItems, &m_waferTree);
//    To set up solid geometry only, without having to worry about sensitive detectors etc., and get loads of debug output,
//    comment out above line and uncomment the following line; also, switch header files above.
//    GmxInterface gmxInterface;

    int flags(0);
    string gmxInput;

    if (m_options->gmxFilename() == "") {
        msg(MSG::INFO) << "gmxFilename not set; getting .gmx from Geometry database Blob" << endmsg;
        flags = 0x1; // Lowest bit ==> string; next bit implies gzip'd but we decided not to gzip
        gmxInput = getBlob();
        string dtdFile = '"' + PathResolver::find_file("geomodel.dtd", "DATAPATH") + '"';
        msg(MSG::INFO) << "dtdFile = " << dtdFile << endmsg;
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
            string errMessage("StripDetectorFactory::create: Unable to find file " + m_options->gmxFilename() +
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

    for (int iChild = nChildren - 1; iChild>=0; --iChild) {
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

string StripDetectorFactory::getBlob() {
    DecodeVersionKey versionKey(geoDbTagSvc(), "SCT");
    std::string versionTag  = versionKey.tag();
    std::string versionNode = versionKey.node();
    msg(MSG::INFO) << "getBlob: versionTag = " << versionTag << endmsg;
    msg(MSG::INFO) << "getBlob: versionNode = " << versionNode << endmsg;

    IRDBAccessSvc *accessSvc = m_athenaComps->rdbAccessSvc();
    //   ADA  accessSvc->connect();
    IRDBRecordset_ptr recordSetSct = accessSvc->getRecordsetPtr("ITKXDD", versionTag, versionNode);
    if (!recordSetSct || recordSetSct->size() == 0) {
        msg(MSG::FATAL) << "getBlob: Unable to obtain SCT recordSet" << endmsg;
        throw runtime_error("getBlob: Unable to obtain SCT recordSet");
    }
    const IRDBRecord *recordSct =  (*recordSetSct)[0];
    string sctString = recordSct->getString("XMLCLOB");
    //   ADA  accessSvc->disconnect();

    return sctString;
}

InDetDD::SCT_DetectorManager * StripDetectorFactory::getDetectorManager() const {
    return m_detectorManager;
}

void StripDetectorFactory::doNumerology() {
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
    msg(MSG::INFO) << endmsg;

    int totalWafers = 0;
    for (BarrelEndcap::iterator bec = m_waferTree.begin(); bec != m_waferTree.end(); ++bec) {
        for (LayerDisk::iterator ld = bec->second.begin(); ld != bec->second.end(); ++ld) {
            for (EtaModule::iterator eta = ld->second.begin(); eta != ld->second.end(); ++eta) {
                for (PhiModule::iterator phi = eta->second.begin(); phi != eta->second.end(); ++phi) {
                    for (Side::iterator side =phi->second.begin(); side != phi->second.end(); ++side) {
                        totalWafers++;
                    }
                }
            }
        }
    }
    msg(MSG::INFO) << "Total number of wafers added is " << totalWafers << endmsg;
    const SCT_ID *sctIdHelper = dynamic_cast<const SCT_ID *> (m_commonItems->getIdHelper());
    msg(MSG::INFO) << "Total number of wafer identifiers is " << sctIdHelper->wafer_hash_max() << endmsg;
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

    msg(MSG::INFO) << "End of numerology\n" << endmsg;

//
//    Alignment preparation
//
    if (m_options->alignable()) {
        msg(MSG::INFO) << "Set up alignment directories" << endmsg;
        const std::string topFolder("/Indet/Align");
        const std::string barrelBase("/SCTB");
        const std::string endcapBase("/SCTE");
        std::string baseName("");
         
        // Register the keys and the level corresponding to the key
        // and whether it expects a global or local shift.
        // level 0: sensor, level 1: module, level 2, layer/disc, level 3: whole barrel/enccap
        InDetDD::AlignFolderType alignFolderType = getAlignFolderType();
        m_detectorManager->addAlignFolderType(alignFolderType);

        switch (alignFolderType) {
        case InDetDD::static_run1:
            m_detectorManager->addFolder(topFolder);
            m_detectorManager->addChannel(topFolder + "/ID", 3, InDetDD::global);
            m_detectorManager->addChannel(topFolder + "/SCT",2,InDetDD::global);
            for (BarrelEndcap::iterator bec = m_waferTree.begin(); bec != m_waferTree.end(); ++bec) {
                switch (bec->first) {
                case -2:
                    baseName = topFolder + endcapBase + "C";
                break;
                case 0:
                    baseName = topFolder + barrelBase;
                break;
                case 2:
                    baseName = topFolder + endcapBase + "A";
                break;
                default:
                    msg(MSG::FATAL) << "Unknown SCT part with bec-ID " << bec->first << " encountered." << endmsg;
                    throw std::runtime_error("Unknown SCT part for alignment.");
                }
                for (LayerDisk::iterator ld = bec->second.begin(); ld != bec->second.end(); ++ld) {
                    std::ostringstream layer; 
                    layer << ld->first + 1;
                    m_detectorManager->addChannel(baseName + layer.str(), 1, InDetDD::local);
                }
            }
        break;
        // To be added: case InDetDD::timedependent_run2:, see SCT_GeoModel
        default:
            msg(MSG::FATAL) << "Alignment requested for unknown alignment folder type in StripDetectorFactory." << endmsg;
            throw std::runtime_error("Wrong alignment folder type for StripDetectorFactory in StripGeoModelXml.");
        }
    }

    return;
}

// Determine which alignment folders are loaded to decide if we register old or new folders                                         
InDetDD::AlignFolderType StripDetectorFactory::getAlignFolderType() const {

    bool static_folderStruct = false;
    bool timedep_folderStruct = false;
    if (detStore()->contains<CondAttrListCollection>("/Indet/AlignL1/ID") &&
        detStore()->contains<CondAttrListCollection>("/Indet/AlignL2/SCT") &&
        detStore()->contains<AlignableTransformContainer>("/Indet/AlignL3") ) timedep_folderStruct = true;
 
    if (detStore()->contains<AlignableTransformContainer>("/Indet/Align") ) static_folderStruct = true;
 
    if (static_folderStruct && !timedep_folderStruct){
        msg(MSG::INFO) << "Static run1 type alignment folder structure found" << endmsg;
        return InDetDD::static_run1;
    }
    else if (!static_folderStruct && timedep_folderStruct){
        msg(MSG::INFO) << " Time dependent run2 type alignment folder structure found" << endmsg;
        return InDetDD::timedependent_run2;
    }
    else if (static_folderStruct && timedep_folderStruct){
        throw std::runtime_error("Old and new alignment folders are loaded at the same time! This should not happen!");
    }
    else {
        return InDetDD::none;
    }
 
}

} // End namespace
