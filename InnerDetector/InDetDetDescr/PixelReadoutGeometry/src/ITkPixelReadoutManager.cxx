/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "ITkPixelReadoutManager.h"

#include <InDetIdentifier/PixelID.h>
#include <InDetReadoutGeometry/SiDetectorElement.h>
#include <PixelReadoutGeometry/PixelModuleDesign.h>
#include <PixelReadoutGeometry/PixelDetectorManager.h>


namespace InDetDD
{

namespace ITk
{

PixelReadoutManager::PixelReadoutManager(const std::string &name,
                                         ISvcLocator *svc)
  : base_class(name, svc)
{
}


StatusCode PixelReadoutManager::initialize()
{
  ATH_MSG_DEBUG("ITkPixelReadoutManager::initialize()");

  ATH_CHECK(m_detStore.retrieve());
  ATH_CHECK(m_detStore->retrieve(m_detManager, "ITkPixel"));
  ATH_CHECK(m_detStore->retrieve(m_idHelper, "PixelID"));

  return StatusCode::SUCCESS;
}


PixelModuleType PixelReadoutManager::getModuleType(Identifier id) const
{
  const Identifier wafer_id = m_idHelper->wafer_id(id);
  const SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const PixelModuleDesign *p_design = static_cast<const PixelModuleDesign *>(&element->design());
  if (p_design->getReadoutTechnology() != PixelReadoutTechnology::RD53) {
    ATH_MSG_ERROR("Non-RD53 readout technologies not supported!");
    return PixelModuleType::NONE;
  }

  if (m_idHelper->barrel_ec(id) == 0) {
    return PixelModuleType::PIX_BARREL;
  }

  if (std::abs(m_idHelper->barrel_ec(id)) == 2) {
    return PixelModuleType::PIX_ENDCAP;
  }

  return PixelModuleType::NONE;
}


PixelDiodeType PixelReadoutManager::getDiodeType(Identifier id) const
{
  const Identifier wafer_id = m_idHelper->wafer_id(id);
  const SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const PixelModuleDesign *p_design = static_cast<const PixelModuleDesign *>(&element->design());
  if (p_design->getReadoutTechnology() != PixelReadoutTechnology::RD53) {
    ATH_MSG_ERROR("Non-RD53 readout technologies not supported!");
    return PixelDiodeType::NORMAL;
  }

  Identifier diodeId = id;
  uint32_t col = getColumn(diodeId, wafer_id);
  uint32_t row = getRow(diodeId, wafer_id);

  // ---------------------
  // Get the pixel type
  // ---------------------
  unsigned int FEs = p_design->numberOfCircuits();
  unsigned int rowsPerFE =  p_design->rowsPerCircuit();
  unsigned int columnsPerFE = p_design->columnsPerCircuit();

  if (FEs == 4) {
    // long pixel row and columns
    // 2 per row/column side
    // TODO: avoid hardcoding this
    if (row > rowsPerFE - 3 && col < columnsPerFE - 2) {
      return PixelDiodeType::LONG;
    }
    if (col > columnsPerFE - 3 && row < rowsPerFE - 2) {
      return PixelDiodeType::LONG;
    }
    // corner big pixels
    if (row > rowsPerFE - 3 && col > columnsPerFE - 3) {
      return PixelDiodeType::LARGE;
    }
  } else if (FEs != 1) {
    ATH_MSG_WARNING("Module with a number of circuits which is not 1 or 4.");
    ATH_MSG_WARNING("Long/pixel identification not implemented");
  }

  return PixelDiodeType::NORMAL;
}


Identifier PixelReadoutManager::getPixelIdfromHash(IdentifierHash offlineIdHash,
                                                   uint32_t FE,
                                                   uint32_t row,
                                                   uint32_t column) const
{
  return getPixelId(m_idHelper->wafer_id(offlineIdHash), FE, row, column);
}


Identifier PixelReadoutManager::getPixelId(Identifier offlineId,
                                           uint32_t FE,
                                           uint32_t row,
                                           uint32_t column) const
{
  const SiDetectorElement *element = m_detManager->getDetectorElement(offlineId);
  const PixelModuleDesign *p_design = static_cast<const PixelModuleDesign *>(&element->design());
  if (p_design->getReadoutTechnology() != PixelReadoutTechnology::RD53) {
    ATH_MSG_ERROR("Non-RD53 readout technologies not supported!");
    return Identifier();
  }

  unsigned int FEs = p_design->numberOfCircuits();
  unsigned int FEsPerRow = p_design->numberOfCircuitsPerRow();
  unsigned int rowsPerFE =  p_design->rowsPerCircuit();
  unsigned int columnsPerFE = p_design->columnsPerCircuit();

  // ---------------------
  // Check input sanity
  // ---------------------
  if (row >= rowsPerFE || column >= columnsPerFE || FE >= FEs) {
    ATH_MSG_DEBUG("Illegal pixel requested OfflineID: " << std::hex << offlineId << std::dec << " FE: " << FE << " row: " << row << " column: " << column);
    ATH_MSG_DEBUG("Limits are: FE < " << FEs << ", row < " << rowsPerFE << ", column < " << columnsPerFE);
    return Identifier(); // illegal Identifier, standardized for PixelRodDecoder
  }

  // ---------------------
  // Convert row/column to eta/phi indices
  // ---------------------
  unsigned int phi_index{}, eta_index{};
  if (FE >= 2) {
    phi_index = 2 * rowsPerFE - 1 - row;
  } else {
    phi_index = row;
  }
  if (FE % 2 == 1) {
    eta_index = 2 * columnsPerFE - 1 - column;
  } else {
    eta_index = column;
  }

  // Identify the module type
  PixelModuleType moduleType = getModuleType(offlineId);
  if (moduleType == PixelModuleType::PIX_ENDCAP) {
    // Swap phi_index for even endcap modules
    int module_phi = m_idHelper->phi_module(offlineId);
    if (module_phi % 2 == 0) {
      phi_index = FEsPerRow * rowsPerFE - phi_index - 1;
      ATH_MSG_DEBUG("Even disk module found, phi module: " << module_phi << " swapped phi index to : " << phi_index);
    }
  }

  return m_idHelper->pixel_id(offlineId, phi_index, eta_index);
}


uint32_t PixelReadoutManager::getFE(Identifier diodeId,
                                    Identifier offlineId) const
{
  const SiDetectorElement *element = m_detManager->getDetectorElement(offlineId);
  const PixelModuleDesign *p_design = static_cast<const PixelModuleDesign *>(&element->design());
  if (p_design->getReadoutTechnology() != PixelReadoutTechnology::RD53) {
    ATH_MSG_ERROR("Non-RD53 readout technologies not supported!");
    return 0xffffffff;
  }

  unsigned int FEsPerRow = p_design->numberOfCircuitsPerRow();
  unsigned int rowsPerFE =  p_design->rowsPerCircuit();
  unsigned int columnsPerFE = p_design->columnsPerCircuit();

  // ---------------------
  // Set module properties
  // ---------------------
  unsigned int phi_index = m_idHelper->phi_index(diodeId);
  unsigned int eta_index = m_idHelper->eta_index(diodeId);

  PixelModuleType moduleType = getModuleType(offlineId);
  if (moduleType == PixelModuleType::PIX_ENDCAP) {
    // Swap phi_index for even endcap modules
    int module_phi = m_idHelper->phi_module(offlineId);
    if (module_phi % 2 == 0) {
      phi_index = FEsPerRow * rowsPerFE - phi_index - 1;
    }
  }

  // ---------------------
  // Compute FE number
  // ---------------------
  // ITk has up to 4 FEs
  unsigned int FErow = static_cast<unsigned int>(std::floor(phi_index / rowsPerFE));
  unsigned int FEcol = static_cast<unsigned int>(std::floor(eta_index / columnsPerFE));
  if (FErow > 0) {
    return 2 + FEcol;
  } else {
    return FEcol;
  }
}


uint32_t PixelReadoutManager::getColumn(Identifier diodeId,
                                        Identifier offlineId) const
{
  const SiDetectorElement *element = m_detManager->getDetectorElement(offlineId);
  const PixelModuleDesign *p_design = static_cast<const PixelModuleDesign *>(&element->design());
  if (p_design->getReadoutTechnology() != PixelReadoutTechnology::RD53) {
    ATH_MSG_ERROR("Non-RD53 readout technologies not supported!");
    return 0xffffffff;
  }

  unsigned int columnsPerFE = p_design->columnsPerCircuit();
  unsigned int eta_index = m_idHelper->eta_index(diodeId);

  // ---------------------
  // Convert eta index to column number
  // ---------------------
  unsigned int column{};
  if (eta_index >= columnsPerFE) {
    column = 2 * columnsPerFE - eta_index - 1;
  } else {
    column = eta_index;
  }

  // ---------------------
  // Check output sanity
  // ---------------------
  if (column >= columnsPerFE) {
    ATH_MSG_ERROR("Computed column number exceeds maximum value: col = " << column << " (max = " << columnsPerFE << ")");
    return 0xffffffff;
  }

  return column;
}


uint32_t PixelReadoutManager::getRow(Identifier diodeId,
                                     Identifier offlineId) const
{
  const SiDetectorElement *element = m_detManager->getDetectorElement(offlineId);
  const PixelModuleDesign *p_design = static_cast<const PixelModuleDesign *>(&element->design());
  if (p_design->getReadoutTechnology() != PixelReadoutTechnology::RD53) {
    ATH_MSG_ERROR("Non-RD53 readout technologies not supported!");
    return 0xffffffff;
  }

  unsigned int FEsPerRow = p_design->numberOfCircuitsPerRow();
  unsigned int rowsPerFE =  p_design->rowsPerCircuit();
  unsigned int phi_index = m_idHelper->phi_index(diodeId);

  // Identify the module type
  PixelModuleType moduleType = getModuleType(offlineId);
  if (moduleType == PixelModuleType::PIX_ENDCAP) {
    // Swap phi_index for even endcap modules
    int module_phi = m_idHelper->phi_module(offlineId);
    if (module_phi % 2 == 0) {
      phi_index = FEsPerRow * rowsPerFE - phi_index - 1;
    }
  }

  // ---------------------
  // Convert phi index to row number
  // ---------------------
  unsigned int row{};
  if (phi_index >= rowsPerFE) {
    row = 2 * rowsPerFE - phi_index - 1;
  } else {
    row = phi_index;
  }

  // ---------------------
  // Check output sanity
  // ---------------------
  if (row >= rowsPerFE) {
    ATH_MSG_ERROR("Computed row number exceeds maximum value: row = " << row << "(max = " << rowsPerFE << ")");
    return 0xffffffff;
  }

  return row;
}

} // namespace ITk
} // namespace InDetDD
