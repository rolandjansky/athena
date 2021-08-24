/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <memory>
#include <unordered_map>

#include "MuonRDO/MM_RawDataContainer.h"
#include "MuonIdHelpers/MmIdHelper.h"

#include "MuonNSWCommonDecode/NSWCommonDecoder.h"
#include "MuonNSWCommonDecode/NSWElink.h"
#include "MuonNSWCommonDecode/VMMChannel.h"
#include "MuonNSWCommonDecode/NSWResourceId.h"

#include "Identifier/IdentifierHash.h"
#include "eformat/Issue.h"
#include "MM_ROD_Decoder.h"

//===============================================================================
Muon::MM_ROD_Decoder::MM_ROD_Decoder( const std::string& type, const std::string& name,const IInterface* parent )
: AthAlgTool(type, name, parent)
{
  declareInterface<IMM_ROD_Decoder>( this );
}


//===============================================================================
StatusCode Muon::MM_ROD_Decoder::initialize() 
{
  ATH_CHECK(detStore()->retrieve(m_MmIdHelper, "MMIDHELPER"));
  return StatusCode::SUCCESS;
}


//===============================================================================
// Processes a ROB fragment and fills the RDO container. 
// If the vector of IdentifierHashes is not empty, then seeded mode is assumed 
// (only requested modules are decoded). This must be made here, because the 
// trigger granularity is a module, whereas ROB granularity is a whole sector. 
// Therefore, refined selection is needed with decoded information.
StatusCode Muon::MM_ROD_Decoder::fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag, const std::vector<IdentifierHash>& rdoIdhVect, MM_RawDataContainer& rdoIdc) const
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
  const std::vector<Muon::nsw::NSWElink *>   elinks = common_decoder.get_elinks();  
  ATH_MSG_DEBUG("Retrieved "<<elinks.size()<<" elinks");
  if (!elinks.size()) return StatusCode::SUCCESS;

  // temporary storage of the individual RDOs corresponding to this ROB
  // (expecting to have one per module).
  std::unordered_map<IdentifierHash, std::unique_ptr<MM_RawDataCollection>> rdo_map;

  // loop on elinks. we need an RDO (collection) per quadruplet!
  for (auto* elink : elinks) {

    // get the offline ID hash (module ctx) to be passed to the RDO 
    // also specifies the index of the RDO in the container.
    bool         is_validID(false);
    const char*  station_name = elink->elinkId()->is_large_station() ? "MML" : "MMS";
    int          station_eta  = (int)elink->elinkId()->station_eta();
    unsigned int station_phi  = (unsigned int)elink->elinkId()->station_phi();
    unsigned int multi_layer  = (unsigned int)elink->elinkId()->multi_layer();
    unsigned int gas_gap      = (unsigned int)elink->elinkId()->gas_gap();
    Identifier   module_ID    = m_MmIdHelper->elementID(station_name, station_eta, station_phi, true, &is_validID);

    if (!is_validID) { ATH_MSG_ERROR("Invalid identifier created for MM station"); continue; }

    IdentifierHash module_hashID;
    m_MmIdHelper->get_module_hash(module_ID, module_hashID);

    // if we are in ROI-seeded mode, check if this hashID is requested
    if (seeded_mode && std::find(rdoIdhVect.begin(), rdoIdhVect.end(), module_hashID) == rdoIdhVect.end()) continue;

    // get the RDO... RDO hash IDs should not repeat for different ROBs,
    // but may repeat for the different elinks of a single ROB.
    // To handle possible repetition, we temporarily store them in an STL map.
    if (rdoIdc.indexFindPtr(module_hashID)) {
       ATH_MSG_WARNING("Collection with hashID "<<module_hashID<<" already exists in the identifiable container. Skipping this elink.\n");
       continue;
    }

    if (!rdo_map[module_hashID]) rdo_map[module_hashID] = std::make_unique<MM_RawDataCollection>(module_hashID);
    MM_RawDataCollection* rdo = rdo_map[module_hashID].get();

    // loop on all channels of this elink to fill the collection
    const std::vector<Muon::nsw::VMMChannel *>& channels = elink->get_channels();
    for (auto channel : channels) {
       unsigned int channel_number = channel->channel_number();
       if (channel_number == 0) continue; // skip disconnected vmm channels
       Identifier channel_ID = m_MmIdHelper->channelID(module_ID, multi_layer, gas_gap, channel_number, true, &is_validID);
       if (!is_validID) { ATH_MSG_ERROR("Invalid identifier created for MM channel"); continue; }

       rdo->push_back(new MM_RawData(channel_ID, channel_number, channel->tdo(), channel->pdo())); // isDead = false (ok?)
    }
  }

  // add the RDO collections created from the data of this ROB into the identifiable container
  for (auto& pair : rdo_map) {
    if (pair.second->size() && !rdoIdc.addCollection(pair.second.release(), pair.first).isSuccess()) {
      ATH_MSG_ERROR("Failed to add STGC RDO into the identifiable container");
    }
  }
  
  rdo_map.clear();

  return StatusCode::SUCCESS;
}

