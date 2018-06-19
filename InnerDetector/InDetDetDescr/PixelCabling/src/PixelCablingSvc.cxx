/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelCablingSvc.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

// Classes
#include "Identifier/Identifier.h"
#include "PixelCablingData.h"
#include "InDetIdentifier/PixelID.h"
#include "eformat/SourceIdentifier.h"
#include "GaudiKernel/StatusCode.h"

#include "PixelFillCablingData.h"

// for callback
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

// COOL includes
#include "CoralBase/Blob.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"

//#define PIXEL_DEBUG

////////////////////////
// constructor
////////////////////////

PixelCablingSvc::PixelCablingSvc(const std::string& name, ISvcLocator*svc):
  AthService(name,svc),
  m_cablingTool("PixelFillCablingData"),
  m_detStore("DetectorStore", name),
  m_detManager(0),
  m_idHelper(0),
  m_cabling(new PixelCablingData),
  m_dataString(""),
  m_key("/PIXEL/ReadoutSpeed"),
  m_keyFEI4("/PIXEL/HitDiscCnfg"),
  m_keyCabling("/PIXEL/CablingMap"),
  m_dump_map_to_file(false)
{
  // "Final": use text file, "COOL": read from COOL
  declareProperty("MappingType", m_mappingType = "COOL");
  // Name of mapping file to use, if mappingType == Final
  declareProperty("MappingFile", m_final_mapping_file = "PixelCabling/Pixels_Atlas_IdMapping_2016.dat");
  // NOT USED
  declareProperty("Bandwidth", m_bandwidth = 0);
  // NOT USED
  declareProperty("Coral_Connectionstring", m_coraldbconnstring = "oracle://ATLAS_COOLPROD/ATLAS_COOLONL_PIXEL"); // used to configure CORAL based tool
  // NOT USED
  declareProperty("DictionaryTag", m_dictTag="PIXEL");
  // NOT USED
  declareProperty("ConnectivityTag", m_connTag="PIT-ALL-V39");
  // Folder name for FE-I4 hit discriminator configuration
  declareProperty("KeyFEI4", m_keyFEI4 = "/PIXEL/HitDiscCnfg");
  // Folder name for readout speed
  declareProperty("Key", m_key, "Key=/PIXEL/ReadoutSpeed");
  // Folder name for cabling map
  declareProperty("KeyCabling", m_keyCabling, "Key=/PIXEL/CablingMap");
  // Write out the cabling map to a text file
  declareProperty("DumpMapToFile", m_dump_map_to_file = false);
}

PixelCablingSvc::~PixelCablingSvc() { }

StatusCode PixelCablingSvc::initialize() {
  ATH_MSG_INFO("PixelCablingSvc::initialize()");

  CHECK(m_detStore.retrieve());

  CHECK(m_detStore->retrieve(m_detManager,"Pixel"));

  CHECK(m_detStore->retrieve(m_idHelper,"PixelID"));

  // Get ToolSvc
  IToolSvc* toolSvc;
  CHECK(service("ToolSvc",toolSvc));

  // Print out all values
#ifdef PIXEL_DEBUG
  ATH_MSG_DEBUG("-- PixelCablingSvc ------------------------------");
  ATH_MSG_DEBUG("m_final_mapping_file = " << m_final_mapping_file);
  ATH_MSG_DEBUG("useMapFromOptions = " << useMapFromOptions);
  ATH_MSG_DEBUG("-------------------------------------------------");
#endif

  // Get the cabling tool
  CHECK(m_cablingTool.retrieve());

  if ((m_mappingType!="COOL") && (m_mappingType!="Final")) {
    ATH_MSG_FATAL("Unknown PixelCablingSvc configuration: " << m_mappingType);
    return StatusCode::FAILURE;
  }

  if (m_mappingType!="COOL") {
    if (!m_cablingTool->fillMapFromFile(m_final_mapping_file,m_cabling.get())) {
      ATH_MSG_ERROR("Filling pixel cabling from file \"" << m_final_mapping_file << "\" failed");
      return StatusCode::FAILURE;
    }
  }

  m_context = m_idHelper->wafer_context();

  // Register readout speed callback
  const DataHandle<AthenaAttributeList> attrlist;
  if (m_detStore->contains<AthenaAttributeList>(m_key)) {
    CHECK(m_detStore->regFcn(&IPixelCablingSvc::IOVCallBack,dynamic_cast<IPixelCablingSvc*>(this),attrlist,m_key));
  }
  else {
    ATH_MSG_WARNING("Folder " << m_key << " not found, using default readoutspeed" << " values (all modules at SINGLE_40)");
  }

  // Register cabling map callback
  if (m_mappingType=="COOL") {
    const DataHandle<AthenaAttributeList> attrlist_cabling;
    if (m_detStore->contains<AthenaAttributeList>(m_keyCabling)) {
      CHECK(m_detStore->regFcn(&IPixelCablingSvc::IOVCallBack,dynamic_cast<IPixelCablingSvc*>(this),attrlist_cabling,m_keyCabling));
    }
    else {
      ATH_MSG_WARNING("Folder " << m_keyCabling << " not found, exiting");
      return StatusCode::FAILURE;
    }
  }

  // Register hitdisccnfg callback
  if (m_idHelper->wafer_hash_max()==2048) {
    const DataHandle<AthenaAttributeList> attrlist_hdc;
    if (m_detStore->contains<AthenaAttributeList>(m_keyFEI4)) {
      CHECK(m_detStore->regFcn(&IPixelCablingSvc::IOVCallBack,dynamic_cast<IPixelCablingSvc*>(this),attrlist_hdc,m_keyFEI4));
    }
    else {
      ATH_MSG_WARNING("Folder " << m_keyFEI4 << " not found, using default HitDiscCnfg" << " values (all FEs at HitDiscCnfg = 3)");
    }
  }
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

void PixelCablingSvc::getOfflineList(std::vector<IdentifierHash>& offlineIdHashList, int robid) {
  std::deque<Identifier> offlineIdList = m_cabling->find_entry_offlineList(robid);
  std::deque<Identifier>::iterator it1 = offlineIdList.begin();
  std::deque<Identifier>::iterator it2 = offlineIdList.end();
  for (; it1!=it2;++it1) {
    offlineIdHashList.push_back(getOfflineIdHashFromOfflineId(*it1));
  }
}

Identifier PixelCablingSvc::getOfflineId(uint64_t onlineId) {
  return m_cabling->find_entry_onoff(onlineId);
}

IdentifierHash PixelCablingSvc::getOfflineIdHash(uint64_t onlineId) {
  Identifier offlineId = m_cabling->find_entry_onoff(onlineId);
  return getOfflineIdHashFromOfflineId(offlineId);
}

IdentifierHash PixelCablingSvc::getOfflineIdHashFromOfflineId(Identifier offlineId) {
  if (offlineId==0) {
    return 0xffffffff; //was -1, but Identifier hash is unsigned, so I return the unsigned equivalent (2^32 -1, assuming int is 32 bit) (shaun, 15 may 2008)
  }
  return m_idHelper->wafer_hash(offlineId);   // ->InDetDetDescr/InDetIdentifier:PixelID - get hash identifier
}

Identifier PixelCablingSvc::getOfflineIdFromDCS(std::string DCSname) {
  return m_cabling->find_entry_DCSoffline(DCSname);
}

uint32_t PixelCablingSvc::getRobId(Identifier offlineId) {
  return m_cabling->find_entry_offrob(offlineId);
}

uint32_t PixelCablingSvc::getRobID(Identifier offlineId) {
  return m_cabling->find_entry_offrob(offlineId);
}

int PixelCablingSvc::getRobId(const int rodid) {
  return m_cabling->find_entry_rodrob(rodid);
}

int PixelCablingSvc::getRodId(const int robid) {
  return m_cabling->find_entry_robrod(robid);
}

uint64_t PixelCablingSvc::getOnlineId(Identifier offlineId) {
  return m_cabling->find_entry_offon(offlineId);
}

Identifier PixelCablingSvc::getPixelIdfromHash(IdentifierHash offlineIdHash, uint32_t FE, uint32_t row, uint32_t column) {
  return PixelCablingSvc::getPixelId(m_idHelper->wafer_id(offlineIdHash), FE, row, column);
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

  if (row>=rowsPerFE || column>=columnsPerFE || FE>=((thisModule==IBL || thisModule==DBM) ? FEsPerHalfModule:2*FEsPerHalfModule)) {
    ATH_MSG_DEBUG("Illegal pixel requested OfflineID: " << std::hex << offlineId << std::dec << " FE: " << FE << " row: " << row << " column: " << column);
    ATH_MSG_DEBUG("Limits are: FE < " << ((thisModule == IBL || thisModule == DBM) ? FEsPerHalfModule : 2*FEsPerHalfModule) << ", row < " << rowsPerFE << ", column < " << columnsPerFE);
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

    case IBL:
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

    case IBL:
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
  if (thisModule==IBL) FE = FE - FEsPerHalfModule;

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

    case IBL:
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

    case IBL:
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

    case IBL:
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

////////////////////////
// getFEwrtSlink
// Function to get the number of an FE-I4 within an Slink,
// i.e. pos. value in the range [0,7], corresponding to
// the 'nnn' bits of a fragment header. To get the number
// of an FE within a module, use getFE.
////////////////////////
uint32_t PixelCablingSvc::getFEwrtSlink(Identifier *pixelId) {
  unsigned int nnn = 99;
  Identifier offlineId = m_idHelper->wafer_id(*pixelId);
  uint64_t onlineId = getOnlineId(offlineId);
  uint32_t linkNum = (onlineId>>24) & 0xFFFF;
  unsigned int localFE = getFE(pixelId, offlineId);   // FE number within module, [0,1]. Increases with increasing eta_index

  if(localFE>1) {
    ATH_MSG_FATAL("Unexpected FE: "<<localFE<<"  PixelCablingSvc::getFEwrtSlink()");
    throw std::runtime_error("Unexpected FE");
  }
  else {
    nnn = (linkNum>>(localFE*8)) & 0xF;
  }

  // Check for errors
  if (nnn>7) {
    ATH_MSG_ERROR("Error in the identification of the FE-I4 w.r.t. Slink");
  }
  return nnn;
}

std::vector<uint32_t>& PixelCablingSvc::getAllRods() {
  return m_cabling->get_allRods();
}

std::vector<uint32_t>& PixelCablingSvc::getAllRobs() {
  return m_cabling->get_allRobs();
}

StatusCode PixelCablingSvc::IOVCallBack(IOVSVC_CALLBACK_ARGS_P(I, keys)) {

  for (std::list<std::string>::const_iterator key=keys.begin(); key != keys.end(); ++key) {
    ATH_MSG_INFO("IOVCALLBACK for key " << *key << " number " << I);
  }

  // Clear all the existing map content
  m_cabling->clear_all_maps();

  // First do readout speed
  const AthenaAttributeList* attrlist = 0;

  CHECK(m_detStore->retrieve(attrlist,m_key));

  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG("AthenaAttributeList for " << m_key << ":");
    attrlist->print(std::cout);
    ATH_MSG_DEBUG("");
  }

  const coral::Blob& blob=(*attrlist)["readoutspeed_per_ROD"].data<coral::Blob>();
  const char* p = static_cast<const char*>(blob.startingAddress());
  unsigned int len = blob.size();
  m_dataString.resize(len);
  for (unsigned int i = 0; i!=len; ++i) m_dataString[i] = *p++;

  int pos=0;
  while (m_dataString.find(",",pos)!=std::string::npos) {
    std::istringstream iss(m_dataString.substr(pos,m_dataString.find(",",pos)));
    uint32_t rod;
    iss >> std::hex >> rod;

    const std::string speed = m_dataString.substr(m_dataString.find(",",pos)+1,m_dataString.find("\n",pos)-m_dataString.find(",",pos)-1);
    if (m_cablingTool->rodReadoutMap.find(rod)==m_cablingTool->rodReadoutMap.end()) {
      if (speed!="SINGLE_40") {
        m_cablingTool->rodReadoutMap.insert(std::make_pair(rod,true));
      }
    }
    else {
      if (speed!="SINGLE_40") {
        m_cablingTool->rodReadoutMap[rod]=true;
      }
      else {
        m_cablingTool->rodReadoutMap[rod]=false;
      }
    }
    pos = m_dataString.find("\n",pos)+1;
  }
  ATH_MSG_INFO("DONE Callback for " << m_key);

  // Now do cabling map
  if (m_mappingType!="COOL") {
    if (!m_cablingTool->fillMapFromFile(m_final_mapping_file,m_cabling.get())) {
      ATH_MSG_ERROR("Refilling pixel cabling from file \"" << m_final_mapping_file << "\" failed");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Refilled pixel cabling from file \"" << m_final_mapping_file << "\"");
  }
  else {
    attrlist = 0;

    CHECK(m_detStore->retrieve(attrlist,m_keyCabling));

    if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG("AthenaAttributeList for " << m_keyCabling << ":");
      attrlist->print(std::cout);
      ATH_MSG_DEBUG("");
    }

    const coral::Blob& blob_cabling=(*attrlist)["CablingMapData"].data<coral::Blob>();
    const char* p_cabling = static_cast<const char*>(blob_cabling.startingAddress());

    unsigned int len_cabling = blob_cabling.size()/sizeof(char);
    ATH_MSG_DEBUG("blob_cabling.size() = " << blob_cabling.size() << ", len_cabling = " << len_cabling);

    bool stat = m_cablingTool->fillMapFromCool(p_cabling, blob_cabling.size(), m_cabling.get(), m_dump_map_to_file);

    if (!stat) {
      ATH_MSG_ERROR("Callback to CablingMap failed, map was not filled");
      return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_INFO("Refilled cabling map");
    }
  }

  // Now do HDC
  if (m_idHelper->wafer_hash_max()==2048) {
    attrlist = 0;

    CHECK(m_detStore->retrieve(attrlist,m_keyFEI4));

    if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG("AthenaAttributeList for " << m_keyFEI4 << ":");
      attrlist->print(std::cout);
      ATH_MSG_DEBUG("");
    }

    const coral::Blob& blob_hdc=(*attrlist)["HitDiscCnfgData"].data<coral::Blob>();
    const uint32_t* p_hdc = static_cast<const uint32_t*>(blob_hdc.startingAddress());

    uint32_t cooldata;
    unsigned int len_hdc = blob_hdc.size()/sizeof(uint32_t);
    ATH_MSG_DEBUG("blob_hdc.size() = " << blob_hdc.size() << ", len_hdc = " << len_hdc);


    for (unsigned int i = 0; i < len_hdc; ++i) {
      cooldata = *p_hdc++;
      ATH_MSG_DEBUG("Got hitdisccnfgData[" << i << "] = 0x" << std::hex << cooldata << std::dec);

      // The implementation below uses one common value, one common 3D value,
      // and an exception list of individual FEs, in order to save DB space.
      // Here we convert this into only one common value and an exception list, i.e.
      // if the 3D FEs have a different common value they are all added to the exception list

      // Update the most common value, identified by 0xH0000000, where H = 00HH
      if ((cooldata & 0x8FFFFFFF) == 0x0) {
        ATH_MSG_DEBUG("Setting common HitDiscCnfg value to " << ((cooldata&0x30000000) >> 28));
        m_cabling->setCommonHitDiscCngf((cooldata&0x30000000) >> 28);
      }

      // Update all 3D sensors with a common value, identified by 0xZ0000000, where Z = 10hh
      else if ((cooldata & 0x8FFFFFFF) == 0x80000000) {
        ATH_MSG_DEBUG("Setting common 3D HitDiscCnfg value to " << ((cooldata&0x30000000) >> 28));

        // Loop over list of robs, find the 3D sensors and insert them in HitDiscCnfg map
        for (auto robid : m_cabling->get_allRobs()) {
          // Skip non-IBL ROBs (DBM is not affected by the common 3D value)
          if (!isIBL(robid)) { continue; }

          for (int link = 0 ; link < 8 ; ++link) {
            if (((robid&0xF)==1 && link > 3) || (((robid&0xF)==2) && link < 4))  {
              m_cabling->add_entry_HitDiscCnfg((robid | (link >> 24)), (cooldata & 0x30000000) >> 28);
            }
          }
        }
      }

      // Update a single link, 0xHLDDRRRR
      else {
        ATH_MSG_DEBUG("Setting HitDiscCnfg value to " << ((cooldata&0x30000000) >> 28) << " for ROB 0x" << std::hex << (cooldata & 0xFFFFFF) << ", link " << std::dec << ((cooldata & 0xF000000) >> 24));
        m_cabling->add_entry_HitDiscCnfg((cooldata & 0xFFFFFFF), (cooldata & 0x30000000) >> 28);
      }
    }
    ATH_MSG_INFO("Refilled HitDiscCnfg values");
  }
  return StatusCode::SUCCESS;
}

uint64_t PixelCablingSvc::getOnlineIdFromRobId(const uint32_t robId, const uint32_t link_module) {
  return m_cabling->getOnlineIdFromRobId(robId, link_module);
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

bool PixelCablingSvc::isIBL(const uint32_t robId) {
  // IBL subdetector ID is 0x14
  if (((robId>>16) & 0xFF)==0x14) {
    return true;
  }
  else {
    return false;
  }
}

bool PixelCablingSvc::isIBL(const Identifier& id) {
  // If IBL is present, an IBL pixelId will have barrel_ec = layer_disk = 0
  if (m_idHelper->wafer_hash_max()==2048 && m_idHelper->barrel_ec(id)==0 && m_idHelper->layer_disk(id)==0) {
    return true;
  }
  else {
    return false;
  }
}

bool PixelCablingSvc::isDBM(const uint32_t robId) {
  // DBM subdetector ID is 0x15
  if (((robId>>16) & 0xFF)==0x15) {
    return true;
  }
  else {
    return false;
  }
}

bool PixelCablingSvc::isDBM(const Identifier& id) {
  return m_idHelper->is_dbm(id);
}

PixelCablingSvc::moduletype PixelCablingSvc::getModuleType(const Identifier& id) {
  moduletype isType = NONE;

  if (isIBL(id)) {
    isType = IBL;
  }
  else if (m_idHelper->is_dbm(id)) {
    isType = DBM;
  }
  else {
    if (abs(m_idHelper->barrel_ec(id))==2) {
      isType = PIX_ENDCAP;
    }
    else isType = PIX_BARREL;
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

int PixelCablingSvc::getHitDiscCnfg(const uint32_t robId, const int link) {
  return m_cabling->getHitDiscCnfg((link<<24) | robId);
}

int PixelCablingSvc::getHitDiscCnfg(Identifier* pixelId) {
  uint32_t robId = getRobId(m_idHelper->wafer_id(*pixelId));
  int link = -999;
  try {
    link = getFEwrtSlink(pixelId);
  }
  catch(std::exception& ex) {
    ATH_MSG_FATAL("Exception caught in PixelCablingSvc::getFEwrtSlink(): "<<ex.what());
  }
  return getHitDiscCnfg(robId, link);
}

int PixelCablingSvc::getIBLOverflowToT(Identifier* pixelId) {
  int HitDiscCnfg = getHitDiscCnfg(pixelId);
  if (HitDiscCnfg==0) { return 14; }
  if (HitDiscCnfg==1) { return 15; }
  if (HitDiscCnfg==2) { return 16; }
  return 16;
}

