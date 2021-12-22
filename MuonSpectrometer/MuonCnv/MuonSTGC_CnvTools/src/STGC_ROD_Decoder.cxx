/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <memory>
#include <unordered_map>

#include "MuonRDO/STGC_RawData.h"
#include "MuonRDO/STGC_RawDataContainer.h"
#include "MuonIdHelpers/sTgcIdHelper.h"

#include "MuonNSWCommonDecode/NSWCommonDecoder.h"
#include "MuonNSWCommonDecode/NSWElink.h"
#include "MuonNSWCommonDecode/VMMChannel.h"
#include "MuonNSWCommonDecode/NSWResourceId.h"
 
#include "Identifier/Identifier.h"
#include "eformat/Issue.h"
#include "STGC_ROD_Decoder.h"

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

//==============================================================================
Muon::STGC_ROD_Decoder::STGC_ROD_Decoder(const std::string& t, const std::string& n, const IInterface*  p) 
: AthAlgTool(t, n, p)
{
  declareInterface<ISTGC_ROD_Decoder>(this);
}


//==============================================================================
StatusCode Muon::STGC_ROD_Decoder::initialize()
{
  ATH_CHECK(detStore()->retrieve(m_stgcIdHelper, "STGCIDHELPER"));
  return StatusCode::SUCCESS;
}


//===============================================================================
// Processes a ROB fragment and fills the RDO container. 
// If the vector of IdentifierHashes is not empty, then seeded mode is assumed 
// (only requested modules are decoded). This must be made here, because the 
// trigger granularity is quadruplet, whereas ROB granularity is a whole sector 
// (or wedge). Therefore, refined selection is needed with decoded information.
StatusCode Muon::STGC_ROD_Decoder::fillCollection(const ROBFragment& robFrag, const std::vector<IdentifierHash>& rdoIdhVect, STGC_RawDataContainer& rdoIdc) const
{

  // check fragment for errors
  try {
    robFrag.check();
  } catch (eformat::Issue &ex) {
    ATH_MSG_WARNING(ex.what());
    return StatusCode::SUCCESS;
  }

  // if the vector of hashes is not empty, then we are in seeded mode
  bool seeded_mode(!rdoIdhVect.empty());

  // have the NSWCommonDecoder take care of the decoding
  Muon::nsw::NSWCommonDecoder common_decoder(robFrag);  
  const std::vector<Muon::nsw::NSWElink *>&   elinks = common_decoder.get_elinks();  
  ATH_MSG_DEBUG("Retrieved "<<elinks.size()<<" elinks");
  if (!elinks.size()) return StatusCode::SUCCESS;

  // RDO hash IDs may repeat in different elinks of the same ROB, but not in different ROBs (quadruplets). 
  // To handle possible repetition, we temporarily store them in an STL map.
  std::unordered_map<IdentifierHash, std::unique_ptr<STGC_RawDataCollection>> rdo_map;

  // error counters
  int nerr_stationID{0}, nerr_duplicate{0}, nerr_channelID{0}, nerr_rdo{0};

  // loop on elinks. for STGCs a "module" is a quadruplet
  // therefore, we need an RDO (collection) per quadruplet!
  for (auto* elink : elinks) {

    // get the offline ID hash (module ctx) to be passed to the RDO 
    // also specifies the index of the RDO in the container.
    bool         is_validID(false);
    const char*  station_name = elink->elinkId()->is_large_station() ? "STL" : "STS";
    int          station_eta  = (int)elink->elinkId()->station_eta();
    unsigned int station_phi  = (unsigned int)elink->elinkId()->station_phi();
    unsigned int multi_layer  = (unsigned int)elink->elinkId()->multi_layer();
    unsigned int gas_gap      = (unsigned int)elink->elinkId()->gas_gap();
    Identifier   module_ID    = m_stgcIdHelper->elementID(station_name, station_eta, station_phi, true, &is_validID);
    if(!is_validID) { 
      ++nerr_stationID; 
      continue; 
    }

    IdentifierHash module_hashID;
    m_stgcIdHelper->get_module_hash(module_ID, module_hashID);

    // if we are in ROI-seeded mode, check if this hashID is requested
    if (seeded_mode && std::find(rdoIdhVect.begin(), rdoIdhVect.end(), module_hashID) == rdoIdhVect.end()) continue;

    if (!rdo_map[module_hashID]) rdo_map[module_hashID] = std::make_unique<STGC_RawDataCollection>(module_hashID);
    STGC_RawDataCollection* rdo = rdo_map[module_hashID].get();

    // loop on all channels of this elink to fill the collection
    const std::vector<Muon::nsw::VMMChannel *>& channels = elink->get_channels();
    for (auto channel : channels) {
       unsigned int channel_number = channel->channel_number();
       unsigned int channel_type   = channel->channel_type();
       if (channel_type != Muon::nsw::OFFLINE_CHANNEL_TYPE_STRIP) continue; // TODO: decoder not ready for pads/wires     
       if (channel_number == 0) continue; // skip disconnected vmm channels

       Identifier channel_ID = m_stgcIdHelper->channelID(module_ID, multi_layer, gas_gap, channel_type, channel_number, true, &is_validID);
       if (!is_validID) { 
         ++nerr_channelID; 
         continue; 
       }

       rdo->push_back(new STGC_RawData(channel_ID, channel->rel_bcid(), channel->tdo(), channel->pdo(), false)); // isDead = false (ok?)
    }
  }

  // add the RDO collections created from the data of this ROB into the identifiable container
  for (auto& pair : rdo_map) {
    
    if (!pair.second->size()) continue; // skip empty collections

    STGC_RawDataContainer::IDC_WriteHandle lock = rdoIdc.getWriteHandle(pair.first);
    if (lock.alreadyPresent()) {
      // RDO hash IDs should not repeat in different ROBs.
      ++nerr_duplicate;
    } else if (!lock.addOrDelete(std::move(pair.second)).isSuccess()) {
      // since we prevent duplicates above, this error should never happen.
      ++nerr_rdo;
    }
  }

  rdo_map.clear();

  // error summary (to reduce the number of messages)
  if (nerr_duplicate) ATH_MSG_WARNING(nerr_duplicate << " elinks skipped since the same module hash has been added by a previous ROB fragment");
  if (nerr_stationID) ATH_MSG_WARNING("Unable to create valid identifier for "<<nerr_stationID<<" modules (corrupt data?); elinks skipped");
  if (nerr_channelID) ATH_MSG_WARNING("Unable to create valid identifier for "<<nerr_channelID<<" channels (corrupt data?); channels skipped");
  if (nerr_rdo)       ATH_MSG_WARNING("Failed to add "<<nerr_rdo<<" RDOs into the identifiable container");

  return StatusCode::SUCCESS;
}



