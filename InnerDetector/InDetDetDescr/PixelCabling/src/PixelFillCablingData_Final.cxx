/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelFillCablingData_Final.cxx
//   Implementation file for class PixelFillCablingData_Final
///////////////////////////////////////////////////////////////////
// (c) ATLAS Pixel Detector software
///////////////////////////////////////////////////////////////////
// reading the final mapping file and depending on the selected bandwidth filling the maps
// - FillMaps() - call intialiseFromFile() with the correct file name
// - initialiseFromFile() - reading the cosmic mapping file and depending on the selected bandwidth filling the maps
// - commentOrBlankLine() - for finding whether the line is valid data or just a comment or blank line
///////////////////////////////////////////////////////////////////
//  Version 00-03-51 25/04/2008 Daniel Dobos
///////////////////////////////////////////////////////////////////
#include "PixelFillCablingData_Final.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "PathResolver/PathResolver.h"
#include "Identifier/Identifier.h"
#include "eformat/SourceIdentifier.h"  // change to new eformat v3

// Classes
#include "PixelCablingData.h"
#include "InDetIdentifier/PixelID.h"

using namespace std;
using eformat::helper::SourceIdentifier; 

static const InterfaceID IID_IPixelFillCablingData_Final("PixelFillCablingData_Final", 1, 0);

////////////////////////
// constructor
////////////////////////
PixelFillCablingData_Final::PixelFillCablingData_Final( const std::string& type, const std::string& name,const IInterface* parent): AthAlgTool(type,name,parent), m_idHelper(0), m_cabling(0)
{
  declareInterface< PixelFillCablingData_Final >( this );   
}
	 
////////////////////////
// destructor
////////////////////////
PixelFillCablingData_Final::~PixelFillCablingData_Final()
{
  delete m_cabling;
  m_cabling=0;
}

////////////////////////
// interfaceID
////////////////////////
const InterfaceID& PixelFillCablingData_Final::interfaceID()
{
  return IID_IPixelFillCablingData_Final;
}

////////////////////////
// initialize
////////////////////////
StatusCode PixelFillCablingData_Final::initialize( )
{
  StatusCode sc;
  msg(MSG::DEBUG) << "PixelFillCablingData_Final::initialize" <<endmsg;
  
  // Get the PixelID Helper
  if (detStore()->retrieve(m_idHelper, "PixelID").isFailure()) {
    msg(MSG::FATAL) << "Could not get Pixel ID helper" << endmsg;
    return StatusCode::FAILURE;
  }
  m_cntxpixel = m_idHelper->wafer_context();
  return sc;
}

////////////////////////
// finalize
////////////////////////
StatusCode PixelFillCablingData_Final::finalize()
{
  StatusCode sc = AlgTool::finalize(); 
  return sc;
}

////////////////////////
// FillMaps - call intialiseFromFile() with the correct file name
////////////////////////
PixelCablingData* PixelFillCablingData_Final::FillMaps()
{
  string file_name = PathResolver::find_file (m_final_mapping_file, "DATAPATH");
  if (file_name.size()==0) {
    msg(MSG::FATAL) << "Mapping File: " << m_final_mapping_file << " not found!" << endmsg;
    return NULL;
  } 
  else{
    msg(MSG::INFO) << "Mapping File: " << m_final_mapping_file << endmsg;   }

  return initialiseFromFile(file_name);
}

////////////////////////
// initialiseFromFile - reading the cosmic mapping file and depending on the selected bandwidth filling the maps
////////////////////////
PixelCablingData* PixelFillCablingData_Final::initialiseFromFile(const string file_name)
{
    m_cabling = new PixelCablingData;

    ifstream fromFileStream(file_name.c_str());
    if (not fromFileStream) return NULL;

    int barrel_ec, eta_module, layer_disk;
    uint32_t phi_module, robid, rodid, fmt, link;
    uint32_t sl_40_fmt, sl_40_link, sl_80_fmt, sl_80_link;
    string DCSname;
    string thisLine;

    uint64_t onlineId = 0;
    uint64_t linknumber = 0;

    while (getline(fromFileStream,thisLine)) {
        if (not commentOrBlankLine(thisLine)) {
            istringstream parse(thisLine);
            parse >> barrel_ec >> layer_disk >> phi_module >> eta_module >> std::hex
                  >> robid >> rodid >> sl_40_fmt >> sl_40_link >> sl_80_fmt
                  >> sl_80_link >> DCSname >> DCSname;
//            eformat::helper::SourceIdentifier sid_rod(robid);
//            eformat::helper::SourceIdentifier sid_rob(sid_rod.subdetector_id(), sid_rod.module_id());
//            uint32_t rodid = sid_rob.code();

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
                if (readoutSpeed == false) {
                    fmt = sl_40_fmt;
                    link = sl_40_link;
                }
                else {
                    fmt = sl_80_fmt;
                    link = sl_80_link;
                }
                linknumber = (link & 0xF) | ((fmt & 0xF) << 4);

            }

            // Compute onlineId
            onlineId = (robid & 0xFFFFFF) | (linknumber << 24);

            IdentifierHash hashId;
            if (m_idHelper->get_hash(offlineId, hashId, &m_cntxpixel)) msg(MSG::WARNING) << "Could not get hash from offlineId" << endmsg;
            if (hashId > m_idHelper->wafer_hash_max()) {
                msg(MSG::ERROR) << "IdHash overflow! HashId is 0x" << std::hex << hashId << endmsg;
                msg(MSG::ERROR) << "not mapped OfflineID: " << std::hex << offlineId << std::dec << " barrel_ec: " << barrel_ec
                                << " layer_disk: " << layer_disk << " phi_module: " << phi_module << " eta_module: " << eta_module << endmsg;
                msg(MSG::ERROR) << "to OnlineID: 0x" << std::hex << onlineId << " robid: 0x" << robid << " rodid: 0x" << rodid << std::dec
                                << " link: 0x" << std::hex << link << " -> Linknumber: 0x" << linknumber << " HashId: 0x"
                                << hashId << std::dec << endmsg;

                // Check if offlineId fail was caused by exceeding eta_module range
                if (eta_module > m_idHelper->eta_module_max(offlineId) || eta_module < m_idHelper->eta_module_min(offlineId)) {
                    // eta_module_max == -999 indicate the module does not exist
                    if (m_idHelper->eta_module_max(offlineId) == -999 && m_idHelper->eta_module_min(offlineId) == -999) {
                        msg(MSG::ERROR) << "Module does not exist in geometry" << endmsg;
                    }
                    else
                    msg(MSG::ERROR) << "eta_module range exceeded: Got eta_module = " << eta_module
                                    << ", but allowed range is [" << m_idHelper->eta_module_min(offlineId)
                                    << "," << m_idHelper->eta_module_max(offlineId) << "]" << endmsg;
                    msg(MSG::ERROR) << "Input geometry tag may not be compatible with mapping file " << file_name << endmsg;
                }

                return NULL;
            }

            // Fill the maps
            m_cabling->add_entry_onoff(onlineId, offlineId);
            m_cabling->add_entry_offon(offlineId, onlineId);
            m_cabling->add_entry_offlineList(robid,offlineId);
            m_cabling->add_entry_offrob(offlineId, robid);
            m_cabling->add_entry_rodrob(rodid, robid);
            m_cabling->add_entry_robrod(robid, rodid);
            m_cabling->add_entry_DCSoffline(DCSname, offlineId);

            // Debug messages
            msg(MSG::DEBUG) << "Mapped offlineID: " << std::hex << offlineId << " to onlineID: 0x" << onlineId
                            << ", robID: 0x" << robid << ", barrel_ec: " << std::dec << barrel_ec << ", layer_disk: " << layer_disk
                            << ", eta_module: " << eta_module << ", phi_module: " << phi_module << ", linknumber: 0x" << std::hex << linknumber << endmsg;
        }
    }
    msg(MSG::INFO) << "map size " << rodReadoutMap.size() <<endmsg;
    m_cabling->set_readout_map(rodReadoutMap);
    return m_cabling;
}

////////////////////////
// commentOrBlankLine - for finding whether the line is valid data or just a comment or blank line
////////////////////////
bool PixelFillCablingData_Final::commentOrBlankLine(string line) const 
{
  if (line.size()==0) {
    return true;
  } else {
    string firstChar=line.substr(0,1);
    return ((firstChar==" ") or (firstChar=="#")); // i.e. decided to be a comment if line starts with space or hash
  }
}



