/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCablingData/MuonMDT_CablingMap.h"

#include <cmath>

#include "GaudiKernel/ISvcLocator.h"
#include "MuonCablingData/MdtMezzanineType.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "StoreGate/StoreGateSvc.h"

MuonMDT_CablingMap::MuonMDT_CablingMap() {
    // initialize the message service

    // retrieve the MdtIdHelper
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    StoreGateSvc* detStore = nullptr;
    StatusCode sc = svcLocator->service("DetectorStore", detStore);
    if (sc != StatusCode::SUCCESS) { throw std::runtime_error("Could not find the detctor store"); }
    sc = detStore->retrieve(m_mdtIdHelper, "MDTIDHELPER");
    if (sc != StatusCode::SUCCESS) { throw std::runtime_error("Could not retrieve the MdtIdHelper"); }
}

MuonMDT_CablingMap::~MuonMDT_CablingMap() = default;

bool MuonMDT_CablingMap::convert(const CablingData& cabling_data, Identifier& id, bool check_valid) const {
    bool valid{!check_valid};
    id = check_valid ? m_mdtIdHelper->channelID(cabling_data.stationIndex, cabling_data.eta, cabling_data.phi, cabling_data.multilayer,
                                                cabling_data.layer, cabling_data.tube, valid)
                     : m_mdtIdHelper->channelID(cabling_data.stationIndex, cabling_data.eta, cabling_data.phi, cabling_data.multilayer,
                                                cabling_data.layer, cabling_data.tube);
    return valid;
}
bool MuonMDT_CablingMap::convert(const Identifier& module_id, CablingData& cabling_data) const {
    if (!m_mdtIdHelper->is_mdt(module_id)) return false;
    cabling_data.stationIndex = m_mdtIdHelper->stationName(module_id);
    cabling_data.eta = m_mdtIdHelper->stationEta(module_id);
    cabling_data.phi = m_mdtIdHelper->stationPhi(module_id);
    cabling_data.tube = m_mdtIdHelper->tube(module_id);
    cabling_data.multilayer = m_mdtIdHelper->multilayer(module_id);
    cabling_data.layer = m_mdtIdHelper->tubeLayer(module_id);
    return true;
}

/** add a new line from the database, describing a mezzanine type */
bool MuonMDT_CablingMap::addMezzanineLine(const int type, const int layer, const int sequence, MsgStream& log) {
    bool added = true;
    bool debug = (log.level() <= MSG::VERBOSE);
    if (debug) { log << MSG::VERBOSE << "Now in MuonMDT_CablingMap::addMezzanineLine" << endmsg; }

    MdtMezzanineType* mezType;

    MezzanineTypes::const_iterator it = m_listOfMezzanineTypes.find(type);

    // check if the type already exists, if yes, retrieve it
    bool isNewMezzanine;
    if (it != m_listOfMezzanineTypes.end()) {
        // if the attempt is to initialize all layers of a mezzanine already
        // initialized, return an error message
        if (!layer) {
            log << MSG::ERROR << "The mezzanine type " << type << "has been already initialized" << endmsg;
            return false;
        }
        mezType = it->second.get();
        isNewMezzanine = false;
    }
    // if it does not exist, create the new type
    else {
        if (debug) { log << MSG::VERBOSE << "Creating a new mezzanine type: " << type << endmsg; }
        mezType = new MdtMezzanineType(type);
        // insert the new mezzanine in the map
        m_listOfMezzanineTypes.insert(MezzanineTypes::value_type(type, mezType));
        isNewMezzanine = true;
    }

    int ntubes = 0;
    int number = sequence;
    int nOfLayers;
    std::array<int, 8> newseq{0};
    // now decode the sequence, up to 8 tubes per sequence
    int tube = number % 10;

    while (tube != 0) {
        // add the tube to the tube sequence
        if (ntubes > 7) {
            log << MSG::ERROR << "More than 8 tubes in a layer, not possible !" << endmsg;
            return false;
        }
        if (debug) {
            log << MSG::VERBOSE << "Adding tube number: " << tube << " to the layer " << layer << " of mezzanine type " << type << endmsg;
        }

        newseq[ntubes] = tube;

        ++ntubes;
        number = (number - tube) / 10;
        tube = number % 10;
    }

    if (ntubes != 8 && ntubes != 6) {
        log << MSG::ERROR << "in type " << type << ": number of tubes per mezzanine layer can be only 6 or 8 ! what are you doing ???"
            << endmsg;
        return false;
    } else {
        nOfLayers = 24 / ntubes;
        if (layer > nOfLayers) {
            log << MSG::ERROR << "The maximum number of layers for this mezzanine is: " << nOfLayers
                << " so you can't initialize layer: " << layer << endmsg;
            return false;
        }
    }

    if (debug) {
        log << MSG::VERBOSE << "Found " << ntubes << " tubes in layer " << layer << endmsg;
        log << MSG::VERBOSE << "This is a " << nOfLayers << " layers mezzanine - OK, OK..." << endmsg;
    }

    // now swap the sequence to have it as in the DB and create the real layers
    MdtLayer newLayer{};
    for (int i = 0; i < ntubes; ++i) { newLayer[i] = newseq[ntubes - i - 1]; }

    // the mezzanine is new, add the
    if (isNewMezzanine) {
        // check if the mapping is valid for all layers
        if (layer == 0) {
            // setting all the layers of this mezzanine
            for (int ll = 1; ll < nOfLayers + 1; ++ll) { mezType->addLayer(ll, newLayer, log); }
        } else {
            mezType->addLayer(layer, newLayer, log);
        }
    }
    // this is an existing mezzanine so just add the single layer, but first
    // check that the layer does not exist yet
    else {
        if (mezType->hasLayer(layer)) {
            log << MSG::ERROR << "Layer: " << layer << " has been already initialized for mezzanine " << type << endmsg;
            return false;
        }
        mezType->addLayer(layer, newLayer, log);
    }
    return added;
}
/** Add a new mezzanine to the map */
bool MuonMDT_CablingMap::addMezzanine(const CablingData& map_data, MsgStream& log) {
    bool debug = (log.level() <= MSG::VERBOSE);
    /// New and hopefully easier to maintain version
    std::vector<std::unique_ptr<MdtTdcMap>>::const_iterator found_tdc =
        std::find_if(m_tdcs.begin(), m_tdcs.end(), [&map_data](const std::unique_ptr<MdtTdcMap>& tdc) {
            return tdc->offId() == map_data && tdc->moduleId() == map_data.tdcId && map_data.channelId == tdc->tdcZero() &&
                   map_data.tube == tdc->tubeZero() && map_data.layer == tdc->layerZero();
        });
    if (found_tdc != m_tdcs.end()) {
        log << MSG::ERROR << "Tdc with " << map_data << " already exists" << (*found_tdc)->offId() << endmsg;
        return false;
    }

    const MdtMezzanineType* mezzaType{nullptr};
    MezzanineTypes::const_iterator it = m_listOfMezzanineTypes.find(map_data.mezzanine_type);
    if (it != m_listOfMezzanineTypes.end()) {
        mezzaType = (*it).second.get();
    } else {
        log << MSG::ERROR << "Mezzanine Type: " << map_data.mezzanine_type << " not found in the list !" << endmsg;
        return false;
    }
    std::unique_ptr<MdtTdcMap> newTdc = std::make_unique<MdtTdcMap>(mezzaType, map_data, m_mdtIdHelper, log);
    if (debug) { log << MSG::VERBOSE << " Added new readout channel " << map_data << endmsg; }
    m_toOnlineConv[map_data].emplace(newTdc.get());
    TdcOnlSet& attachedTdcs = m_toOfflineConv[map_data].all_modules;
    if (attachedTdcs.size() <= map_data.tdcId) attachedTdcs.resize(map_data.tdcId+1);
    attachedTdcs [map_data.tdcId] = MdtTdcOnlSorter{newTdc.get()};
    m_tdcs.push_back(std::move(newTdc));

    if (!addChamberToRODMap(map_data, log) && debug) { log << MSG::VERBOSE << "Station already in the map !" << endmsg; }

    return true;
}
bool MuonMDT_CablingMap::getOfflineId(CablingData& cabling_map, MsgStream& log) const {
    OnlToOffMap::const_iterator module_itr = m_toOfflineConv.find(cabling_map);
    if (module_itr == m_toOfflineConv.end()) {
        log << MSG::WARNING << "Could not find a cabling module to recieve offline Id for " << cabling_map << endmsg;
        return false;
    }
    /// if it's the dummy TDC (i.e. the 0xff used to convert the full station)
    if (cabling_map.tdcId == 0xff && cabling_map.channelId == 0xff) {
        cabling_map.channelId = 0;
        if (!module_itr->second.zero_module) {
            log << MSG::WARNING << " No tdc with channel zero found for " << module_itr->first << endmsg;
            return false;
        }
        if (!module_itr->second.zero_module->offlineId(cabling_map, log)) {
            log << MSG::WARNING << "MdtTdMap::getOfflineId() -- Channel: 0x" << MSG::hex << static_cast<unsigned>(cabling_map.channelId)
                << MSG::dec << " Tdc: 0x" << MSG::hex << static_cast<unsigned>(cabling_map.tdcId) << MSG::dec << " not found in CSM: 0x"
                << MSG::hex << MSG::dec << " subdetector: 0x" << MSG::hex << static_cast<unsigned>(cabling_map.subdetectorId) << MSG::dec
                << endmsg;
            return false;
        }
    } else {
        const TdcOnlSet& attachedTdcs = module_itr->second.all_modules;
        if (attachedTdcs.size() < cabling_map.tdcId) {
            log << MSG::WARNING<<"getOfflineId() -- Tdc: "<< static_cast<unsigned>(cabling_map.tdcId)<<" is not part of "<<module_itr->first<<". Maximally "<<attachedTdcs.size()<<" Tdcs were attached. "<<endmsg;
            return false;
        }
        const MdtTdcOnlSorter& TdcItr = attachedTdcs.at(cabling_map.tdcId);
        if (!TdcItr) {
            log << MSG::WARNING << "getOfflineId() -- Tdc: 0x" << MSG::hex << static_cast<unsigned>(cabling_map.tdcId) << MSG::dec
                << " not found in CSM: 0x" << MSG::hex << static_cast<unsigned>(cabling_map.csm) << MSG::dec << " of MROD: 0x" << MSG::hex
                << static_cast<unsigned>(cabling_map.mrod) << MSG::dec << " subdetector: 0x" << MSG::hex
                << static_cast<unsigned>(cabling_map.subdetectorId) << MSG::dec << endmsg;
            return false;
        }
        if (!TdcItr->offlineId(cabling_map, log)) {
            log << MSG::WARNING << "MdtTdMap::getOfflineId() -- Channel: 0x" << MSG::hex << static_cast<unsigned>(cabling_map.channelId)
                << MSG::dec << " Tdc: 0x" << MSG::hex << static_cast<unsigned>(cabling_map.tdcId) << MSG::dec << " not found in CSM: 0x"
                << MSG::hex << MSG::dec << " subdetector: 0x" << MSG::hex << static_cast<unsigned>(cabling_map.subdetectorId) << MSG::dec
                << endmsg;
            return false;
        }
    }
    if (log.level() <= MSG::VERBOSE) {
        log << MSG::VERBOSE << "Channel: 0x" << MSG::hex << static_cast<unsigned>(cabling_map.channelId) << MSG::dec << " Tdc: 0x"
            << MSG::hex << static_cast<unsigned>(cabling_map.tdcId) << MSG::dec << " CSM: 0x" << MSG::hex
            << static_cast<unsigned>(cabling_map.csm) << MSG::dec << " of MROD: 0x" << MSG::hex << static_cast<unsigned>(cabling_map.mrod)
            << MSG::dec << " subdetector: 0x" << MSG::hex << static_cast<unsigned>(cabling_map.subdetectorId) << MSG::dec << endmsg;

        log << MSG::VERBOSE << "Mapped to: Station: " << static_cast<int>(cabling_map.stationIndex)
            << " eta: " << static_cast<int>(cabling_map.eta)
            << " phi: " << static_cast<int>(cabling_map.phi)
            << " multiLayer: " << static_cast<int>(cabling_map.multilayer)
            << " layer: " << cabling_map.layer
            << " tube: " << cabling_map.tube << endmsg;
    }
    return true;
}

/** get the online id from the offline id */
bool MuonMDT_CablingMap::getOnlineId(CablingData& cabling_map, MsgStream& log) const {
    OffToOnlMap::const_iterator module_itr = m_toOnlineConv.find(cabling_map);
    if (module_itr == m_toOnlineConv.end()) {
        log << MSG::WARNING << "Could not find a cabling module to recieve online Id for " << cabling_map << endmsg;
        return false;
    }
    const TdcOffSet& attachedTdcs = module_itr->second;
    TdcOffSet::const_iterator tdc_itr = attachedTdcs.find(cabling_map);
    if (tdc_itr == attachedTdcs.end()) {
        log << MSG::WARNING << "No matching Tdc channel was found for " << cabling_map << endmsg;
    } else if ((*tdc_itr)->onlineId(cabling_map, log))
        return true;
    /// May be we missed it?
    TdcOffSet::const_iterator control_itr =
        std::find_if(attachedTdcs.begin(), attachedTdcs.end(),
                     [&cabling_map, &log](const MdtTdcOffSorter& tdc) { return tdc->onlineId(cabling_map, log); });
    if (control_itr == attachedTdcs.end()) {
        log << MSG::WARNING << "Second trial to find a valid cabling channel for " << cabling_map << " failed as well. " << endmsg;
        return false;
    }
    return true;
}
const MuonMDT_CablingMap::OnlToOffMap& MuonMDT_CablingMap::getOnlineConvMap() const { return m_toOfflineConv; }
const MuonMDT_CablingMap::OffToOnlMap& MuonMDT_CablingMap::getOfflineConvMap() const { return m_toOnlineConv; }

bool MuonMDT_CablingMap::addChamberToRODMap(const CablingData& map_data, MsgStream& log) {
    bool debug = (log.level() <= MSG::VERBOSE);
    IdentifierHash chamberId;
    bool hashFound = getStationCode(map_data, chamberId, log);
    if (!hashFound) {
        log << MSG::ERROR << "Could not found hashId for station: " << map_data << endmsg;
        return false;
    }
    int sub = map_data.subdetectorId;
    int rod = map_data.mrod;

    uint32_t hardId = (sub << 16) | rod;
    if (debug) {
        log << MSG::VERBOSE << "Adding the chamber with Id: " << chamberId << " and subdetector+rod ID: 0x" << MSG::hex << hardId
            << MSG::dec << endmsg;
    }

    // check if the chamber has already been put in the map
    ChamberToRODMap::const_iterator it = m_chamberToROD.find(chamberId);
    if (it != m_chamberToROD.end()) { return false; }

    m_chamberToROD.insert(std::pair<IdentifierHash, uint32_t>(chamberId, hardId));

    // new function to do the opposite of the above
    RODToChamberMap::iterator Rod_it = m_RODToChamber.find(hardId);
    if (Rod_it == m_RODToChamber.end()) {
        std::vector<IdentifierHash> Idvec;
        Idvec.push_back(chamberId);
        m_RODToChamber.insert(std::make_pair(hardId, Idvec));
    } else {
        Rod_it->second.push_back(chamberId);
    }

    // now check if the ROD is already in the list of ROD vector
    bool rodInitialized = false;
    for (unsigned int i = 0; i < m_listOfROD.size(); ++i) {
        if (hardId == m_listOfROD[i]) { rodInitialized = true; }
    }
    if (!rodInitialized) {
        if (debug) { log << MSG::VERBOSE << "Adding the ROD 0x" << MSG::hex << hardId << MSG::dec << " to the list" << endmsg; }
        m_listOfROD.push_back(hardId);
    }

    return true;
}
bool MuonMDT_CablingMap::getStationCode(const CablingData& map_data, IdentifierHash& mdtHashId, MsgStream& log) const {
    // create the station identifier
    Identifier elementId = m_mdtIdHelper->elementID(map_data.stationIndex, map_data.eta, map_data.phi);

    if (m_mdtIdHelper->get_module_hash(elementId, mdtHashId)) {
        log << MSG::ERROR << "Could not find HashId for module: " << endmsg;
        elementId.show();
        return false;
    }

    return true;
}
uint32_t MuonMDT_CablingMap::getROBId(const IdentifierHash& stationCode, MsgStream& log) const {
    ChamberToRODMap::const_iterator it = m_chamberToROD.find(stationCode);
    if (it != m_chamberToROD.end()) { return it->second; }
    log << MSG::WARNING << "Rod ID not found !" << endmsg;
    return 0;
}
// get the robs corresponding to a vector of hashIds, copied from Svc before the readCdo migration
std::vector<uint32_t> MuonMDT_CablingMap::getROBId(const std::vector<IdentifierHash>& mdtHashVector, MsgStream& log) const {
    std::vector<uint32_t> robVector;
    bool debug = (log.level() <= MSG::VERBOSE);
    for (unsigned int i = 0; i < mdtHashVector.size(); ++i) {
        int robId = getROBId(mdtHashVector[i], log);
        if (robId == 0) {
            log << MSG::ERROR << "ROB id not found for Hash Id: " << mdtHashVector[i] << endmsg;
        } else {
            if (debug) {
                log << MSG::VERBOSE << "Found ROB id 0x" << MSG::hex << robId << MSG::dec << " for hashId " << mdtHashVector[i] << endmsg;
            }
        }
        robVector.push_back(robId);
    }
    { log << MSG::VERBOSE << "Size of ROB vector is: " << robVector.size() << endmsg; }
    return robVector;
}

const std::vector<IdentifierHash>& MuonMDT_CablingMap::getChamberHashVec(const uint32_t ROBId, MsgStream& log) const {
    RODToChamberMap::const_iterator Rob_it = m_RODToChamber.find(ROBId);
    if (Rob_it != m_RODToChamber.end()) { return Rob_it->second; }

    log << MSG::WARNING << "Rod ID not found !" << endmsg;
    static const std::vector<IdentifierHash> emptyIdHashVec;
    return emptyIdHashVec;
}

std::vector<IdentifierHash> MuonMDT_CablingMap::getChamberHashVec(const std::vector<uint32_t>& ROBId_list, MsgStream& log) const {
    std::vector<IdentifierHash> HashVec;
    for (unsigned int i = 0; i < ROBId_list.size(); ++i) {
        RODToChamberMap::const_iterator Rob_it = m_RODToChamber.find(ROBId_list[i]);
        if (Rob_it == m_RODToChamber.end()) {
            log << MSG::WARNING << "Rod ID " << ROBId_list[i] << " not found, continuing with the rest of the ROBId" << endmsg;
            continue;
        }
        HashVec.insert(HashVec.end(), Rob_it->second.begin(), Rob_it->second.end());
    }
    return HashVec;
}

const MuonMDT_CablingMap::ListOfROD& MuonMDT_CablingMap::getAllROBId() const { return m_listOfROD; }

bool MuonMDT_CablingMap::finalize_init(MsgStream& log) {
    if (m_tdcs.empty()) {
        log << MSG::ERROR << "No tdc maps were loaded " << endmsg;
        return false;
    }

    const unsigned int offToOnlChan = std::accumulate(m_toOnlineConv.begin(), m_toOnlineConv.end(), 0,
                                                      [](unsigned int N, const auto& map) { return N + map.second.size(); });
    const unsigned int onlToOffChan = std::accumulate(m_toOfflineConv.begin(), m_toOfflineConv.end(), 0,
                                                      [](unsigned int N, const auto& map) { 
                return N + std::accumulate(map.second.begin(), map.second.end(), 0, [](unsigned int M, const auto& tdc){ return  M +  tdc; }); });

    if (offToOnlChan != onlToOffChan || onlToOffChan != m_tdcs.size()) {
        log << MSG::ERROR << "Offline <-> online conversion channels were lost. Expect " << m_tdcs.size()
            << " cabling channels. Counted Offline -> online channels " << offToOnlChan << ". Conted Online -> offline channels "
            << onlToOffChan << endmsg;
        return false;
    }
    for (auto& toOff : m_toOfflineConv) {
        MdtTdcModule& mod = toOff.second;
        TdcOnlSet::const_iterator itr =
            std::find_if(mod.begin(), mod.end(), [](const MdtTdcOnlSorter& sorter) { return sorter && sorter->tdcZero() == 0; });
        if (itr == mod.end()) {
            log << MSG::ERROR << "There is no tdc with channel 0 in " << toOff.first
                << ". That is important to decode the station names later" << endmsg;
            return false;
        }
        mod.zero_module = (*itr);
    }
    m_listOfMezzanineTypes.clear();
    log << MSG::INFO << "MdtCabling successfully loaded. Found in total " << m_tdcs.size() << " channels." << endmsg;

    return true;
}
