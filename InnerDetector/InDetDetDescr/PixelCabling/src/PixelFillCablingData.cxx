/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelFillCablingData.h
//   Header file for class PixelFillCablingData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Pixel Detector software
///////////////////////////////////////////////////////////////////
// A replacement for the classes PixelFillCablingData_Final and
// PixelFillCablingData_CoralDB.
// This class reads the mapping info either from Cool (for Run2)
// or from a text file (MC and Run1)
///////////////////////////////////////////////////////////////////


#include "PixelFillCablingData.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "PathResolver/PathResolver.h"
#include "Identifier/Identifier.h"
#include "eformat/SourceIdentifier.h"

// Classes
#include "PixelCablingData.h"
#include "InDetIdentifier/PixelID.h"


using eformat::helper::SourceIdentifier;
static const InterfaceID IID_IPixelFillCablingData("PixelFillCablingData", 1, 0);


////////////////////////
// constructor
////////////////////////
PixelFillCablingData::PixelFillCablingData( const std::string& type, const std::string& name,const IInterface* parent)
    : AthAlgTool(type, name, parent), m_idHelper(0)
{
  declareInterface< PixelFillCablingData >( this );
}

////////////////////////
// destructor
////////////////////////
PixelFillCablingData::~PixelFillCablingData()
{ }

////////////////////////
// interfaceID
////////////////////////
const InterfaceID& PixelFillCablingData::interfaceID()
{
  return IID_IPixelFillCablingData;
}


////////////////////////
// initialize
////////////////////////
StatusCode PixelFillCablingData::initialize()
{
  StatusCode sc;
  msg(MSG::DEBUG) << "PixelFillCablingData::initialize" <<endmsg;

  // Get the PixelID Helper
  if (detStore()->retrieve(m_idHelper, "PixelID").isFailure()) {
    ATH_MSG_FATAL("Could not get Pixel ID helper");
    return StatusCode::FAILURE;
  }
  m_cntxpixel = m_idHelper->wafer_context();


  return sc;
}



////////////////////////
// fill from text file
////////////////////////
bool PixelFillCablingData::fillMapFromFile(const std::string infilename, PixelCablingData* cabling)
{
    std::string filename = PathResolverFindCalibFile(infilename);
    if (filename.size() == 0) {
      ATH_MSG_FATAL("Mapping File: " << infilename << " not found!");
      return false;
    }

    std::ifstream fin(filename.c_str());
    if (!fin) return false;

    return parseAndFill(fin,cabling);
}



////////////////////////
// fill from COOL
////////////////////////
bool PixelFillCablingData::fillMapFromCool(const char* data, unsigned int size, PixelCablingData* cabling, bool dump_map_to_file) //FIXME: use std::string
{
    if (!data) return false;
    std::stringstream instr;

    //The detour via std::string make the code resiliant against missing c-string termination 
    instr.str(std::string(data,size)); 

    return parseAndFill(instr, cabling, dump_map_to_file);
}



////////////////////////
// fill map from stream
////////////////////////
bool PixelFillCablingData::parseAndFill(std::istream &instr, PixelCablingData* cabling, bool dump_map_to_file)
{

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

    // For debugging purposes
    std::ofstream output_mapping_file_raw;
    if (dump_map_to_file) output_mapping_file_raw.open("pixel_cabling_map_raw.txt");
    std::ofstream output_mapping_file_interpreted;
    if (dump_map_to_file) output_mapping_file_interpreted.open("pixel_cabling_map_interpreted.txt");

    // Each entry in the mapping is sepated by a newline.
    // Loop over all lines and parse the values

    while (instr.good() && getline(instr, line)) {

        if (dump_map_to_file) output_mapping_file_raw << line << std::endl;

        // Skip empty lines and comments (i.e. starting with a hash or a space)
        if (line.size() == 0) continue;
        if (line.substr(0,1) == " " || line.substr(0,1) == "#") continue;

        // There are 11 columns, so there must be at least 21 characters in a valid line.
        if (line.length() < 21) continue;

        // If reading from COOL, skip the datestamp
        if (line.substr(line.length()-3, line.length()) == "GMT") continue;

        std::istringstream parse(line);
        parse >> barrel_ec >> layer_disk >> phi_module >> eta_module >> std::hex
              >> robid >> rodid >> sl_40_fmt >> sl_40_link >> sl_80_fmt
              >> sl_80_link >> DCSname;

        // Debug
        if (dump_map_to_file) {
            output_mapping_file_interpreted << barrel_ec << "\t" << layer_disk << "\t" << phi_module << "\t"
                                            << eta_module << "\t" << std::hex << robid << "\t" << rodid << "\t"
                                            << sl_40_fmt << "\t" << sl_40_link << "\t" << sl_80_fmt << "\t"
                                            << sl_80_link << "\t" << DCSname << std::dec << std::endl;
        }


        // Get the offline ID for this module
        Identifier offlineId = m_idHelper->wafer_id(barrel_ec, layer_disk, phi_module, eta_module);

        // Set linknumber for IBL / DBM entries
        if ((robid & 0xFFFFFF) >= 0x140000) {
            linknumber = sl_40_link | (sl_40_fmt << 4) | (sl_80_link << 8) | (sl_80_fmt << 12);
        }
        // Set linknumber for pixel entries
        else {
            bool readoutSpeed = false;
            if (rodReadoutMap.find(rodid) != rodReadoutMap.end())
                readoutSpeed = rodReadoutMap[rodid];

            if (readoutSpeed == false) linknumber = (sl_40_link & 0xF) | ((sl_40_fmt & 0xF) << 4);
            else linknumber = (sl_80_link & 0xF) | ((sl_80_fmt & 0xF) << 4);
        }


        // Compute onlineId
        onlineId = (robid & 0xFFFFFF) | (linknumber << 24);

        IdentifierHash hashId;

        // Do checks to verify consistency
        if (m_idHelper->get_hash(offlineId, hashId, &m_cntxpixel)) ATH_MSG_WARNING("Could not get hash from offlineId");
        if (hashId > m_idHelper->wafer_hash_max()) {

            ATH_MSG_ERROR("IdHash overflow! HashId is 0x" << std::hex << hashId);
            ATH_MSG_ERROR("not mapped OfflineID: " << std::hex << offlineId << std::dec << " barrel_ec: " << barrel_ec
                            << " layer_disk: " << layer_disk << " phi_module: " << phi_module << " eta_module: " << eta_module);
            ATH_MSG_ERROR("to OnlineID: 0x" << std::hex << onlineId << " robid: 0x" << robid << " rodid: 0x" << rodid << std::dec
                             << " link: 0x" << std::hex /*<< link*/ << " -> Linknumber: 0x" << linknumber << " HashId: 0x"
                            << hashId << std::dec);

            // Check if offlineId fail was caused by exceeding eta_module range
            if (eta_module > m_idHelper->eta_module_max(offlineId) || eta_module < m_idHelper->eta_module_min(offlineId)) {
                // eta_module_max == -999 indicates the module does not exist
                if (m_idHelper->eta_module_max(offlineId) == -999 && m_idHelper->eta_module_min(offlineId) == -999) {
                    ATH_MSG_ERROR("Module does not exist in geometry");
                }
                else {
                ATH_MSG_ERROR("eta_module range exceeded: Got eta_module = " << eta_module
                                << ", but allowed range is [" << m_idHelper->eta_module_min(offlineId)
                                << "," << m_idHelper->eta_module_max(offlineId) << "]");
                ATH_MSG_ERROR("Input geometry tag may not be compatible with mapping file");
                }
            }

            return false;
        }

        // Fill the maps
        cabling->add_entry_onoff(onlineId, offlineId);
        cabling->add_entry_offon(offlineId, onlineId);
        cabling->add_entry_offlineList(robid,offlineId);
        cabling->add_entry_offrob(offlineId, robid);
        cabling->add_entry_rodrob(rodid, robid);
        cabling->add_entry_robrod(robid, rodid);
        cabling->add_entry_DCSoffline(DCSname, offlineId);


        // Debug messages
        ATH_MSG_DEBUG("Mapped offlineID: " << std::hex << offlineId << " to onlineID: 0x" << onlineId
                        << ", robID: 0x" << robid << ", barrel_ec: " << std::dec << barrel_ec << ", layer_disk: " << layer_disk
                        << ", eta_module: " << eta_module << ", phi_module: " << phi_module << ", linknumber: 0x" << std::hex << linknumber);


    }

    if (dump_map_to_file) {
        output_mapping_file_raw.close();
        output_mapping_file_interpreted.close();
    }

    ATH_MSG_DEBUG("Size of ROD readoutspeed map: " << rodReadoutMap.size());
    cabling->set_readout_map(rodReadoutMap);
    return true;

}





////////////////////////
// finalize
////////////////////////
StatusCode PixelFillCablingData::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}
