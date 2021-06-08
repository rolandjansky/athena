/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelCablingSvc.h"

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "PixelReadoutGeometry/PixelDetectorManager.h"

//#define PIXEL_DEBUG

PixelCablingSvc::PixelCablingSvc(const std::string& name, ISvcLocator*svc):
  AthService(name,svc),
  m_detStore("DetectorStore", name),
  m_detManager(nullptr),
  m_idHelper(nullptr)
{
}

PixelCablingSvc::~PixelCablingSvc() { }

StatusCode PixelCablingSvc::initialize() {
  ATH_MSG_INFO("PixelCablingSvc::initialize()");

  ATH_CHECK(m_detStore.retrieve());

  ATH_CHECK(m_detStore->retrieve(m_detManager,m_detManagerName));

  ATH_CHECK(m_detStore->retrieve(m_idHelper,"PixelID"));

  return StatusCode::SUCCESS;
}

StatusCode PixelCablingSvc::finalize() {
  ATH_MSG_INFO("PixelCablingSvc::finalize()");
  return StatusCode::SUCCESS;
}

StatusCode PixelCablingSvc::queryInterface(const InterfaceID &riid, void** ppvInterface) {
  if (IPixelCablingSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<IPixelCablingSvc*>(this);
  }
  else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

Identifier PixelCablingSvc::getPixelIdfromHash(IdentifierHash offlineIdHash, uint32_t FE, uint32_t row, uint32_t column) {
  return getPixelId(m_idHelper->wafer_id(offlineIdHash), FE, row, column);
}

Identifier PixelCablingSvc::getPixelId(Identifier offlineId, uint32_t FE, uint32_t row, uint32_t column) {

  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(offlineId);
  const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&element->design());

  unsigned int columnsPerFE     = p_design->columnsPerCircuit();
  unsigned int FEsPerHalfModule = p_design->numberOfCircuits();
  unsigned int rowsPerFE = 0;
  int column_row_offset = 0;
  if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4) {
    rowsPerFE    = p_design->rowsPerCircuit();
    column_row_offset = -1;
  }
  else if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI3) {
    rowsPerFE    = p_design->rowsPerCircuit()/2+4;  // normal + ganged
  }

  // ---------------------
  // Check input sanity
  // ---------------------
  // Identify the module type
  moduletype thisModule = getModuleType(offlineId);

  // Correct row, column
  row = row + column_row_offset;
  column = column + column_row_offset;

  if (row>=rowsPerFE || column>=columnsPerFE || FE>=((thisModule==IBL_PLANAR || thisModule==IBL_3D || thisModule==DBM) ? FEsPerHalfModule:2*FEsPerHalfModule)) {
    ATH_MSG_DEBUG("Illegal pixel requested OfflineID: " << std::hex << offlineId << std::dec << " FE: " << FE << " row: " << row << " column: " << column);
    ATH_MSG_DEBUG("Limits are: FE < " << ((thisModule==IBL_PLANAR || thisModule==IBL_3D || thisModule == DBM) ? FEsPerHalfModule : 2*FEsPerHalfModule) << ", row < " << rowsPerFE << ", column < " << columnsPerFE);
    return Identifier(); // illegal Identifier, standardized for PixelRodDecoder
  }

  // ---------------------
  // Convert row/column to eta/phi indices
  // ---------------------
  unsigned int phi_index, eta_index;
  switch (thisModule) {
    case DBM:
      eta_index = rowsPerFE-1-row;
      if (m_idHelper->barrel_ec(offlineId)>0) {     // A side (pos. eta)
        phi_index = column;
      } 
      else {                                        // C side
        phi_index = columnsPerFE-1-column;
      }
      break;

    case IBL_PLANAR:
      phi_index = rowsPerFE-1-row;
      eta_index = FE*columnsPerFE+column;
      break;

    case IBL_3D:
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
      if (thisModule==PIX_ENDCAP) {
        // Swap phi_index for even endcap modules
        if ((m_idHelper->phi_module(offlineId))%2==0) {
          phi_index = 2*rowsPerFE-phi_index-1;
          ATH_MSG_DEBUG("Even disk module found, phi module: " << m_idHelper->phi_module(offlineId) << " swapped phi index to : " << phi_index);
        }
      }
      break;
  }

  Identifier pixelId = m_idHelper->pixel_id(offlineId,phi_index,eta_index);

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
  uint32_t check_FE = getFE(&pixelId,offlineId);
  uint32_t check_row = getRow(&pixelId,offlineId) + column_row_offset;
  uint32_t check_column = getColumn(&pixelId,offlineId) + column_row_offset;
  if (check_FE!=FE || check_row!=row || check_column!=column) {
    ATH_MSG_WARNING("identify OfflineID: 0x" << std::hex << offlineId << std::dec << " FE: " << FE << " row: " << row << " column: " << column << " unequal to:");
    ATH_MSG_WARNING("identify PixelID: 0x" << std::hex << pixelId << std::dec << " FE: " << check_FE << " row: " << check_row << " column: " << check_column);
  }
#endif

  return pixelId;
}

uint32_t PixelCablingSvc::getFE(Identifier *pixelId, Identifier offlineId) {

  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(offlineId);
  const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&element->design());

  unsigned int columnsPerFE     = p_design->columnsPerCircuit();
  unsigned int FEsPerHalfModule = p_design->numberOfCircuits();
  unsigned int rowsPerFE = 0;
  if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4) {
    rowsPerFE    = p_design->rowsPerCircuit();
  }
  else if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI3) {
    rowsPerFE    = p_design->rowsPerCircuit()/2+4;  // normal + ganged
  }

  // ---------------------
  // Set module properties
  // ---------------------
  moduletype thisModule = getModuleType(offlineId);
  unsigned int FE;
  unsigned int phi_index = m_idHelper->phi_index(*pixelId);
  int eta_index = m_idHelper->eta_index(*pixelId);

  switch (thisModule) {

    case DBM:
      FE = 0;     // simple as that
      return FE;
      break;

    case PIX_ENDCAP:
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
  if (thisModule==IBL_PLANAR || thisModule==IBL_3D) { FE = FE - FEsPerHalfModule; }

  return FE;
}

uint32_t PixelCablingSvc::getColumn(Identifier *pixelId, Identifier offlineId) {

  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(offlineId);
  const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&element->design());

  unsigned int columnsPerFE     = p_design->columnsPerCircuit();
  unsigned int rowsPerFE = 0;
  int column_offset = 0;
  if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4) {
    rowsPerFE    = p_design->rowsPerCircuit();
    column_offset = 1;
  }
  else if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI3) {
    rowsPerFE    = p_design->rowsPerCircuit()/2+4;  // normal + ganged
  }

  moduletype thisModule = getModuleType(offlineId);

  unsigned int phi_index = m_idHelper->phi_index(*pixelId);
  int eta_index = m_idHelper->eta_index(*pixelId);

  // ---------------------
  // Set module properties
  // ---------------------
  switch (thisModule) {

    case DBM:
      break;

    case PIX_ENDCAP:
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
  int column;

  // DBM (column <-> phi_index)
  if (thisModule==DBM) {
    if (m_idHelper->barrel_ec(offlineId)>0) {
      column = m_idHelper->phi_index(*pixelId);     // A side
    }
    else {
      column = columnsPerFE-m_idHelper->phi_index(*pixelId)-1;  // C side
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
  if (column>=(int)columnsPerFE) {
    ATH_MSG_ERROR("Computed column number exceeds maximum value: col = " << column + column_offset << " (max = " << columnsPerFE << ")");
    return 0xffffffff;
  }

  return column + column_offset;
}

uint32_t PixelCablingSvc::getRow(Identifier *pixelId, Identifier offlineId) {

  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(offlineId);
  const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&element->design());

  unsigned int rowsPerFE = 0;
  int row_offset = 0;
  if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4) {
    rowsPerFE    = p_design->rowsPerCircuit();
    row_offset = 1;
  }
  else if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI3) {
    rowsPerFE    = p_design->rowsPerCircuit()/2+4;  // normal + ganged
  }

  moduletype thisModule = getModuleType(offlineId);

  unsigned int phi_index = m_idHelper->phi_index(*pixelId);

  // ---------------------
  // Set module properties
  // ---------------------
  switch (thisModule) {

    case DBM:
      break;

    case PIX_ENDCAP:
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
  int row;

  switch (thisModule) {

    case DBM:
      // Row <-> eta_index
      row = rowsPerFE-m_idHelper->eta_index(*pixelId)-1;
      break;

    case IBL_PLANAR:
      row = rowsPerFE-1-phi_index;
      break;

    case IBL_3D:
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

unsigned int PixelCablingSvc::getLocalFEI4(const uint32_t fe, const uint64_t onlineId) {
  unsigned int linknum40 = (onlineId>>24) & 0xFF;
  unsigned int linknum80 = (onlineId>>32) & 0xFF;

  if (fe==linknum40) { 
    return 0;
  }
  else if (fe==linknum80) {
    return 1;
  }
  else {
    ATH_MSG_ERROR("Error in retrieving local FE-I4 number: linknumber " << fe << " not found in onlineID " << std::hex << onlineId);
  }
  return 0xF;
}

PixelCablingSvc::moduletype PixelCablingSvc::getModuleType(const Identifier& id) {
  moduletype isType = NONE;

  const Identifier wafer_id = m_idHelper->wafer_id(id);
  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&element->design());

  if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4) {
    if (p_design->numberOfCircuits()==2) { isType = IBL_PLANAR; }
    else                                 { isType = IBL_3D; }

    if (m_idHelper->is_dbm(id)) { isType = DBM; }
  }
  else if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI3) {
    if      (abs(m_idHelper->barrel_ec(id))==0) { isType = PIX_BARREL; }
    else if (abs(m_idHelper->barrel_ec(id))==2) { isType = PIX_ENDCAP; }
    else                                        { isType = NONE; }
  }
  else {
    isType = NONE;
  }
  return isType;
}

PixelCablingSvc::pixeltype PixelCablingSvc::getPixelType(const Identifier& id) {
  pixeltype isType = NORMAL;

  const Identifier wafer_id = m_idHelper->wafer_id(id);
  Identifier pixelId = id;
  Identifier offlineId = m_idHelper->wafer_id(pixelId);
  int col = getColumn(&pixelId, offlineId);
  int row = getRow(&pixelId, offlineId);

  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&element->design());

  if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4) {
    if (p_design->numberOfCircuits()==2) {       // IBL planar
      if (col==0 || col==p_design->columnsPerCircuit()-1) {   // column edge =0,79
        isType = LONG;
      }
      else {
        isType = NORMAL;
      }
    }
    else {
      isType = NORMAL;
    }
  }
  else {
    if (col>0 && col<p_design->columnsPerCircuit()-1) {
      if (row>=p_design->rowsPerCircuit()/2-1-6-1 && row<=p_design->rowsPerCircuit()/2-1) {
        if ((row-(p_design->rowsPerCircuit()/2-1-6)+1)%2+1==1) {
          isType = LONG;
        }
        else if ((row-(p_design->rowsPerCircuit()/2-1-6)+1)%2+1==2) {
          isType = GANGED;
        }
        else {
          isType = NORMAL;
        }
      }
      else {
        isType = NORMAL;
      }
    } 
    else if (col==0 || col==p_design->columnsPerCircuit()-1) {
      if (row>=p_design->rowsPerCircuit()/2-1-6-1) {
        isType = GANGED;
      }
      else {
        isType = LONG;
      }
    }
    else {
      ATH_MSG_WARNING("Pixel Type : the col number should be 0-" << p_design->columnsPerCircuit() << ", not " <<col);
      isType = NORMAL;
    }
  }
  return isType;
}

