/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelCablingCondAlg.h"
#include "GaudiKernel/EventIDRange.h"
#include "AthenaKernel/IOVInfiniteRange.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "PathResolver/PathResolver.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdContext.h"
#include "CoralBase/Blob.h"


PixelCablingCondAlg::PixelCablingCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode PixelCablingCondAlg::initialize() {
  ATH_MSG_DEBUG("PixelCablingCondAlg::initialize()");

  ATH_CHECK(detStore()->retrieve(m_pixelID,"PixelID"));

  ATH_CHECK(m_condSvc.retrieve());
  ATH_CHECK(m_moduleDataKey.initialize());
  ATH_CHECK(m_readoutspeedKey.initialize());
  ATH_CHECK(m_readKey.initialize(!m_readKey.empty()));

  ATH_CHECK(m_writeKey.initialize());
  if (m_condSvc->regHandle(this,m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  if (!m_recordInInitialize.value()) {
    ATH_MSG_WARNING("Special treatment disabled.");
    return StatusCode::SUCCESS;
  }

  ATH_MSG_WARNING("Special treatment: Once RegionSelectorTable is fixed, these lines should be removed.");

  SG::WriteCondHandle<PixelCablingCondData> writeHandle(m_writeKey);
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }
  std::unique_ptr<PixelCablingCondData> writeCdo(std::make_unique<PixelCablingCondData>());
  const EventIDBase::number_type UNDEFNUM = EventIDBase::UNDEFNUM;
  const EventIDBase::event_number_t UNDEFEVT = EventIDBase::UNDEFEVT;

  /// FIXME: this is a hack to get the code to work - the comfiguration should be 
  ///        such that the cabling is now only configured with a proper IoV and not 
  ///        in the initialise method
  EventIDRange rangeW (EventIDBase (0, UNDEFEVT, UNDEFNUM, 0, 0),
                       EventIDBase (9999999, UNDEFEVT, UNDEFNUM, 0, 0));

  // Signed values
  int barrel_ec, eta_module;

  // Unsigned 32 bit values
  uint32_t layer_disk, phi_module;
  uint32_t robid, rodid;
  uint32_t sl_40_fmt, sl_40_link, sl_80_fmt, sl_80_link;

  // Unsigned 64-bit values
  uint64_t onlineId = 0;
  uint64_t linknumber = 0;

  // Strings
  std::string DCSname;
  std::string line;

  std::string filename = PathResolverFindCalibFile(m_final_mapping_file);
  if (filename.size()==0) {
    ATH_MSG_FATAL("Mapping File: " << m_final_mapping_file << " not found!");
    return StatusCode::FAILURE;
  }
  std::ifstream fin(filename.c_str());
  if (!fin) { return StatusCode::FAILURE; }
  std::stringstream instr;
  instr << fin.rdbuf();

  while (instr.good() && getline(instr, line)) {
    if (line.size()==0) { continue; }
    if (line.substr(0,1)==" " || line.substr(0,1)=="#") { continue; }
    if (line.length()<21) { continue; }
    if (line.substr(line.length()-3,line.length())=="GMT") { continue; }
    std::istringstream parse(line);
    parse >> barrel_ec >> layer_disk >> phi_module >> eta_module >> std::hex >> robid >> rodid >> sl_40_fmt >> sl_40_link >> sl_80_fmt >> sl_80_link >> DCSname;
    Identifier offlineId = m_pixelID->wafer_id(barrel_ec,layer_disk,phi_module,eta_module);
    if ((robid & 0xFFFFFF)>=0x140000) {
      linknumber = sl_40_link | (sl_40_fmt<<4) | (sl_80_link<<8) | (sl_80_fmt<<12);
    }
    else {
      // Valid for data
      bool readoutSpeed = true;
      if (rodid>m_rodidForSingleLink40) { readoutSpeed=false; }
      if (readoutSpeed==false) { linknumber=(sl_40_link & 0xF) | ((sl_40_fmt & 0xF)<<4); }
      else                     { linknumber=(sl_80_link & 0xF) | ((sl_80_fmt & 0xF)<<4); }
    }
    onlineId = (robid & 0xFFFFFF) | (linknumber<<24);
    IdentifierHash hashId;
    IdContext cntxpixel = m_pixelID->wafer_context();
    if (m_pixelID->get_hash(offlineId, hashId, &cntxpixel)) {
      ATH_MSG_WARNING("Could not get hash from offlineId");
    }
    writeCdo->add_entry_onoff(onlineId, offlineId);
    writeCdo->add_entry_offon(offlineId, onlineId);
    writeCdo->add_entry_offlineList(robid,offlineId);
    writeCdo->add_entry_offrob(offlineId, robid);
    writeCdo->add_entry_rodrob(rodid, robid);
    writeCdo->add_entry_robrod(robid, rodid);
    writeCdo->add_entry_DCSoffline(DCSname, offlineId);
  }
  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixelCablingCondData " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_WARNING("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");
  ATH_MSG_WARNING("Tempolary fix!!: Refilled pixel cabling from file \"" << m_final_mapping_file << "\"");

  return StatusCode::SUCCESS;
}

StatusCode PixelCablingCondAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("PixelCablingCondAlg::execute()");

  SG::WriteCondHandle<PixelCablingCondData> writeHandle(m_writeKey, ctx);
  ATH_MSG_DEBUG("Conditions updates every event!!! This should be avoided once RegionSelectorTable is fixed!!");
//   if (writeHandle.isValid()) {
//     ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
//     return StatusCode::SUCCESS; 
//   }

  // Construct the output Cond Object and fill it in
  std::unique_ptr<PixelCablingCondData> writeCdo(std::make_unique<PixelCablingCondData>());

  // Signed values
  int barrel_ec, eta_module;

  // Unsigned 32 bit values
  uint32_t layer_disk, phi_module;
  uint32_t robid, rodid;
  uint32_t sl_40_fmt, sl_40_link, sl_80_fmt, sl_80_link;

  // Unsigned 64-bit values
  uint64_t onlineId = 0;
  uint64_t linknumber = 0;

  // Strings
  std::string DCSname;
  std::string line;

  SG::ReadCondHandle<PixelModuleData>moduleData(m_moduleDataKey, ctx);

  // For debugging purposes
  std::ofstream output_mapping_file_raw;
  if (moduleData->getCablingMapToFile()) { output_mapping_file_raw.open("pixel_cabling_map_raw.txt"); }
  std::ofstream output_mapping_file_interpreted;
  if (moduleData->getCablingMapToFile()) { output_mapping_file_interpreted.open("pixel_cabling_map_interpreted.txt"); }

  std::stringstream instr;
  if (moduleData->getUseCablingConditions()) {
    SG::ReadCondHandle<AthenaAttributeList> readHandle(m_readKey, ctx);
    const AthenaAttributeList* readCdo = *readHandle; 
    if (readCdo==nullptr) {
      ATH_MSG_FATAL("Null pointer to the read conditions object");
      return StatusCode::FAILURE;
    }
    writeHandle.addDependency(readHandle);

    ATH_MSG_DEBUG("Size of AthenaAttributeList " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_DEBUG("Range of input is " << readHandle.getRange());

    const coral::Blob& blob_cabling=(*readCdo)["CablingMapData"].data<coral::Blob>();
    const char* p_cabling = static_cast<const char*>(blob_cabling.startingAddress());

    unsigned int len_cabling = blob_cabling.size()/sizeof(char);
    ATH_MSG_DEBUG("blob_cabling.size() = " << blob_cabling.size() << ", len_cabling = " << len_cabling);

    instr.str(std::string(p_cabling,blob_cabling.size())); 
  }
  else {
    const std::string filename = PathResolverFindCalibFile(moduleData->getCablingMapFileName());
    if (filename.size()==0) {
      ATH_MSG_FATAL("Mapping File: " << moduleData->getCablingMapFileName() << " not found!");
      return StatusCode::FAILURE;
    }
    std::ifstream fin(filename.c_str());
    if (!fin) { return StatusCode::FAILURE; }
    instr << fin.rdbuf();

    writeHandle.addDependency(IOVInfiniteRange::infiniteRunLB()); //When reading from file, use infinite IOV
    ATH_MSG_DEBUG("Refilled pixel cabling from file \"" << moduleData->getCablingMapFileName() << "\"");
  }

  // Each entry in the mapping is sepated by a newline.
  // Loop over all lines and parse the values
  std::map<uint32_t,bool> rodReadoutMap = SG::ReadCondHandle<PixelReadoutSpeedData>(m_readoutspeedKey, ctx)->getReadoutMap();

  while (instr.good() && getline(instr, line)) {

    if (moduleData->getCablingMapToFile()) { output_mapping_file_raw << line << std::endl; }

    // Skip empty lines and comments (i.e. starting with a hash or a space)
    if (line.empty()) { continue; }
    if (line[0]==' ' || line[0]=='#') { continue; }

    // There are 11 columns, so there must be at least 21 characters in a valid line.
    if (line.length()<21) { continue; }

    // If reading from COOL, skip the datestamp
    if (line.substr(line.length()-3,line.length())=="GMT") { continue; }

    std::istringstream parse(line);
    parse >> barrel_ec >> layer_disk >> phi_module >> eta_module >> std::hex >> robid >> rodid >> sl_40_fmt >> sl_40_link >> sl_80_fmt >> sl_80_link >> DCSname;

    // Debug
    if (moduleData->getCablingMapToFile()) {
      output_mapping_file_interpreted << barrel_ec << "\t" << layer_disk << "\t" << phi_module << "\t"
                                      << eta_module << "\t" << std::hex << robid << "\t" << rodid << "\t"
                                      << sl_40_fmt << "\t" << sl_40_link << "\t" << sl_80_fmt << "\t"
                                      << sl_80_link << "\t" << DCSname << std::dec << std::endl;
    }

    // Get the offline ID for this module
    Identifier offlineId = m_pixelID->wafer_id(barrel_ec,layer_disk,phi_module,eta_module);

    // Set linknumber for IBL / DBM entries
    if ((robid & 0xFFFFFF)>=0x140000) {
      linknumber = sl_40_link | (sl_40_fmt<<4) | (sl_80_link<<8) | (sl_80_fmt<<12);
    }
    // Set linknumber for pixel entries
    else {
      bool readoutSpeed = false;
      if (rodReadoutMap.find(rodid)!=rodReadoutMap.end()) { readoutSpeed=rodReadoutMap[rodid]; }

      if (readoutSpeed==false) { linknumber=(sl_40_link & 0xF) | ((sl_40_fmt & 0xF)<<4); }
      else                     { linknumber=(sl_80_link & 0xF) | ((sl_80_fmt & 0xF)<<4); }
    }

    // Compute onlineId
    onlineId = (robid & 0xFFFFFF) | (linknumber<<24);

    IdentifierHash hashId;

    // Do checks to verify consistency
    IdContext cntxpixel = m_pixelID->wafer_context();
    if (m_pixelID->get_hash(offlineId, hashId, &cntxpixel)) {
      ATH_MSG_WARNING("Could not get hash from offlineId");
    }

    if (hashId>m_pixelID->wafer_hash_max()) {
      ATH_MSG_ERROR("IdHash overflow! HashId is 0x" << std::hex << hashId);
      ATH_MSG_ERROR("not mapped OfflineID: " << std::hex << offlineId << std::dec << " barrel_ec: " << barrel_ec
                     << " layer_disk: " << layer_disk << " phi_module: " << phi_module << " eta_module: " << eta_module);
      ATH_MSG_ERROR("to OnlineID: 0x" << std::hex << onlineId << " robid: 0x" << robid << " rodid: 0x" << rodid << std::dec
                     << " link: 0x" << std::hex /*<< link*/ << " -> Linknumber: 0x" << linknumber << " HashId: 0x" << hashId << std::dec);

      // Check if offlineId fail was caused by exceeding eta_module range
      if (eta_module>m_pixelID->eta_module_max(offlineId) || eta_module<m_pixelID->eta_module_min(offlineId)) {
        // eta_module_max == -999 indicates the module does not exist
        if (m_pixelID->eta_module_max(offlineId)==-999 && m_pixelID->eta_module_min(offlineId)==-999) {
          ATH_MSG_ERROR("Module does not exist in geometry");
        }
        else {
          ATH_MSG_ERROR("eta_module range exceeded: Got eta_module = " << eta_module
                        << ", but allowed range is [" << m_pixelID->eta_module_min(offlineId)
                        << "," << m_pixelID->eta_module_max(offlineId) << "]");
          ATH_MSG_ERROR("Input geometry tag may not be compatible with mapping file");
        }
      }
    }

    // Fill the maps
    writeCdo->add_entry_onoff(onlineId, offlineId);
    writeCdo->add_entry_offon(offlineId, onlineId);
    writeCdo->add_entry_offlineList(robid,offlineId);
    writeCdo->add_entry_offrob(offlineId, robid);
    writeCdo->add_entry_rodrob(rodid, robid);
    writeCdo->add_entry_robrod(robid, rodid);
    writeCdo->add_entry_DCSoffline(DCSname, offlineId);

    // Debug messages
    ATH_MSG_DEBUG("Mapped offlineID: " << std::hex << offlineId << " to onlineID: 0x" << onlineId
        << ", robID: 0x" << robid << ", barrel_ec: " << std::dec << barrel_ec << ", layer_disk: " << layer_disk
        << ", eta_module: " << eta_module << ", phi_module: " << phi_module << ", linknumber: 0x" << std::hex << linknumber);
  }

  if (moduleData->getCablingMapToFile()) {
    output_mapping_file_raw.close();
    output_mapping_file_interpreted.close();
  }

  ATH_MSG_DEBUG("Size of ROD readoutspeed map: " << rodReadoutMap.size());
  writeCdo->set_readout_map(rodReadoutMap);

  if (writeHandle.record(std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixelCablingCondData " << writeHandle.key() << " with EventRange " << writeHandle.getRange() << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("recorded new CDO " << writeHandle.key() << " with range " << writeHandle.getRange() << " into Conditions Store");

  return StatusCode::SUCCESS;
}

