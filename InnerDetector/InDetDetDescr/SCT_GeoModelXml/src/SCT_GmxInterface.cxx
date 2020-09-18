/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModelXml/SCT_GmxInterface.h"

#include <cstdlib>
#include <sstream>

#include "InDetSimEvent/SiHitIdHelper.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
#include "SCT_ReadoutGeometry/StripBoxDesign.h"
#include "SCT_ReadoutGeometry/StripAnnulusDesign.h"
#include "SCT_ReadoutGeometry/StripStereoAnnulusDesign.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "ReadoutGeometryBase/SiCommonItems.h"

const int SCT_HitIndex = 1;

using namespace std;

SCT_GmxInterface::SCT_GmxInterface(InDetDD::SCT_DetectorManager *detectorManager, InDetDD::SiCommonItems *commonItems, 
                                   WaferTree *waferTree): 
    m_detectorManager(detectorManager),
    m_commonItems(commonItems),
    m_waferTree(waferTree) {
    // Logging: ref https://wiki.bnl.gov/dayabay/index.php?title=Logging
    // Turn on logging in job-options with: MessageSvc.setDebug += {"SCT_GmxInterface"}
    ServiceHandle<IMessageSvc> msgh("MessageSvc", "SCT_GmxInterface");

    m_log = new MsgStream(&(*msgh), "SCT_GmxInterface");
}

SCT_GmxInterface::~SCT_GmxInterface() {
    delete m_log;
}

int SCT_GmxInterface::sensorId(map<string, int> &index) {
//
//    Return the Simulation HitID (nothing to do with "ATLAS Identifiers" aka "O
    
    int hitIdOfWafer = SiHitIdHelper::GetHelper()->buildHitId(SCT_HitIndex, index["barrel_endcap"], index["layer_wheel"], 
                                                              index["eta_module"], index["phi_module"], index["side"]);

    *m_log << MSG::DEBUG  << "Index list: " << index["barrel_endcap"] << " " << index["layer_wheel"] << " " << 
                                       index["eta_module"] << " " << index["phi_module"] << " " << index["side"] << endmsg;
    *m_log << MSG::DEBUG << "hitIdOfWafer = " << std::hex << hitIdOfWafer << std::dec << endmsg;
    *m_log << MSG::DEBUG << " bec = " << SiHitIdHelper::GetHelper()->getBarrelEndcap(hitIdOfWafer) << 
                                      " lay = " << SiHitIdHelper::GetHelper()->getLayerDisk(hitIdOfWafer) << 
                                      " eta = " << SiHitIdHelper::GetHelper()->getEtaModule(hitIdOfWafer) << 
                                      " phi = " << SiHitIdHelper::GetHelper()->getPhiModule(hitIdOfWafer) << 
                                      " side = " << SiHitIdHelper::GetHelper()->getSide(hitIdOfWafer) << endmsg; 
    return hitIdOfWafer;
    
}


void SCT_GmxInterface::addSensorType(string clas, string typeName, map<string, string> parameters) {

    *m_log << MSG::DEBUG << "SCT_GmxInterface::addSensorType called for class " << clas << " typeName " << typeName << 
                                      endmsg;
    if (clas == "SiStripBox") {
         makeSiStripBox(typeName, parameters);
    }
    else if (clas == "StereoAnnulus") {
         makeStereoAnnulus(typeName, parameters);
    } // To-do: add "Annulus"
    else {
        *m_log << MSG::ERROR << "SCT_GmxInterface::addSensorType: unrecognised sensor class, " << clas << endmsg;
        *m_log << MSG::ERROR << "No sensor design created" << endmsg;
    }
}

void SCT_GmxInterface::makeSiStripBox(string typeName, map<string, string> &par) { 
//
//    Get all parameters. 
//
InDetDD::SiDetectorDesign::Axis stripDirection;
InDetDD::SiDetectorDesign::Axis fieldDirection;
double thickness(0.320);
int readoutSide(1);
InDetDD::CarrierType carrier(InDetDD::electrons);
int nRows(1);
double pitch(0.080);
int nStrips(1280); // Per row
double length(25.0);

    string car = getstr(typeName, "carrierType", par);
    if (car == "electrons") {
        carrier = InDetDD::electrons;
    }
    else if (car == "holes") {
        carrier = InDetDD::holes;
    }
    else {
        *m_log << MSG::FATAL << 
           "SCT_GmxInterface::makeSiStripBox: Error: parameter carrierType should be electrons or holes for " << 
            typeName << endmsg;
        exit(999);
    }

    string ros = getstr(typeName, "readoutSide", par);
    if (ros == "+") {
        readoutSide = 1;
    }
    else if (ros == "-") {
        readoutSide = -1;
    }
    else {
        *m_log << MSG::FATAL << 
           "SCT_GmxInterface::makeSiStripBox: Error: parameter readoutSide should be + or - for " << typeName << endmsg;
        exit(999);
    }

    string fd = getstr(typeName, "fieldDirection", par);
    if (fd == "x") {
        fieldDirection = InDetDD::SiDetectorDesign::xAxis; 
    }
    else if (fd == "y") {
        fieldDirection = InDetDD::SiDetectorDesign::yAxis; 
    }
    else if (fd == "z") {
        fieldDirection = InDetDD::SiDetectorDesign::zAxis; 
    }
    else {
        *m_log << MSG::FATAL << 
         "SCT_GmxInterface::makeSiStripBox: Error: parameter fieldDirection should be x, y, or z for " << typeName << endmsg;
        exit(999);
    }

    string sd = getstr(typeName, "stripDirection", par);
    if (sd == "x") {
        stripDirection = InDetDD::SiDetectorDesign::xAxis; 
    }
    else if (sd == "y") {
        stripDirection = InDetDD::SiDetectorDesign::yAxis; 
    }
    else if (sd == "z") {
        stripDirection = InDetDD::SiDetectorDesign::zAxis; 
    }
    else {
        *m_log << MSG::FATAL << 
         "SCT_GmxInterface::makeSiStripBox: Error: parameter stripDirection should be x, y, or z for " << typeName << endmsg;
        exit(999);
    }

    getparm(typeName, "thickness", par, thickness);
    getparm(typeName, "nRows", par, nRows);
    getparm(typeName, "nStrips", par, nStrips);
    getparm(typeName, "pitch", par, pitch);
    getparm(typeName, "stripLength", par, length);
//
//    Make Sensor Design and add to DetectorManager
//
    const InDetDD::StripBoxDesign *design = new InDetDD::StripBoxDesign(stripDirection, fieldDirection, thickness, readoutSide,
                                                                        carrier, nRows, nStrips, pitch, length);

    m_detectorManager->addDesign(dynamic_cast<const InDetDD::SiDetectorDesign*> (design));
//
//    Add to map for addSensor routine
//
    m_geometryMap[typeName] = design;
}

void SCT_GmxInterface::makeStereoAnnulus(string typeName, map<std::string, string> &par) { 
//
//    Get all parameters. 
//
int readoutSide;
InDetDD::SiDetectorDesign::Axis fieldDirection;
InDetDD::SiDetectorDesign::Axis stripDirection;
InDetDD::CarrierType carrier(InDetDD::electrons);
double thickness(0.320);
double stereoAngle(0.020);
double centreR(500.);
int nRows(1);
vector <int> nStrips;
vector<double> phiPitch;
vector<double> startR;
vector<double> endR;

    string car = getstr(typeName, "carrierType", par);
    if (car == "electrons") {
        carrier = InDetDD::electrons;
    }
    else if (car == "holes") {
        carrier = InDetDD::holes;
    }
    else {
        *m_log << MSG::FATAL << 
           "SCT_GmxInterface::makeStereoAnnulus: Error: parameter carrierType should be electrons or holes for " << 
            typeName << endmsg;
        exit(999);
    }

    string ros = getstr(typeName, "readoutSide", par);
    if (ros == "+") {
        readoutSide = 1;
    }
    else if (ros == "-") {
        readoutSide = -1;
    }
    else {
        *m_log << MSG::FATAL << "SCT_GmxInterface::makeStereoAnnulus: Error: parameter readoutSide should be + or - for " << 
                                          typeName << endmsg;
        exit(999);
    }

    string fd = getstr(typeName, "fieldDirection", par);
    if (fd == "x") {
        fieldDirection = InDetDD::SiDetectorDesign::xAxis; 
    }
    else if (fd == "y") {
        fieldDirection = InDetDD::SiDetectorDesign::yAxis; 
    }
    else if (fd == "z") {
        fieldDirection = InDetDD::SiDetectorDesign::zAxis; 
    }
    else {
        *m_log << MSG::FATAL << 
          "SCT_GmxInterface::makeStereoAnnulus: Error: parameter fieldDirection should be x, y, or z for " << typeName << endmsg;
        exit(999);
    }

    string sd = getstr(typeName, "stripDirection", par);
    if (sd == "x") {
        stripDirection = InDetDD::SiDetectorDesign::xAxis; 
    }
    else if (sd == "y") {
        stripDirection = InDetDD::SiDetectorDesign::yAxis; 
    }
    else if (sd == "z") {
        stripDirection = InDetDD::SiDetectorDesign::zAxis; 
    }
    else {
        *m_log << MSG::FATAL << 
         "SCT_GmxInterface::makeStereoAnnulus: Error: parameter stripDirection should be x, y, or z for " << typeName << endmsg;
        exit(999);
    }

    getparm(typeName, "thickness", par, thickness);
    getparm(typeName, "stereoAngle", par, stereoAngle);
    getparm(typeName, "centreR", par, centreR);
    getparm(typeName, "nRows", par, nRows);
    getparms(typeName, "nStrips", par, nStrips);
    if (nStrips.size() != (unsigned int) nRows) {
        *m_log << MSG::FATAL << 
           "SCT_GmxInterface::makeStereoAnnulus: Error: Wrong number of nStrip's " << nStrips.size() << " " << typeName << endmsg;
        exit(999);
    }
    getparms(typeName, "phiPitch", par, phiPitch);
    if (phiPitch.size() != (unsigned int) nRows) {
        *m_log << MSG::FATAL << 
          "SCT_GmxInterface::makeStereoAnnulus: Error: Wrong number of pitch's " << phiPitch.size() << " " << typeName << endmsg;
        exit(999);
    }
    getparms(typeName, "startR", par, startR);
    if (startR.size() != (unsigned int) nRows) {
        *m_log << MSG::FATAL << "SCT_GmxInterface::makeStereoAnnulus: Error: Wrong number of startR's " << 
                                           typeName << endmsg;
        exit(999);
    }
    getparms(typeName, "endR", par, endR);
    if (endR.size() != (unsigned int) nRows) {
        *m_log << MSG::FATAL << "SCT_GmxInterface::makeStereoAnnulus: Error: Wrong number of endR's " << typeName << endmsg;
        exit(999);
    }
//
//    Make Sensor Design and add it to the DetectorManager
//
    const InDetDD::StripStereoAnnulusDesign *design = new InDetDD::StripStereoAnnulusDesign(stripDirection, fieldDirection,
     thickness, readoutSide, carrier, nRows, nStrips, phiPitch, startR, endR, stereoAngle, centreR); 
    m_detectorManager->addDesign(dynamic_cast <const InDetDD::SiDetectorDesign*> (design));
//
//    Add to map for addSensor routine
//
    m_geometryMap[typeName] = design;
}

string SCT_GmxInterface::getstr(const string typeName, const string name, const map<string, string> &par) {
    map<string, string>::const_iterator found;
    if ((found = par.find(name)) != par.end()) {
        return found->second;
    }
    else {
        *m_log << MSG::FATAL << "SCT_GmxInterface::addSensorType: Error: missing parameter " << name << " for " << 
                                          typeName << endmsg;
        exit(999);
    }

}

void SCT_GmxInterface::addSensor(string typeName, map<string, int> &index, int /*sensitiveId*/, GeoVFullPhysVol *fpv) {
//
//    Get the ATLAS "Offline" wafer identifier 
//
    const SCT_ID *sctIdHelper = dynamic_cast<const SCT_ID *> (m_commonItems->getIdHelper());
    Identifier id = sctIdHelper->wafer_id(index["barrel_endcap"], index["layer_wheel"], index["phi_module"], 
                                          index["eta_module"], index["side"]);
    IdentifierHash hashId = sctIdHelper->wafer_hash(id);
//
//    Now do our best to check if this is a valid id. If either the gmx file is wrong, or the xml file
//    defining the allowed id's is wrong, you can get disallowed id's. These cause a crash later 
//    if allowed through. To do the check, we ask for the hash-id of this id. Invalid ids give a 
//    special invalid hash-id (0xFFFFFFFF). But we don't exit the run, to help debug things quicker.
//
    if (!hashId.is_valid()) {
        *m_log << MSG::ERROR <<"Invalid id for sensitive wafer " << typeName << " volume with indices \n";
        for (map<string, int>::iterator i = index.begin(); i != index.end(); ++i) {
            *m_log << MSG::ERROR << i->first << " = " << i->second << "; ";
        }
        *m_log << MSG::ERROR << 
          "\nRefusing to make it into a sensitive element. Incompatible gmx and identifier-xml files." << endmsg;
        return;
    }
//
//    Create the detector element and add to the DetectorManager
//
    const InDetDD::SiDetectorDesign *design = m_geometryMap[typeName];
    if (!design) {
        *m_log << MSG::FATAL << "SCT_GmxInterface::addSensor: Error: Readout sensor type " << typeName << 
          " not found.\n" << endmsg;
        exit(999);
    }
    InDetDD::SiDetectorElement *detector = new InDetDD::SiDetectorElement(id, design, fpv, m_commonItems);
    m_detectorManager->addDetectorElement(detector);
//
//    Build up a map-structure for numerology
//
    Wafer wafer((unsigned int) hashId);
    string errorMessage("");
    if (!m_waferTree->add(index["barrel_endcap"], index["layer_wheel"], index["eta_module"], 
                          index["phi_module"], index["side"], wafer, errorMessage)) {
        *m_log << MSG::ERROR << errorMessage << endmsg;
    }
    return;
}

void SCT_GmxInterface::addAlignable(int level, map<string, int> &index, GeoVFullPhysVol * fpv, GeoAlignableTransform *transform) {
    *m_log << MSG::DEBUG << "SCT_GmxInterface::addAlignable called" << endmsg;
/*
 *    Get the offline-id appropriate to the level (0 = wafer, 1 = module, 2 = wheel/cylinder, 3 = part, i.e barrel or an endcap)
 */

    const SCT_ID *sctIdHelper = dynamic_cast<const SCT_ID *> (m_commonItems->getIdHelper());
    Identifier id;
    switch (level) {
        case 0:
            id = sctIdHelper->wafer_id(index["barrel_endcap"], index["layer_wheel"], index["phi_module"], index["eta_module"], 
                                       index["side"]);
        break;
        case 1:
            id = sctIdHelper->wafer_id(index["barrel_endcap"], index["layer_wheel"], index["phi_module"], index["eta_module"], 0);
        break;
        case 2:
            id = sctIdHelper->wafer_id(index["barrel_endcap"], index["layer_wheel"], 0, 0, 0);
        break;
        case 3:
            id = sctIdHelper->wafer_id(index["barrel_endcap"], 0, 0, 0, 0);
        break;
        default:
            *m_log << MSG::FATAL << "Unknown level " << level << " for alignment in SCT_GmxInterface::addAlignable" << 
                                               endmsg;
            exit(999);
        break;
    }
    m_detectorManager->addAlignableTransform(level, id, transform, fpv);
}
