/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondSvc/TGCTriggerDbAlg.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"

TGCTriggerDbAlg::TGCTriggerDbAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_condSvc("CondSvc", name)
{}

StatusCode TGCTriggerDbAlg::initialize(){

  ATH_MSG_INFO( "initialize " << name() );                

  ATH_CHECK(m_condSvc.retrieve());
  
  ATH_CHECK(m_readKey_bw.initialize());
  ATH_CHECK(m_readKey_eifi.initialize());
  ATH_CHECK(m_readKey_tile.initialize());

  ATH_CHECK(m_writeKey.initialize());

  if(m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode TGCTriggerDbAlg::execute()
{
  SG::WriteCondHandle<TGCTriggerData> writeHandle{m_writeKey};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
		  << ". In theory this should not be called, but may happen"
		  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  auto writeCdo = std::make_unique<TGCTriggerData>();

  // Big wheel
  SG::ReadCondHandle<CondAttrListCollection> readHandle_bw(m_readKey_bw);
  const CondAttrListCollection* readCdo_bw(*readHandle_bw);
  
  if (readCdo_bw == nullptr) {
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE; 
  } 

  ATH_MSG_INFO("Size of CondAttrListCollection" << readHandle_bw.fullKey() << " = " << readCdo_bw->size());
 
  EventIDRange rangeW_bw;
  if ( !readHandle_bw.range(rangeW_bw) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle_bw.key());
    return StatusCode::FAILURE;
  } 
  ATH_MSG_INFO("Range of input is " << rangeW_bw);

  fillReadMapBw(writeCdo.get(), readCdo_bw);

  // EIFI
  SG::ReadCondHandle<CondAttrListCollection> readHandle_eifi(m_readKey_eifi);
  const CondAttrListCollection* readCdo_eifi(*readHandle_eifi);
  
  if (readCdo_eifi == nullptr) {
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE; 
  } 

  ATH_MSG_INFO("Size of CondAttrListCollection" << readHandle_eifi.fullKey() << " = " << readCdo_eifi->size());
 
  EventIDRange rangeW_eifi;
  if ( !readHandle_eifi.range(rangeW_eifi) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle_eifi.key());
    return StatusCode::FAILURE;
  } 
  ATH_MSG_INFO("Range of input is " << rangeW_eifi);

  fillTrigBitEifi(writeCdo.get(), readCdo_eifi);
 
  // Tile
  SG::ReadCondHandle<CondAttrListCollection> readHandle_tile(m_readKey_tile);
  const CondAttrListCollection* readCdo_tile = *readHandle_tile;
  if (readCdo_tile == nullptr) {
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE; 
  } 
  ATH_MSG_INFO("Size of CondAttrListCollection" << readHandle_tile.fullKey() << " = " << readCdo_tile->size());

  EventIDRange rangeW_tile;
  if ( !readHandle_tile.range(rangeW_tile) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle_tile.key());
    return StatusCode::FAILURE;
  } 
  ATH_MSG_INFO("Range of input is " << rangeW_tile);

  fillTrigBitTile(writeCdo.get(), readCdo_tile);

  // write condition object
  EventIDRange rangeIntersection = EventIDRange::intersect(rangeW_bw,rangeW_eifi,rangeW_tile);
  if(rangeIntersection.start()>rangeIntersection.stop()) {
    ATH_MSG_ERROR("Invalid intersection range: " << rangeIntersection);
    return StatusCode::FAILURE;
  }

  if (writeHandle.record(rangeIntersection, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record TGCTriggerData " << writeHandle.key() 
		  << " with EventRange " << rangeIntersection
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }		  
  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << rangeIntersection << " into Conditions Store");

  return StatusCode::SUCCESS;
}


StatusCode TGCTriggerDbAlg::finalize(){
  
  ATH_MSG_INFO( "finalize " << name() );
  return StatusCode::SUCCESS;
}


void TGCTriggerDbAlg::fillReadMapBw(TGCTriggerData* writeCdo,
                                    const CondAttrListCollection* readCdo)
{
  const uint8_t kNMODULETYPE = 12;
  const uint8_t modulenumber[kNMODULETYPE]    = {0, 1, 2, 2, 3, 4, 5, 5, 6, 7, 8, 8};
  const std::string modulename[kNMODULETYPE]         = {"0","1","2a","2b","3","4","5a","5b","6","7","8a","8b"};
  const std::string sidename[TGCTriggerData::N_SIDE] = {"A","C"};

  bool first = true;

  for(auto& attrmap : *readCdo) {
    const coral::AttributeList& atr = attrmap.second;
    writeCdo->m_active[TGCTriggerData::CW_BW] = atr["active"].data<bool>();
    writeCdo->m_type[TGCTriggerData::CW_BW]   = atr["type"].data<std::string>();

    std::string version = *(static_cast<const std::string*>((atr["version"]).addressOfData()));
    std::string file = *(static_cast<const std::string*>((atr["file"]).addressOfData()));
    ATH_MSG_DEBUG("channel: " << attrmap.first << ", file: " << file);
    if ( first ) {  
      ATH_MSG_INFO("type: " << writeCdo->m_type[TGCTriggerData::CW_BW]
		    << " active: " << writeCdo->m_active[TGCTriggerData::CW_BW] << " version: " << version);
      first = false;
    } 
    else { 
      ATH_MSG_DEBUG("type: " << writeCdo->m_type[TGCTriggerData::CW_BW]
		    << " active: " << writeCdo->m_active[TGCTriggerData::CW_BW] << " version: " << version);
    }

    if (!writeCdo->m_active[TGCTriggerData::CW_BW]) continue;

    bool fullCW = (writeCdo->m_type[TGCTriggerData::CW_BW] == "full");

    uint8_t sideId = 0;  // if the file is not fullCW, sideId should be zero.
    if(fullCW) {
      std::string side = file.substr(strlen("RPhiCoincidenceMap."), 1);
      for(int id=0; id<TGCTriggerData::N_SIDE; id++) {
        if(sidename[id] == side) {
           sideId = id;
           break;
        }
      }
    }

    uint16_t octantId = 0;  // if the file is not fullCW, octantId should be zero.
    if(fullCW) {
      octantId = std::stoi(file.substr(strlen("RPhiCoincidenceMap.X"), 1));
    }

    uint32_t octaddr = ((sideId & TGCTriggerData::SIDE_MASK)<<TGCTriggerData::SIDE_SHIFT) +
                       ((octantId & TGCTriggerData::OCTANT_MASK)<<TGCTriggerData::OCTANT_SHIFT);

    uint16_t moduleId = 0;
    for(size_t iModule = 0; iModule < kNMODULETYPE; iModule++) {
      std::ostringstream dbname;
      dbname << "RPhiCoincidenceMap.";
      if (fullCW) dbname << sidename[sideId] << octantId << ".";
      dbname << "mod" << modulename[iModule] + "." + version +"._12.db";
      if(dbname.str() == file) {
        moduleId = iModule;
        break;
      }
    }

    std::string data = *(static_cast<const std::string*>((atr["data"]).addressOfData()));
    std::istringstream stream(data);

    char delimiter = '\n';
    std::string field, tag;
    uint32_t phimod2 = modulename[moduleId].find('b') != std::string::npos ? 1 : 0;
    uint32_t modaddr = ((modulenumber[moduleId] & TGCTriggerData::MODULE_MASK)<<TGCTriggerData::MODULE_SHIFT) +
                       ((phimod2 & TGCTriggerData::PHIMOD2_MASK)<<TGCTriggerData::PHIMOD2_SHIFT);

    while (std::getline(stream, field, delimiter)) {

      std::istringstream header(field);
      header >> tag;

      if (tag == "#") { // read header part.
        uint16_t ptLevel, roi, mod;
        int16_t lDR, hDR, lDPhi, hDPhi;
        header >> ptLevel >> roi >> mod >> lDR >> hDR >> lDPhi >> hDPhi;

        int16_t type = writeCdo->getTYPE( lDR, hDR, lDPhi, hDPhi );

        // check moduleNumber and ptLevel
        if( mod != modulenumber[moduleId] ||
          ptLevel > TGCTriggerData::N_PT_THRESH || type < 0 ) {
          ATH_MSG_WARNING("Invalid configuration of DB file! - Nothing to load this DB file");
          break;
        }

        uint32_t cwaddr = ((uint8_t(type) & TGCTriggerData::TYPE_MASK)<<TGCTriggerData::TYPE_SHIFT) + 
                          ((roi & TGCTriggerData::ROI_MASK)<<TGCTriggerData::ROI_SHIFT);

        // get window data
        std::getline(stream, field, delimiter);
        std::istringstream cont(field);

        for (uint8_t ir = 0; ir < 31; ir++) {  // ir=0, 15 and 30 point to -15, 0 and +15 of dR, respectively.
          uint32_t draddr = (ir & TGCTriggerData::DR_MASK)<<TGCTriggerData::DR_SHIFT;

          uint32_t bit;
          cont >> bit;
          if (bit == 0) continue; // none of window is opened in this dR

          for(uint8_t iphi=0; iphi<15; iphi++) {  // iphi=0, 7 and 14 point to -7, 0 and +7 of dPhi, respectively.
            if(bit>>iphi & 0x1) {
              uint32_t theaddr = octaddr + modaddr + cwaddr + draddr + iphi;
              writeCdo->m_ptmap_bw[theaddr] = (uint8_t)(ptLevel & TGCTriggerData::PT_MASK);
            }
          }
        }

      }   // end of if(tag)...
    }   // end of while(getline...)

  }   // end of for(attrmap)

  ATH_MSG_DEBUG("BW-CW LUT size: " << writeCdo->m_ptmap_bw.size());
}

void TGCTriggerDbAlg::fillTrigBitEifi(TGCTriggerData* writeCdo,
                                      const CondAttrListCollection* readCdo)
{
  const std::string sidename[TGCTriggerData::N_SIDE] = {"A","C"};

  bool first = true;

  for(auto& attrmap : *readCdo) {
    const coral::AttributeList& atr = attrmap.second;
    writeCdo->m_active[TGCTriggerData::CW_EIFI] = atr["active"].data<bool>();
    writeCdo->m_type[TGCTriggerData::CW_EIFI]   = atr["type"].data<std::string>();

    std::string version = *(static_cast<const std::string*>((atr["version"]).addressOfData()));
    std::string file = *(static_cast<const std::string*>((atr["file"]).addressOfData()));
    ATH_MSG_DEBUG("channel: " << attrmap.first << ", file: " << file);

    /// preumably, as this is in a loop, there should be some ouput for the channel as well, 
    /// although that is in a DEBUG statement, so presumably this prinout was demoted to DEBUG, 
    /// but the following lines were forgotten  
    if ( first ) { 
      ATH_MSG_INFO("type: " << writeCdo->m_type[TGCTriggerData::CW_EIFI]
                 << " active: " << writeCdo->m_active[TGCTriggerData::CW_EIFI] << " version: " << version);
      first = false;
    }
    else { 
      ATH_MSG_DEBUG("type: " << writeCdo->m_type[TGCTriggerData::CW_EIFI]
		    << " active: " << writeCdo->m_active[TGCTriggerData::CW_EIFI] << " version: " << version);
    }

    if (!writeCdo->m_active[TGCTriggerData::CW_EIFI]) continue;

    bool fullCW = (writeCdo->m_type[TGCTriggerData::CW_EIFI] == "full");

    uint8_t sideId = 0;  // if the file is not fullCW, sideId should be zero.
    if(fullCW) {
      std::string side = file.substr(strlen("InnerCoincidenceMap."), 1);
      for(int id=0; id<TGCTriggerData::N_SIDE; id++) {
        if(sidename[id] == side) {
           sideId = id;
           break;
        }
      }
    }

    std::string dbname="";
    if (!fullCW) {
      dbname = "InnerCoincidenceMap." + version + "._12.db";
    } else {
      dbname = "InnerCoincidenceMap." + sidename[sideId]
               + "." + version + "._12.db";
    }
    if(file != dbname) {
      ATH_MSG_WARNING("The file name is different. Skip to load.");
      continue;
    }

    std::istringstream stream(*(static_cast<const std::string*>((atr["data"]).addressOfData())));

    char delimiter = '\n';
    std::string field;
    std::string tag;

    while (std::getline(stream, field, delimiter)) {
      int sectorId = -1;
      int sscId    = -1;

      std::istringstream header(field);
      header >> tag;
      if(tag=="#"){ // read header part.
        header >> sectorId >> sscId;
      }

      if (sectorId < 0 || sectorId >= TGCTriggerData::N_ENDCAP_SECTOR ||
          sscId < 0    || sscId >= TGCTriggerData::N_ENDCAP_SSC ) {
        ATH_MSG_WARNING("Invalid configuration of DB file.");
        return;
      }

      uint16_t addr = ((uint16_t(sideId) & TGCTriggerData::SIDE_MASK)<<TGCTriggerData::ADDR_SIDE_SHIFT) +
                      ((uint16_t(sectorId) & TGCTriggerData::SECTOR_MASK)<<TGCTriggerData::ADDR_SECTOR_SHIFT) +
                      (uint16_t(sscId) & TGCTriggerData::SSC_MASK);

      uint8_t flag_pt = 0x0;
      for (size_t pt = 0; pt < TGCTriggerData::N_PT_THRESH; pt++){
        uint8_t use;
        header  >> use;
        flag_pt |= (use&0x1)<<pt;
      }
      writeCdo->m_flagpt_eifi[addr] = flag_pt;

      uint8_t flag_roi = 0x0;
      for (size_t pos = 0; pos < TGCTriggerData::N_ROI_IN_SSC; pos++){
        uint8_t use;
        header >> use;
        flag_roi |= (use&0x1)<<pos;
      }
      writeCdo->m_flagroi_eifi[addr] = flag_roi;

      std::getline(stream, field, delimiter);
      std::istringstream cont(field);
      for(size_t pos=0; pos < TGCTriggerData::N_EIFI_INPUT; pos++){
        unsigned int word;
        cont >> word;
        uint16_t inputaddr = (addr<<TGCTriggerData::EIFI_TRIGBIT_SHIFT) + pos;
        writeCdo->m_trigbit_eifi[inputaddr] = word;
      }
    }  // end of while(std::geline(...))

    if (!fullCW) break;

  }  // end of for(attrmap)

  ATH_MSG_DEBUG("EIFI-CW LUT size: " << writeCdo->m_flagpt_eifi.size() + writeCdo->m_flagroi_eifi.size() + writeCdo->m_trigbit_eifi.size());
}

void TGCTriggerDbAlg::fillTrigBitTile(TGCTriggerData* writeCdo,
                                      const CondAttrListCollection* readCdo)
{

  bool first = true;

  for(auto& attrmap : *readCdo) {
    const coral::AttributeList& atr = attrmap.second;
    writeCdo->m_active[TGCTriggerData::CW_TILE] = atr["active"].data<bool>();
    writeCdo->m_type[TGCTriggerData::CW_TILE]   = atr["type"].data<std::string>();

    ATH_MSG_DEBUG("channel: " << attrmap.first << ", file: " << atr["file"].data<std::string>());

    if ( first ) { 
      ATH_MSG_INFO("type: " << writeCdo->m_type[TGCTriggerData::CW_TILE]
		   << " active: " << writeCdo->m_active[TGCTriggerData::CW_TILE]
		   << " version: " << atr["version"].data<std::string>());
      first = false;
    }
    else { 
      ATH_MSG_DEBUG("type: " << writeCdo->m_type[TGCTriggerData::CW_TILE]
		    << " active: " << writeCdo->m_active[TGCTriggerData::CW_TILE]
		    << " version: " << atr["version"].data<std::string>());
    }

    if(!writeCdo->m_active[TGCTriggerData::CW_TILE]) continue;

    std::istringstream stream(*(static_cast<const std::string*>((atr["data"]).addressOfData())));

    char delimiter = '\n';
    std::string field;

    while (std::getline(stream, field, delimiter)) {

      int16_t sideId   = -1;
      int16_t sectorId = -1;
      int16_t sscId    = -1;

      std::istringstream header(field);
      std::string tag;
      header >> tag;
      if (tag=="#"){ // read header part.
        header >> sideId >> sectorId >> sscId;
      }

      if(sideId < 0   || sideId >= TGCTriggerData::N_SIDE ||
         sectorId < 0 || sectorId >= TGCTriggerData::N_ENDCAP_SECTOR ||
         sscId < 0    || sscId >= TGCTriggerData::N_ENDCAP_SSC ) {
        ATH_MSG_WARNING("Invalid configuration of DB file.");
        return;
      }
      uint16_t addr = ((uint16_t(sideId) & TGCTriggerData::SIDE_MASK)<<TGCTriggerData::ADDR_SIDE_SHIFT) +
                      ((uint16_t(sectorId) & TGCTriggerData::SECTOR_MASK)<<TGCTriggerData::ADDR_SECTOR_SHIFT) +
                      (uint16_t(sscId) & TGCTriggerData::SSC_MASK);

      uint8_t flagpt = 0;
      for (size_t pt = 0; pt < TGCTriggerData::N_PT_THRESH; pt++){
        uint8_t use;
        header >> use;
        flagpt |= (use&0x1)<<pt;
      }
      writeCdo->m_flagpt_tile[addr] = flagpt;

      uint8_t roi = 0;
      for (size_t pos=0; pos< TGCTriggerData::N_ROI_IN_SSC; pos++){
        uint8_t use;
        header >> use;
        roi |= (use&0x1)<<pos;
      }
      writeCdo->m_flagroi_tile[addr] = roi;

      std::getline(stream, field, delimiter);
      std::istringstream cont(field);
      uint16_t trigbit = 0x0;
      for(size_t pos=0; pos < TGCTriggerData::N_TILE_INPUT; pos++){
        uint16_t word;
        cont >> word;
        trigbit |= (word & 0xf)<<(pos*4);
     }
     writeCdo->m_trigbit_tile[addr] = trigbit;
    }
  }

  ATH_MSG_DEBUG("Tile-CW LUT size: " << writeCdo->m_flagpt_tile.size() + writeCdo->m_flagroi_tile.size() + writeCdo->m_trigbit_tile.size());
}
