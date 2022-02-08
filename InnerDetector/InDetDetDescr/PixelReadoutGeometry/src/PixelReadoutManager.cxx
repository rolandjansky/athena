/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelReadoutManager.h"

#include <InDetIdentifier/PixelID.h>
#include <InDetReadoutGeometry/SiDetectorElement.h>
#include <PixelReadoutGeometry/PixelModuleDesign.h>
#include <PixelReadoutGeometry/PixelDetectorManager.h>

// #define PIXEL_DEBUG

namespace InDetDD
{

PixelReadoutManager::PixelReadoutManager(const std::string &name,
                                         ISvcLocator *svc)
  : base_class(name, svc)
{
}


StatusCode PixelReadoutManager::initialize()
{
  ATH_MSG_DEBUG("PixelReadoutManager::initialize()");

  ATH_CHECK(m_detStore.retrieve());
  ATH_CHECK(m_detStore->retrieve(m_detManager, "Pixel"));
  ATH_CHECK(m_detStore->retrieve(m_idHelper, "PixelID"));

  return StatusCode::SUCCESS;
}


PixelModuleType PixelReadoutManager::getModuleType(Identifier id) const
{
  const Identifier wafer_id = m_idHelper->wafer_id(id);
  const SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const PixelModuleDesign *p_design = static_cast<const PixelModuleDesign *>(&element->design());
  if (p_design->getReadoutTechnology() == PixelReadoutTechnology::RD53) {
    ATH_MSG_ERROR("RD53 readout technologies not supported!");
    return PixelModuleType::NONE;
  }

  if (p_design->getReadoutTechnology() == PixelReadoutTechnology::FEI4) {
    if (m_idHelper->is_dbm(id)) {
      return PixelModuleType::DBM;
    }

    if (p_design->numberOfCircuits() == 2) {
      return PixelModuleType::IBL_PLANAR;
    }

    return PixelModuleType::IBL_3D;
  }

  if (p_design->getReadoutTechnology() == PixelReadoutTechnology::FEI3) {
    if (std::abs(m_idHelper->barrel_ec(id)) == 0) {
      return PixelModuleType::PIX_BARREL;
    }
    if (std::abs(m_idHelper->barrel_ec(id)) == 2) {
      return PixelModuleType::PIX_ENDCAP;
    }
  }

  return PixelModuleType::NONE;
}


PixelDiodeType PixelReadoutManager::getDiodeType(Identifier id) const
{
  const Identifier wafer_id = m_idHelper->wafer_id(id);
  Identifier diodeId = id;
  Identifier offlineId = m_idHelper->wafer_id(diodeId);
  int col = getColumn(diodeId, offlineId);
  int row = getRow(diodeId, offlineId);

  const SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const PixelModuleDesign *p_design = static_cast<const PixelModuleDesign *>(&element->design());
  if (p_design->getReadoutTechnology() == PixelReadoutTechnology::RD53) {
    ATH_MSG_ERROR("RD53 readout technologies not supported!");
    return PixelDiodeType::NORMAL;
  }

  if (p_design->getReadoutTechnology() == PixelReadoutTechnology::FEI4) {
    if (p_design->numberOfCircuits() == 2) {       // IBL planar
      if (col == 0 || col == p_design->columnsPerCircuit() - 1) {   // column edge =0,79
        return PixelDiodeType::LONG;
      }
      return PixelDiodeType::NORMAL;
    }
    return PixelDiodeType::NORMAL;
  }

  if (p_design->getReadoutTechnology() == PixelReadoutTechnology::FEI3) {
    if (col > 0 && col < p_design->columnsPerCircuit() - 1) {
      if (row >= p_design->rowsPerCircuit() / 2 - 1 - 6 - 1 && row <= p_design->rowsPerCircuit() / 2 - 1) {
        if ((row - (p_design->rowsPerCircuit() / 2 - 1 - 6) + 1) % 2 + 1 == 1) {
          return PixelDiodeType::LONG;
        }
        if ((row - (p_design->rowsPerCircuit() / 2 - 1 - 6) + 1) % 2 + 1 == 2) {
          return PixelDiodeType::GANGED;
        }
      }
      return PixelDiodeType::NORMAL;
    } 

    if (col == 0 || col==p_design->columnsPerCircuit() - 1) {
      if (row >= p_design->rowsPerCircuit() / 2 - 1 - 6 - 1) {
        return PixelDiodeType::GANGED;
      }
      return PixelDiodeType::LONG;
    }

    ATH_MSG_WARNING("Pixel Type : the col number should be 0-" << p_design->columnsPerCircuit() << ", not " <<col);
    return PixelDiodeType::NORMAL;
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
  if (p_design->getReadoutTechnology() == PixelReadoutTechnology::RD53) {
    ATH_MSG_ERROR("RD53 readout technologies not supported!");
    return {};
  }

  unsigned int columnsPerFE     = p_design->columnsPerCircuit();
  unsigned int FEsPerHalfModule = p_design->numberOfCircuits();
  unsigned int rowsPerFE = 0;
  int column_row_offset = 0;
  if (p_design->getReadoutTechnology() == PixelReadoutTechnology::FEI4) {
    rowsPerFE    = p_design->rowsPerCircuit();
    column_row_offset = -1;
  }
  else if (p_design->getReadoutTechnology() == PixelReadoutTechnology::FEI3) {
    rowsPerFE    = p_design->rowsPerCircuit()/2+4;  // normal + ganged
  }

  // ---------------------
  // Check input sanity
  // ---------------------
  // Identify the module type
  PixelModuleType moduleType = getModuleType(offlineId);

  // Correct row, column
  row = row + column_row_offset;
  column = column + column_row_offset;

  if (row>=rowsPerFE || column>=columnsPerFE || FE>=((moduleType == PixelModuleType::IBL_PLANAR || moduleType == PixelModuleType::IBL_3D || moduleType == PixelModuleType::DBM) ? FEsPerHalfModule:2*FEsPerHalfModule)) {
    ATH_MSG_DEBUG("Illegal pixel requested OfflineID: " << std::hex << offlineId << std::dec << " FE: " << FE << " row: " << row << " column: " << column);
    ATH_MSG_DEBUG("Limits are: FE < " << ((moduleType == PixelModuleType::IBL_PLANAR || moduleType == PixelModuleType::IBL_3D || moduleType == PixelModuleType::DBM) ? FEsPerHalfModule : 2*FEsPerHalfModule) << ", row < " << rowsPerFE << ", column < " << columnsPerFE);
    return {}; // illegal Identifier, standardized for PixelRodDecoder
  }

  // ---------------------
  // Convert row/column to eta/phi indices
  // ---------------------
  unsigned int phi_index, eta_index;
  switch (moduleType)
  {
    case PixelModuleType::DBM:
      eta_index = rowsPerFE-1-row;
      if (m_idHelper->barrel_ec(offlineId)>0) {     // A side (pos. eta)
        phi_index = column;
      } 
      else {                                        // C side
        phi_index = columnsPerFE-1-column;
      }
      break;

    case PixelModuleType::IBL_PLANAR:
      phi_index = rowsPerFE-1-row;
      eta_index = FE*columnsPerFE+column;
      break;

    case PixelModuleType::IBL_3D:
      phi_index = rowsPerFE-1-row;
      eta_index = FE*columnsPerFE+column;
      break;

    default:    // pixels
      if (FE<FEsPerHalfModule) {
        phi_index = ((2*rowsPerFE)-1)-row;
        eta_index = ((columnsPerFE*FEsPerHalfModule)-1)-(column+(FE*columnsPerFE));
      } 
      else {
        phi_index = row;
        eta_index = ((FE-FEsPerHalfModule)*columnsPerFE)+column;
      }
      if (moduleType == PixelModuleType::PIX_ENDCAP) {
        // Swap phi_index for even endcap modules
        if ((m_idHelper->phi_module(offlineId))%2==0) {
          phi_index = 2*rowsPerFE-phi_index-1;
          ATH_MSG_DEBUG("Even disk module found, phi module: " << m_idHelper->phi_module(offlineId) << " swapped phi index to : " << phi_index);
        }
      }
      break;
  }

  Identifier diodeId = m_idHelper->pixel_id(offlineId, phi_index, eta_index);

#ifdef PIXEL_DEBUG
  unsigned int eta_index_max = m_idHelper->eta_index_max(offlineId);
  unsigned int phi_index_max = m_idHelper->phi_index_max(offlineId);
  if (eta_index>eta_index_max) {
    ATH_MSG_DEBUG("Error! eta_index: " << eta_index << " > eta_index_max: " << eta_index_max);
  }
  if (phi_index>phi_index_max) {
    ATH_MSG_DEBUG("Error! phi_index: " << phi_index << " > phi_index_max: " << phi_index_max);
  }
  //consistency check - to be removed to speed up
  uint32_t check_FE = getFE(diodeId, offlineId);
  uint32_t check_row = getRow(diodeId, offlineId) + column_row_offset;
  uint32_t check_column = getColumn(diodeId, offlineId) + column_row_offset;
  if (check_FE!=FE || check_row!=row || check_column!=column) {
    ATH_MSG_WARNING("identify OfflineID: 0x" << std::hex << offlineId << std::dec << " FE: " << FE << " row: " << row << " column: " << column << " unequal to:");
    ATH_MSG_WARNING("identify PixelID: 0x" << std::hex << diodeId << std::dec << " FE: " << check_FE << " row: " << check_row << " column: " << check_column);
  }
#endif

  return diodeId;
}


uint32_t PixelReadoutManager::getFE(Identifier diodeId,
                                    Identifier offlineId) const
{
  const SiDetectorElement *element = m_detManager->getDetectorElement(offlineId);
  const PixelModuleDesign *p_design = static_cast<const PixelModuleDesign *>(&element->design());
  if (p_design->getReadoutTechnology() == PixelReadoutTechnology::RD53) {
    ATH_MSG_ERROR("RD53 readout technologies not supported!");
    return 0xffffffff;
  }

  unsigned int columnsPerFE     = p_design->columnsPerCircuit();
  unsigned int FEsPerHalfModule = p_design->numberOfCircuits();
  unsigned int rowsPerFE = 0;
  if (p_design->getReadoutTechnology() == PixelReadoutTechnology::FEI4) {
    rowsPerFE    = p_design->rowsPerCircuit();
  }
  else if (p_design->getReadoutTechnology() == PixelReadoutTechnology::FEI3) {
    rowsPerFE    = p_design->rowsPerCircuit()/2+4;  // normal + ganged
  }

  // ---------------------
  // Set module properties
  // ---------------------
  PixelModuleType moduleType = getModuleType(offlineId);
  unsigned int FE;
  unsigned int phi_index = m_idHelper->phi_index(diodeId);
  int eta_index = m_idHelper->eta_index(diodeId);

  switch (moduleType)
  {
    case PixelModuleType::DBM:
      FE = 0;     // simple as that
      return FE;
      break;

    case PixelModuleType::PIX_ENDCAP:
      // Swap phi_index for even endcap modules
      if ((m_idHelper->phi_module(offlineId))%2==0) {
        phi_index = 2*rowsPerFE-phi_index-1;
      }
      break;

    default:        // PIX_BARREL + IBL
      break;
  }


  // ---------------------
  // Compute FE number
  // ---------------------
  if (phi_index>=rowsPerFE) {
    FE = (int)((FEsPerHalfModule-1)-(eta_index/columnsPerFE));
  } 
  else {
    FE = (int)(eta_index/columnsPerFE)+FEsPerHalfModule;
  }

  // For IBL, the above returns FE number in range [2,3] (planar), or [1] (3D sensors).
  // Change that to be [0,1] (planar) and [0] (3D) for consistency
  if (moduleType == PixelModuleType::IBL_PLANAR || moduleType == PixelModuleType::IBL_3D) { FE = FE - FEsPerHalfModule; }

  return FE;
}


uint32_t PixelReadoutManager::getColumn(Identifier diodeId,
                                        Identifier offlineId) const
{
  const SiDetectorElement *element = m_detManager->getDetectorElement(offlineId);
  const PixelModuleDesign *p_design = static_cast<const PixelModuleDesign *>(&element->design());
  if (p_design->getReadoutTechnology() == PixelReadoutTechnology::RD53) {
    ATH_MSG_ERROR("RD53 readout technologies not supported!");
    return 0xffffffff;
  }

  unsigned int columnsPerFE     = p_design->columnsPerCircuit();
  unsigned int rowsPerFE = 0;
  int column_offset = 0;
  if (p_design->getReadoutTechnology() == PixelReadoutTechnology::FEI4) {
    rowsPerFE    = p_design->rowsPerCircuit();
    column_offset = 1;
  }
  else if (p_design->getReadoutTechnology() == PixelReadoutTechnology::FEI3) {
    rowsPerFE    = p_design->rowsPerCircuit()/2+4;  // normal + ganged
  }

  PixelModuleType moduleType = getModuleType(offlineId);

  unsigned int phi_index = m_idHelper->phi_index(diodeId);
  int eta_index = m_idHelper->eta_index(diodeId);

  // ---------------------
  // Set module properties
  // ---------------------
  switch (moduleType)
  {
    case PixelModuleType::DBM:
      break;

    case PixelModuleType::PIX_ENDCAP:
      // Swap phi_index for even endcap modules
      if ((m_idHelper->phi_module(offlineId))%2==0) {
        phi_index = 2*rowsPerFE-phi_index-1;
      }
      break;

    default:        // PIX_BARREL + IBL
      break;
  }

  // ---------------------
  // Convert eta index to column number
  // ---------------------
  int column{};
  // DBM (column <-> phi_index)
  if (moduleType == PixelModuleType::DBM) {
    if (m_idHelper->barrel_ec(offlineId)>0) {
      column = m_idHelper->phi_index(diodeId);     // A side
    }
    else {
      column = columnsPerFE-m_idHelper->phi_index(diodeId)-1;  // C side
    }
  }
  // Pixels, IBL
  else {
    if ((phi_index>=rowsPerFE)) {
      column = (columnsPerFE-1)-(eta_index%columnsPerFE);
    } 
    else {
      column = eta_index%columnsPerFE;
    }
  }

  // ---------------------
  // Check output sanity
  // ---------------------
  if (column >= (int)columnsPerFE) {
    ATH_MSG_ERROR("Computed column number exceeds maximum value: col = " << column + column_offset << " (max = " << columnsPerFE << ")");
    return 0xffffffff;
  }

  return column + column_offset;
}


uint32_t PixelReadoutManager::getRow(Identifier diodeId,
                                     Identifier offlineId) const
{
  const SiDetectorElement *element = m_detManager->getDetectorElement(offlineId);
  const PixelModuleDesign *p_design = static_cast<const PixelModuleDesign *>(&element->design());
  if (p_design->getReadoutTechnology() == PixelReadoutTechnology::RD53) {
    ATH_MSG_ERROR("RD53 readout technologies not supported!");
    return 0xffffffff;
  }

  unsigned int rowsPerFE = 0;
  int row_offset = 0;
  if (p_design->getReadoutTechnology() == PixelReadoutTechnology::FEI4) {
    rowsPerFE    = p_design->rowsPerCircuit();
    row_offset = 1;
  }
  else if (p_design->getReadoutTechnology() == PixelReadoutTechnology::FEI3) {
    rowsPerFE    = p_design->rowsPerCircuit()/2+4;  // normal + ganged
  }

  PixelModuleType moduleType = getModuleType(offlineId);

  unsigned int phi_index = m_idHelper->phi_index(diodeId);

  // ---------------------
  // Set module properties
  // ---------------------
  switch (moduleType)
  {
    case PixelModuleType::DBM:
      break;

    case PixelModuleType::PIX_ENDCAP:
      // Swap phi_index for even endcap modules
      if ((m_idHelper->phi_module(offlineId))%2==0) {
        phi_index = 2*rowsPerFE-phi_index-1;
      }
      break;

    default:        // PIX_BARREL + IBL
      break;
  }

  // ---------------------
  // Convert phi index to row number
  // ---------------------
  int row{};
  switch (moduleType)
  {
    case PixelModuleType::DBM:
      // Row <-> eta_index
      row = rowsPerFE-m_idHelper->eta_index(diodeId)-1;
      break;

    case PixelModuleType::IBL_PLANAR:
      row = rowsPerFE-1-phi_index;
      break;

    case PixelModuleType::IBL_3D:
      row = rowsPerFE-1-phi_index;
      break;

    default:    // Pixels
      if (phi_index>=rowsPerFE) {
        row = ((2*rowsPerFE)-1)-phi_index;
      } 
      else {
        row = phi_index;
      }
      break;
  }

  // ---------------------
  // Check output sanity
  // ---------------------
  if (row >= (int)rowsPerFE) {
    ATH_MSG_ERROR("Computed row number exceeds maximum value: row = " << row + row_offset << "(max = " << rowsPerFE << ")");
    return 0xffffffff;
  }
  return row + row_offset;
}

} // namespace InDetDD
