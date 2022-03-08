/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
   */

#include "RPC_CondCabling/RpcCablingCondAlg.h"

#include <sstream>

#include "PathResolver/PathResolver.h"

RpcCablingCondAlg::RpcCablingCondAlg(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator) {}

StatusCode RpcCablingCondAlg::initialize() {
    ATH_MSG_DEBUG("initializing" << name());
    ATH_CHECK(m_readKey_map_schema.initialize());
    ATH_CHECK(m_readKey_map_schema_corr.initialize());
    ATH_CHECK(m_readKey_cm_thr_eta.initialize());
    ATH_CHECK(m_readKey_cm_thr_phi.initialize());
    ATH_CHECK(m_writeKey.initialize());
    ATH_CHECK(m_idHelperSvc.retrieve());
    RDOindex::setRpcIdHelper(&m_idHelperSvc->rpcIdHelper());
    return StatusCode::SUCCESS;
}

StatusCode RpcCablingCondAlg::execute() {
    
    const EventContext& ctx = Gaudi::Hive::currentContext();
    ATH_MSG_DEBUG("executing" << name());

    SG::WriteCondHandle<RpcCablingCondData> writeHandle{m_writeKey, ctx};
    if (writeHandle.isValid()) {
        ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                                    << ". In theory this should not be called, but may happen"
                                    << " if multiple concurrent events are being processed out of order.");
        return StatusCode::SUCCESS;
    }

    // read in the 4 different cond objects:
    // map
    SG::ReadCondHandle<CondAttrListCollection> readHandle_map_schema{m_readKey_map_schema, ctx};
    const CondAttrListCollection* readCdoMap{*readHandle_map_schema};
    if (!readCdoMap) {
        ATH_MSG_FATAL("Null pointer to the read conditions object");
        return StatusCode::FAILURE;
    }
    writeHandle.addDependency(readHandle_map_schema);
    ATH_MSG_DEBUG("Size of CondAttrListCollection readCdoMap->size()= " << readCdoMap->size());

    // corr
    SG::ReadCondHandle<CondAttrListCollection> readHandle_map_schema_corr{m_readKey_map_schema_corr, ctx};
    const CondAttrListCollection* readCdoCorr{*readHandle_map_schema_corr};
    if (!readCdoCorr) {
        ATH_MSG_FATAL("Null pointer to the read conditions object");
        return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Size of CondAttrListCollection readCdoMap_corr->size()= " << readCdoCorr->size());
    writeHandle.addDependency(readHandle_map_schema_corr);

    // EtaTable
    SG::ReadCondHandle<CondAttrListCollection> readHandle_cm_thr_eta{m_readKey_cm_thr_eta, ctx};
    const CondAttrListCollection* readCdoEta{*readHandle_cm_thr_eta};
    if (!readCdoEta) {
        ATH_MSG_FATAL("Null pointer to the read conditions object");
        return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Size of CondAttrListCollection readCdo_cm_thr_eta->size()= " << readCdoEta->size());
    writeHandle.addDependency(readHandle_cm_thr_eta);

    // phiTable
    SG::ReadCondHandle<CondAttrListCollection> readHandle_cm_thr_phi{m_readKey_cm_thr_phi, ctx};
    const CondAttrListCollection* readCdoPhi{*readHandle_cm_thr_phi};
    if (!readCdoPhi) {
        ATH_MSG_FATAL("Null pointer to the read conditions object");
        return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Size of CondAttrListCollection readCdo_cm_thr_phi->size()= " << readCdoPhi->size());
    writeHandle.addDependency(readHandle_cm_thr_phi);

    std::unique_ptr<RpcCablingCondData> writeCdo{std::make_unique<RpcCablingCondData>()};
    ATH_CHECK(setup(readCdoMap, readCdoCorr, readCdoEta, readCdoPhi, writeCdo.get()));

    if (writeHandle.record(std::move(writeCdo)).isFailure()) {
        ATH_MSG_FATAL("Could not record RpcCondCablingData " << writeHandle.key() << " with EventRange " << writeHandle.getRange()
                                                             << " into Conditions Store");
        return StatusCode::SUCCESS;
    }

    ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << writeHandle.getRange());
    return StatusCode::SUCCESS;
}

StatusCode RpcCablingCondAlg::setup(const CondAttrListCollection* readCdoMap, const CondAttrListCollection* readCdoCorr,
                                    const CondAttrListCollection* readCdoEta, const CondAttrListCollection* readCdoPhi,
                                    RpcCablingCondData* writeCdo) const {
    ATH_MSG_DEBUG("starting setup()");

    int maxType = 0;
    sectorMap_t sectorMap;
    RpcCablingCondData::STvec sectorType;
    std::vector<unsigned short int> feedPadThresholds;
    std::map<std::string, std::string> trigroads;
    SLmap_t sectorLogic;

    // ------------------------------
    // readCdoMap
    // ------------------------------
    ATH_MSG_INFO("setup() - Reading " << m_readKey_map_schema.key());
    const coral::AttributeList& atr_map = readCdoMap->attributeList(1);  // All data sits in cool channel 1
    std::stringstream MAP(atr_map["Map"].data<std::string>());

    // ------------------------------
    // readCdoCorr
    // ------------------------------
    ATH_MSG_INFO("setup() - Reading " << m_readKey_map_schema_corr.key());
    const coral::AttributeList& atr_map_corr = readCdoCorr->attributeList(1);  // All data sits in cool channel 1
    std::stringstream MAP_corr(atr_map_corr["Map"].data<std::string>());

    // ------------------------------
    // readCdoEta
    // ------------------------------
    ATH_MSG_INFO("setup() - Reading " << m_readKey_cm_thr_eta.key());
    std::string etaCM_File;
    std::string etaTh0;
    CondAttrListCollection::const_iterator itr_eta;
    for (itr_eta = readCdoEta->begin(); itr_eta != readCdoEta->end(); ++itr_eta) {
        const coral::AttributeList& atr = itr_eta->second;
        etaCM_File = *(static_cast<const std::string*>((atr["CM_File"]).addressOfData()));
        etaTh0 = *(static_cast<const std::string*>((atr["Th0"]).addressOfData()));
        trigroads[etaCM_File] = etaTh0;
    }

    // ------------------------------
    // readCdoPhi
    // ------------------------------
    ATH_MSG_INFO("setup() - Reading " << m_readKey_cm_thr_phi.key());
    std::string phiCM_File;
    std::string phiTh0;
    CondAttrListCollection::const_iterator itr;
    for (itr = readCdoPhi->begin(); itr != readCdoPhi->end(); ++itr) {
        const coral::AttributeList& atr = itr->second;
        phiCM_File = *(static_cast<const std::string*>((atr["CM_File"]).addressOfData()));
        phiTh0 = *(static_cast<const std::string*>((atr["Th0"]).addressOfData()));
        trigroads[phiCM_File] = phiTh0;
    }
    // ------------------------------

    DBline data(MAP);
    // store the setup environment
    std::string version, setup, layout;

    // Start reading routine
    unsigned int nlines = 0;

    std::string dataName = PathResolver::find_directory(m_database_repository, "DATAPATH");

    // Start reading routine
    while (++data) {
        ATH_MSG_DEBUG("setup() - Reading routine. nlines = " << nlines);
        ++nlines;
        data("Version") >> version >> setup >> layout;

        // Read the Sector Type enumeration
        int start = 0;
        int stop = 0;
        if (data("SECTOR TYPES (") >> start >> "-" >> stop >> "):") {
            for (int i = start; i <= stop; ++i) { data >> sectorMap[i]; }
        }

        // Set the maxType variable and the type of SectorMap objects
        if (stop == 63 || stop == 8) {
            for (int i = 0; i < 64; ++i) { maxType = std::max(sectorMap[i], maxType); }
            sectorType.clear();
            //sectorType.reserve(maxType); // this would require a copy-constructor
            ATH_MSG_DEBUG("setup() - Loop over " << maxType << " sector-types");

            for (int i = 1; i <= maxType; ++i) {
                sectorType.emplace_back(i, dataName, layout, m_cosmic_configuration);
                RPC_CondCabling::SectorLogicSetup& sec = sectorType[i - 1];
                sec.SetPtoTrigRoads(&trigroads);
                for (int j = 0; j < 64; ++j) {
                    if (sectorMap[j] == i) {
                        sec << j;
                        sectorLogic.insert(SLmap_t::value_type(j, &sec));
                        ATH_MSG_DEBUG("setup() - filling sectorLogicSetup Map for type " << i << " sector  " << j);
                    }
                }
            }
        }

        // Loop on GEOMETRY TYPES
        for (int i = 1; i <= maxType; ++i) {
            // Read the RPC geometry
            if (data("RPC GEOM  # :", i)) {
                RPC_CondCabling::RPCchamberdata RPCdata(data, i);
                if (!(sectorType[i - 1] += RPCdata)) return StatusCode::FAILURE;
            }
            // Read the Wired OR geometry
            if (data("WIRED OR  # :", i)) {
                RPC_CondCabling::WiredORdata WORdata(data, i);
                if (!(sectorType[i - 1] += WORdata)) return StatusCode::FAILURE;
            }
            // Read the CMAs segmentation
            if (data("CMAs  # : pivot segmentation", i)) {
                RPC_CondCabling::CMApivotdata CMAdata(data, i, layout);
                if (!(sectorType[i - 1] += CMAdata)) return StatusCode::FAILURE;
            }
            // Read the CMAs cabling
            if (data("CMAs  # : eta cabling", i)) {
                RPC_CondCabling::CMAcablingdata CMAdata(data, i);
                if (!(sectorType[i - 1] += CMAdata)) return StatusCode::FAILURE;
            }
        }
    }
    ATH_MSG_DEBUG("setup() - map n. of lines read is " << nlines);
    ATH_MSG_INFO("setup() - version is " << version << " " << setup << " " << layout << " (cosmic=" << (int)m_cosmic_configuration << ")");

    for (int i = 1; i <= maxType; ++i) {
        if (!sectorType[i - 1].setup(msg())) return StatusCode::FAILURE;
        if (!sectorType[i - 1].check()) return StatusCode::FAILURE;
        if (msgLvl(MSG::DEBUG)) {
            ATH_MSG_DEBUG("calling get_cabling for i=" << i);
            const RPC_CondCabling::SectorLogicSetup::EtaCMAmap& CMAs = sectorType[i - 1].giveEtaCMA();
            for (const auto& cma : CMAs) {
                unsigned int cabling = UINT_MAX;
                if (cma.second.get_cabling(CMAinput::Pivot, 0, 0, 0, cabling)) {
                    ATH_MSG_DEBUG("got pivot cabling=" << cabling);
                } else {
                    ATH_MSG_DEBUG("failed to get cabling");
                }
            }
        }
    }

    ATH_MSG_DEBUG("setup() - " << m_readKey_map_schema.key() << " maps have been parsed");

    for (int sector = 0; sector < 64; ++sector) {
        unsigned int nRDOentries = 0;
        int side = (sector < 32) ? 0 : 1;
        int logic_sector = sector % 32;

        if (sectorMap[sector]) {
            // get the Sector Logic Setup
            const RPC_CondCabling::SectorLogicSetup& Sector = sectorType[sectorMap[sector] - 1];

            // get the Eta CMA map from the Sector Logic Setup
            const RPC_CondCabling::SectorLogicSetup::EtaCMAmap& CMAs = Sector.giveEtaCMA();
            RPC_CondCabling::SectorLogicSetup::EtaCMAmap::const_iterator it = CMAs.begin();

            bool isFirst = false;

            // loop over the whole set of Eta CMAs
            while (it != CMAs.end()) {
                // get the set of parameters for idenfying the first RPC strip
                unsigned int ID = (*it).second.id().Ixx_index();
                bool inversion = (*it).second.inversion();
                isFirst = (ID == 1 && inversion) || (ID == 0 && !inversion);
                unsigned int PADid = (*it).first.PAD_index();
                unsigned int cabling = UINT_MAX;
                if ((*it).second.get_cabling(CMAinput::Pivot, 0, 0, 0, cabling)) {
                    unsigned int RPC_strip = cabling % 100;
                    unsigned int RPC_chamber = (cabling / 100) % 100;
                    unsigned int RPC_layer = 0;
                    unsigned int RPC_station = (*it).second.whichCMAstation(CMAinput::Pivot);
                    unsigned int lvl1_sector = sector;

                    const RPC_CondCabling::RPCchamber* rpc = Sector.find_chamber(RPC_station, RPC_chamber);
                    std::string name = rpc->stationName();
                    int sEta = (side) ? rpc->stationEta() : -rpc->stationEta();
                    int sPhi = (logic_sector == 31) ? 1 : (logic_sector + 1) / 4 + 1;
                    int dR = rpc->doubletR();
                    int dZ = rpc->doubletZ();
                    int dP = (rpc->phiReadoutPannels() == 2) ? (logic_sector + 1) % 2 + 1 : 1;

                    // build the Level-1 code index
                    RPCdecoder decode(Eta, lvl1_sector, RPC_station, RPC_layer, RPC_chamber, RPC_strip);

                    // instanciate the corresponding RDO index
                    RDOindex rdo(PADid, decode.code(), name, sEta, sPhi, dR, dZ, dP);

                    // compute the key for retrieving RDO into the map
                    int key = side * 10000 + logic_sector * 100 + PADid;
                    // insert the RDO into the map
                    std::pair<RpcCablingCondData::RDOmap::iterator, bool> ins =
                        writeCdo->m_RDOs.insert(RpcCablingCondData::RDOmap::value_type(key, rdo));

                    if (!ins.second) {
                        ATH_MSG_ERROR("RpcCablingCondData::RDOmap is false for stationName="
                                      << name << ", stationEta=" << sEta << ", stationPhi=" << sPhi << ", doubletR=" << dR
                                      << ", doubletZ=" << dZ << ", doubletPhi=" << dP);
                        return StatusCode::FAILURE;
                    }

                    ++nRDOentries;
                }

                else if (!(*it).second.get_cabling(CMAinput::Pivot, 0, 0, 0, cabling) && isFirst) {
                    // try to catch configrm cabling
                    bool existLow = false;
                    bool existHigh = false;

                    if (!(existLow = (*it).second.get_cabling(CMAinput::LowPt, 0, 0, 0, cabling)))
                        existHigh = (*it).second.get_cabling(CMAinput::HighPt, 0, 0, 0, cabling);

                    if (!existLow && !existHigh) {
                        ATH_MSG_ERROR("Error while configuring the RDO map");
                        return StatusCode::FAILURE;
                    }

                    unsigned int RPC_strip = cabling % 100;
                    unsigned int RPC_chamber = (cabling / 100) % 100;
                    unsigned int RPC_layer = 0;
                    unsigned int RPC_station = 0;
                    if (existLow)
                        RPC_station = (*it).second.whichCMAstation(CMAinput::LowPt);
                    else
                        RPC_station = (*it).second.whichCMAstation(CMAinput::HighPt);
                    unsigned int lvl1_sector = sector;

                    const RPC_CondCabling::RPCchamber* rpc = Sector.find_chamber(RPC_station, RPC_chamber);
                    std::string name = rpc->stationName();
                    int sEta = (side) ? rpc->stationEta() : -rpc->stationEta();
                    int sPhi = (logic_sector == 31) ? 1 : (logic_sector + 1) / 4 + 1;
                    int dR = rpc->doubletR();
                    int dZ = rpc->doubletZ();
                    int dP = (rpc->phiReadoutPannels() == 2) ? (logic_sector + 1) % 2 + 1 : 1;

                    // build the Level-1 code index
                    RPCdecoder decode(Eta, lvl1_sector, RPC_station, RPC_layer, RPC_chamber, RPC_strip);

                    // instanciate the corresponding RDO index
                    RDOindex rdo(PADid, decode.code(), name, sEta, sPhi, dR, dZ, dP);

                    // compute the key for retrieving RDO into the map
                    int key = side * 10000 + logic_sector * 100 + PADid;

                    // insert the RDO into the map
                    std::pair<RpcCablingCondData::RDOmap::iterator, bool> ins =
                        writeCdo->m_RDOs.insert(RpcCablingCondData::RDOmap::value_type(key, rdo));

                    if (!ins.second) {
                        ATH_MSG_ERROR("RpcCablingCondData::RDOmap is false for stationName="
                                      << name << ", stationEta=" << sEta << ", stationPhi=" << sPhi << ", doubletR=" << dR
                                      << ", doubletZ=" << dZ << ", doubletPhi=" << dP);
                        return StatusCode::FAILURE;
                    }

                    ++nRDOentries;
                }

                // increase the iterator
                while (it != CMAs.end() && (unsigned int)(*it).first.PAD_index() == PADid) { ++it; }
            }
        }
        ATH_MSG_DEBUG("setup() - sector " << sector << ", number of associated RDO (i.e. Pads) = " << nRDOentries);
    }

    DBline data_corr(MAP_corr);
    nlines = 0;
    while (++data_corr) {
        ++nlines;
        unsigned short int SubId;
        unsigned short int SecId;
        unsigned short int PADId;
        unsigned short int CMAId;
        unsigned short int ijk;
        unsigned short int inputType;
        unsigned int layer;
        unsigned int type;
        unsigned short int Channel1;
        unsigned short int Channel2;
        int Number;

        if (data_corr("CABLE") >> data_corr.dbhex() >> SubId >> SecId >> PADId >> CMAId >> ijk >> data_corr.dbdec() >> type >> Channel1 >>
            Channel2 >> Number) {
            bool ok = CableParamCheck(SubId, SecId, PADId, CMAId, ijk, type, Channel1, Channel2, Number);

            layer = (ijk == 0 || ijk == 2 || ijk == 3) ? 0 : 1;

            if (ijk > 1) {
                Channel1 += 32 * (ijk % 2);
                Channel2 += 32 * (ijk % 2);
            }

            unsigned short int lh = (CMAId >> 2) & 1;

            if (ijk == 0 || ijk == 1)
                inputType = 1;
            else
                inputType = (lh) ? 2 : 0;

            if (ijk > 5) inputType = 3;
            if (type > 3) type = 4;

            if (ok && !correct(SubId, SecId, PADId, CMAId, static_cast<CMAinput>(inputType), layer, Channel1, Channel2, Number,
                               static_cast<L1RPCcabCorrection>(type), sectorMap, sectorType)) {
                ATH_MSG_WARNING("Cannot apply correction: "
                                << std::hex << std::setw(4) << std::showbase << SubId << " " << std::setw(4) << std::showbase << SecId
                                << " " << std::setw(3) << std::showbase << PADId << " " << std::setw(3) << std::showbase << CMAId << " "
                                << std::setw(3) << std::showbase << ijk << "  " << std::dec << std::setw(1) << std::showbase << type << " "
                                << std::setw(2) << std::showbase << Channel1 << " " << std::setw(2) << std::showbase << Channel2 << " "
                                << std::setw(2) << std::showbase << Number);
            }
        }
        if (data_corr("BOARD") >> data_corr.dbhex() >> SubId >> SecId >> PADId >> CMAId >> data_corr.dbdec() >> inputType >> layer >>
            type >> Channel1 >> Channel2 >> Number) {
            bool ok = BoardParamCheck(SubId, SecId, PADId, CMAId, inputType, layer, type, Channel1, Channel2, Number);

            if (inputType > 2) inputType = 3;
            if (type > 3) type = 4;

            if (ok && !correct(SubId, SecId, PADId, CMAId, static_cast<CMAinput>(inputType), layer, Channel1, Channel2, Number,
                               static_cast<L1RPCcabCorrection>(type), sectorMap, sectorType)) {
                ATH_MSG_WARNING("Cannot apply correction: "
                                << std::hex << std::setw(4) << std::showbase << SubId << " " << std::setw(4) << std::showbase << SecId
                                << " " << std::setw(3) << std::showbase << PADId << " " << std::setw(3) << std::showbase << CMAId << "  "
                                << std::dec << std::setw(1) << inputType << " " << std::setw(1) << layer << " " << std::dec << std::setw(1)
                                << std::showbase << type << " " << std::setw(2) << std::showbase << Channel1 << " " << std::setw(2)
                                << std::showbase << Channel2 << " " << std::setw(2) << std::showbase << Number);
            }
        }
    }
    ATH_MSG_DEBUG("setup() - corrected map n. of lines read is " << nlines);
    ATH_MSG_DEBUG("setup() - " << m_readKey_map_schema_corr.key() << " maps have been parsed");

    for (int side = 0; side < 2; ++side) {
        for (int rod = 0; rod < 16; ++rod) { (writeCdo->m_rod2hash[side][rod]).clear(); }
    }

    int hashID = 0;
    std::set<uint32_t> ROBid;
    IdContext rpcModuleContext = m_idHelperSvc->rpcIdHelper().module_context();

    // reserve enough space in the hash-vector
    writeCdo->m_HashVec.reserve(writeCdo->m_RDOs.size());

    ATH_MSG_INFO("setup() - start building OfflineOnlineMap for " << writeCdo->m_RDOs.size() << " RDOs");

    RpcCablingCondData::RDOmap::iterator pad_beg = writeCdo->m_RDOs.begin();
    RpcCablingCondData::RDOmap::iterator pad_end = writeCdo->m_RDOs.end();
    for (; pad_beg != pad_end; ++pad_beg) {
        const RDOindex* pRDOindex = &((*pad_beg).second);
        (*pad_beg).second.set_hash(hashID);

        // get pointer to RDOindex class
        writeCdo->m_HashVec.push_back(pRDOindex);

        if (writeCdo->m_HashVec.size() != pRDOindex->hash() + 1) {
            ATH_MSG_ERROR("Size of hash vector and RDO hash does not match");
            return StatusCode::FAILURE;
        }

        // calculate  m_fullListOfRobIds
        const unsigned short int rob_id = pRDOindex->ROBid();
        const unsigned short int rod_id = pRDOindex->RODid();
        const unsigned short int sub_id = pRDOindex->side();
        const unsigned short int sec_id = pRDOindex->SLid();
        const unsigned short int pad_id = pRDOindex->PADid();

        uint32_t ROD_ID = (sub_id << 16) | rod_id;
        uint32_t ROB_ID = (sub_id << 16) | rob_id;

        ROBid.insert(ROB_ID);
        unsigned short int sub_id_index = ((sub_id == 0x65) ? 1 : 0);  // convert 0x65 -> 1 (side A) and 0x66 -> 0 (side C)

        Identifier id;
        pRDOindex->pad_identifier(id);

        ATH_MSG_DEBUG(hashID << "-th entry has sub_id_index=" << sub_id_index << ", ROBid=" << rob_id << ", RODid=" << rod_id << ", side="
                             << sub_id << ", SLid=" << sec_id << ", PADid=" << pad_id << ", pad_identifier=" << id.get_compact());

        // build the offline_id vector
        writeCdo->m_offline_id[sub_id_index][sec_id][pad_id] = id;

        // build the map
        std::pair<RpcCablingCondData::OfflineOnlineMap::iterator, bool> ins =
            writeCdo->m_RDOmap.insert(RpcCablingCondData::OfflineOnlineMap::value_type(id, pRDOindex));
        ATH_MSG_DEBUG("OfflineOnlineMap new entry: Identifier with technology="
                      << m_idHelperSvc->rpcIdHelper().technology(id) << ", stationName=" << m_idHelperSvc->rpcIdHelper().stationName(id)
                      << ", stationEta=" << m_idHelperSvc->rpcIdHelper().stationEta(id) << ", stationPhi="
                      << m_idHelperSvc->rpcIdHelper().stationPhi(id) << ", doubletR=" << m_idHelperSvc->rpcIdHelper().doubletR(id)
                      << ", doubletZ=" << m_idHelperSvc->rpcIdHelper().doubletZ(id) << ", doubletPhi="
                      << m_idHelperSvc->rpcIdHelper().doubletPhi(id) << " and hash of the RDOindex(key)= " << pRDOindex->hash());
        if (!ins.second) {
            ATH_MSG_ERROR("RpcCablingCondData::OfflineOnlineMap is false for technology="
                          << m_idHelperSvc->rpcIdHelper().technology(id) << ", stationName=" << m_idHelperSvc->rpcIdHelper().stationName(id)
                          << ", stationEta=" << m_idHelperSvc->rpcIdHelper().stationEta(id) << ", stationPhi="
                          << m_idHelperSvc->rpcIdHelper().stationPhi(id) << ", doubletR=" << m_idHelperSvc->rpcIdHelper().doubletR(id)
                          << ", doubletZ=" << m_idHelperSvc->rpcIdHelper().doubletZ(id) << ", doubletPhi="
                          << m_idHelperSvc->rpcIdHelper().doubletPhi(id) << " and hash of the RDOindex(key)= " << pRDOindex->hash());
            return StatusCode::FAILURE;
        }

        // build the ROB->RDO map
        std::pair<std::set<IdentifierHash>::iterator, bool> insert_ROB_RDO_returnVal =
            writeCdo->m_ROB_RDO_map[ROB_ID].insert(IdentifierHash(pRDOindex->hash()));
        if (insert_ROB_RDO_returnVal.second)
            ATH_MSG_DEBUG("A new RDO HashId = " << pRDOindex->hash() << " registered for ROB Id = " << ROB_ID);
        else
            ATH_MSG_VERBOSE("The RDO HashId = " << pRDOindex->hash() << " was already registered for ROB Id = " << ROB_ID);

        // build the PRD->RDO and PRD->ROB maps
        ATH_MSG_VERBOSE("Looking for PRDs corresponding to this RDO");
        std::list<Identifier> strip_id_list;
        IdentifierHash rdoHashId((IdentifierHash::value_type)pRDOindex->hash());
        ATH_MSG_DEBUG("RDO HashId = " << (int)rdoHashId << " RDO Id = " << id.get_compact() << " ROB Id = " << MSG::hex << ROB_ID
                                      << MSG::dec << " ROD Id = " << MSG::hex << ROD_ID << MSG::dec);

        for (unsigned short int CMAId : {2, 3, 6, 7}) {  // loop over phi CMA IDs
            for (unsigned short int ijk : {1, 2, 3}) {   // loop over IJK identifiers
                strip_id_list.clear();
                for (unsigned short int channel : {0, 31}) {  // check for the first and the last channel
                    strip_id_list.splice(strip_id_list.end(),
                                         give_strip_id(sub_id_index, sec_id, pad_id, CMAId, ijk, channel, sectorMap, sectorType));
                }
                for (Identifier strip_id : strip_id_list) {
                    Identifier idp = m_idHelperSvc->rpcIdHelper().parentID(strip_id);
                    IdentifierHash prdHashId;
                    int gethash_code = m_idHelperSvc->rpcIdHelper().get_hash(idp, prdHashId, &rpcModuleContext);
                    if (gethash_code != 0) {
                        ATH_MSG_DEBUG("Unable to get the PRD HashId! parentID(strip_id)=" << idp.getString());
                        continue;
                    }

                    // fill the PRD->RDO map
                    std::pair<std::set<IdentifierHash>::iterator, bool> insertRDO_returnVal =
                        writeCdo->m_PRD_RDO_map[prdHashId].insert(rdoHashId);
                    if (insertRDO_returnVal.second) {
                        ATH_MSG_DEBUG("A new RDO HashId = " << (int)rdoHashId << " registered for PRD HashId = " << (int)prdHashId);
                    } else {
                        ATH_MSG_VERBOSE("The RDO HashId = " << (int)rdoHashId
                                                            << " was already registered for PRD HashId = " << (int)prdHashId);
                    }

                    // fill the PRD->ROB map
                    std::pair<std::set<uint32_t>::iterator, bool> insertROB_returnVal = writeCdo->m_PRD_ROB_map[prdHashId].insert(ROB_ID);
                    if (insertROB_returnVal.second) {
                        ATH_MSG_DEBUG("A new ROB Id = " << MSG::hex << ROB_ID << MSG::dec
                                                        << " registered for PRD HashId = " << (int)prdHashId);
                    } else {
                        ATH_MSG_VERBOSE("The ROB Id = " << MSG::hex << ROB_ID << MSG::dec
                                                        << " was already registered for PRD HashId = " << (int)prdHashId);
                    }
                }
            }
        }

        // Trigger Roads Header
        std::map<std::string, std::string>::const_iterator it;
        it = trigroads.find("infos.txt");
        if (it == trigroads.end()) {
            ATH_MSG_WARNING("Missing HEADER FILE infos.txt");
        } else {
            ATH_MSG_VERBOSE("======== RPC Trigger Roads from COOL - Header infos ========");
            ATH_MSG_VERBOSE("\n" + it->second + "\n");
            // Read FeetPadThresholds from infos.txt
            if (!m_ForceFeetPadThresholdsFromJO) {
                std::stringstream ss;
                ss << it->second;
                std::string word;
                while (ss >> word) {
                    if (word == "FeetPadThresholds") {
                        feedPadThresholds.assign(3, 0);
                        ss >> feedPadThresholds.at(0);
                        ss >> feedPadThresholds.at(1);
                        ss >> feedPadThresholds.at(2);
                        ATH_MSG_VERBOSE("FeetPadThresholds set from COOL to: " << feedPadThresholds.at(0) << "," << feedPadThresholds.at(1)
                                                                               << "," << feedPadThresholds.at(2));
                    }
                }
            }
        }

        // ------ begin like  PCcablingInterface::RpcPadIdHash::RpcPadIdHash()
        RDOindex index = (*pad_beg).second;
        index.pad_identifier(id);

        writeCdo->m_int2id.push_back(id);

        if (writeCdo->m_int2id.size() != index.hash() + 1) {
            ATH_MSG_ERROR("Inconsistence between PAD hash and RpcPadIdHash");
            ATH_MSG_ERROR(index);
            ATH_MSG_ERROR("Position into RpcPadIdHash map is " << writeCdo->m_int2id.size() - 1);
            return StatusCode::FAILURE;
        }

        unsigned short int side = index.side();
        unsigned short int rod = index.RODid();

        if (rod > 15) {
            ATH_MSG_ERROR("RPC ROD greater than 15");
            return StatusCode::FAILURE;
        }

        IdentifierHash HashID = index.hash();

        (writeCdo->m_rod2hash[(side == 0x66) ? 0 : 1][rod]).push_back(HashID);

        writeCdo->m_lookup[id] = index.hash();

        ATH_MSG_DEBUG("RDO loop entry " << hashID << " done");
        ++hashID;
    }
    ATH_MSG_DEBUG("setup() - start recording RpcCablingCondData");

    // this must be done both in case of source = COOL or ASCII
    // -----  Initialization of Pad configuration ------ //
    if (m_ApplyFeetPadThresholds) {
        if (feedPadThresholds.size() != 3) {
            // if thresholds vector empty, set it to default
            feedPadThresholds.assign(3, 0);
            feedPadThresholds.at(0) = 0;
            feedPadThresholds.at(1) = 2;
            feedPadThresholds.at(2) = 5;
        }
        ATH_MSG_INFO("Applying FeetPadThresholds : " << feedPadThresholds.at(0) << "," << feedPadThresholds.at(1) << ","
                                                     << feedPadThresholds.at(2));

        const unsigned int NumFeetSectors = 8;
        unsigned int FeetSectors[NumFeetSectors] = {21, 22, 25, 26, 53, 54, 57, 58};
        const unsigned int NumSpecialFeetPads = 4;
        unsigned int SpecialFeetPads[NumSpecialFeetPads] = {2, 4, 5, 7};

        for (unsigned int is = 0; is < NumFeetSectors; is++) {
            for (unsigned int it = 0; it < NumSpecialFeetPads; it++) {
                writeCdo->m_RPCPadParameters_array[FeetSectors[is]][SpecialFeetPads[it]].set_feet_on(true);
                for (unsigned int th = 0; th < 3; th++) {
                    writeCdo->m_RPCPadParameters_array[FeetSectors[is]][SpecialFeetPads[it]].set_feet_threshold(th,
                                                                                                                feedPadThresholds.at(th));
                }
            }
        }
    }

    // reserve enough space
    writeCdo->m_fullListOfRobIds.reserve(ROBid.size());
    for (uint32_t robid : ROBid) writeCdo->m_fullListOfRobIds.push_back(robid);

    ATH_MSG_DEBUG("Number of valid RPC Pad IDs " << writeCdo->m_int2id.size());

    for (int i = 0; i < 64; i++) writeCdo->m_SectorMap[i] = sectorMap[i];
    writeCdo->m_SectorType = std::move(sectorType);
    writeCdo->m_MaxType = maxType;

    if (msgLvl(MSG::DEBUG)) {
        std::stringstream ss1;
        ss1 << "Level-1 configuration database " << std::endl;
        ss1 << "Contains " << maxType << " Trigger Sector Types:" << std::endl;
        ss1 << "negative sectors  0 - 15 ==> ";
        for (int i = 0; i < 16; i++) ss1 << std::setw(2) << sectorMap[i] << " ";
        ss1 << std::endl << "negative sectors 16 - 31 ==> ";
        for (int i = 16; i < 32; i++) ss1 << std::setw(2) << sectorMap[i] << " ";
        ss1 << std::endl << "positive sectors 32 - 47 ==> ";
        for (int i = 32; i < 48; i++) ss1 << std::setw(2) << sectorMap[i] << " ";
        ss1 << std::endl << "positive sectors 48 - 63 ==> ";
        for (int i = 48; i < 64; i++) ss1 << std::setw(2) << sectorMap[i] << " ";
        ss1 << std::endl;
        ATH_MSG_DEBUG(ss1.str());
    }

    // record
    if (writeCdo->m_RDOs.empty()) {
        ATH_MSG_ERROR("Could not read any map configuration");
        return StatusCode::FAILURE;
    }
    if (writeCdo->m_HashVec.empty()) {
        ATH_MSG_ERROR("Could not read any HashID");
        return StatusCode::FAILURE;
    }
    if (writeCdo->m_SectorType.empty()) {
        ATH_MSG_ERROR("Could not read any sectorMap");
        return StatusCode::FAILURE;
    }
    if (writeCdo->m_int2id.empty()) {
        ATH_MSG_ERROR("Could not read any HashID");
        return StatusCode::FAILURE;
    }
    if (writeCdo->m_lookup.empty()) {
        ATH_MSG_ERROR("Could not read any HashID");
        return StatusCode::FAILURE;
    }
    if (writeCdo->m_fullListOfRobIds.empty()) {
        ATH_MSG_ERROR("Could not read any HashID");
        return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("setup() - RpcCablingCondData recorded");
    return StatusCode::SUCCESS;
}

std::list<Identifier> RpcCablingCondAlg::give_strip_id(const unsigned short int SubsystemId, const unsigned short int SectorId,
                                                       const unsigned short int PADId, const unsigned short int CMAId,
                                                       const unsigned short ijk, const unsigned short int Channel, const sectorMap_t& smap,
                                                       const RpcCablingCondData::STvec& sType) const {
    std::list<unsigned int> CodeList;

    int logic_sector = SectorId + SubsystemId * 32;
    unsigned short int Ixx = CMAId & 1;
    unsigned short int ep = (CMAId >> 1) & 1;
    unsigned short int lh = (CMAId >> 2) & 1;

    ep = (ep == 1) ? 0 : 1;

    // retrieve the Sector Logic setup
    const RPC_CondCabling::SectorLogicSetup& s = sType[smap[logic_sector] - 1];

    // retrieve the CMAparameters associated to the identifiers
    if (ep) {
        CMAcoverage PhiCov = (logic_sector % 2) ? OddSectors : EvenSectors;
        CMAidentity PHI(ViewType::Phi, PhiCov, PADId, Ixx);
        CodeList = s.give_strip_code(PHI, logic_sector, lh, ijk, Channel);
    } else {
        CMAidentity ETA(ViewType::Eta, CMAcoverage::AllSectors, PADId, Ixx);
        CodeList = s.give_strip_code(ETA, logic_sector, lh, ijk, Channel);
    }

    std::list<RPCofflineId> offlineIdList;
    std::list<unsigned int>::const_iterator it = CodeList.begin();
    while (it != CodeList.end()) {
        RPCdecoder decode(*it);
        if (!decode)
            throw std::runtime_error("RpcCablingCondAlg::give_strip_id() - cannot decode LVL1 Id at " +
                                     std::string(__FILE__) + ":" + std::to_string(__LINE__));
        RPCofflineId rpcId;

        int RPC_strip = decode.strip_number();
        int RPC_chamber = decode.rpc_z_index();
        int RPC_layer = decode.rpc_layer();
        int RPC_station = decode.lvl1_station();
        int sector = (decode.logic_sector()) % 32;

        const RPC_CondCabling::RPCchamber* rpc = s.find_chamber(RPC_station, RPC_chamber);

        rpcId.stationName = rpc->stationName();
        rpcId.stationEta = (decode.half_barrel() == Positive) ? rpc->stationEta() : -rpc->stationEta();
        rpcId.stationPhi = (sector == 31) ? 1 : (sector + 1) / 4 + 1;
        rpcId.doubletR = rpc->doubletR();
        rpcId.doubletZ = rpc->doubletZ();
        rpcId.doubletPhi = (rpc->phiReadoutPannels() == 2) ? (sector + 1) % 2 + 1 : 1;
        rpcId.gasGap = RPC_layer + 1;
        rpcId.measuresPhi = static_cast<int>(decode.view());
        rpcId.strip = RPC_strip + 1;

        offlineIdList.push_back(rpcId);

        ++it;
    }

    std::list<Identifier> id;
    std::list<RPCofflineId>::const_iterator iterator = offlineIdList.begin();
    while (iterator != offlineIdList.end()) {
        Identifier rpcId = m_idHelperSvc->rpcIdHelper().channelID((*iterator).stationName, (*iterator).stationEta, (*iterator).stationPhi,
                                                                  (*iterator).doubletR, (*iterator).doubletZ, (*iterator).doubletPhi,
                                                                  (*iterator).gasGap, (*iterator).measuresPhi, (*iterator).strip);
        id.push_back(rpcId);
        ++iterator;
    }

    return id;
}

bool RpcCablingCondAlg::correct(const unsigned short int SubsystemId, const unsigned short int SectorId, const unsigned short int PADId,
                                const unsigned short int CMAId, const CMAinput it, const unsigned int layer,
                                const unsigned short int Channel1, const unsigned short int Channel2, const short int number,
                                const L1RPCcabCorrection type, const sectorMap_t& smap, const RpcCablingCondData::STvec& sType) {
    int logic_sector = (SubsystemId == 0x65) ? SectorId + 32 : SectorId;
    unsigned short int Ixx = CMAId & 1;
    unsigned short int ep = (CMAId >> 1) & 1;

    ep = (ep == 1) ? 0 : 1;

    // retrieve the Sector Logic setup
    const RPC_CondCabling::SectorLogicSetup& s = sType[smap[logic_sector] - 1];

    // retrieve the CMAparameters associated to the identifiers
    if (ep) {
        CMAcoverage PhiCov = (logic_sector % 2) ? OddSectors : EvenSectors;
        CMAidentity PHI(Phi, PhiCov, PADId, Ixx);
        return s.correct(PHI, type, it, layer, Channel1, Channel2, number);
    } else {
        CMAidentity ETA(Eta, AllSectors, PADId, Ixx);
        return s.correct(ETA, type, it, layer, Channel1, Channel2, number);
    }
    return false;
}

bool RpcCablingCondAlg::BoardParamCheck(const unsigned short int SubId, const unsigned short int SecId, const unsigned short int PADId,
                                        const unsigned short int CMAId, const unsigned short int inputType, const unsigned int layer,
                                        const unsigned int type, const unsigned short int Channel1, const unsigned short int Channel2,
                                        const short int Number) const {
    if (SubId != 0x65 && SubId != 0x66) {
        ATH_MSG_ERROR("Subsystem Id out of range: " << SubId << " not in [0x65,0x66].");
        return false;
    }
    if (SecId > 31) {
        ATH_MSG_ERROR("Sector Id out of range [0/31].");
        return false;
    }
    if (PADId > 8) {
        ATH_MSG_ERROR("Pad Id out of range [0/8].");
        return false;
    }
    if (CMAId > 7) {
        ATH_MSG_ERROR("CMA Id out of range [0/7].");
        return false;
    }
    if (inputType > 2) {
        ATH_MSG_ERROR("Board input out of range [0/2].");
        return false;
    }
    if (layer > 1) {
        ATH_MSG_ERROR("Board layer out of range [0/1].");
        return false;
    }
    if (type > 3) {
        ATH_MSG_ERROR("correction type out of range [0/3].");
        return false;
    }
    if (Channel1 > 63) {
        ATH_MSG_ERROR("ijk Channel 1 out of range [0/31].");
        return false;
    }
    if (Channel2 > 63) {
        ATH_MSG_ERROR("ijk Channel 2 out of range [0/31].");
        return false;
    }
    if (Number > 64) {
        ATH_MSG_ERROR("Number out of range [0/64].");
        return false;
    }
    return true;
}

bool RpcCablingCondAlg::CableParamCheck(const unsigned short int SubId, const unsigned short int SecId, const unsigned short int PADId,
                                        const unsigned short int CMAId, const unsigned short int ijk, const unsigned int type,
                                        const unsigned short int Channel1, const unsigned short int Channel2,
                                        const short int Number) const {
    if (SubId != 0x65 && SubId != 0x66) {
        ATH_MSG_ERROR("Subsystem Id out of range [0x65,0x66].");
        return false;
    }
    if (SecId > 31) {
        ATH_MSG_ERROR("Sector Id out of range [0/31].");
        return false;
    }
    if (PADId > 8) {
        ATH_MSG_ERROR("Pad Id out of range [0/8].");
        return false;
    }
    if (CMAId > 7) {
        ATH_MSG_ERROR("CMA Id out of range [0/7].");
        return false;
    }
    if (ijk > 5) {
        ATH_MSG_ERROR("ijk out of range [0/5].");
        return false;
    }
    if (type > 3) {
        ATH_MSG_ERROR("correction type out of range [0/3].");
        return false;
    }
    if (Channel1 > 31) {
        ATH_MSG_ERROR("ijk Channel 1 out of range [0/31].");
        return false;
    }
    if (Channel2 > 31) {
        ATH_MSG_ERROR("ijk Channel 2 out of range [0/31].");
        return false;
    }
    if (Number > 32) {
        ATH_MSG_ERROR("Number out of range [0/32].");
        return false;
    }
    return true;
}
