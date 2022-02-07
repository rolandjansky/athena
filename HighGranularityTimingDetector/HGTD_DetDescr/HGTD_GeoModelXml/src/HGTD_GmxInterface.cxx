/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "HGTD_GeoModelXml/HGTD_GmxInterface.h"

#include "HGTD_Identifier/HGTD_ID.h"
#include "HGTD_ReadoutGeometry/HGTD_ModuleDesign.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorElement.h"
#include "ReadoutGeometryBase/SiCommonItems.h"

#include "InDetSimEvent/SiHitIdHelper.h"
#include "AthenaKernel/getMessageSvc.h"

namespace
{
constexpr int HGTD_HitIndex{2};
}

HGTD_GmxInterface::HGTD_GmxInterface(HGTD_DetectorManager* detectorManager,
                                     InDetDD::SiCommonItems* commonItems):
    AthMessaging(Athena::getMessageSvc(), "HGTD_GmxInterface"),
    m_detectorManager(detectorManager),
    m_commonItems(commonItems) {

}

int HGTD_GmxInterface::sensorId(std::map<std::string, int> &index) const {
    //
    // Return the Simulation HitID (nothing to do with "ATLAS Identifiers" aka "Offline Identifiers"

  // Check if identifier is valid

    const HGTD_ID* hgtdIdHelper = dynamic_cast<const HGTD_ID *> (m_commonItems->getIdHelper());
    Identifier id = hgtdIdHelper->wafer_id(index["hgtd_endcap"],
                                           index["hgtd_layer"],
                                           index["hgtd_phi_module"],
                                           index["hgtd_eta_module"]);
    IdentifierHash hashId = hgtdIdHelper->wafer_hash(id);
    if (!hashId.is_valid()) {
    ATH_MSG_WARNING("Invalid hash for Index list: " << index["hgtd_endcap"] << " " << index["hgtd_layer"] << " "
                    << index["hgtd_phi_module"] << " " << index["hgtd_eta_module"]);
    return -1;
    }

    if (hashId.is_valid()){
        ATH_MSG_INFO("Valid hash for Index list: : " << index["hgtd_endcap"] << " " << index["hgtd_layer"] << " "
                    << index["hgtd_phi_module"] << " " << index["hgtd_eta_module"]);}




    int hitIdOfWafer = SiHitIdHelper::GetHelper()->buildHitId(HGTD_HitIndex,
                                                              index["hgtd_endcap"],
                                                              index["hgtd_layer"],
                                                              index["hgtd_eta_module"],
                                                              index["hgtd_phi_module"],
                                                               0); // side is just 0 for HGTD
    
    

    ATH_MSG_DEBUG("Index list: " << index["hgtd_endcap"] << " " << index["hgtd_layer"] << " "
                                 << index["hgtd_phi_module"] << " " << index["hgtd_eta_module"]);
    ATH_MSG_DEBUG("hitIdOfWafer = " << std::hex << hitIdOfWafer << std::dec);
    ATH_MSG_DEBUG(" endcap = " << SiHitIdHelper::GetHelper()->getBarrelEndcap(hitIdOfWafer)
                  << " lay = " << SiHitIdHelper::GetHelper()->getLayerDisk(hitIdOfWafer)
                  << " eta = " << SiHitIdHelper::GetHelper()->getEtaModule(hitIdOfWafer)
                  << " phi = " << SiHitIdHelper::GetHelper()->getPhiModule(hitIdOfWafer));
    return hitIdOfWafer;
}

void HGTD_GmxInterface::addSensorType(const std::string& clas,
                                      const std::string& typeName,
                                      const std::map<std::string, std::string>& parameters) {
    ATH_MSG_DEBUG("addSensorType called for class " << clas << ", typeName " << typeName);

    if (clas == "LGAD_module") {
        // TODO: implement method to actually add the sensor type (also to the detector manager)
        makeLgadModule(typeName, parameters);
    } else {
        ATH_MSG_ERROR("addSensorType: unrecognised sensor class: " << clas);
        ATH_MSG_ERROR("No sensor design created");
    }
}

void HGTD_GmxInterface::makeLgadModule(const std::string &typeName,
                                       const std::map<std::string, std::string> &parameters)
{

    double thickness{};
    double xPitch{};
    double yPitch{};
    int circuitsPerColumn{};
    int circuitsPerRow{};
    int padColumns{};
    int padRows{};

    // read parameters
    // TO DO : checking for unlogical values
    getParameter(typeName, parameters, "thickness", thickness);
    getParameter(typeName, parameters, "xPitch", xPitch);
    getParameter(typeName, parameters, "yPitch", yPitch);
    getParameter(typeName, parameters, "circuitsPerColumn", circuitsPerColumn);
    getParameter(typeName, parameters, "circuitsPerRow", circuitsPerRow);
    getParameter(typeName, parameters, "padColumns", padColumns);
    getParameter(typeName, parameters, "padRows", padRows);

    
    std::shared_ptr<const InDetDD::PixelDiodeMatrix> normalCell = InDetDD::PixelDiodeMatrix::construct(xPitch, yPitch);
    std::shared_ptr<const InDetDD::PixelDiodeMatrix> singleRow  = InDetDD::PixelDiodeMatrix::construct(InDetDD::PixelDiodeMatrix::phiDir, 0,
                                                                          normalCell, padColumns, 0);
    std::shared_ptr<const InDetDD::PixelDiodeMatrix> fullMatrix = InDetDD::PixelDiodeMatrix::construct(InDetDD::PixelDiodeMatrix::etaDir, 0,
                                                                          singleRow, padRows, 0);


    InDetDD::DetectorDesign::Axis yDirection = InDetDD::DetectorDesign::yAxis;

    InDetDD::HGTD_ModuleDesign* design = new InDetDD::HGTD_ModuleDesign(thickness,
                                                                        circuitsPerColumn, circuitsPerRow,
                                                                        padColumns, padRows/2,
                                                                        padColumns, padRows/2,
                                                                        fullMatrix,
                                                                        InDetDD::CarrierType::electrons, 1, yDirection );


   m_geometryMap[typeName] = design;

}

void HGTD_GmxInterface::addSensor(const std::string& typeName,
                                  std::map<std::string, int> &index,
                                  int /* sensitiveId */,
                                  GeoVFullPhysVol *fpv) {
    //
    // Get the ATLAS "Offline" wafer identifier
    //
    const HGTD_ID* hgtdIdHelper = dynamic_cast<const HGTD_ID *> (m_commonItems->getIdHelper());
    Identifier id = hgtdIdHelper->wafer_id(index["hgtd_endcap"],
                                           index["hgtd_layer"],
                                           index["hgtd_phi_module"],
                                           index["hgtd_eta_module"]);
    IdentifierHash hashId = hgtdIdHelper->wafer_hash(id);
    //
    // Now do our best to check if this is a valid id. If either the gmx file is wrong, or the xml file
    // defining the allowed id's is wrong, you can get disallowed id's. These cause a crash later
    // if allowed through. To do the check, we ask for the hash-id of this id. Invalid ids give a
    // special invalid hash-id (0xFFFFFFFF). But we don't exit the run, to help debug things quicker.
    //
    if (!hashId.is_valid()) {
        ATH_MSG_ERROR("Invalid id for sensitive module " << typeName << " volume with indices");
        for (const auto& [key, value] : index) {
            ATH_MSG_ERROR(key << " = " << value << "; ");
        }
        ATH_MSG_ERROR("Refusing to make it into a sensitive element. Incompatible gmx and identifier-xml files.");
        return;
    }

    //
    // Create the detector element and add to the DetectorManager
    //
    const InDetDD::HGTD_ModuleDesign* design = m_geometryMap[typeName];
    if (design == nullptr) {
        ATH_MSG_ERROR("addSensor: Error: Readout sensor type " << typeName << " not found.");
        throw std::runtime_error("readout sensor type " + typeName + " not found.");
    }

    m_detectorManager->addDetectorElement(new InDetDD::HGTD_DetectorElement(id, design, fpv, m_commonItems));

    return;
}
