/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModelXml/PixelGmxInterface.h"

#include <cstdlib>
#include <sstream>

#include "InDetSimEvent/SiHitIdHelper.h" 

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h" 
#include "GaudiKernel/MsgStream.h"

#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "ReadoutGeometryBase/PixelDiodeMatrix.h"
#include "ReadoutGeometryBase/PixelMultipleConnection1D.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "ReadoutGeometryBase/SiCommonItems.h"


namespace
{
constexpr int PixelHitIndex{0};
}


namespace ITk
{

PixelGmxInterface::PixelGmxInterface(InDetDD::PixelDetectorManager *detectorManager,
                                     InDetDD::SiCommonItems *commonItems,
                                     WaferTree *moduleTree)
  : AthMessaging(Athena::getMessageSvc(), "PixelGmxInterface"),
    m_detectorManager(detectorManager),
    m_commonItems(commonItems),
    m_moduleTree(moduleTree)
{}


int PixelGmxInterface::sensorId(std::map<std::string, int> &index) const
{
  // Return the Simulation HitID (nothing to do with "ATLAS Identifiers" aka "Offline Identifiers")

  // Check if identifier is valid
  // TODO: drop this check in the future
  const PixelID *pixelIdHelper = dynamic_cast<const PixelID *>(m_commonItems->getIdHelper());
  Identifier id = pixelIdHelper->wafer_id(index["barrel_endcap"],
                                          index["layer_wheel"],
                                          index["phi_module"], 
					                                index["eta_module"]);
  IdentifierHash hashId = pixelIdHelper->wafer_hash(id);
  if (!hashId.is_valid()) {
    return -1;
  }

  // Compute the actuall SiHitId
  int hitIdOfModule = SiHitIdHelper::GetHelper()->buildHitId(PixelHitIndex,
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


void PixelGmxInterface::addSensorType(std::string clas,
                                      std::string typeName,
                                      std::map<std::string, std::string> parameters)
{
  ATH_MSG_DEBUG("addModuleType called for class " << clas << ", typeName " << typeName);

  if (clas == "PixelModule_Sensitive") {
    makePixelModule(typeName, parameters);
  } else {
    ATH_MSG_ERROR("addModuleType: unrecognised module class: " << clas);
    ATH_MSG_ERROR("No module design created");
  }
}


void PixelGmxInterface::makePixelModule(const std::string &typeName,
                                        const std::map<std::string, std::string> &parameters)
{

  //
  // Get all parameters.
  // This comes from PixelModuleDesign
  //
  double thickness(2.075);
  int circuitsPerColumn(10);
  int circuitsPerRow(10);
  int cellColumnsPerCircuit(10);
  int cellRowsPerCircuit(10);
  int diodeColumnsPerCircuit(10);
  int diodeRowsPerCircuit(10);
  InDetDD::CarrierType carrier(InDetDD::electrons);
  int readoutSide(-1);
  bool is3D(false);

  // carrier type
  std::string carrierType;
  getParameter(typeName, parameters, "carrierType", carrierType);
  if (carrierType == "electrons") {
    carrier = InDetDD::electrons;
  } else if (carrierType == "holes") {
    carrier = InDetDD::holes;
  } else {
    ATH_MSG_FATAL("makePixelModule: Error: parameter carrierType should be electrons or holes for " << typeName);
    throw std::runtime_error("invalid carrierType");
  }
  // readout side
  std::string readoutSideStr;
  getParameter(typeName, parameters, "readoutSide", readoutSideStr);
  if (readoutSideStr == "+") {
    readoutSide = 1;
  } else if (readoutSideStr == "-") {
    readoutSide = -1;
  } else {
    ATH_MSG_FATAL("makePixelModule: Error: parameter readoutSide should be + or - for " << typeName);
    throw std::runtime_error("invalid readoutSide");
  }
  // other parameters
  getParameter(typeName, parameters, "thickness", thickness);
  getParameter(typeName, parameters, "circuitsPerColumn", circuitsPerColumn);
  getParameter(typeName, parameters, "circuitsPerRow", circuitsPerRow);
  getParameter(typeName, parameters, "cellColumnsPerCircuit", cellColumnsPerCircuit);
  getParameter(typeName, parameters, "cellRowsPerCircuit", cellRowsPerCircuit);  
  getParameter(typeName, parameters, "diodeColumnsPerCircuit", diodeColumnsPerCircuit);  
  getParameter(typeName, parameters, "diodeRowsPerCircuit", diodeRowsPerCircuit);  
  getParameter(typeName, parameters, "is3D", is3D);

  //
  // Make Module Design and add to DetectorManager
  //
  std::shared_ptr<InDetDD::PixelDiodeMatrix> matrix{};
  auto design = std::make_unique<InDetDD::PixelModuleDesign>(thickness, 
                                                             circuitsPerColumn, circuitsPerRow, 
                                                             cellColumnsPerCircuit, cellRowsPerCircuit, 
                                                             diodeColumnsPerCircuit, diodeRowsPerCircuit,
                                                             matrix,
                                                             carrier, 
                                                             readoutSide, is3D); 

  ATH_MSG_DEBUG("readout geo - design : " << design->width() << " " << design->length() << " " << design->thickness() << "    " <<design->rows() << " " << design->columns());

  m_detectorManager->addDesign(std::move(design));

  // Add to map for addModule routine
  m_geometryMap[typeName] = m_detectorManager->numDesigns() - 1;
}


void PixelGmxInterface::addSensor(std::string typeName,
                                  std::map<std::string, int> &index,
                                  int /*sensitiveId*/,
                                  GeoVFullPhysVol *fpv)
{
  //
  // Get the ATLAS "Offline" wafer identifier 
  //
  const PixelID *pixelIdHelper = dynamic_cast<const PixelID *>(m_commonItems->getIdHelper());
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
    ATH_MSG_ERROR("Refusing to make it into a sensitive element. Incompatible gmx and identifier-xml files.");
    return;
  }

  //
  // Create the detector element and add to the DetectorManager
  //
  const InDetDD::SiDetectorDesign *design = m_detectorManager->getDesign(m_geometryMap[typeName]);
  if (design == nullptr) {
    ATH_MSG_ERROR("addSensor: Error: Readout sensor type " << typeName << " not found.");
    throw std::runtime_error("readout sensor type " + typeName + " not found.");
  }

  m_detectorManager->addDetectorElement(new InDetDD::SiDetectorElement(id, design, fpv, m_commonItems));

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

} // namespace ITk
