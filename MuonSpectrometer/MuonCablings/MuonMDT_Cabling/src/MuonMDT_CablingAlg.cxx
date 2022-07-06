/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonMDT_CablingAlg.h"

#include <stdlib.h>

#include <fstream>
#include <map>
#include <string>

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "MuonCondSvc/MdtStringUtils.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "PathResolver/PathResolver.h"
#include "SGTools/TransientAddress.h"
MuonMDT_CablingAlg::MuonMDT_CablingAlg(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator) {}

StatusCode MuonMDT_CablingAlg::initialize() {
    ATH_MSG_DEBUG("initialize " << name());
    ATH_CHECK(m_readKeyMez.initialize());
    ATH_CHECK(m_readKeyMap.initialize());
    ATH_CHECK(m_writeKey.initialize());
    ATH_CHECK(m_idHelperSvc.retrieve());
    m_isRun3 = m_idHelperSvc->hasMM() || m_idHelperSvc->hasSTgc();
    return StatusCode::SUCCESS;
}

StatusCode MuonMDT_CablingAlg::execute() {
    ATH_MSG_VERBOSE("MuonMDT_CablingAlg::execute()");
    const EventContext& ctx = Gaudi::Hive::currentContext();
    // Write Cond Handle
    SG::WriteCondHandle<MuonMDT_CablingMap> writeHandle{m_writeKey, ctx};
    if (writeHandle.isValid()) {
        ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                                    << ". In theory this should not be called, but may happen"
                                    << " if multiple concurrent events are being processed out of order.");
        return StatusCode::SUCCESS;
    }
    ATH_MSG_INFO("Load the Mdt cabling");
    std::unique_ptr<MuonMDT_CablingMap> writeCdo{std::make_unique<MuonMDT_CablingMap>()};

    EventIDRange rangeMez;
    ATH_CHECK(loadMezzanineSchema(ctx, rangeMez, *writeCdo));

    EventIDRange rangeMap;
    ATH_CHECK(loadCablingSchema(ctx, rangeMap, *writeCdo));
    if (!writeCdo->finalize_init(msgStream())) return StatusCode::FAILURE;
    // Create an intersection of input IOVs
    EventIDRange rangeIntersection = EventIDRange::intersect(rangeMez, rangeMap);
    if (rangeIntersection.start() > rangeIntersection.stop()) {
        ATH_MSG_ERROR("Invalid intersection range: " << rangeIntersection);
        return StatusCode::FAILURE;
    }

    if (writeHandle.record(rangeIntersection, std::move(writeCdo)).isFailure()) {
        ATH_MSG_FATAL("Could not record MuonMDT_CablingMap " << writeHandle.key() << " with EventRange " << rangeIntersection
                                                             << " into Conditions Store");
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << rangeIntersection << " into Conditions Store");
    return StatusCode::SUCCESS;
}
StatusCode MuonMDT_CablingAlg::loadCablingSchema(const EventContext& ctx, EventIDRange& iov_range, MuonMDT_CablingMap& cabling_map) const {
    SG::ReadCondHandle<CondAttrListCollection> readHandleMap{m_readKeyMap, ctx};
    const CondAttrListCollection* readCdoMap{*readHandleMap};
    if (!readCdoMap) {
        ATH_MSG_ERROR("Null pointer to the read conditions object");
        return StatusCode::FAILURE;
    }

    if (!readHandleMap.range(iov_range)) {
        ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandleMap.key());
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Size of CondAttrListCollection " << readHandleMap.fullKey() << " readCdoMap->size()= " << readCdoMap->size());
    ATH_MSG_INFO("Range of input is " << iov_range);

    ATH_MSG_VERBOSE("Collection CondAttrListCollection CLID " << readCdoMap->clID());

    // access to Map Schema Table to obtained the Map
    CondAttrListCollection::const_iterator itrMap;
    for (itrMap = readCdoMap->begin(); itrMap != readCdoMap->end(); ++itrMap) {
        const coral::AttributeList& atr = itrMap->second;

        CablingData map_data;
        if (!extractStationInfo(atr, map_data)) continue;

        constexpr char delimiter = ',';
        const std::string map = *(static_cast<const std::string*>((atr["Map"]).addressOfData()));
        std::vector<std::string> info_map;
        MuonCalib::MdtStringUtils::tokenize(map, info_map, delimiter);

        while (extractLayerInfo(info_map, map_data)) {
            /// now this mezzanine can be added to the map:
            ATH_MSG_VERBOSE("Adding new mezzanine stationName: " << m_idHelperSvc->mdtIdHelper().stationNameString(map_data.stationIndex)
                                                                 << " " << map_data);
            if (!cabling_map.addMezzanine(map_data, msgStream())) {
                ATH_MSG_ERROR("Failed to add cabling " << map_data);
                return StatusCode::FAILURE;
            } else ATH_MSG_VERBOSE("Added new cabling channel "<<map_data);
        }
    }  // end of CondAttrListCollection loop
    /// Load manually the BIX7 / BIY7 chambers into the data base if neccessary
    if (m_isRun3) {
        ATH_MSG_INFO("Found a round 3 setup. Will add the cabling for BIS78 manually");
        const int BIS_stat = m_idHelperSvc->mdtIdHelper().stationNameIndex("BIS");
        constexpr int eta_index = 7;
        for (unsigned int sector = 1; sector <= 8; ++sector) {
            int is_biy{0}, tdcId{0};
            auto increment_tdc = [&is_biy, &tdcId]() {
                ++tdcId;
                constexpr int tdcMax = 17;  // Maximum 18 tdcs per CSM
                if (tdcId > tdcMax) {
                    tdcId = 0;
                    ++is_biy;
                }
            };
            constexpr int tubesPerCsm = 54;  /// That translates to 54 tubes per layer per CSM
            bool added_tdc = true;
            while (added_tdc) {
                added_tdc = false;
                for (int ml = 1; ml <= 2; ++ml) {
                    const Identifier layer_id = m_idHelperSvc->mdtIdHelper().channelID(BIS_stat, eta_index, sector, ml, 1, 1);
                    const int tubes_per_layer = m_idHelperSvc->mdtIdHelper().tubeMax(layer_id);

                    const int first_tube = 6 * (1 + tdcId / 2) + tubesPerCsm * is_biy;
                    const bool valid_tube = (first_tube <= tubes_per_layer);
                    added_tdc |= valid_tube;
                    if (!valid_tube) {
                        increment_tdc();
                        continue;
                    }

                    CablingData bis78_data{};
                    bis78_data.tubes_per_layer = MdtIdHelper::maxNTubesPerLayer;
                    bis78_data.stationIndex = BIS_stat;
                    bis78_data.eta = eta_index;
                    bis78_data.phi = sector;
                    /// The mrod numbering is 8 for sector 2, 9 for sector 4 etc..
                    /// For  BIY it's 1 for sector 2, 2 for sector 4, etc.
                    bis78_data.mrod = (is_biy ? -1 : 7) + sector;
                    bis78_data.csm = is_biy ? 5 : 0;
                    bis78_data.subdetectorId = 99;

                    bis78_data.mezzanine_type = sector != 6 ? 61 : 71;
                    const unsigned int tube_coding = first_tube + bis78_data.tubes_per_layer * (4 + bis78_data.layer_block * ml);
                    ATH_MSG_VERBOSE("ooooh BIS78 is being hacked in " << m_idHelperSvc->toString(layer_id) << " tubes_per_layer  "
                                                                      << m_idHelperSvc->mdtIdHelper().tubeMax(layer_id) << " tdcId: "
                                                                      << tdcId << " is_Biy: " << is_biy << " first_tube: " << first_tube);

                    std::vector<std::string> db_coding{std::to_string(tdcId), "0", std::to_string(tube_coding)};
                    if (extractLayerInfo(db_coding, bis78_data)) {
                        ATH_MSG_VERBOSE("Adding new mezzanine stationName: "
                                        << m_idHelperSvc->mdtIdHelper().stationNameString(bis78_data.stationIndex) << " " << bis78_data);
                        if (!cabling_map.addMezzanine(bis78_data, msgStream())) {
                            ATH_MSG_ERROR("Failed to add mezzanine card " << bis78_data);
                            return StatusCode::FAILURE;
                        }
                    }
                    if (bis78_data.tube != first_tube) {
                        ATH_MSG_ERROR("BIS78 data tube encoding failed. Expected " << first_tube << " got " << bis78_data.tube << ".");
                        return StatusCode::FAILURE;
                    } else if (bis78_data.multilayer != ml) {
                        ATH_MSG_ERROR("BIS78 data ml encoding failed. Expected " << ml << " got " << bis78_data.multilayer << ".");
                        return StatusCode::FAILURE;
                    }
                    increment_tdc();
                }
            }
        }
    }
    return StatusCode::SUCCESS;
}

bool MuonMDT_CablingAlg::extractStationInfo(const coral::AttributeList& atr, CablingData& map_data) const {
    map_data.eta = *(static_cast<const int*>((atr["Eta"]).addressOfData()));
    map_data.phi = *(static_cast<const int*>((atr["Phi"]).addressOfData()));
    map_data.mrod = *(static_cast<const int*>((atr["MROD"]).addressOfData()));
    map_data.csm = *(static_cast<const int*>((atr["CSM"]).addressOfData()));
    map_data.channelId = *(static_cast<const int*>((atr["Chan"]).addressOfData()));
    map_data.mezzanine_type = *(static_cast<const int*>((atr["Mezzanine_Type"]).addressOfData()));

    const std::string chamber_name = *(static_cast<const std::string*>((atr["Chamber_Name"]).addressOfData()));
    const std::string subdetector_id = *(static_cast<const std::string*>((atr["SubDet_Id"]).addressOfData()));

    // convert the string name to index
    std::string stationNameString = chamber_name.substr(0, 3);
    // fix for the BOE chambers, which in the offline are treated as BOL
    if (stationNameString == "BOE") { stationNameString = "BOL"; }
    /// Hack for the BIS78 sMDT chambers
    if (stationNameString == "BIX" || stationNameString == "BIY") {
        stationNameString = "BIS";
        static std::atomic<bool> sInfoPrinted{false};
        if (!sInfoPrinted)
            ATH_MSG_INFO("Found an entry concerning the new BIS78 chambers. Will increase the number of tube per layer to "
                         << MdtIdHelper::maxNTubesPerLayer);
        sInfoPrinted = true;

        map_data.tubes_per_layer = MdtIdHelper::maxNTubesPerLayer;
    }
    /// Temporary hack to remove all EI chambers until the proper data base tag arrives
    if (m_isRun3) {
        /// It's still useful to keep the C-side chamber if the asymmetric geometry is loaded
        if ( ( stationNameString == "EIS" && (!m_idHelperSvc->hasCSC() || chamber_name.find('A') != std::string::npos) ) 
            || chamber_name.find("BIS7A") != std::string::npos) {
            ATH_MSG_VERBOSE("It's sooo sad but the chamber " << chamber_name << " is no longer with us");
            return false;
        }
    }
    map_data.stationIndex = m_idHelperSvc->mdtIdHelper().stationNameIndex(stationNameString);
    ATH_MSG_VERBOSE("station name: " << stationNameString << " index: " << static_cast<int>(map_data.stationIndex));
    // convert the subdetector id to integer
    map_data.subdetectorId = atoi(subdetector_id.c_str());

    ATH_MSG_VERBOSE("Data load is: /n"
                    << "Chamber_Name = " << chamber_name << " eta= " << map_data.eta << "   Phi= " << map_data.phi
                    << " sub_id = " << subdetector_id << "  mrod = " << int(map_data.mrod) << " csm = " << int(map_data.csm)
                    << "  chan= " << int(map_data.channelId) << " mezzanine_type= " << int(map_data.mezzanine_type) << " FINISHED HERE ");

    if (map_data.stationIndex < 0) {
        static std::atomic<bool> stWarningPrinted{false};
        if (!stWarningPrinted) {
            ATH_MSG_WARNING("Found stationIndex=" << static_cast<int>(map_data.stationIndex)
                                                  << " which is not reasonable, maybe related to ATLASRECTS-5961, continuing...");
            stWarningPrinted = true;
        }
    }
    return map_data.stationIndex >= 0;
}
bool MuonMDT_CablingAlg::extractLayerInfo(std::vector<std::string>& info_map, CablingData& map_data) const {
    ATH_MSG_VERBOSE(" parsing of the map");
    /// The map data is endcoded in repitive blocks of
    ///   <tdcId>,<channelId>,<tube_numbering>,<tdcId>,<channelId>,<tube_numbering>,...
    constexpr unsigned int coding_block_size = 3;
    const unsigned int n = info_map.size() >= coding_block_size ? coding_block_size : info_map.size();
    bool decoded_full_block{false};
    for (unsigned int i = 0; i < n; ++i) {
        ATH_MSG_VERBOSE(i << "..." << info_map[i]);
        int info = MuonCalib::MdtStringUtils::atoi(info_map[i]);
        // this is a tdcid
        if (i == 0) {
            map_data.tdcId = info;
        }
        // this is a channel Id
        else if (i == 1) {
            map_data.channelId = info;
        }
        // this is a tube id that must be unpacked
        else if (i == 2) {
            /// unpack the tube Id
            map_data.tube = info % map_data.tubes_per_layer;
            map_data.layer = ((info - map_data.tube) / map_data.tubes_per_layer) % map_data.layer_block;
            map_data.multilayer = (((info - map_data.tube) / map_data.tubes_per_layer) - map_data.layer) / map_data.layer_block;
            decoded_full_block = true;
        }
    }

    // the stationIndex is later on passed to the MdtIdHelper, thus, it must be a reasonable station name, i.e. not < 0
    if (decoded_full_block) info_map.erase(info_map.begin(), info_map.begin() + coding_block_size);
    return decoded_full_block;
}

StatusCode MuonMDT_CablingAlg::loadMezzanineSchema(const EventContext& ctx, EventIDRange& iov_range,
                                                   MuonMDT_CablingMap& cabling_map) const {
    /// Read Cond Handle

    SG::ReadCondHandle<CondAttrListCollection> readHandleMez{m_readKeyMez, ctx};
    const CondAttrListCollection* readCdoMez{*readHandleMez};
    if (!readHandleMez.range(iov_range)) {
        ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandleMez.key());
        return StatusCode::FAILURE;
    }
    if (!readCdoMez) {
        ATH_MSG_ERROR("Null pointer to the read conditions object");
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("Size of CondAttrListCollection " << readHandleMez.fullKey() << " readCdoMez->size()= " << readCdoMez->size());
    ATH_MSG_INFO("Range of input is " << iov_range);

    CondAttrListCollection::const_iterator itrMez;
    for (itrMez = readCdoMez->begin(); itrMez != readCdoMez->end(); ++itrMez) {
        const coral::AttributeList& atr = itrMez->second;
        int sequence{0}, layer{0}, mezzanine_type{0};

        mezzanine_type = *(static_cast<const int*>((atr["Mezzanine_Type"]).addressOfData()));
        layer = *(static_cast<const int*>((atr["Layer"]).addressOfData()));
        sequence = *(static_cast<const int*>((atr["Sequence"]).addressOfData()));
        ATH_MSG_VERBOSE("Sequence load is " << sequence << " for the mezzanine type =  " << mezzanine_type
                                            << " for the layer  number  = " << layer);

        // here add the mezzanine type to the cabling class
        if (!cabling_map.addMezzanineLine(mezzanine_type, layer, sequence, msgStream())) {
            ATH_MSG_ERROR("Could not add the mezzanine sequence to the map ");
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_VERBOSE("Sequence added successfully to the map");
        }
    }
    /// Add the October fest mezznines
    if (m_isRun3) {
        if (!cabling_map.addMezzanineLine(61, 0, 666666, msgStream())) {
            ATH_MSG_ERROR("There's the custom saying everything that happens @ bavarian regional customs also remains there");
            return StatusCode::FAILURE;
        }
        if (!cabling_map.addMezzanineLine(71, 0, 666666, msgStream())) {
            ATH_MSG_ERROR("There's the custom saying everything that happens @ bavarian regional customs also remains there");
            return StatusCode::FAILURE;
        }
    }
    return StatusCode::SUCCESS;
}
