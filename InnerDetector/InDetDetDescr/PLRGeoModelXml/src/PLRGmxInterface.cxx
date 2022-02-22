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
    m_detectorManager(detectorManager)
{}


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
