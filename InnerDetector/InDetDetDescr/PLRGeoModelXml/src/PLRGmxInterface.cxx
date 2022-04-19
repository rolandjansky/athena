/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PLRGmxInterface.h"

#include <InDetReadoutGeometry/SiDetectorDesign.h>
#include <InDetReadoutGeometry/SiDetectorElement.h>
#include <InDetSimEvent/SiHitIdHelper.h>
#include <PixelReadoutGeometry/PixelDetectorManager.h>
#include <PixelReadoutGeometry/PixelModuleDesign.h>
#include <ReadoutGeometryBase/PixelDiodeMatrix.h>
#include <ReadoutGeometryBase/SiCommonItems.h>


namespace InDetDD
{

PLRGmxInterface::PLRGmxInterface(PixelDetectorManager *detectorManager,
                                 SiCommonItems *commonItems,
                                 WaferTree *moduleTree)
  : PixelGmxInterface(detectorManager, commonItems, moduleTree),
    m_detectorManager(detectorManager),
    m_commonItems(commonItems),
    m_moduleTree(moduleTree)
{}

int PLRGmxInterface::sensorId(std::map<std::string, int> &index) const
{
  // Return the Simulation HitID (nothing to do with "ATLAS Identifiers" aka "Offline Identifiers")

  // Check if identifier is valid
  // TODO: drop this check in the future
  const PLR_ID *pixelIdHelper = dynamic_cast<const PLR_ID *>(m_commonItems->getIdHelper());
  Identifier id = pixelIdHelper->wafer_id(index["barrel_endcap"],
                                          index["layer_wheel"],
                                          index["phi_module"],
                                          index["eta_module"]);
  IdentifierHash hashId = pixelIdHelper->wafer_hash(id);
  if (!hashId.is_valid()) {
    ATH_MSG_WARNING("PLR Invalid hash for Index list: " << index["barrel_endcap"] << " " << index["layer_wheel"] << " "
                    << index["eta_module"] << " " << index["phi_module"] << " " << index["side"]);
    return -1;
  }
  // Compute the actuall SiHitId, first number is the part number: lumi=2
  int hitIdOfModule = SiHitIdHelper::GetHelper()->buildHitId(2,
                                                             index["barrel_endcap"],
                                                             index["layer_wheel"],
                                                             index["eta_module"],
                                                             index["phi_module"],
                                                             index["side"]);
  ATH_MSG_DEBUG("Index list: " << index["barrel_endcap"] << " " << index["layer_wheel"] << " "
                               << index["eta_module"] << " " << index["phi_module"] << " " << index["side"]);
  ATH_MSG_DEBUG("hitIdOfModule = " << std::hex << hitIdOfModule << std::dec);
  ATH_MSG_DEBUG(" bec = " << SiHitIdHelper::GetHelper()->getBarrelEndcap(hitIdOfModule)
                << " lay = " << SiHitIdHelper::GetHelper()->getLayerDisk(hitIdOfModule)
                << " eta = " << SiHitIdHelper::GetHelper()->getEtaModule(hitIdOfModule)
                << " phi = " << SiHitIdHelper::GetHelper()->getPhiModule(hitIdOfModule)
                << " side = " << SiHitIdHelper::GetHelper()->getSide(hitIdOfModule));

  return hitIdOfModule;
}


void PLRGmxInterface::addSensorType(const std::string& clas,
                                    const std::string& typeName,
                                    const std::map<std::string, std::string>& parameters)
{
  ATH_MSG_DEBUG("addSensorType called for class " << clas << ", typeName " << typeName);
  // only load the sensor type that the PLR will use
  if (clas == "SingleChip_RD53" && typeName == "RD53_20x19_Single_25x100") {
    makePLRModule(typeName, parameters);
  }
}


void PLRGmxInterface::addSensor(const std::string& typeName,
                                  std::map<std::string, int> &index,
                                  int /*sensitiveId*/,
                                  GeoVFullPhysVol *fpv)
{
  //
  // Get the ATLAS "Offline" wafer identifier
  //
  const PLR_ID *pixelIdHelper = dynamic_cast<const PLR_ID *>(m_commonItems->getIdHelper());
  Identifier id = pixelIdHelper->wafer_id(index["barrel_endcap"],
                                          index["layer_wheel"],
                                          index["phi_module"],
                                          index["eta_module"]);
  IdentifierHash hashId = pixelIdHelper->wafer_hash(id);
  //
  //    Now do our best to check if this is a valid id. If either the gmx file is wrong, or the xml file
  //    defining the allowed id's is wrong, you can get disallowed id's. These cause a crash later
  //    if allowed through. To do the check, we ask for the hash-id of this id. Invalid ids give a
  //    special invalid hash-id (0xFFFFFFFF). But we don't exit the run, to help debug things quicker.
  //
  if (!hashId.is_valid()) {
    ATH_MSG_ERROR("Invalid id for sensitive module " << typeName << " volume with indices");
    for (const auto& [key, value] : index) {
      msg() << MSG::ERROR << key << " = " << value << "; ";
    }
    msg() << MSG::ERROR << endmsg;
    ATH_MSG_ERROR("Refusing to make it into a sensitive element. Incompatible gmx and identifier-xml files.");
    return;
  }

  //
  // Create the detector element and add to the DetectorManager
  //
  auto it = m_geometryMap.find(typeName);
  if(it == m_geometryMap.end()) {
    ATH_MSG_ERROR("addSensor: Error: Readout sensor type " << typeName << " not found.");
    throw std::runtime_error("readout sensor type " + typeName + " not found.");
  }
  const SiDetectorDesign *design = m_detectorManager->getDesign(it->second);
  ATH_MSG_VERBOSE("Adding sensor with design: " << typeName << " " << design);
  if (design == nullptr) {
    ATH_MSG_ERROR("addSensor: Error: Readout sensor type " << typeName << " not found.");
    throw std::runtime_error("readout sensor type " + typeName + " not found.");
  }

  m_detectorManager->addDetectorElement(new SiDetectorElement(id, design, fpv, m_commonItems));

  //
  // Build up a map-structure for numerology
  //
  Wafer module((unsigned int) hashId);
  std::string errorMessage("");
  if (!m_moduleTree->add(index["barrel_endcap"],
                         index["layer_wheel"],
                         index["eta_module"],
                         index["phi_module"],
                         module,
                         errorMessage)) {
    ATH_MSG_ERROR(errorMessage);
  }

  return;
}


void PLRGmxInterface::makePLRModule(const std::string &typeName,
                                    const std::map<std::string, std::string> &parameters)
{
  int circuitsPerEta{1}; // row
  int circuitsPerPhi{1}; // column
  double thickness{0.150};
  double pitchEta{};
  double pitchPhi{};
  double pitchEtaLong{};
  double pitchPhiLong{};
  double pitchEtaEnd{};
  double pitchPhiEnd{};
  int nEtaLongPerSide{};
  int nPhiLongPerSide{};
  int nEtaEndPerSide{};
  int nPhiEndPerSide{};
  int rowsPerChip{};
  int columnsPerChip{};

  // unused
  InDetDD::CarrierType carrier{InDetDD::electrons};
  int readoutSide{1};
  bool is3D{true};

  // read parameters
  getParameter(typeName, parameters, "circuitsPerEta", circuitsPerEta);
  getParameter(typeName, parameters, "circuitsPerPhi", circuitsPerPhi);
  getParameter(typeName, parameters, "thickness", thickness);
  getParameter(typeName, parameters, "is3D", is3D);
  getParameter(typeName, parameters, "rows", rowsPerChip);
  getParameter(typeName, parameters, "columns", columnsPerChip);
  getParameter(typeName, parameters, "pitchEta", pitchEta);
  getParameter(typeName, parameters, "pitchPhi", pitchPhi);
  getParameter(typeName, parameters, "pitchEtaLong", pitchEtaLong);
  getParameter(typeName, parameters, "pitchPhiLong", pitchPhiLong);
  getParameter(typeName, parameters, "pitchEtaEnd", pitchEtaEnd);
  getParameter(typeName, parameters, "pitchPhiEnd", pitchPhiEnd);
  getParameter(typeName, parameters, "nPhiLongPerSide", nPhiLongPerSide);
  getParameter(typeName, parameters, "nEtaLongPerSide", nEtaLongPerSide);
  getParameter(typeName, parameters, "nPhiEndPerSide", nPhiEndPerSide);
  getParameter(typeName, parameters, "nEtaEndPerSide", nEtaEndPerSide);

  //
  // Make Module Design and add to DetectorManager
  //
  std::shared_ptr<const PixelDiodeMatrix> fullMatrix = buildMatrix(pitchPhi, pitchEta,
                                                                   pitchPhiLong, pitchPhiEnd,
                                                                   pitchEtaLong, pitchEtaEnd,
                                                                   nPhiLongPerSide, nPhiEndPerSide,
                                                                   nEtaLongPerSide, nEtaEndPerSide,
                                                                   circuitsPerPhi, circuitsPerEta,
                                                                   columnsPerChip, rowsPerChip);

  ATH_MSG_DEBUG("fullMatrix = buildMatrix(" << pitchPhi << ", " << pitchEta << ", "
                                            << pitchPhiLong << ", " << pitchPhiEnd << ", "
                                            << pitchEtaLong << ", " << pitchEtaEnd << ", "
                                            << nPhiLongPerSide << ", " << nPhiEndPerSide << ", "
                                            << nEtaLongPerSide << ", " << nEtaEndPerSide << ", "
                                            << circuitsPerPhi << ", " << circuitsPerEta << ", "
                                            << columnsPerChip << ", " << rowsPerChip << ")");
  ATH_MSG_DEBUG("readout geo - design " << thickness << " "
                                        << circuitsPerPhi << " " << circuitsPerEta << " "
                                        << columnsPerChip << " " << rowsPerChip << " "
                                        << columnsPerChip << " " << rowsPerChip << " "
                                        << carrier << " " << readoutSide);

  // Setting module identifier to InDetDD::PLR
  // (so far) primarily useful to avoid orientation warnings
  InDetDD::DetectorType detectorType{InDetDD::PLR};

  auto design = std::make_unique<PixelModuleDesign>(thickness,
                                                    circuitsPerPhi, circuitsPerEta,
                                                    columnsPerChip, rowsPerChip,
                                                    columnsPerChip, rowsPerChip,
                                                    fullMatrix, carrier,
                                                    readoutSide, is3D, detectorType);
  

  ATH_MSG_DEBUG("readout geo - design : " << design->width() << " " << design->length() << " " << design->thickness() << " " <<design->rows() << " " << design->columns());

  [[maybe_unused]] auto observedPtr = m_detectorManager->addDesign(std::move(design));

  // Add to map for addModule routine
  m_geometryMap[typeName] = m_detectorManager->numDesigns() - 1;
}

} // namespace InDetDD
